#ifndef _paintown_ast_collectable_h_
#define _paintown_ast_collectable_h_

#include <list>
#include <iostream>
#include "all.h"

#define constructor(type, str_val, section_val, section_list_val, value_val, identifier_val, number_val, attribute_val) \
    Collectable(const type * x):\
        str(str_val),\
        section(section_val),\
        section_list(section_list_val),\
        value(value_val),\
        identifier(identifier_val),\
        number(number_val),\
        attribute(attribute_val),\
        _destroy(false){}

namespace Ast{

/* this could be vastly simplified if all AST objects derived
 * from a single Ast class that had a pure virtual marks() method.
 */
class Collectable{
public:

    constructor(std::string,        x, 0, 0, 0, 0, 0, 0)
    constructor(Section,            0, x, 0, 0, 0, 0, 0)
    constructor(std::list<Section*>,0, 0, x, 0, 0, 0, 0)
    constructor(Value,              0, 0, 0, x, 0, 0, 0)
    constructor(Identifier,         0, 0, 0, 0, x, 0, 0)
    constructor(double,             0, 0, 0, 0, 0, x, 0)
    constructor(Attribute,          0, 0, 0, 0, 0, 0, x)

    /* is this needed? */
    Collectable(const Collectable & copy):
        str(copy.str),
        section(copy.section),
        section_list(copy.section_list),
        value(copy.value),
        identifier(copy.identifier),
        number(copy.number),
        attribute(copy.attribute),
        _destroy(copy._destroy)
    {
    }

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
        if (attribute){ return attribute; }
        return 0;
    }

    void mark(std::map<const void *, bool> & marks) const {
        if (str){ marks[str] = true; }
        if (section){ section->mark(marks); }
        if (section_list){
            marks[section_list] = true;
            for (std::list<Section*>::const_iterator it = section_list->begin(); it != section_list->end(); it++){
                Section * section = *it;
                section->mark(marks);
            }
        }
        if (value){ value->mark(marks); }
        if (identifier){ identifier->mark(marks); }
        if (number){ marks[number] = true; }
        if (attribute){ attribute->mark(marks); }
    }

    /*
    const bool referenced(const std::list<Section*>* list, const void * value) const {
        if (list == value){
            return true;
        }

        for (std::list<Section*>::const_iterator it = list->begin(); it != list->end(); it++){
            const Section * section = *it;
            if (section->referenced(value)){
                return true;
            }
        }

        return false;
    }
    */

    /*
    const bool dv(const Value * v1, const void * value) const {
        return v1->referenced(value);
    }
    */

#if 0
    const bool referenced(const void * value) const {
        /* for debugging
        bool a = (str && str == value);
        bool b = (section && section->referenced(value));
        bool c = (section_list && referenced(section_list, value)) ;
        bool d = (this->value && dv(this->value, value)) ;
        // (this->value && this->value->referenced(value)) ||
        bool e = (identifier && identifier->referenced(value)) ;
        bool f = (number && number == value);
        bool g = (attribute && attribute->referenced(value));
        return a || b || c || d || e || f || g;
        */

        return (str && str == value) ||
               (section && section->referenced(value)) ||
               (section_list && referenced(section_list, value)) ||
               (this->value && this->value->referenced(value)) ||
               (identifier && identifier->referenced(value)) ||
               (number && number == value) ||
               (attribute && attribute->referenced(value));

        /*
        return (str && str == value) ||
               (section && section->referenced(value)) ||
               (section_list && referenced(section_list, value)) ||
               (this->value && this->value->referenced(value)) ||
               (identifier && identifier->referenced(value)) ||
               (number && number == value);
               */
    }
#endif

    ~Collectable(){
        if (_destroy){
            // std::cout << "Collect " << pointer() << std::endl;
            delete str;
            delete section;
            if (section_list != 0){
                for (std::list<Section*>::const_iterator it = section_list->begin(); it != section_list->end(); it++){
                    delete (*it);
                }
            }
            delete section_list;
            delete value;
            delete identifier;
            delete number;
            delete attribute;
        }
    }

protected:
    const std::string * str;
    const Section * section;
    const std::list<Section*>* section_list;
    const Value * value;
    const Identifier * identifier;
    const double * number;
    const Attribute * attribute;
    bool _destroy;
};

}

#undef constructor

#endif
