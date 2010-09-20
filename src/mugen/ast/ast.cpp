#include <string>
#include "util/token.h"
#include "attribute.h"
#include "attribute-simple.h"
#include "attribute-array.h"
#include "helper.h"
#include "function.h"
#include "hitdef.h"
#include "value-list.h"
#include "range.h"
#include "string.h"
#include "Section.h"
#include "Expression.h"
#include "number.h"

using std::string;

namespace Ast{
    
AttributeSimple * AttributeSimple::deserialize(Token * token){
    int line, column;
    *token >> line;
    *token >> column;
    Token * nameToken;
    *token >> nameToken;
    Identifier * name = Identifier::deserialize(nameToken);
    Value * value = NULL;
    try{
        Token * valueToken;
        *token >> valueToken;
        value = Value::deserialize(valueToken);
    } catch (const TokenException & e){
    }

    return new AttributeSimple(line, column, name, value);
}
    
AttributeArray * AttributeArray::deserialize(Token * token){
    Token * name;
    *token >> name;
    Identifier * identifier = NULL;
    Keyword * keyword = NULL;
    if (*name == "identifier"){
        identifier = Identifier::deserialize(name);
    } else if (*name == "keyword"){
        keyword = Keyword::deserialize(name);
    }
    Token * indexToken;
    Token * valueToken;
    *token >> indexToken >> valueToken;
    Value * index = Value::deserialize(indexToken);
    Value * value = Value::deserialize(valueToken);
    if (identifier != NULL){
        return new AttributeArray(identifier, index, value);
    } else if (keyword != NULL){
        return new AttributeArray(keyword, index, value);
    }
    throw Exception("Deserialization error: no name given for an attribute array");
}
    
Identifier * Identifier::deserialize(Token * token){
    int line, column;
    string name;
    *token >> line >> column >> name;
    return new SimpleIdentifier(line, column, name);
}

Attribute * Attribute::deserialize(Token * token){
    if (*token == "attribute-simple"){
        return AttributeSimple::deserialize(token);
    }
    if (*token == "attribute-array"){
        return AttributeArray::deserialize(token);
    }
    throw Exception("Don't know how to deserialize " + token->getName());
}
    
Value * Value::deserialize(Token * token){
    if (*token == "number"){
        return Number::deserialize(token);
    }
    if (*token == "expression-infix"){
        return ExpressionInfix::deserialize(token);
    }
    if (*token == "expression-unary"){
        return ExpressionUnary::deserialize(token);
    }
    if (*token == "keyword"){
        return Keyword::deserialize(token);
    }
    if (*token == "function"){
        return Function::deserialize(token);
    }
    if (*token == "identifier"){
        return Identifier::deserialize(token);
    }
    if (*token == "helper"){
        return Helper::deserialize(token);
    }
    if (*token == "string"){
        return String::deserialize(token);
    }
    if (*token == "value-list"){
        return ValueList::deserialize(token);
    }
    if (*token == "range"){
        return Range::deserialize(token);
    }
    if (*token == "hitdef-attribute"){
        return HitDefAttribute::deserialize(token);
    }
    if (*token == "hitdef-attack-attribute"){
        return HitDefAttackAttribute::deserialize(token);
    }

    throw Exception("Don't know how to deserialize " + token->getName());
}
    
Function * Function::deserialize(Token * token){
    string name;
    *token >> name;
    ValueList * args = NULL;
    try{
        Token * next;
        *token >> next;
        args = ValueList::deserialize(next);
    } catch (const TokenException & e){
    }

    return new Function(name, args);
}

}
