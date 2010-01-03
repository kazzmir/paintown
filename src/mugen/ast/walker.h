#ifndef _paintown_mugen_ast_walker_h
#define _paintown_mugen_ast_walker_h

namespace Ast{

class Section;
class AttributeSimple;
class AttributeKeyword;
class AttributeArray;
class Attribute;
class Value;
class ValueList;
class KeySingle;
class KeyModifier;
class KeyCombined;
class ExpressionInfix;
class Identifier;
class String;

/* the visitor pattern */
class Walker{
public:
    Walker(){
    }

    virtual void onValue(const Value & value){
    }
    
    virtual void onValueList(const ValueList & value){
    }

    virtual void onAttribute(const Attribute & attribute){
    }

    virtual void onKeySingle(const KeySingle & key){
    }
    
    virtual void onKeyModifier(const KeyModifier & key){
    }

    virtual void onKeyCombined(const KeyCombined & key){
    }

    virtual void onSection(const Section & section){
    }

    virtual void onAttributeSimple(const AttributeSimple & simple){
    }
    
    virtual void onAttributeKeyword(const AttributeKeyword & simple){
    }
    
    virtual void onAttributeArray(const AttributeArray & simple){
    }
        
    virtual void onExpressionInfix(const ExpressionInfix & expression){
    }

    virtual void onIdenfitier(const Identifier & identifier){
    }

    virtual void onString(const String & string){
    }

    virtual ~Walker(){
    }
};

}

#endif
