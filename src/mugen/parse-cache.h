#ifndef _paintown_mugen_parse_cache_h
#define _paintown_mugen_parse_cache_h

#include <list>
#include <string>
#include <map>
#include "util/thread.h"

class OptionMugenMenu;

namespace PaintownUtil = ::Util;

namespace Ast{
    class Section;
}

int main(int argc, char ** argv);

namespace Mugen{

class Parser{
public:
    Parser();

    virtual ~Parser();

    std::list<Ast::Section*> * parse(const std::string & path);

    void destroy();

protected:
    virtual std::list<Ast::Section*> * doParse(const std::string & path) = 0;
    std::list<Ast::Section*> * loadFile(const std::string & path);

    std::map<const std::string, std::list<Ast::Section*>*> cache;
    PaintownUtil::Thread::LockObject lock;
};

class CmdCache: public Parser {
public:
    CmdCache();
    virtual ~CmdCache();
protected:
    virtual std::list<Ast::Section*> * doParse(const std::string & path);
};

class AirCache: public Parser {
public:
    AirCache();
    virtual ~AirCache();

protected:
    virtual std::list<Ast::Section*> * doParse(const std::string & path);
};

class DefCache: public Parser {
public:
    DefCache();
    virtual ~DefCache();
protected:
    virtual std::list<Ast::Section*> * doParse(const std::string & path);
};

class ParseCache{
public:
    ParseCache();
    ~ParseCache();

    /* pass in the full path to the file */
    static std::list<Ast::Section*> * parseCmd(const std::string & path);
    static std::list<Ast::Section*> * parseAir(const std::string & path);
    static std::list<Ast::Section*> * parseDef(const std::string & path);

    /* clear the cache */
    static void destroy();
protected:

    std::list<Ast::Section*> * doParseCmd(const std::string & path);
    std::list<Ast::Section*> * doParseAir(const std::string & path);
    std::list<Ast::Section*> * doParseDef(const std::string & path);
    void destroyCache();

    static ParseCache * cache;

    CmdCache cmdCache;
    AirCache airCache;
    DefCache defCache;
};

}

#endif
