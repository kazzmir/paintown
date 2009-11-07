#ifndef _paintown_ast_number_h_
#define _paintown_ast_number_h_

#include "Value.h"
#include <string>
#include <sstream>

namespace Ast{

class Number: public Value{
public:
    Number(double value):
    value(value){
    }

    using Value::operator>>;

    virtual const Value & operator>>(int & x) const {
        x = (int) value;
        return *this;
    }
    
    virtual const Value & operator>>(double & x) const {
        x = value;
        return *this;
    }
    
    virtual std::string getType() const {
        return "number";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        out << value;
        return out.str();
    }

    virtual ~Number(){
    }
protected:
    double value;
};

}

#endif
