#ifndef _paintown_ast_filename_h_
#define _paintown_ast_filename_h_

#include "Value.h"
#include <string>

namespace Ast{

class Filename: public Value{
public:
    Filename(const std::string * str):
    str(str){
    }

    virtual std::string toString() const {
        return *str;
    }

    virtual ~Filename(){
        delete str;
    }

protected:
    const std::string * str;
};

}

#endif
