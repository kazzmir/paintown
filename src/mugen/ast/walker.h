#ifndef _paintown_mugen_ast_walker_h
#define _paintown_mugen_ast_walker_h

namespace Ast{

class Section;
class AttributeSimple;
class AttributeKeyword;
class AttributeArray;
class Attribute;
class HitDefAttribute;
class HitDefAttackAttribute;
class Value;
class Range;
class ValueList;
class KeySingle;
class KeyModifier;
class KeyCombined;
class ExpressionInfix;
class ExpressionUnary;
class Resource;
class Helper;
class Identifier;
class String;
class Function;
class Keyword;
class Number;

/* the visitor pattern */
class Walker{
public:
    Walker(){
    }

    virtual void onValue(const Value & value){
    }
    
    virtual void onRange(const Range & range){
    }
    
    virtual void onValueList(const ValueList & value){
    }

    virtual void onAttribute(const Attribute & attribute){
    }

    virtual void onKeySingle(const KeySingle & key){
    }
    
    virtual void onKeyModifier(const KeyModifier & key){
    }

    virtual void onHitDefAttribute(const HitDefAttribute & attribute){
    }
    
    virtual void onHitDefAttackAttribute(const HitDefAttackAttribute & attribute){
    }

    virtual void onKeyCombined(const KeyCombined & key){
    }

    /* start a new section */
    virtual void onSection(const Section & section){
    }
    
    /* end a section */
    virtual void onSectionFinished(const Section & section){
    }

    virtual void onAttributeSimple(const AttributeSimple & simple){
    }
    
    virtual void onAttributeKeyword(const AttributeKeyword & simple){
    }
    
    virtual void onAttributeArray(const AttributeArray & simple){
    }
        
    virtual void onExpressionInfix(const ExpressionInfix & expression){
    }
        
    virtual void onExpressionUnary(const ExpressionUnary & expression){
    }

    virtual void onIdentifier(const Identifier & identifier){
    }
    
    virtual void onHelper(const Helper & identifier){
    }

    virtual void onString(const String & string){
    }
    
    virtual void onFunction(const Function & string){
    }

    virtual void onKeyword(const Keyword & keyword){
    }
    
    virtual void onNumber(const Number & keyword){
    }
    
    virtual void onResource(const Resource & keyword){
    }

    virtual ~Walker(){
    }
};

}

#endif
