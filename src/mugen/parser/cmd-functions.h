#ifndef _peg_cmd_functions_h
#define _peg_cmd_functions_h

#include "cmd.h"
#include "mugen/ast/all.h"

typedef std::list<Ast::Section*> SectionList;

namespace Mugen{
namespace Cmd{

template<class X>
X as(const Value & value){
    return (X) value.getValue();
}

Ast::Value * makeValueList(const Value & front, const Value & rest);
Ast::Attribute * makeAttribute(int line, int column, const Value & id, const Value & data);
Ast::Attribute * makeAttribute(const Value & id, const Value & data);
Ast::Attribute * makeAttribute(const Value & id);
Ast::Attribute * makeAttribute(const char * name, const Value & data);
Ast::Attribute * makeIndexedAttribute(const Value & id, const Value & index, const Value & data);
Ast::Identifier * makeSimpleIdentifier(const Value & name);
Ast::Identifier * makeSimpleIdentifier(int line, int column, const char * name);
Ast::Identifier * makeSimpleIdentifier(const char * name);
Ast::Identifier * makeSimpleIdentifier(const std::string & str);
Ast::Value * negateExpression(const Value & exp);
Ast::Value * makeNumber(double value);
Ast::Value * makeNumber(const Value & sign, const Value & number);
void addSection(const Value & section_list_value, const Value & section_value);
Ast::Value * makeExpressionLessThanEquals(const Value & left, const Value & right);
Ast::Value * makeExpressionOr(const Value & left, const Value & right);
Ast::Value * makeExpressionBitwiseOr(const Value & left, const Value & right);
Ast::Value * makeExpressionBitwiseXOr(const Value & left, const Value & right);
Ast::Value * makeExpressionBitwiseAnd(const Value & left, const Value & right);
Ast::Value * makeExpressionAssignment(const Value & left, const Value & right);
Ast::Value * makeExpressionEquals(const Value & left, const Value & right);
Ast::Value * makeExpressionUnequals(const Value & left, const Value & right);
Ast::Value * makeExpressionGreaterThanEquals(const Value & left, const Value & right);
Ast::Value * makeExpressionGreaterThan(const Value & left, const Value & right);
Ast::Value * makeExpressionLessThan(const Value & left, const Value & right);
Ast::Value * makeExpressionXOr(const Value & left, const Value & right);
Ast::Value * makeExpressionAnd(const Value & left, const Value & right);
Ast::Value * makeExpressionAdd(const Value & left, const Value & right);
Ast::Value * makeExpressionSubtract(const Value & left, const Value & right);
Ast::Value * makeExpressionMultiply(const Value & left, const Value & right);
Ast::Value * makeExpressionDivide(const Value & left, const Value & right);
Ast::Value * makeExpressionModulo(const Value & left, const Value & right);
Ast::Value * makeExpressionPower(const Value & left, const Value & right);
Ast::Value * makeUnaryExpression(const Value & unaries, const Value & exp);
double * parseDouble(const Value & value);
double * parseDouble(const Value & left, const Value & right);
Ast::Value * makeFunction1(const std::string & name, const Value & arg1);
Ast::Value * makeFunction1(const Value & name, const Value & arg1);
Ast::Value * makeFunction(const std::string & name, const Value & arg1);
Ast::Value * makeFunction(const Value & name, const Value & arg1);
Ast::Value * makeAnimElem(const Value & line, const Value & arg1, const Value & comparison, const Value & arg2);
Ast::Value * makeAnimElem(const Value & line, const Value & arg1);
std::string combine(const char * name, const Value & what);
Ast::Value * makeHelper(const Value & name, const Value & id);
Ast::Value * makeKeyword(const char * name);
std::string * toString(const Value & input);
std::string * toString(char front, const Value & input);
std::string * toString(const char * x);
Ast::HitDefAttackAttribute * makeHitDefAttackAttribute();
Ast::HitDefAttribute * makeHitDefAttribute(const Value & input);
Ast::Identifier * makeIdentifier(int line, int column, const Value & front, const Value & rest);
Ast::Value * makeInteger(const Value & sign, const Value & digits);
Ast::Key * makeKeyModifier(Ast::Key * in, Ast::KeyModifier::ModifierType type, int ticks = 0);
Ast::Key * makeKeyCombined(const Value & left, const Value & right);

class KeyModifier{
public:
    virtual Ast::Key * apply(Ast::Key * in) = 0;
    virtual ~KeyModifier(){
    }
};

class ReleaseKeyModifier: public KeyModifier {
public:
    ReleaseKeyModifier(int ticks):
    ticks(ticks){
    }

    virtual ~ReleaseKeyModifier(){
    }

    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::Release, ticks);
    }

protected:

    int ticks;
};

class DirectionKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::Direction);
    }

    virtual ~DirectionKeyModifier(){
    }
};

class HeldDownKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::MustBeHeldDown);
    }

    virtual ~HeldDownKeyModifier(){
    }
};

class OnlyKeyModifier: public KeyModifier {
public:
    virtual Ast::Key * apply(Ast::Key * in){
        return makeKeyModifier(in, Ast::KeyModifier::Only);
    }

    virtual ~OnlyKeyModifier(){
    }
};

Ast::Key * applyKeyModifiers(const Value & mods, Ast::Key * key);
Ast::Key * makeKey(const Value & value);
Ast::Key * makeKeyList(const Value & first, const Value & rest);
Ast::Key * makeEmptyKeyList();
Ast::Value * makeRange(Ast::Range::RangeType type, const Value & low, const Value & high);
Ast::Section * makeSection(const Value & str);
SectionList * makeSectionList();
Ast::String * makeString(const Value & value);

}
}

#endif
