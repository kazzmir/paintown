#include <string>
#include "util/token.h"
#include "all.h"

using std::string;

namespace Ast{

/* These constants are used for serializing the tokens into as few bytes as possible,
 * thus the single letter names. If two letter names are required thats ok, but try
 * to choose unique single letter (any ascii value is ok) first.
 */
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
string Element::SERIAL_VALUE_ATTRIBUTE = "w";
string Element::SERIAL_RESOURCE = "x";
    
string Section::SERIAL_SECTION_ATTRIBUTE = "s1";
string Section::SERIAL_SECTION_VALUE = "s2";
    
AttributeSimple * AttributeSimple::deserialize(const Token * token){
    int line, column;
    TokenView view = token->view();
    view >> line;
    view >> column;
    const Token * nameToken;
    view >> nameToken;
    Identifier * name = Identifier::deserialize(nameToken);
    Value * value = NULL;
    try{
        const Token * valueToken;
        view >> valueToken;
        value = Value::deserialize(valueToken);
    } catch (const TokenException & e){
    }

    return new AttributeSimple(line, column, name, value);
}
    
AttributeArray * AttributeArray::deserialize(const Token * token){
    const Token * name;
    int line, column;
    TokenView view = token->view();
    view >> line >> column >> name;
    Identifier * identifier = NULL;
    Keyword * keyword = NULL;
    if (*name == SERIAL_IDENTIFIER){
        identifier = Identifier::deserialize(name);
    } else if (*name == SERIAL_KEYWORD){
        keyword = Keyword::deserialize(name);
    }
    const Token * indexToken;
    const Token * valueToken;
    view >> indexToken >> valueToken;
    Value * index = Value::deserialize(indexToken);
    Value * value = Value::deserialize(valueToken);
    if (identifier != NULL){
        return new AttributeArray(line, column, identifier, index, value);
    } else if (keyword != NULL){
        return new AttributeArray(line, column, keyword, index, value);
    }
    throw Exception("Deserialization error: no name given for an attribute array");
}

Identifier * Identifier::deserialize(const Token * token){
    int line, column;
    string name;
    token->view() >> line >> column >> name;
    return new SimpleIdentifier(line, column, name);
}

Attribute * Attribute::deserialize(const Token * token){
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
    
Value * Value::deserialize(const Token * token){
    if (*token == SERIAL_RESOURCE){
        return Resource::deserialize(token);
    }
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
    if (*token == SERIAL_VALUE_ATTRIBUTE){
        return ValueAttribute::deserialize(token);
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
    
Function * Function::deserialize(const Token * token){
    string name;
    TokenView view = token->view();
    view >> name;
    int line = -1;
    int column = -1;
    view >> line >> column;
    ValueList * args = NULL;
    try{
        const Token * next;
        view >> next;
        args = ValueList::deserialize(next);
    } catch (const TokenException & e){
    }

    return new Function(line, column, name, args);
}

}
