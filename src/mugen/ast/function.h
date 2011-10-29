#ifndef _paintown_9830566ef6b184c0cfd8002334ee2477
#define _paintown_9830566ef6b184c0cfd8002334ee2477

#include "Value.h"
#include "value-list.h"

namespace Ast{

class Function: public Value {
public:
    /*
    Function(const std::string & name, const ValueList * args = NULL):
    Value(-1, -1),
    name(name),
    args(args){
    }
    */

    Function(int line, int column,const std::string & name, const ValueList * args = NULL):
    Value(line, column),
    name(name),
    args(args){
    }

    virtual std::string toString() const {
        std::ostringstream out;
        out << name << "(";
        if (args != NULL){
            out << args->toString();
        }
        out << ")";
        return out.str();
    }

    virtual void walk(Walker & walker) const {
        walker.onFunction(*this);
    }

    virtual const Value * getArg(int n) const {
        if (args != NULL){
            return args->get(n);
        }
        return 0;
    }

    virtual const Value * getArg1() const {
        return getArg(0);
    }
    
    virtual const Value * getArg2() const {
        return getArg(1);
    }
    
    virtual const Value * getArg3() const {
        return getArg(2);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_FUNCTION << name << getLine() << getColumn();
        if (args != NULL){
            *token << args->serialize();
        }
        return token;
    }

    static Function * deserialize(const Token * token);

    static std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
    }

    static int lowerCase( int c ){
        return tolower(c);
    }

    const std::string & getName() const {
        return name;
    }

    using Element::operator==;
    bool operator==(const Value & him) const {
        return him == *this;
    }

    bool operator==(const Function & him) const {
        if (name != him.name){
            return false;
        }

        if (args != NULL && him.args != NULL){
            return *args == *him.args;
        }

        if ((args == NULL && him.args != NULL) ||
            (args != NULL && him.args == NULL)){
            return false;
        }

        return true;
    }

    bool operator==(const std::string & str) const {
        return downcase(name) == downcase(str);
    }

    class FunctionView: public ViewImplementation {
    public:
        FunctionView(const Function * owner):
        owner(owner){
        }

        const Function * owner;

        virtual std::string getType() const {
            return owner->getType();
        }
        
        virtual const Value * self() const {
            return owner;
        }
        
        virtual std::string toString() const {
            return owner->toString();
        }
    };

    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new FunctionView(this)));
    }
    
    virtual Element * copy() const {
        ValueList * args_copy = 0;
        if (args){
            args_copy = (ValueList*) args->copy();
        }
        return new Function(getLine(), getColumn(), name, args_copy);
    }
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        if (args){
            args->mark(marks);
        }
    }
    
    virtual std::string getType() const {
        return "function";
    }
    
    virtual ~Function(){
        delete args;
    }

protected:
    virtual void fail(const std::string & what) const {
        throw Exception("Cannot read a " + what + " from function " + getName());
    }

    std::string name;
    const ValueList * args;
};

}

#endif
