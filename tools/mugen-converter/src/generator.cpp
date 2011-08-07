#include "generator.h"

#include "ast/all.h"
#include "util/regex.h"

#include <list>

using namespace Mugen;

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
filename(filename){
}

CharacterGenerator::~CharacterGenerator(){
}

class CharacterDefWalker: public Ast::Walker {
    public:
        CharacterDefWalker(PythonStream & stream ):
        stream(stream){
            
        }
        
        PythonStream & stream;
        std::string sectionName;
        
        virtual void onSection(const Ast::Section & section){
            sectionName = section.getName();
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
                    stream << "self.cmdFile = '" << simple.valueAsString() << "'" << endl;
                } catch (const Ast::Exception & fail){
                }
            } else if (simple == "cns"){
                try{
                    stream << "self.cnsFile = '" << simple.valueAsString() << "'" << endl;
                } catch (const Ast::Exception & fail){
                }
            } else if (simple == "st"){
                try{
                    stream << "self.stateFiles.append('" << simple.valueAsString() << "')" << endl;
                } catch (const Ast::Exception & fail){
                }
            } else if (simple == "stcommon"){
                try{
                    stream << "self.stateFiles.append('" << simple.valueAsString() << "')" << endl;
                } catch (const Ast::Exception & fail){
                }
            } else if (Util::matchRegex(simple.idString(), "st[0-9]+")){
                // std::cout << "Found additional st file: " << simple.idString() << std::endl;
                try{
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
        
    private:

        bool filter(char c) {
            std::string list = "1234567890=()|&![],<>";
            for(std::string::size_type pos = 0;pos < list.size();pos++) {
                if (list[pos] == c) return true;
            }
            return false;
        }
        
        std::string findword(std::string::size_type starpos, std::string phrase) {
            std::string word = "";
            for (;starpos < phrase.size();starpos++) {
                if (phrase[starpos] == '"') {
                    std::string::size_type pos = phrase.find('"', ++starpos);
                if (pos != std::string::npos) {
                    starpos = pos;
                }
                continue;
                }

                if(filter(phrase[starpos]) && word == "") {
                    continue;
                }

                if (phrase[starpos] == ' ') break;
                word += phrase[starpos];
            }  

            return word;
        }
};




void CharacterGenerator::output(const std::string & file){
    try {
        Mugen::PythonStream out;
        out.open(file);
        out << "import mugen" << endl;
        out << endl;
        out <<  "class " << Mugen::stripExtension(file) << "(mugen.Character):" << endl;
                out << indent;
                out << "def __init__(self):" << endl;
                    out << indent;
        CharacterDefWalker walker(out);
        std::cout << "Preparing character definition file " << filename << " for output to file: " << file << std::endl;
        std::list<Ast::Section*> * sections = parseDef(filename);
        for (std::list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
            Ast::Section * section = *it;
            section->walk(walker);
            std::list<Ast::Attribute*>  attributes = section->getAttributes();
        }
        destroy(sections);
        out.close();
    } catch (const Mugen::Cmd::ParseException & fail){
        std::cout << "Failed to parse " << file << " because " << fail.getReason() << std::endl;
    } catch (...){
        std::cout << "Unknown Exception Caught!\nAborting..." << std::endl;
    }
}