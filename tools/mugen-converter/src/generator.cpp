#include "generator.h"

#include "ast/all.h"
#include "util/regex.h"

#include <ctime>
#include <list>

using namespace Mugen;

std::string getCurrentDate(){
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    strftime (buffer,80,"%A - %m/%d/%Y - %I:%M%p.",timeinfo);
    
    return std::string(buffer);
}

/* Example on how to use the PythonStream */
void testStream(const std::string & file){
    Mugen::PythonStream out;
    out.open(file);
    out << "import mugen" << PythonStream::endl;
    out <<  "class TestCharacter(mugen.Character):" << PythonStream::endl;
        out << PythonStream::indent;
        out << "def __init__(self):" << PythonStream::endl;
            out << PythonStream::indent;
            out << "self.setName('TestCharacter')" << PythonStream::endl;
    out.clearIndents();
    out << "char = TestCharacter()" << PythonStream::endl;
    out << "print char.name" << PythonStream::endl;
    out.close();
}

/* Simplify use */
PythonStream::IndentType indent = PythonStream::indent;
PythonStream::IndentType unindent = PythonStream::unindent;
StreamEnd endl = PythonStream::endl;

// Content
Content::Content():
totalLines(0){
}
Content::Content(unsigned int level, const std::string & str):
totalLines(1){
    indentLevel.push_back(level);
    content.push_back(str);
}

Content::Content(const Content & copy):
totalLines(0){
    totalLines = copy.totalLines;
    indentLevel = copy.indentLevel;
    content = copy.content;
}
Content::~Content(){
}

const Content & Content::operator=(const Content & copy){
    totalLines = copy.totalLines;
    indentLevel = copy.indentLevel;
    content = copy.content;
    
    return *this;
}

void Content::addLine(unsigned int level, const std::string & str){
    indentLevel.push_back(level);
    content.push_back(str);
    totalLines++;
}

void Content::output(PythonStream & stream, unsigned int indentStart){
    for (unsigned int i = 0; i < totalLines; ++i){
        stream.setIndentLevel(indentStart + indentLevel[i]);
        stream << content[i] << endl;
    }
}

const unsigned int Content::getIndentLevel() const{
    if (totalLines!=0){
        return indentLevel[0];
    }
    return 0;
}


// Python definition
PythonDefinition::PythonDefinition(const Content & content):
defLine(content){
}
PythonDefinition::PythonDefinition(const PythonDefinition & copy){
    defLine = copy.defLine;
    content = copy.content;
}
PythonDefinition::~PythonDefinition(){
}
const PythonDefinition & PythonDefinition::operator=(const PythonDefinition & copy){
    defLine = copy.defLine;
    content = copy.content;
    return *this;
}
void PythonDefinition::addContent(const Content & cont){
    content.push_back(cont);
}
void PythonDefinition::output(PythonStream & stream, unsigned int indentStart){
    defLine.output(stream, indentStart);
    stream.setIndentLevel(indentStart + defLine.getIndentLevel());
    for (std::vector<Content>::iterator i = content.begin(); i != content.end(); ++i){
        Content & cont = *i;
        cont.output(stream, indentStart + defLine.getIndentLevel());
    }
}

// Python class
PythonClass::PythonClass(const Content & content):
classLine(content),
init(Content(1,"def __init__(self):")){
}
PythonClass::PythonClass(const PythonClass & copy):
init(Content(1,"def __init__(self):")){
    classLine = copy.classLine;
    init = copy.init;
    definitions = definitions;
}
PythonClass::~PythonClass(){
}
const PythonClass & PythonClass::operator=(const PythonClass & copy){
    classLine = copy.classLine;
    init = copy.init;
    definitions = definitions;
    return *this;
}
void PythonClass::add(const PythonDefinition & def){
    definitions.push_back(def);
}
void PythonClass::output(PythonStream & stream, unsigned int indentStart){
    classLine.output(stream, indentStart);
    stream.setIndentLevel(indentStart + classLine.getIndentLevel());
    init.output(stream, indentStart + classLine.getIndentLevel());
    for (std::vector<PythonDefinition>::iterator i = definitions.begin(); i != definitions.end(); ++i){
        PythonDefinition & def = *i;
        def.output(stream, indentStart + classLine.getIndentLevel());
    }
}


CharacterGenerator::CharacterGenerator(const std::string & filename):
filename(filename),
directory(Mugen::stripFilename(filename)){
}

CharacterGenerator::~CharacterGenerator(){
}

void CharacterGenerator::output(const std::string & file){
    try {
        std::cout << "Preparing character definition file " << filename << " for output to file: " << file << std::endl;
        
        Mugen::PythonStream out;
        out.open(file);;
        
        out << "# File generated from original file '" << Mugen::stripDir(filename) << "' on " << getCurrentDate() << endl;
        out << endl;
        
        out << "import mugen" << endl;
        out << endl;
        PythonClass character(Content(0, "class " + Mugen::stripExtension(file) + "(mugen.Character):"));
        //out <<  "class " << Mugen::stripExtension(file) << "(mugen.Character):" << endl;
                //out << indent;
                //out << "def __init__(self):" << endl;
        character.output(out, 0);
                    out << indent;
                    // Base definitions, files, etc (usually charactername.def)
                    handleBaseDef(out);
                    handleConstants(out);
        out.close();
    } catch (const Mugen::Cmd::ParseException & fail){
        std::cout << "Failed to parse " << file << " because " << fail.getReason() << std::endl;
    } catch (...){
        std::cout << "Unknown Exception Caught!\nAborting..." << std::endl;
    }
}

void CharacterGenerator::handleBaseDef(Mugen::PythonStream & stream){
    class BaseWalker: public Ast::Walker {
        public:
            BaseWalker(CharacterGenerator & character, PythonStream & stream ):
            character(character),
            stream(stream){
                
            }
            
            CharacterGenerator & character;
            PythonStream & stream;
            std::string sectionName;
            
            virtual void onSection(const Ast::Section & section){
                sectionName = section.getName();
                stream << endl;
                stream << "# [" << Mugen::stripDir(character.filename) << "] Section - " << sectionName << endl;
                std::cout << "Parsing Section: " << sectionName << std::endl;
            }
            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "name"){
                    try{
                        stream << "self.name = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "displayname"){
                    try{
                        stream << "self.displayName = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "versiondate"){
                    try{
                    std::vector<int> numbers;
                        simple >> numbers;
                        stream << "self.versionDate = [";
                        unsigned int count = 0;
                        for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            stream << *it << (count != numbers.size()-1 ? "," : "");
                            count++;
                        }
                        stream << "]" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "mugenversion"){
                    try{
                        std::vector<int> numbers;
                        simple >> numbers;
                        stream << "self.mugenVersion = [";
                        unsigned int count = 0;
                        for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            stream << *it << (count != numbers.size()-1 ? "," : "");
                            count++;
                        }
                        stream << "]" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "author"){
                    try{
                        stream << "self.author = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "pal.defaults"){
                    std::vector<int> numbers;
                        simple >> numbers;
                        stream << "self.paletteDefaults = [";
                        unsigned int count = 0;
                        for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            stream << *it << (count != numbers.size()-1 ? "," : "");
                            count++;
                        }
                        stream << "]" << endl;
                } else if (simple == "cmd"){
                    try{
                        simple >> character.commandFile;
                        stream << "self.cmdFile = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "cns"){
                    try{
                        simple >> character.constantsFile;
                        stream << "self.cnsFile = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "st"){
                    try{
                        character.stateFiles.push_back(simple.valueAsString());
                        stream << "self.stateFiles.append('" << simple.valueAsString() << "')" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "stcommon"){
                    try{
                        character.stateFiles.push_back(simple.valueAsString());
                        stream << "self.stateFiles.append('" << simple.valueAsString() << "')" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (Util::matchRegex(simple.idString(), "st[0-9]+")){
                    // std::cout << "Found additional st file: " << simple.idString() << std::endl;
                    try{
                        character.stateFiles.push_back(simple.valueAsString());
                        stream << "self.stateFiles.append('" << simple.valueAsString() << "')" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "sprite"){
                    try{
                        stream << "self.spriteFile = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "anim"){
                    try{
                        simple >> character.airFile;
                        stream << "self.animFile = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "sound"){
                    try{
                        stream << "self.soundFile = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (Util::matchRegex(simple.idString(), "pal[0-9]+")){
                    // std::cout << "Found pallete file: " << id << std::endl;
                    std::string id = simple.idString();
                    try{
                        stream << "self.palleteFiles[" << id.substr(3).c_str() << "] = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "intro.storyboard"){
                    try{
                        stream << "self.introStoryboard = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "ending.storyboard"){
                    try{
                        stream << "self.endingStoryboard = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else {
                    std::cout << "Unhandled option in " << sectionName << " Section: " << simple.toString() << std::endl;
                }
            }
            
            virtual void onAttributeKeyword (const Ast::AttributeKeyword &simple){
                std::cout << "Attribute keyword: " << simple.toString() << std::endl;
            }
            
            virtual void onAttributeArray (const Ast::AttributeArray &simple){
                std::cout << "Attribute array: " << simple.toString() << std::endl;
            }

            virtual void onNumber (const Ast::Number &simple){
                std::cout << "number: " << simple.toString() << std::endl;
            }
    };
    
    BaseWalker walker(*this, stream);
    std::list<Ast::Section*> * sections = parseDef(filename);
    for (std::list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
        Ast::Section * section = *it;
        section->walk(walker);
        std::list<Ast::Attribute*>  attributes = section->getAttributes();
    }
    destroy(sections);
}

void CharacterGenerator::handleConstants(Mugen::PythonStream & stream){
    
    // For re-use
    std::list<Ast::Section*> * sections;
    
    class CnsWalker: public Ast::Walker {
        public:
            CnsWalker(CharacterGenerator & character, PythonStream & stream ):
            character(character),
            stream(stream){
                
            }
            
            CharacterGenerator & character;
            PythonStream & stream;
            std::string sectionName;
            
            virtual void onSection(const Ast::Section & section){
                sectionName = section.getName();
                stream << endl;
                stream << "# [" << Mugen::stripDir(character.filename) << "] Section - " << sectionName << endl;
                std::cout << "Parsing Section: " << sectionName << std::endl;
            }
            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "command.time"){
                    try{
                        stream << "self.commandTime = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else if (simple == "command.buffer.time"){
                    try{
                        stream << "self.commandBufferTime = '" << simple.valueAsString() << "'" << endl;
                    } catch (const Ast::Exception & fail){
                    }
                } else {
                    std::cout << "Unhandled option in " << sectionName << " Section: " << simple.toString() << std::endl;
                }
            }
            
            virtual void onAttributeKeyword (const Ast::AttributeKeyword &simple){
                std::cout << "Attribute keyword: " << simple.toString() << std::endl;
            }
            
            virtual void onAttributeArray (const Ast::AttributeArray &simple){
                std::cout << "Attribute array: " << simple.toString() << std::endl;
            }

            virtual void onNumber (const Ast::Number &simple){
                std::cout << "number: " << simple.toString() << std::endl;
            }
    };
    
    // Command Walker
    CnsWalker cmd(*this, stream);
    sections = parseCmd(directory + commandFile);
    for (std::list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
        Ast::Section * section = *it;
        if (section->getName() == "Defaults"){
            section->walk(cmd);
            std::list<Ast::Attribute*>  attributes = section->getAttributes();
            break;
        }
    }
    destroy(sections);
    
    CnsWalker cns(*this, stream);
    sections = parseCmd(directory + constantsFile);
    bool found[4] = { false, false, false, false };
    for (std::list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
        Ast::Section * section = *it;
        if (section->getName() == "Data"){
            section->walk(cns);
            std::list<Ast::Attribute*>  attributes = section->getAttributes();
            found[0] = true;
        } else if (section->getName() == "Size"){
            section->walk(cns);
            std::list<Ast::Attribute*>  attributes = section->getAttributes();
            found[1] = true;
        } else if (section->getName() == "Velocity"){
            section->walk(cns);
            std::list<Ast::Attribute*>  attributes = section->getAttributes();
            found[2] = true;
        } else if (section->getName() == "Movement"){
            section->walk(cns);
            std::list<Ast::Attribute*>  attributes = section->getAttributes();
            found[3] = true;
        } else if (found[0] == true && found[1] == true && found[2] == true && found[3] == true) {
            break;
        }
    }
    destroy(sections);
}