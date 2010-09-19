#include <string>
#include "util/token.h"
#include "attribute.h"
#include "attribute-simple.h"
#include "Section.h"
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
    } catch (const Exception & e){
    }

    return new AttributeSimple(line, column, name, value);
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
    throw Exception("Don't know how to deal with " + token->getName());
}
    
Value * Value::deserialize(Token * token){
    if (*token == "number"){
        return Number::deserialize(token);
    }
    throw Exception("Don't know how to deal with " + token->getName());
}

}
