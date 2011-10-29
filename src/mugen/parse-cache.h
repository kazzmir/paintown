#ifndef _paintown_mugen_parse_cache_h
#define _paintown_mugen_parse_cache_h

#include <string>
#include <map>
#include "util/thread.h"
#include "util/pointer.h"
#include "ast/extra.h"

class OptionMugenMenu;

namespace PaintownUtil = ::Util;

int main(int argc, char ** argv);

namespace Mugen{

class Parser{
public:
    Parser();

    virtual ~Parser();

    PaintownUtil::ReferenceCount<Ast::AstParse> parse(const std::string & path);

    void destroy();

protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const std::string & path) = 0;
    PaintownUtil::ReferenceCount<Ast::AstParse> loadFile(const std::string & path);

    std::map<const std::string, PaintownUtil::ReferenceCount<Ast::AstParse> > cache;
    PaintownUtil::Thread::LockObject lock;
};

class CmdCache: public Parser {
public:
    CmdCache();
    virtual ~CmdCache();
protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const std::string & path);
};

class AirCache: public Parser {
public:
    AirCache();
    virtual ~AirCache();

protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const std::string & path);
};

class DefCache: public Parser {
public:
    DefCache();
    virtual ~DefCache();
protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const std::string & path);
};

class ParseCache{
public:
    ParseCache();
    ~ParseCache();

    /* pass in the full path to the file */
    static PaintownUtil::ReferenceCount<Ast::AstParse> parseCmd(const std::string & path);
    static PaintownUtil::ReferenceCount<Ast::AstParse> parseAir(const std::string & path);
    static PaintownUtil::ReferenceCount<Ast::AstParse> parseDef(const std::string & path);

    /* clear the cache */
    static void destroy();
protected:

    PaintownUtil::ReferenceCount<Ast::AstParse> doParseCmd(const std::string & path);
    PaintownUtil::ReferenceCount<Ast::AstParse> doParseAir(const std::string & path);
    PaintownUtil::ReferenceCount<Ast::AstParse> doParseDef(const std::string & path);
    void destroyCache();

    static ParseCache * cache;

    CmdCache cmdCache;
    AirCache airCache;
    DefCache defCache;
};

}

#endif
