#ifndef _paintown_ast_keyword_h_
#define _paintown_ast_keyword_h_

#include "Value.h"
#include <string>

namespace Ast{

class Keyword: public Value{
public:
    /* str is not allocated!!! */
    Keyword(const char * str):
    str(str){
    }

    virtual std::string toString() const {
        return std::string(str);
    }
    
    virtual ~Keyword(){
        /* don't delete str */
    }

protected:
    const char * str;
};

}

#endif
