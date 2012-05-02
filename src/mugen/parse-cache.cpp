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
#include "util/debug.h"

using namespace std;

typedef PaintownUtil::ReferenceCount<Ast::AstParse> AstRef;

namespace Mugen{

static int replaceSlash(int what){
    if (what == '/'){
        return '-';
    }

    if (what == '\\'){
        return '-';
    }

    return what;
}

const char * MUGEN_CACHE = "mugen-cache";

/* true if path1 has a newer modification time than path2 */
static bool newer(const Filesystem::AbsolutePath & path1, const Filesystem::AbsolutePath & path2){
    return System::getModificationTime(path1.path()) >= System::getModificationTime(path2.path());
}

static AstRef loadCached(const Filesystem::AbsolutePath & path){
    if (!Storage::instance().exists(path)){
        throw MugenException("Original file does not exist", __FILE__, __LINE__);
    }
    string converted = Storage::instance().cleanse(path).path();
    std::transform(converted.begin(), converted.end(), converted.begin(), replaceSlash);
    Filesystem::AbsolutePath fullPath = Storage::instance().userDirectory().join(Filesystem::RelativePath(MUGEN_CACHE)).join(Filesystem::RelativePath(converted));
    if (!newer(fullPath, path)){
        throw MugenException("File is old", __FILE__, __LINE__);
    }
    TokenReader reader;
    Global::debug(1, "mugen-parse-cache") << "Loading from cache " << fullPath.path() << endl;
    return PaintownUtil::ReferenceCount<Ast::AstParse>(new Ast::AstParse(reader.readTokenFromFile(fullPath.path().c_str())));
}

static void saveParse(const Filesystem::AbsolutePath & path, const PaintownUtil::ReferenceCount<Ast::AstParse> & parse){
    ofstream out(path.path().c_str());
    Token * serial = parse->serialize();
    serial->toStringCompact(out);
    out << endl;
    out.close();
    delete serial;
}

static void saveCached(const Util::ReferenceCount<Ast::AstParse> & parse, const Filesystem::AbsolutePath & path){
    string converted = Storage::instance().cleanse(path).path();
    std::transform(converted.begin(), converted.end(), converted.begin(), replaceSlash);
    Filesystem::AbsolutePath cache = Storage::instance().userDirectory().join(Filesystem::RelativePath(MUGEN_CACHE));

    if (!System::isDirectory(cache.path())){
        /* like mkdir -p */
        System::makeAllDirectory(cache.path());
    }

    Filesystem::AbsolutePath fullPath = cache.join(Filesystem::RelativePath(converted));
    Global::debug(1, "mugen-parse-cache") << "Saving cache to " << fullPath.path() << endl;
    saveParse(fullPath, parse);
}

/* attempts to load from a file on disk. if the file doesn't exist then
 * parse it for real and save it to disk.
 */
Util::ReferenceCount<Ast::AstParse> Parser::loadFile(const Filesystem::AbsolutePath & path){
    try{
        return loadCached(path);
    } catch (const TokenException & fail){
    } catch (const MugenException & e){
    }

    Global::debug(1, "mugen-parse-cache") << "Parsing " << path.path() << endl;
    Util::ReferenceCount<Ast::AstParse> out = doParse(path);
    try{
        saveCached(out, path);
    } catch (...){
        Global::debug(0) << "Failed to save cached file " << path.path() << endl;
        /* failed for some reason */
    }

    return out;
}

void Parser::destroy(){
    cache.clear();
}

Parser::Parser(){
}

Parser::~Parser(){
    destroy();
}

/*
static list<Ast::Section*> * copy(list<Ast::Section*> * input){
    list<Ast::Section*> * output = new list<Ast::Section*>();

    for (list<Ast::Section*>::iterator it = input->begin(); it != input->end(); it++){
        output->push_back((Ast::Section*) (*it)->copy());
    }

    return output;
}
*/

/* check for an in-memory copy of the parse. if it doesnt exist
 * then either load it from disk or parse it.
 * returns a new copy of the AST so you must delete it later.
 */
Util::ReferenceCount<Ast::AstParse> Parser::parse(const Filesystem::AbsolutePath & path){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (cache[path] == NULL){
        cache[path] = loadFile(path);
    }

    return cache[path];
}

CmdCache::CmdCache(){
}

CmdCache::~CmdCache(){
}
    
AirCache::AirCache(){
}

AirCache::~AirCache(){
}

DefCache::DefCache(){
}

DefCache::~DefCache(){
}

static list<Ast::Section*> * reallyParseCmd(const Filesystem::AbsolutePath & path){
    return (list<Ast::Section*>*) Cmd::parse(path.path());
}

static list<Ast::Section*> * reallyParseAir(const Filesystem::AbsolutePath & path){
    return (list<Ast::Section*>*) Air::parse(path.path());
}

static list<Ast::Section*> * reallyParseDef(const Filesystem::AbsolutePath & path){
    return (list<Ast::Section*>*) Def::parse(path.path());
}

Util::ReferenceCount<Ast::AstParse> CmdCache::doParse(const Filesystem::AbsolutePath & path){
    return Util::ReferenceCount<Ast::AstParse>(new Ast::AstParse(reallyParseCmd(path)));
}

Util::ReferenceCount<Ast::AstParse> AirCache::doParse(const Filesystem::AbsolutePath & path){
    return Util::ReferenceCount<Ast::AstParse>(new Ast::AstParse(reallyParseAir(path)));
}

Util::ReferenceCount<Ast::AstParse> DefCache::doParse(const Filesystem::AbsolutePath & path){
    return Util::ReferenceCount<Ast::AstParse>(new Ast::AstParse(reallyParseDef(path)));
}

ParseCache * ParseCache::cache = NULL;

Util::ReferenceCount<Ast::AstParse> ParseCache::parseCmd(const Filesystem::AbsolutePath & path){
    if (cache == NULL){
        return Util::ReferenceCount<Ast::AstParse>(new Ast::AstParse(reallyParseCmd(path)));
    }
    return cache->doParseCmd(path);
}
    
Util::ReferenceCount<Ast::AstParse> ParseCache::parseAir(const Filesystem::AbsolutePath & path){
    if (cache == NULL){
        return Util::ReferenceCount<Ast::AstParse>(new Ast::AstParse(reallyParseAir(path)));
    }
    return cache->doParseAir(path);
}

Util::ReferenceCount<Ast::AstParse> ParseCache::parseDef(const Filesystem::AbsolutePath & path){
    if (cache == NULL){
        return Util::ReferenceCount<Ast::AstParse>(new Ast::AstParse(reallyParseDef(path)));
    }
    return cache->doParseDef(path);
}

void ParseCache::destroy(){
    if (cache){
        cache->destroyCache();
    }
}

ParseCache::ParseCache(){
    if (cache != NULL){
        Global::debug(0) << "Only one parse cache is allowed" << endl;
        throw exception();
    }
    cache = this;
}

void ParseCache::destroyCache(){
    cmdCache.destroy();
    airCache.destroy();
    defCache.destroy();
}

ParseCache::~ParseCache(){
    cache = NULL;
}

Util::ReferenceCount<Ast::AstParse> ParseCache::doParseCmd(const Filesystem::AbsolutePath & path){
    return cmdCache.parse(path);
}
    
Util::ReferenceCount<Ast::AstParse> ParseCache::doParseAir(const Filesystem::AbsolutePath & path){
    return airCache.parse(path);
}

Util::ReferenceCount<Ast::AstParse> ParseCache::doParseDef(const Filesystem::AbsolutePath & path){
    return defCache.parse(path);
}

}
