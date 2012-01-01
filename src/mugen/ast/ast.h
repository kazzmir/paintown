#ifndef _paintown_38e6ee3f07e8e75d6d134308f189249e
#define _paintown_38e6ee3f07e8e75d6d134308f189249e

#include <map>
#include <string>

class Token;

namespace Ast{

class Section;
class AttributeSimple;
class AttributeArray;
class AttributeKeyword;
class Number;
class Attribute;
class Identifier;
class Value;
class Number;
class ExpressionInfix;
class ExpressionUnary;
class Function;
class Filename;
class ValueList;
class ValueAttribute;
class HitDefAttribute;
class HitDefAttackAttribute;
class String;
class Resource;
class Range;
class Helper;
class Keyword;
class Key;
class KeySingle;
class KeyModifier;
class KeyCombined;
class KeyList;

class Element{
public:
    Element(int line, int column):
    line(line),
    column(column){
    }

    static std::string SERIAL_STRING;
    static std::string SERIAL_FUNCTION;
    static std::string SERIAL_RANGE;
    static std::string SERIAL_SECTION_LIST;
    static std::string SERIAL_IDENTIFIER;
    static std::string SERIAL_KEYWORD;
    static std::string SERIAL_HITDEF_ATTRIBUTE;
    static std::string SERIAL_HITDEF_ATTACK_ATTRIBUTE;
    static std::string SERIAL_EXPRESSION_UNARY;
    static std::string SERIAL_EXPRESSION_INFIX;
    static std::string SERIAL_VALUE_LIST;
    static std::string SERIAL_VALUE_ATTRIBUTE;
    static std::string SERIAL_ATTRIBUTE_SIMPLE;
    static std::string SERIAL_ATTRIBUTE_ARRAY;
    static std::string SERIAL_ATTRIBUTE_KEYWORD;
    static std::string SERIAL_HELPER;
    static std::string SERIAL_NUMBER;
    static std::string SERIAL_RESOURCE;
    static std::string SERIAL_FILENAME;
    static std::string SERIAL_KEY_SINGLE;
    static std::string SERIAL_KEY_MODIFIER;
    static std::string SERIAL_KEY_COMBINED;
    static std::string SERIAL_KEY_LIST;

    /* This synchronizes on-disk serialized files with the code.
     * If anything in the AST structure changes and causes an incompatibility
     * between the code and any existing serialized files then you *must*
     * increment this variable.
     */
    static const int SERIAL_VERSION = 17;

    virtual void mark(std::map<const void*, bool> & marks) const = 0;

#define define_equals(class_name) virtual bool operator!=(const class_name & him) const { return !(*this == him); } virtual bool operator==(const class_name & him) const { return false; }

    define_equals(Element)
    define_equals(Section)
    define_equals(Attribute)
    define_equals(AttributeSimple)
    define_equals(AttributeArray)
    define_equals(AttributeKeyword)
    define_equals(Identifier)
    define_equals(Value)
    define_equals(Number)
    define_equals(ExpressionInfix);
    define_equals(ExpressionUnary);
    define_equals(Function);
    define_equals(ValueList);
    define_equals(HitDefAttribute);
    define_equals(HitDefAttackAttribute);
    define_equals(String);
    define_equals(Range);
    define_equals(Resource);
    define_equals(Helper);
    define_equals(Keyword);
    define_equals(Key);
    define_equals(KeySingle);
    define_equals(KeyModifier);
    define_equals(KeyCombined);
    define_equals(KeyList);
    define_equals(Filename);
    define_equals(ValueAttribute);

#undef define_equals

    virtual Token * serialize() const = 0;

    /* create a deep copy of this object */
    virtual Element * copy() const = 0;

    virtual ~Element(){
    }

    virtual int getLine() const {
        return line;
    }

    virtual int getColumn() const {
        return column;
    }

private:
    int line, column;
};

};

#endif
