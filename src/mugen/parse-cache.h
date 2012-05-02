#ifndef _paintown_mugen_parse_cache_h
#define _paintown_mugen_parse_cache_h

#include <string>
#include <map>
#include "util/thread.h"
#include "util/pointer.h"
#include "util/file-system.h"
#include "ast/extra.h"

class OptionMugenMenu;

namespace PaintownUtil = ::Util;

int main(int argc, char ** argv);

namespace Mugen{

class Parser{
public:
    Parser();

    virtual ~Parser();

    PaintownUtil::ReferenceCount<Ast::AstParse> parse(const Filesystem::AbsolutePath & path);

    void destroy();

protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const Filesystem::AbsolutePath & path) = 0;
    PaintownUtil::ReferenceCount<Ast::AstParse> loadFile(const Filesystem::AbsolutePath & path);

    std::map<const Filesystem::AbsolutePath, PaintownUtil::ReferenceCount<Ast::AstParse> > cache;
    PaintownUtil::Thread::LockObject lock;
};

class CmdCache: public Parser {
public:
    CmdCache();
    virtual ~CmdCache();
protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const Filesystem::AbsolutePath & path);
};

class AirCache: public Parser {
public:
    AirCache();
    virtual ~AirCache();

protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const Filesystem::AbsolutePath & path);
};

class DefCache: public Parser {
public:
    DefCache();
    virtual ~DefCache();
protected:
    virtual PaintownUtil::ReferenceCount<Ast::AstParse> doParse(const Filesystem::AbsolutePath & path);
};

class ParseCache{
public:
    ParseCache();
    ~ParseCache();

    /* pass in the full path to the file */
    static PaintownUtil::ReferenceCount<Ast::AstParse> parseCmd(const Filesystem::AbsolutePath & path);
    static PaintownUtil::ReferenceCount<Ast::AstParse> parseAir(const Filesystem::AbsolutePath & path);
    static PaintownUtil::ReferenceCount<Ast::AstParse> parseDef(const Filesystem::AbsolutePath & path);

    /* clear the cache */
    static void destroy();
protected:

    PaintownUtil::ReferenceCount<Ast::AstParse> doParseCmd(const Filesystem::AbsolutePath & path);
    PaintownUtil::ReferenceCount<Ast::AstParse> doParseAir(const Filesystem::AbsolutePath & path);
    PaintownUtil::ReferenceCount<Ast::AstParse> doParseDef(const Filesystem::AbsolutePath & path);
    void destroyCache();

    static ParseCache * cache;

    CmdCache cmdCache;
    AirCache airCache;
    DefCache defCache;
};

}

#endif
