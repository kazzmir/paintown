#ifndef _paintown_ast_value_list_h_
#define _paintown_ast_value_list_h_

#include "Value.h"
#include <list>
#include <string>
#include <sstream>

namespace Ast{

class Walker;

class ValueList: public Value{
public:
    ValueList(const std::list<Value*> & values):
    values(values){
        current_value = this->values.begin();
    }
    
    virtual void walk(Walker & walker){
        walker.onValueList(*this);
    }

    using Value::operator>>;
    
    virtual const Value & operator>>(std::string & item) const {
        if (current_value == this->values.end()){
            throw Exception("No more values in this value list: " + toString());
        }
        Value * value = *current_value;
        *value >> item;
        current_value++;
        return *this;
    }

    virtual const Value & operator>>(int & item) const {
        if (current_value == this->values.end()){
            throw Exception("No more values in this value list: " + toString());
        }
        Value * value = *current_value;
        *value >> item;
        current_value++;
        return *this;
    }

    virtual const Value & operator>>(double & item) const {
        if (current_value == this->values.end()){
            throw Exception("No more values in this value list: " + toString());
        }
        Value * value = *current_value;
        *value >> item;
        current_value++;
        return *this;
    }

    virtual const Value & operator>>(bool & item) const {
        if (current_value == this->values.end()){
            throw Exception("No more values in this value list: " + toString());
        }
        Value * value = *current_value;
        *value >> item;
        current_value++;
        return *this;
    }


    /*
    virtual const Value & operator>>(std::string & str) const {
        str = toString();
        return *this;
    }
    */
    
    virtual std::string getType() const {
        return "list of values";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        bool first = true;
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            if (!first){
                out << ", ";
            } else {
                first = false;
            }
            Value * value = *it;
            if (value != 0){
                out << value->toString();
            }
        }
        return out.str();
    }
    
    virtual bool referenced(const void * value) const {
        if (Value::referenced(value)){
            return true;
        }
        
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            if ((*it)->referenced(value)){
                return true;
            }
        }

        return false;
    }

    virtual ~ValueList(){
        for (std::list<Value*>::iterator it = values.begin(); it != values.end(); it++){
            delete *it;
        }
    }

protected:
    std::list<Value*> values;
    mutable std::list<Value*>::iterator current_value;
};

}

#endif
