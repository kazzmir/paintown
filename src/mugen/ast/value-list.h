#ifndef _paintown_ast_value_list_h_
#define _paintown_ast_value_list_h_

#include "Value.h"
#include <list>
#include <string>
#include <sstream>
#include "util/token.h"
#include "util/token_exception.h"

namespace Ast{

class Walker;

class ValueList: public Value {
public:
    ValueList(const std::list<Value*> & values):
    values(values){
        current_value = this->values.begin();
    }

    ValueList(Value * value){
        values.push_back(value);
        current_value = this->values.begin();
    }
    
    virtual void walk(Walker & walker) const {
        walker.onValueList(*this);
    }
    
    virtual void reset() const {
        current_value = this->values.begin();
    }

    virtual bool hasMultiple() const {
        return true;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_VALUE_LIST;
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            const Value * value = *it;
            *token << value->serialize();
        }
        return token;
    }

    static ValueList * deserialize(const Token * token){
        std::list<Value*> values;
        try{
            TokenView view = token->view();
            while (true){
                const Token * next;
                view >> next;
                values.push_back(Value::deserialize(next));
            }
        } catch (const TokenException & e){
        }
        return new ValueList(values);
    }

    virtual Value * get(unsigned int index) const {
        if (index < values.size()){
            unsigned int count = 0;
            std::list<Value*>::const_iterator it;
            for (it = values.begin(); count < index && it != values.end(); it++, count++){
            }
            return *it;
        }
        return 0;
    }
    
    virtual Element * copy() const {
        std::list<Value*> copied;
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            const Value * value = *it;
            copied.push_back((Value*) value->copy());
        }
        return new ValueList(copied);
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const ValueList & him) const {
        std::list<Value*>::const_iterator my_it = values.begin();
        std::list<Value*>::const_iterator him_it = him.values.begin();
        while (true){
            if (my_it == values.end() || him_it == him.values.end()){
                break;
            }

            if (**my_it != **him_it){
                return false;
            }

            my_it++;
            him_it++;
        }

        return my_it == values.end() && him_it == him.values.end();
    }

    using Value::operator>>;
    
    virtual const Value & operator>>(const Value *& value) const {
        if (current_value == this->values.end()){
            throw Exception("No more values in this value list: " + toString());
        }
        value = *current_value;
        current_value++;
        return *this;
    }
    
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

    virtual const Value & operator>>(std::vector<int> & itemList) const {
        while (current_value != this->values.end()){
            int item;
            Value * value = *current_value;
            *value >> item;
            itemList.push_back(item);
            current_value++;
        }
        return *this;
    }

    virtual const Value & operator>>(std::vector<double> & itemList) const {
        while (current_value != this->values.end()){
            double item;
            Value * value = *current_value;
            *value >> item;
            itemList.push_back(item);
            current_value++;
        }
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
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            Value * value = *it;
            value->mark(marks);
        }
    }

    virtual ~ValueList(){
        for (std::list<Value*>::iterator it = values.begin(); it != values.end(); it++){
            delete *it;
        }
    }

protected:
    std::list<Value*> values;
    mutable std::list<Value*>::const_iterator current_value;
};

}

#endif
