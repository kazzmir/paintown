#include <string>
#include "util/token.h"
#include "attribute.h"
#include "attribute-simple.h"
#include "attribute-array.h"
#include "attribute-keyword.h"
#include "helper.h"
#include "function.h"
#include "hitdef.h"
#include "value-list.h"
#include "range.h"
#include "string.h"
#include "Section.h"
#include "filename.h"
#include "Expression.h"
#include "number.h"
#include "key.h"

using std::string;

namespace Ast{

string Element::SERIAL_STRING = "a";
string Element::SERIAL_FUNCTION = "b";
string Element::SERIAL_RANGE = "c";
string Element::SERIAL_SECTION_LIST = "d";
string Element::SERIAL_IDENTIFIER = "e";
string Element::SERIAL_KEYWORD = "f";
string Element::SERIAL_HITDEF_ATTRIBUTE = "g";
string Element::SERIAL_HITDEF_ATTACK_ATTRIBUTE = "h";
string Element::SERIAL_EXPRESSION_UNARY = "i";
string Element::SERIAL_EXPRESSION_INFIX = "j";
string Element::SERIAL_VALUE_LIST = "k";
string Element::SERIAL_ATTRIBUTE_SIMPLE = "l";
string Element::SERIAL_ATTRIBUTE_ARRAY = "m";
string Element::SERIAL_HELPER = "n";
string Element::SERIAL_NUMBER = "o";
string Element::SERIAL_FILENAME = "p";
string Element::SERIAL_ATTRIBUTE_KEYWORD = "q";
string Element::SERIAL_KEY_SINGLE = "r";
string Element::SERIAL_KEY_MODIFIER = "s";
string Element::SERIAL_KEY_COMBINED = "t";
string Element::SERIAL_KEY_LIST = "u";
    
string Section::SERIAL_SECTION_ATTRIBUTE = "s1";
string Section::SERIAL_SECTION_VALUE = "s2";
    
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
    if (*name == SERIAL_IDENTIFIER){
        identifier = Identifier::deserialize(name);
    } else if (*name == SERIAL_KEYWORD){
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
    if (*token == SERIAL_ATTRIBUTE_SIMPLE){
        return AttributeSimple::deserialize(token);
    }
    if (*token == SERIAL_ATTRIBUTE_ARRAY){
        return AttributeArray::deserialize(token);
    }
    if (*token == SERIAL_ATTRIBUTE_KEYWORD){
        return AttributeKeyword::deserialize(token);
    }

    throw Exception("Don't know how to deserialize attribute " + token->getName());
}
    
Value * Value::deserialize(Token * token){
    if (*token == SERIAL_NUMBER){
        return Number::deserialize(token);
    }
    if (*token == SERIAL_EXPRESSION_INFIX){
        return ExpressionInfix::deserialize(token);
    }
    if (*token == SERIAL_EXPRESSION_UNARY){
        return ExpressionUnary::deserialize(token);
    }
    if (*token == SERIAL_KEYWORD){
        return Keyword::deserialize(token);
    }
    if (*token == SERIAL_FUNCTION){
        return Function::deserialize(token);
    }
    if (*token == SERIAL_IDENTIFIER){
        return Identifier::deserialize(token);
    }
    if (*token == SERIAL_HELPER){
        return Helper::deserialize(token);
    }
    if (*token == SERIAL_STRING){
        return String::deserialize(token);
    }
    if (*token == SERIAL_VALUE_LIST){
        return ValueList::deserialize(token);
    }
    if (*token == SERIAL_RANGE){
        return Range::deserialize(token);
    }
    if (*token == SERIAL_HITDEF_ATTRIBUTE){
        return HitDefAttribute::deserialize(token);
    }
    if (*token == SERIAL_HITDEF_ATTACK_ATTRIBUTE){
        return HitDefAttackAttribute::deserialize(token);
    }
    if (*token == SERIAL_KEY_LIST){
        return KeyList::deserialize(token);
    }
    if (*token == SERIAL_KEY_SINGLE){
        return KeySingle::deserialize(token);
    }
    if (*token == SERIAL_KEY_MODIFIER){
        return KeyModifier::deserialize(token);
    }
    if (*token == SERIAL_KEY_COMBINED){
        return KeyCombined::deserialize(token);
    }
    if (*token == SERIAL_FILENAME){
        return Filename::deserialize(token);
    }

    throw Exception("Don't know how to deserialize value " + token->getName());
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
