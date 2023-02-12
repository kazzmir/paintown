#ifndef _rtech1_parameter_h
#define _rtech1_parameter_h

/* parameters are to be used to control the dynamic extent of values. values
 * can be pushed onto the current extent and are automatically popped off
 * upon function return.
 * this is similar to 'parameters' in Racket.
 *
 * This class is not thread safe.
 */

#include <vector>
#include "pointer.h"

namespace Util{

/* the static variable `stack' has to be defined somewhere. use this syntax to define it
 *  template <class Value> typename Util::Parameter<Value>::container Util::Parameter<Value>::stacks;
 */
template <class Value>
class Parameter{
public:
    /* push a new value on the stack */
    Parameter(Parameter & parent, const Value & what):
    stack(parent.stack),
    items(0){
        push(what);
    }

    Parameter(Parameter & parent):
    stack(parent.stack),
    items(0){
    }

    Parameter():
    items(0){
        stack = new std::vector<Value>();
    }

    /* pop last value */
    virtual ~Parameter(){
        for (int i = 0; i < items; i++){
            if (stack->size() > 0){
                stack->pop_back();
            }
        }
    }

    void push(const Value & what){
        items += 1;
        stack->push_back(what);
    }

    /* get the current value */
    Value current(){
        if (stack->size() > 0){
            return stack->back();
        }
        return Value();
    }

    ReferenceCount<std::vector<Value> > stack;

protected:
    /* number of things pushed onto the stack by this object. note this is
     * not the number of items on the stack because if there are multiple
     * parameter objects then each one could have pushed 1 object on. thus
     * the stack will have 2 things in it but each parameter will have an
     * 'items' count of 1.
     */
    int items;
};

}

#endif
