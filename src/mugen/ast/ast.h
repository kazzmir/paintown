#ifndef _paintown_38e6ee3f07e8e75d6d134308f189249e
#define _paintown_38e6ee3f07e8e75d6d134308f189249e

#include <map>

class Token;

namespace Ast{

class Element{
public:
    Element(){
    }

    virtual void mark(std::map<const void*, bool> & marks) const = 0;
    
    virtual Token * serialize() const = 0;

    /* create a deep copy of this object */
    virtual Element * copy() const = 0;

    virtual ~Element(){
    }
};

};

#endif
