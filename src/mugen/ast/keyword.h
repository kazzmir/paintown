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
    
    virtual Element * copy() const {
        throw Exception("Copy not implemented yet");
    }
    
    bool operator==(const std::string & str) const {
        return downcase(toString()) == downcase(str);
    }

    static int lowerCase( int c ){
        return tolower(c);
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
