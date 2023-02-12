#ifndef _paintown_ast_value_list_h_
#define _paintown_ast_value_list_h_

#include "Value.h"
#include <list>
#include <string>
#include <sstream>
#include "r-tech1/token.h"
#include "r-tech1/token_exception.h"

namespace Ast{

class Walker;

class ValueList: public Value {
public:
    ValueList(int line, int column, const std::list<Value*> & values):
    Value(line, column),
    values(values){
    }

    ValueList(const std::list<Value*> & values):
    Value(-1, -1),
    values(values){
    }

    ValueList(Value * value):
    Value(-1, -1){
        values.push_back(value);
    }
    
    virtual void walk(Walker & walker) const {
        walker.onValueList(*this);
    }
    
    virtual bool hasMultiple() const {
        return true;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_VALUE_LIST << getLine() << getColumn();
        for (std::list<Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            const Value * value = *it;
            *token << value->serialize();
        }
        return token;
    }

    static ValueList * deserialize(const Token * token){
        std::list<Value*> values;
        int line = -1, column = -1;
        try{
            TokenView view = token->view();
            view >> line;
            view >> column;
            while (true){
                const Token * next;
                view >> next;
                values.push_back(Value::deserialize(next));
            }
        } catch (const TokenException & e){
        }
        return new ValueList(line, column, values);
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
        return new ValueList(getLine(), getColumn(), copied);
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


    void listfail() const {
        if (getLine() != -1 && getColumn() != -1){
            std::ostringstream out;
            out << "No more values in this value list at line " << getLine() << " column " << getColumn() << ": " << toString();
            throw Exception(out.str());
        } else {
            throw Exception("No more values in this value list: " + toString());
        }
    }

    class ValueListView: public ViewImplementation {
    public:
        ValueListView(const ValueList * owner):
        owner(owner){
            current_value = owner->values.begin();
        }
    
        const ValueList * owner;
        std::list<Value*>::const_iterator current_value;

        virtual std::string getType() const {
            return owner->getType();
        }
        
        virtual const Value * self() const {
            return owner;
        }
        
        virtual std::string toString() const {
            return owner->toString();
        }

        /* why doesn't this work?
        using ViewImplementation::operator>>;
        template <typename Kind>
        ViewImplementation & operator>>(Kind & value){
            if (current_value == owner->values.end()){
                owner->listfail();
            }
            (*current_value)->view() >> value;
            current_value++;
            return *this;
        }
        */

        virtual ViewImplementation & operator>>(const Value *& value){
            if (current_value == owner->values.end()){
                owner->listfail();
            }
            (*current_value)->view() >> value;
            current_value++;
            return *this;
        }

        virtual ViewImplementation & operator>>(std::string & value){
            if (current_value == owner->values.end()){
                owner->listfail();
            }
            (*current_value)->view() >> value;
            current_value++;
            return *this;
        }

        virtual ViewImplementation & operator>>(int & x){
            if (current_value == owner->values.end()){
                owner->listfail();
            }
            (*current_value)->view() >> x;
            current_value++;
            return *this;
        }

        virtual ViewImplementation & operator>>(double & x){
            if (current_value == owner->values.end()){
                owner->listfail();
            }
            (*current_value)->view() >> x;
            current_value++;
            return *this;
        }

        virtual ViewImplementation & operator>>(bool & x){
            if (current_value == owner->values.end()){
                owner->listfail();
            }
            (*current_value)->view() >> x;
            current_value++;
            return *this;
        }

        virtual ViewImplementation & operator>>(std::vector<int> & x){
            while (current_value != owner->values.end()){
                int get;
                (*current_value)->view() >> get;
                x.push_back(get);
                current_value++;
            }
            return *this;
        }

        virtual ViewImplementation & operator>>(std::vector<double> & x){
            while (current_value != owner->values.end()){
                double get;
                (*current_value)->view() >> get;
                x.push_back(get);
                current_value++;
            }
            return *this;
        }
    };
    
    using Value::view;
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new ValueListView(this)));
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
};

}

#endif
