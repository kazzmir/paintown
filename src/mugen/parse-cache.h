#ifndef _paintown_mugen_parse_cache_h
#define _paintown_mugen_parse_cache_h

#include <list>
#include <string>
#include <map>

class OptionMugenMenu;

namespace Ast{
    class Section;
}

namespace Mugen{

class ParseCache{
public:
    /* pass in the full path to the file */
    std::list<Ast::Section*> * parseCmd(const std::string & path);

protected:
    friend class ::OptionMugenMenu;
    ParseCache();
    ~ParseCache();

    std::list<Ast::Section*> * doParseCmd(const std::string & path);
    void destroySectionList(std::list<Ast::Section*> * sections);

    static ParseCache * cache;
    std::map<const std::string, std::list<Ast::Section*>* > cmdCache;
};

}

#endif
