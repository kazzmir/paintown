#ifndef _paintown_ast_hitdef_h_
#define _paintown_ast_hitdef_h_

#include "Value.h"
#include <string>
#include <sstream>

namespace Ast{

class HitDefAttribute: public Value {
public:
    HitDefAttribute(const std::string & value):
    value(value){
    }

    using Value::operator>>;

    virtual const Value & operator>>(std::string & str) const {
        str = value;
        return *this;
    }

    virtual void walk(Walker & walker) const {
        walker.onHitDefAttribute(*this);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << "hitdef-attribute" << value;
        return token;
    }
    
    virtual Element * copy() const {
        return new HitDefAttribute(value);
    }
          
    virtual std::string getType() const {
        return "hitdef attribute";
    }

    virtual std::string toString() const {
        return value;
    }

    virtual ~HitDefAttribute(){
    }

protected:
    std::string value;
};

class HitDefAttackAttribute: public Value {
public:
    HitDefAttackAttribute(){
    }

    using Value::operator>>;

    virtual const Value & operator>>(std::string & str) const {
        if (current != values.end()){
            str = *current;
            current++;
        } else {
            throw Exception("No more values in this hitdef attack attribute: " + toString());
        }
        return *this;
    }

    virtual void walk(Walker & walker) const {
        walker.onHitDefAttackAttribute(*this);
    }

    virtual void addAttribute(const std::string & value){
        values.push_back(value);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << "hitdef-attack-attribute";
        for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++){
            *token << *it;
        }
        return token;
    }

    virtual void reset() const {
        current = values.begin();
    }
    
    virtual Element * copy() const {
        HitDefAttackAttribute * attribute = new HitDefAttackAttribute();
        for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++){
            attribute->addAttribute(*it);
        }
        attribute->reset();
        return attribute;
    }
          
    virtual std::string getType() const {
        return "hitdef attack attribute";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        bool first = true;
        for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); it++){
            if (!first){
                out << ", ";
            }
            out << *it;
            first = false;
        }
        return out.str();
    }

    virtual ~HitDefAttackAttribute(){
    }

protected:
    std::vector<std::string> values;
    mutable std::vector<std::string>::const_iterator current;
};

}

#endif
