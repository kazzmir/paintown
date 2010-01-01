#ifndef _paintown_ast_collectable_h_
#define _paintown_ast_collectable_h_

#include <list>
#include <iostream>
#include "all.h"

#define constructor(type, str_val, element_val, section_list_val, number_val) \
    Collectable(const type * x):\
        str(str_val),\
        element(element_val),\
        section_list(section_list_val),\
        number(number_val),\
        _destroy(false){}

namespace Ast{

class Collectable{
public:

    constructor(std::string,        x, 0, 0, 0)
    constructor(Element,            0, x, 0, 0)
    constructor(std::list<Section*>,0, 0, x, 0)
    constructor(double,             0, 0, 0, x)

    /* is this needed? */
    Collectable(const Collectable & copy):
        str(copy.str),
        element(copy.element),
        section_list(copy.section_list),
        number(copy.number),
        _destroy(copy._destroy){
    }

    void destroy(){
        _destroy = true;
    }

    const void * pointer(){
        if (str){ return str; }
        if (element){ return element; }
        if (section_list){ return section_list; }
        if (number){ return number; }
        return 0;
    }

    void mark(std::map<const void *, bool> & marks) const {
        if (str){ marks[str] = true; }
        if (element){ element->mark(marks); }
        if (section_list){
            marks[section_list] = true;
            for (std::list<Section*>::const_iterator it = section_list->begin(); it != section_list->end(); it++){
                Section * section = *it;
                section->mark(marks);
            }
        }
        if (number){ marks[number] = true; }
    }

    virtual ~Collectable(){
        if (_destroy){
            // std::cout << "Collect " << pointer() << std::endl;
            delete str;
            delete element;
            if (section_list != 0){
                for (std::list<Section*>::const_iterator it = section_list->begin(); it != section_list->end(); it++){
                    delete (*it);
                }
            }
            delete section_list;
            delete number;
        }
    }

protected:
    const std::string * str;
    const Element * element;
    const std::list<Section*>* section_list;
    const double * number;
    bool _destroy;
};

}

#undef constructor

#endif
