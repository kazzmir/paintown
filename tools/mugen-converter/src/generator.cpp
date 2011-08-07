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
        out <<  "class " << Mugen::stripExtension(file) << "(mugen.Character):" << endl;
                out << indent;
                out << "def __init__(self):" << endl;
                    out << indent;
                    // Base definitions, files, etc (usually charactername.def)
                    handleBaseDef(out);
        
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