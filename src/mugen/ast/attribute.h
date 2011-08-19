#ifndef _paintown_attribute_h
#define _paintown_attribute_h

#include "walker.h"
#include <map>
#include <string>
#include "ast.h"
#include "exception.h"
#include "util/token.h"

namespace Ast{

class Attribute: public Element {
public:
    enum Kind{
        None = 0,
        Simple = 1,
        /* to avoid name clash with the Keyword class */
        KeywordAttribute = 2,
        Array = 3,
    };

    Attribute(int line, int column, Kind kind):
    Element(line, column),
    kind(kind){
    }
    
    virtual void walk(Walker & walker){
        walker.onAttribute(*this);
    }

    static Attribute * deserialize(const Token * token);

    using Element::operator==;
    virtual bool operator==(const std::string & str) const = 0;
    
    virtual Kind getKind(){
        return kind;
    }

    virtual void debugExplain(){
    }

    virtual std::string toString() const = 0;

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
    }

    /*
    virtual bool referenced(const void * value) const {
        return value == this;
    }
    */

    virtual ~Attribute(){
    }
protected:
    Kind kind;
};

}

#endif
