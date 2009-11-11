#ifndef _paintown_mugen_ast_walker_h
#define _paintown_mugen_ast_walker_h

namespace Ast{

class Section;
class AttributeSimple;
class Attribute;
class Value;

/* the visitor pattern */
class Walker{
public:
    Walker(){
    }

    virtual void onValue(const Value & value){
    }

    virtual void onAttribute(const Attribute & attribute){
    }

    virtual void onSection(const Section & section){
    }

    virtual void onAttributeSimple(const AttributeSimple & simple){
    }
};

}

#endif
