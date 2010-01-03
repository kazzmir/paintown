#ifndef _paintown_9830566ef6b184c0cfd8002334ee2477
#define _paintown_9830566ef6b184c0cfd8002334ee2477

#include "Value.h"

namespace Ast{

class Function: public Value {
public:
    Function(const std::string & name, const Value * arg1):
    name(name),
    arg1(arg1){
    }

    virtual std::string toString() const {
        std::ostringstream out;
        out << name << "(" << arg1->toString() << ")";
        return out.str();
    }
    
    virtual Element * copy() const {
        return new Function(name, (Value*) arg1->copy());
    }
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        arg1->mark(marks);
    }
    
    virtual std::string getType() const {
        return "function";
    }
    
    virtual ~Function(){
        delete arg1;
    }

protected:
    std::string name;
    const Value * arg1;
};

}

#endif
