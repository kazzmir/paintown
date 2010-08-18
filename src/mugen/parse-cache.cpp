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

static list<Ast::Section*> * reallyParseCmd(const std::string & path){
    return (list<Ast::Section*>*) Cmd::parse(path);
}

list<Ast::Section*> * ParseCache::parseCmd(const string & path){
    if (cache == NULL){
        return reallyParseCmd(path);
        /*
        Global::debug(0) << "Error: accessing cache without instantiating it" << endl;
        throw exception();
        */
    }
    return cache->doParseCmd(path);
}

void ParseCache::destroy(){
    cache->destroyCache();
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

void ParseCache::destroyCache(){
    for (map<const string, list<Ast::Section*>* >::iterator it = cmdCache.begin(); it != cmdCache.end(); it++){
        list<Ast::Section*> * sections = (*it).second;
        if (sections != NULL){
            destroySectionList(sections);
        }
    }
    cmdCache.clear();
}

ParseCache::~ParseCache(){
    cache = NULL;
    destroyCache();
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
