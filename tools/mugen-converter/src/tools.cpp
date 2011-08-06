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