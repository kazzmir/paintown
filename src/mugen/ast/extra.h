#ifndef _paintown_ast_extra_h
#define _paintown_ast_extra_h

#include <list>
#include <string>
#include <algorithm>
#include <string.h>
#include "Section.h"
#include "exception.h"
#include "util/token.h"
#include "util/tokenreader.h"

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

    AstParse(Token * token):
    sections(NULL){
        checkVersion(token);
        std::vector<Token*> tokens = token->findTokens("mugen/section-list");
        sections = new std::list<Section*>();
        for (std::vector<Token*>::iterator it = tokens.begin(); it != tokens.end(); it++){
            sections->push_back(Section::deserialize(*it));
        }
    }

    void checkVersion(Token * token){
        Token * version = token->findToken("mugen/version");
        if (version != NULL){
            int what;
            *version >> what;
            if (what != Element::SERIAL_VERSION){
                throw Exception("Mistmatch between serial versions");
            }
        } else {
            throw Exception("No version specified in the serialized stream");
        }
    }

    typedef std::list<Section*>::iterator section_iterator;

    bool operator!=(const AstParse & him) const {
        return !(*this == him);
    }

    bool operator==(const AstParse & him) const {
        if (sections == NULL && him.sections != NULL){
            return false;
        }

        if (sections != NULL && him.sections == NULL){
            return false;
        }

        std::list<Section*>::const_iterator my_it = sections->begin();
        std::list<Section*>::const_iterator him_it = him.sections->begin();
        while (true){
            if (my_it == sections->end()){
                break;
            }

            if (him_it == him.sections->end()){
                break;
            }

            const Section * mine = *my_it;
            const Section * him = *him_it;
            if (*mine != *him){
                return false;
            }

            my_it++;
            him_it++;
        }

        return my_it == sections->end() && him_it == him.sections->end();
    }

    virtual ~AstParse(){
        if (sections != NULL){
            for (std::list<Section*>::iterator section_it = sections->begin(); section_it != sections->end(); section_it++){
                delete (*section_it);
            }
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
        *(token->newToken()) << "version" << Element::SERIAL_VERSION;
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
