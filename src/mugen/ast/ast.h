#ifndef _paintown_38e6ee3f07e8e75d6d134308f189249e
#define _paintown_38e6ee3f07e8e75d6d134308f189249e

#include <map>

class Token;

namespace Ast{

class Section;
class AttributeSimple;
class Number;
class Attribute;
class Identifier;
class Value;
class Number;

class Element{
public:
    Element(){
    }

    /* This synchronizes on-disk serialized files with the code.
     * If anything in the AST structure changes and causes an incompatibility
     * between the code and any existing serialized files then you *must*
     * increment this variable.
     */
    static const int SERIAL_VERSION = 1;

    virtual void mark(std::map<const void*, bool> & marks) const = 0;

#define define_equals(class_name) virtual bool operator!=(const class_name & him) const { return !(*this == him); } virtual bool operator==(const class_name & him) const { return false; }

    define_equals(Element)
    define_equals(Section)
    define_equals(Attribute)
    define_equals(AttributeSimple)
    define_equals(Identifier)
    define_equals(Value)
    define_equals(Number)

#undef define_equals

    /*
    virtual bool operator!=(const Element & him) const {
        return !(*this == him);
    }

    virtual bool operator!=(const Section & him) const {
        return !(*this == him);
    }

    virtual bool operator!=(const AttributeSimple & him) const {
        return !(*this == him);
    }

    virtual bool operator==(const AttributeSimple & him) const {
        return false;
    }

    virtual bool operator==(const Section & him) const {
        return false;
    }

    virtual bool operator==(const Element & him) const {
        if (this == &him){
            return true;
        }

        return false;
    }
    */
    
    virtual Token * serialize() const = 0;

    /* create a deep copy of this object */
    virtual Element * copy() const = 0;

    virtual ~Element(){
    }
};

};

#endif
