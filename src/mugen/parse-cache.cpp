#include <list>
#include <string>
#include <exception>
#include "parse-cache.h"
#include "parser/all.h"
#include "ast/all.h"
#include "ast/extra.h"
#include "globals.h"
#include "util/file-system.h"
#include "util/system.h"

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
    if (cache != NULL){
        Global::debug(0) << "Only one parse cache is allowed" << endl;
        throw exception();
    }
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

static int replaceSlash(int what){
    if (what == '/'){
        return '-';
    }

    return what;
}

const char * MUGEN_CACHE = "mugen-cache";

static list<Ast::Section*> * loadCached(const string & path){
    string converted = path;
    std::transform(converted.begin(), converted.end(), converted.begin(), replaceSlash);
    Filesystem::AbsolutePath fullPath = Filesystem::userDirectory().join(Filesystem::RelativePath(MUGEN_CACHE)).join(Filesystem::RelativePath(converted));
    TokenReader reader;
    Global::debug(1, "mugen-parse-cache") << "Loading from " << fullPath.path() << endl;
    return Ast::AstParse::deserialize(reader.readTokenFromFile(fullPath.path().c_str()));
}

static void saveCached(list<Ast::Section*> * parse, const string & path){
    string converted = path;
    std::transform(converted.begin(), converted.end(), converted.begin(), replaceSlash);
    Filesystem::AbsolutePath cache = Filesystem::userDirectory().join(Filesystem::RelativePath(MUGEN_CACHE));

    if (!System::isDirectory(cache.path())){
        System::makeDirectory(cache.path());
    }

    Filesystem::AbsolutePath fullPath = cache.join(Filesystem::RelativePath(converted));
    Global::debug(1, "mugen-parse-cache") << "Saving cache to " << fullPath.path() << endl;
    ofstream out(fullPath.path().c_str());
    Token * serial = Ast::AstParse::serialize(parse);
    serial->toStringCompact(out);
    out << endl;
    out.close();
    delete serial;
}

list<Ast::Section*> * loadFile(const string & path){
    try{
        return loadCached(path);
    } catch (const TokenException & fail){
    }

    list<Ast::Section*> * out = reallyParseCmd(path);
    try{
        saveCached(out, path);
    } catch (...){
        /* failed for some reason */
    }

    return out;
}

list<Ast::Section*> * ParseCache::doParseCmd(const std::string & path){
    if (cmdCache[path] == NULL){
        cmdCache[path] = loadFile(path);
    }
    return copy(cmdCache[path]);
}

}
