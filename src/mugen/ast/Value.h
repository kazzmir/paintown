#ifndef _paintown_value_h
#define _paintown_value_h

#include <iostream>
#include <map>
#include <vector>
#include "walker.h"
#include "exception.h"
#include "ast.h"
#include "util/pointer.h"

class Token;

namespace Ast{

class Value;
class ViewImplementation{
public:
    virtual ViewImplementation & operator>>(const Value *& value){
        value = self();
        return *this;
    }

    virtual ViewImplementation & operator>>(std::string & value){
        value = toString();
        return *this;
    }
    
    virtual ViewImplementation & operator>>(int & x){
        fail("int");
        return *this;
    }

    virtual ViewImplementation & operator>>(double & x){
        fail("double");
        return *this;
    }
    
    virtual ViewImplementation & operator>>(bool & x){
        fail("bool");
        return *this;
    }
    
    virtual ViewImplementation & operator>>(std::vector<int> & x){
        fail("list of int");
        return *this;
    }
    
    virtual ViewImplementation & operator>>(std::vector<double> & x){
        fail("list of double");
        return *this;
    }

    virtual ~ViewImplementation(){
    }

protected:
    virtual std::string getType() const = 0;
    virtual std::string toString() const = 0;
    virtual const Value * self() const = 0;

    virtual void fail(const std::string & what) const {
        throw Exception("Cannot read a " + what + " from a " + getType());
    }
};

class View{
public:
    View(const Util::ReferenceCount<ViewImplementation> & view):
    view(view){
    }

    View(const View & copy):
    view(copy.view){
    }

    View & operator=(const View & copy){
        view = copy.view;
        return *this;
    }

    template <typename X>
    View & operator>>(X & value){
        view->operator>>(value);
        return *this;
    }

    Util::ReferenceCount<ViewImplementation> view;
};

class Value: public Element {
public:

    Value(int line, int column):
    Element(line, column){
    }

public:
    virtual std::string toString() const {
        return "";
    }
    
    virtual void walk(Walker & walker) const {
        walker.onValue(*this);
    }

    /* true if this value has multiple values in it
     * should only be true for value-list
     */
    virtual bool hasMultiple() const {
        return false;
    }

    virtual void debugExplain(){
        std::cout << toString() << std::endl;
    }

    static Value * deserialize(const Token * token);

    /*
    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        throw Exception("Override this method");
    }
    */

    class ValueView: public ViewImplementation {
    public:
        ValueView(const Value * owner):
        owner(owner){
        }

        using ViewImplementation::operator>>;
        virtual const ValueView & operator>>(const Value *& value) const {
            value = owner;
            // fail("value");
            return *this;
        }

        const Value * owner;

    protected:
        virtual std::string getType() const {
            return owner->getType();
        }

        virtual std::string toString() const {
            return owner->toString();
        }
    };

    /*
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new ValueView(this)));
    }
    */

    virtual View view() const = 0;
    
    

    /*
    virtual bool referenced(const void * value) const {
        return value == this;
    }
    */
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
    }

    virtual std::string getType() const = 0;

    virtual ~Value(){
    }

protected:
    virtual void fail(const std::string & what) const {
        throw Exception("Cannot read a " + what + " from a " + getType());
    }
};

}

#endif
