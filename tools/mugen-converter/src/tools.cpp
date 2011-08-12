#include "tools.h"
#include "ast/all.h"

using namespace std;

Mugen::StreamEnd Mugen::PythonStream::endl;

const std::string addTab(int total = 1){
    std::string tabAmount;
    for(int i = 0; i < total; ++i){
        tabAmount += "    ";
    }
    return tabAmount;
}

string Util::trim(const std::string & str){
    string s;
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");
    // if all spaces or empty return an empty string  
    if ((string::npos == startpos) ||
        (string::npos == endpos)){
        return "";
    } else {
        return str.substr(startpos, endpos-startpos+1);
    }
    return str;
}

void Mugen::destroy(std::list<Ast::Section*> * list){
    for (std::list<Ast::Section*>::iterator it = list->begin(); it != list->end(); it++){
        delete (*it);
    }
    delete list;
}

list<Ast::Section*> * Mugen::parseAir(const string & file){
    return (std::list<Ast::Section*>*) Mugen::Air::parse(file);
}

list<Ast::Section*> * Mugen::parseCmd(const string & file){
    return (std::list<Ast::Section*>*) Mugen::Cmd::parse(file);
}

list<Ast::Section*> * Mugen::parseDef(const string & file){
    return (std::list<Ast::Section*>*) Mugen::Def::parse(file);
}

/* d.bla -> d */
std::string Mugen::stripExtension(const std::string & str){
    size_t pos = str.find_last_of(".");
    return str.substr(0,pos);
}

/* a/b/c/d -> d */
std::string Mugen::stripDir(const std::string & str){
    if (str.find("/") != std::string::npos || str.find( "\\") != std::string::npos){
        std::string temp = str;
        size_t rem = temp.find_last_of( "/" );
        if (rem != std::string::npos){
            return str.substr(rem+1,str.size());
        }
        rem = temp.find_last_of( "\\" );
        if( rem != std::string::npos ){
            return str.substr(rem+1,str.size());
        }
    }
    return str; 
}

/* a/b/c/d -> a/b/c/ */
std::string Mugen::stripFilename(const std::string & str){
    std::string temp = str;
    if( str.find( "/") != std::string::npos || str.find( "\\") != std::string::npos ){
        size_t rem = temp.find_last_of( "/" );
        if( rem != std::string::npos ){
            return str.substr(0,rem+1);
        }
        rem = temp.find_last_of( "\\" );
        if( rem != std::string::npos ){
            return str.substr(0,rem+1);
        }
    }
    return "";
}

static int toLower( int c ){ return tolower( c );}

const std::string Mugen::lowercase( const std::string &str ){
    std::string tempStr = str;
    transform( tempStr.begin(), tempStr.end(), tempStr.begin(), toLower );
    return tempStr;
}

Mugen::PythonStream::PythonStream():
indentAmount(0){   
}

Mugen::PythonStream::~PythonStream(){
}

void Mugen::PythonStream::open(const std::string & file){
    stream.open(file.c_str());
}

void Mugen::PythonStream::close(){
    stream.close();
}

const Mugen::PythonStream & Mugen::PythonStream::operator<<(const IndentType & type){
    switch (type){
        case indent:
            indentAmount++;
            break;
        case unindent:
            if (indentAmount > 0){
                indentAmount--;
            }
            break;
        default:
            break;
    }
    
    return *this;
}

const Mugen::PythonStream & Mugen::PythonStream::operator<<(const StreamEnd & end){
    stream << addTab(indentAmount) << stringHolder.str() << std::endl;
    // Clear string holder and start a new
    stringHolder.str(std::string());
}


// Content
Mugen::Content::Content():
totalLines(0){
}
Mugen::Content::Content(unsigned int level, const std::string & str):
totalLines(1){
    indentLevel.push_back(level);
    content.push_back(str);
}

Mugen::Content::Content(const Content & copy):
totalLines(0){
    totalLines = copy.totalLines;
    indentLevel = copy.indentLevel;
    content = copy.content;
}
Mugen::Content::~Content(){
}

const Mugen::Content & Mugen::Content::operator=(const Mugen::Content & copy){
    totalLines = copy.totalLines;
    indentLevel = copy.indentLevel;
    content = copy.content;
    
    return *this;
}

void Mugen::Content::addLine(unsigned int level, const std::string & str){
    indentLevel.push_back(level);
    content.push_back(str);
    totalLines++;
}

void Mugen::Content::addSpace(){
    indentLevel.push_back(0);
    content.push_back("");
    totalLines++;
}

void Mugen::Content::output(Mugen::PythonStream & stream, unsigned int indentStart){
    for (unsigned int i = 0; i < totalLines; ++i){
        stream.setIndentLevel(indentStart + indentLevel[i]);
        stream << content[i] << PythonStream::endl;
    }
}

const unsigned int Mugen::Content::getIndentLevel() const{
    if (totalLines!=0){
        return indentLevel[0];
    }
    return 0;
}


// Python definition
Mugen::PythonDefinition::PythonDefinition(const Mugen::Content & content):
defLine(content){
}
Mugen::PythonDefinition::PythonDefinition(const Mugen::PythonDefinition & copy){
    defLine = copy.defLine;
    content = copy.content;
}
Mugen::PythonDefinition::~PythonDefinition(){
}
const Mugen::PythonDefinition & Mugen::PythonDefinition::operator=(const Mugen::PythonDefinition & copy){
    defLine = copy.defLine;
    content = copy.content;
    return *this;
}
void Mugen::PythonDefinition::addContent(const Mugen::Content & cont){
    content.push_back(cont);
}
void Mugen::PythonDefinition::addSpace(){
    content.push_back(Mugen::Content(0,""));
}
void Mugen::PythonDefinition::output(PythonStream & stream, unsigned int indentStart){
    defLine.output(stream, indentStart);
    stream.setIndentLevel(indentStart + defLine.getIndentLevel());
    for (std::vector<Mugen::Content>::iterator i = content.begin(); i != content.end(); ++i){
        Mugen::Content & cont = *i;
        cont.output(stream, indentStart + defLine.getIndentLevel());
    }
}

// Python class
Mugen::PythonClass::PythonClass(const Mugen::Content & content):
classLine(content),
init(Mugen::Content(1,"def __init__(self):")){
    init.addSpace();
    init.addContent(Mugen::Content(1,"# Initialize"));
    init.addContent(Mugen::Content(1,"mugen.Character.__init__(self)"));
}
Mugen::PythonClass::PythonClass(const Mugen::PythonClass & copy):
init(Mugen::Content(1,"def __init__(self):")){
    classLine = copy.classLine;
    init = copy.init;
    definitions = definitions;
}
Mugen::PythonClass::~PythonClass(){
}
const Mugen::PythonClass & Mugen::PythonClass::operator=(const Mugen::PythonClass & copy){
    classLine = copy.classLine;
    init = copy.init;
    definitions = definitions;
    return *this;
}
void Mugen::PythonClass::add(const Mugen::PythonDefinition & def){
    definitions.push_back(def);
}
void Mugen::PythonClass::output(PythonStream & stream, unsigned int indentStart){
    classLine.output(stream, indentStart);
    stream.setIndentLevel(indentStart + classLine.getIndentLevel());
    init.output(stream, indentStart + classLine.getIndentLevel());
    for (std::vector<Mugen::PythonDefinition>::iterator i = definitions.begin(); i != definitions.end(); ++i){
        Mugen::PythonDefinition & def = *i;
        def.output(stream, indentStart + classLine.getIndentLevel());
    }
}
