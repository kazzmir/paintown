#ifndef _paintown_ast_collectable_h_
#define _paintown_ast_collectable_h_

#include <list>
#include "all.h"

#define constructor(type, str_val, section_val, section_list_val, value_val, identifier_val, number_val) \
    Collectable(const type * x):\
        str(str_val),\
        section(section_val),\
        section_list(section_list_val),\
        value(value_val),\
        identifier(identifier_val),\
        number(number_val),\
        _destroy(false){}

namespace Ast{

class Collectable{
public:

    constructor(std::string,        x, 0, 0, 0, 0, 0)
    constructor(Section,            0, x, 0, 0, 0, 0)
    constructor(std::list<Section*>,0, 0, x, 0, 0, 0)
    constructor(Value,              0, 0, 0, x, 0, 0)
    constructor(Identifier,         0, 0, 0, 0, x, 0)
    constructor(double,             0, 0, 0, 0, 0, x)

    void destroy(){
        _destroy = true;
    }

    const void * pointer(){
        if (str){ return str; }
        if (section){ return section; }
        if (section_list){ return section_list; }
        if (value){ return value; }
        if (identifier){ return identifier; }
        if (number){ return number; }
        return 0;
    }

    ~Collectable(){
        if (_destroy){
            delete str;
            delete section;
            delete section_list;
            delete value;
            delete identifier;
            delete number;
        }
    }

protected:
    const std::string * str;
    const Section * section;
    const std::list<Section*>* section_list;
    const Value * value;
    const Identifier * identifier;
    const double * number;
    bool _destroy;
};

}

#undef constructor

#endif
