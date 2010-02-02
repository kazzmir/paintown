#include <list>
#include <string>
#include <exception>
#include "parse-cache.h"
#include "parser/all.h"
#include "ast/all.h"
#include "globals.h"

using namespace std;

namespace Mugen{

ParseCache * ParseCache::cache = 0;

list<Ast::Section*> * ParseCache::parseCmd(const string & path){
    if (cache == NULL){
        Global::debug(0) << "Error: accessing cache without instantiating it" << endl;
        throw exception();
    }
    return cache->doParseCmd(path);
}

ParseCache::ParseCache(){
    cache = this;
}

void ParseCache::destroySectionList(list<Ast::Section*> * sections){
    for (list<Ast::Section*>::iterator section_it = sections->begin(); section_it != sections->end(); section_it++){
        delete (*section_it);
    }
    delete sections;
}

ParseCache::~ParseCache(){
    cache = NULL;

    for (map<const string, list<Ast::Section*>* >::iterator it = cmdCache.begin(); it != cmdCache.end(); it++){
        destroySectionList((*it).second);
    }
}

static list<Ast::Section*> * reallyParseCmd(const std::string & path){
    return (list<Ast::Section*>*) Cmd::main(path);
}

static list<Ast::Section*> * copy(list<Ast::Section*> * input){
    list<Ast::Section*> * output = new list<Ast::Section*>();

    for (list<Ast::Section*>::iterator it = input->begin(); it != input->end(); it++){
        output->push_back((Ast::Section*) (*it)->copy());
    }

    return output;
}

list<Ast::Section*> * ParseCache::doParseCmd(const std::string & path){
    if (cmdCache[path] == NULL){
        cmdCache[path] = reallyParseCmd(path);
    }
    return copy(cmdCache[path]);
}

}
