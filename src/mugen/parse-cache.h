#ifndef _paintown_mugen_parse_cache_h
#define _paintown_mugen_parse_cache_h

#include <list>
#include <string>
#include <map>

class OptionMugenMenu;

namespace Ast{
    class Section;
}

int main(int argc, char ** argv);

namespace Mugen{

class ParseCache{
public:
    /* pass in the full path to the file */
    static std::list<Ast::Section*> * parseCmd(const std::string & path);

protected:
    friend class ::OptionMugenMenu;
    friend int ::main(int argc, char ** argv);
    ParseCache();
    ~ParseCache();

    std::list<Ast::Section*> * doParseCmd(const std::string & path);
    void destroySectionList(std::list<Ast::Section*> * sections);

    static ParseCache * cache;
    std::map<const std::string, std::list<Ast::Section*>* > cmdCache;
};

}

#endif
