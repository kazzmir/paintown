#ifndef _paintown_ast_extra_h
#define _paintown_ast_extra_h

#include <list>
#include <string>
#include <algorithm>
#include <string.h>
#include "Section.h"
#include "exception.h"
#include "util/token.h"

namespace Ast{

/* output from the def parser. I think all mugen files are just lists
 * of sections so we can probably rename this to AstResult or something
 * like that and make it more generic.
 */
class AstParse{
public:
    /* boiler plate stuff */

    AstParse(std::list<Section*> * sections):
    sections(sections){
    }

    typedef std::list<Section*>::iterator section_iterator;

    virtual ~AstParse(){
        for (std::list<Section*>::iterator section_it = sections->begin(); section_it != sections->end(); section_it++){
            delete (*section_it);
        }
        delete sections;
    }

public:
    /* useful stuff */

    inline std::list<Section*> * getSections() const {
        return sections;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << "mugen";
        for (std::list<Section*>::const_iterator section_it = sections->begin(); section_it != sections->end(); section_it++){
            Section * section = *section_it;
            *token << section->serialize();
        }
        return token;
    }

    static int lowerCase( int c ){ return tolower(c); }

    std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
    }

    Section * findSection(const std::string & find){
        for (std::list<Section*>::iterator section_it = sections->begin(); section_it != sections->end(); section_it++){
            Section * section = *section_it;
            if (downcase(section->getName()) == downcase(find)){
                return section;
            }
        }
        throw Exception("Could not find section " + find);
    }

protected:

    std::list<Section*> * sections;
};

}

#endif
