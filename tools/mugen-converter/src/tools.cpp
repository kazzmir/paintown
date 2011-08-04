#include "tools.h"
#include "ast/all.h"

using namespace std;

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