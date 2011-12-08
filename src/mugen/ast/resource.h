#ifndef _paintown_ast_resource_h_
#define _paintown_ast_resource_h_

#include "Value.h"
#include <string>
#include <sstream>
#include <math.h>

/* This class deals with sounds/animations that might come from either the
 * player's own sff files or fightfx.
 *
 * f800 = fightfx.sff
 * s800 = players own sff file
 */

namespace Ast{

class Resource: public Value {
public:
    Resource(int line, int column, int value, bool fightfx, bool own):
    Value(line, column),
    value(value),
    fightfx(fightfx),
    own(own){
    }

    class ResourceView: public ViewImplementation {
    public:
        ResourceView(const Resource * owner):
        owner(owner){
        }

        const Resource * owner;

        virtual std::string getType() const {
            return owner->getType();
        }
    
        virtual const Value * self() const {
            return owner;
        }
        
        virtual std::string toString() const {
            return owner->toString();
        }

        using ViewImplementation::operator>>;
        virtual ResourceView & operator>>(const Value *& v){
            v = owner;
            return *this;
        }

        virtual ResourceView & operator>>(int & x){
            x = owner->value;
            return *this;
        }
    };

    using Value::view;
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new ResourceView(this)));
    }

    virtual void walk(Walker & walker) const {
        walker.onResource(*this);
    }
    
    virtual Element * copy() const {
        return new Resource(getLine(), getColumn(), value, fightfx, own);
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const Resource & him) const {
        return value == him.value &&
               fightfx == him.fightfx &&
               own == him.own;
    }
   
    static Resource * deserialize(const Token * token){
        double value;
        int line, column;
        bool fightfx, own;
        token->view() >> line >> column >> value >> fightfx >> own;
        return new Resource(line, column, value, fightfx, own);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_RESOURCE << getLine() << getColumn() << value << fightfx << own;
        return token;
    }

    /* if it starts with 'f' */
    bool isFight() const {
        return fightfx;
    }

    /* if it starts with 's' */
    bool isOwn() const {
        return own;
    }

    virtual std::string getType() const {
        return "resource";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        if (fightfx){
            out << "f";
        }
        if (own){
            out << "s";
        }
        out << value;
        return out.str();
    }

    virtual ~Resource(){
    }

protected:
    int value;
    bool fightfx;
    bool own;
};

}

#endif
