#ifndef _paintown_9830566ef6b184c0cfd8002334ee2477
#define _paintown_9830566ef6b184c0cfd8002334ee2477

#include "Value.h"

namespace Ast{

class Function: public Value {
public:
    Function(const std::string & name, const Value * arg1 = 0, const Value * arg2 = 0, const Value * arg3 = 0):
    name(name),
    arg1(arg1),
    arg2(arg2),
    arg3(arg3){
    }

    virtual std::string toString() const {
        std::ostringstream out;
        out << name << "(";
        if (arg1 != 0){
            out << arg1->toString();
        }
        if (arg2 != 0){
            out << ", " << arg2->toString();
        }
        if (arg3 != 0){
            out << ", " << arg3->toString();
        }
        out << ")";
        return out.str();
    }

    virtual void walk(Walker & walker) const {
        walker.onFunction(*this);
    }

    virtual const Value * getArg1() const {
        return arg1;
    }
    
    virtual const Value * getArg2() const {
        return arg2;
    }
    
    virtual const Value * getArg3() const {
        return arg3;
    }

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

    bool operator==(const std::string & str) const {
        return downcase(name) == downcase(str);
    }
    
    virtual Element * copy() const {
        Value * arg1_copy = 0;
        Value * arg2_copy = 0;
        Value * arg3_copy = 0;
        if (arg1){
            arg1_copy = (Value*) arg1->copy();
        }
        if (arg2){
            arg2_copy = (Value*) arg2->copy();
        }
        if (arg3){
            arg3_copy = (Value*) arg3->copy();
        }
        return new Function(name, arg1_copy, arg2_copy, arg3_copy);
    }
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        if (arg1){
            arg1->mark(marks);
        }
        if (arg2){
            arg2->mark(marks);
        }
        if (arg3){
            arg3->mark(marks);
        }
    }
    
    virtual std::string getType() const {
        return "function";
    }
    
    virtual ~Function(){
        delete arg1;
        delete arg2;
        delete arg3;
    }

protected:
    std::string name;
    const Value * arg1;
    const Value * arg2;
    const Value * arg3;
};

}

#endif
