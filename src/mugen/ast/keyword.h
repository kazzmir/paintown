#ifndef _paintown_ast_keyword_h_
#define _paintown_ast_keyword_h_

#include "Value.h"
#include <string>

namespace Ast{

class Keyword: public Value{
public:
    /* do not pass in a dynamically allocated string!!! */
    Keyword(const char * str):
    str(str){
    }

    using Value::operator>>;

    virtual const Value & operator>>(std::string & str) const {
        str = this->str;
        return *this;
    }
    
    virtual void walk(Walker & walker) const {
        walker.onKeyword(*this);
    }
    
    virtual Element * copy() const {
        return new Keyword(str);
    }
    
    using Element::operator==;
    bool operator==(const std::string & str) const {
        return downcase(toString()) == downcase(str);
    }

    static int lowerCase( int c ){
        return tolower(c);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << "keyword" << toString();
        return token;
    }

    static std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
    }
    
    virtual std::string getType() const {
        return "string";
    }

    virtual std::string toString() const {
        return std::string(str);
    }
    
    virtual ~Keyword(){
        /* don't delete str */
    }

protected:
    const char * str;
};

}

#endif
