#ifndef _paintown_ast_keyword_h_
#define _paintown_ast_keyword_h_

#include "Value.h"
#include <string>

namespace Ast{

class Keyword: public Value {
public:
    /* do not pass in a dynamically allocated string!!! */
    Keyword(int line, int column, std::string str):
    Value(line, column),
    str(str){
    }

    class KeywordView: public ViewImplementation {
    public:
        KeywordView(const Keyword * keyword):
        keyword(keyword){
        }

        const Keyword * keyword;

        using ViewImplementation::operator>>;
        virtual const KeywordView & operator>>(std::string & str) const {
            str = keyword->str;
            return *this;
        }

        virtual std::string getType() const {
            return keyword->getType();
        }
        
        virtual const Value * self() const {
            return keyword;
        }
        
        virtual std::string toString() const {
            return keyword->toString();
        }
    };

    using Value::view;
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new KeywordView(this)));
    }
    
    virtual void walk(Walker & walker) const {
        walker.onKeyword(*this);
    }
    
    virtual Element * copy() const {
        return new Keyword(getLine(), getColumn(), str);
    }
    
    using Element::operator==;
    bool operator==(const Value & him) const {
        return him == *this;
    }

    bool operator==(const Keyword & him) const {
        return str == him.str;
    }

    bool operator==(const std::string & str) const {
        return downcase(toString()) == downcase(str);
    }

    static int lowerCase( int c ){
        return tolower(c);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_KEYWORD << getLine() << getColumn() << toString();
        return token;
    }

    static Keyword * deserialize(const Token * token){
        std::string name;
        int line, column;
        token->view() >> line >> column >> name;
        return new Keyword(line, column, name);
    }

    static std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
    }
    
    virtual std::string getType() const {
        return "string";
    }

    virtual std::string toString() const {
        return str;
    }
    
    virtual ~Keyword(){
        /* don't delete str */
    }

protected:
    std::string str;
};

}

#endif
