#include <iostream>
#include <string>
#include <list>
#include "mugen/parser/all.h"
#include "mugen/ast/all.h"

using namespace std;

namespace Util{
string trim(const std::string & str){
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
}

/* parses a cmd/def/cns/air file and spits out the parse tree */

bool hasExtension(const string & file, const string & extension){
    return file.find(extension) != string::npos;
}

bool isCmd(const string & file){
    return hasExtension(file, ".cmd") || hasExtension(file, ".cns");
}

bool isDef(const string & file){
    return hasExtension(file, ".def");
}

bool isAir(const string & file){
    return hasExtension(file, ".air");
}

void destroy(list<Ast::Section*> * sections){
    for (list<Ast::Section*>::iterator it = sections->begin(); it != sections->end(); it++){
        delete (*it);
    }
    delete sections;
}

void doParse(const string & file, const void * (*parse)(const std::string &, bool stats)){
    try{
        list<Ast::Section*> * result = (list<Ast::Section*>*) parse(file, false);
        for (list<Ast::Section*>::iterator it = result->begin(); it != result->end(); it++){
            Ast::Section * section = *it;
            cout << section->toString() << endl;
        }
        destroy(result);
    } catch (const Mugen::Cmd::ParseException & fail){
        cout << "failed to parse: " << fail.getReason() << endl;
    }
}

void parseCmd(const string & file){
    doParse(file, Mugen::Cmd::parse);
}

void parseDef(const string & file){
    doParse(file, Mugen::Def::parse);
}

void parseAir(const string & file){
    doParse(file, Mugen::Air::parse);
}

void parse(const char * file){
    if (isCmd(file)){
        parseCmd(file);
    } else if (isDef(file)){
        parseDef(file);
    } else if (isAir(file)){
        parseAir(file);
    } else {
        cout << "Don't know how to parse " << file << endl;
    }
}

int main(int argc, char ** argv){
    if (argc > 1){
        parse(argv[1]);
    } else {
        cout << "Give an argument (foo.cns, foo.def, foo.air, or foo.cmd)" << endl;
    }
}
