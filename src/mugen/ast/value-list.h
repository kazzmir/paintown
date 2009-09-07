#ifndef _paintown_ast_value_list_h_
#define _paintown_ast_value_list_h_

#include "Value.h"
#include <list>
#include <string>
#include <sstream>

namespace Ast{

class ValueList: public Value{
public:
    ValueList(const std::list<Value*> & values):
    values(values){
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
