#include "serialize.h"
#include "util/token.h"
#include "compiler.h"
#include <sstream>
#include "character-state.h"

using std::vector;
using std::string;

namespace Mugen{

static const char * BOOL_VALUE = "b";
static const char * STRING_VALUE = "s";
static const char * DOUBLE_VALUE = "d";
static const char * LIST_STRING_VALUE = "l";
static const char * RANGE_VALUE = "r";
static const char * STATE_VALUE = "v";
static const char * ATTACK_VALUE = "a";
static const char * INTS_VALUE = "i";

static RuntimeValue::Type getRuntimeValueType(const string & type){
    if (type == BOOL_VALUE){
        return RuntimeValue::Bool;
    }
    if (type == STRING_VALUE){
        return RuntimeValue::String;
    }
    if (type == DOUBLE_VALUE){
        return RuntimeValue::Double;
    }
    if (type == LIST_STRING_VALUE){
        return RuntimeValue::ListOfString;
    }
    if (type == RANGE_VALUE){
        return RuntimeValue::RangeType;
    }
    if (type == STATE_VALUE){
        return RuntimeValue::StateType;
    }
    if (type == ATTACK_VALUE){
        return RuntimeValue::AttackAttribute;
    }
    if (type == INTS_VALUE){
        return RuntimeValue::ListOfInt;
    }
    return RuntimeValue::Invalid;
}

Token * serialize(const RuntimeValue & value){
    Token * token = new Token();
    switch (value.getType()){
        case RuntimeValue::Invalid: {
            break;
        }
        case RuntimeValue::Bool: {
            *token << BOOL_VALUE << value.getBoolValue();
            break;
        }
        case RuntimeValue::String: {
            *token << STRING_VALUE << value.getStringValue();
            break;
        }
        case RuntimeValue::Double: {
            *token << DOUBLE_VALUE;
            if (value.getDoubleValue() != 0){
                *token << value.getDoubleValue();
            }
            break;
        }
        case RuntimeValue::ListOfString: {
            *token << LIST_STRING_VALUE;
            for (vector<string>::const_iterator it = value.strings_value.begin(); it != value.strings_value.end(); it++){
                *token << *it;
            }
            break;
        }
        case RuntimeValue::RangeType: {
            *token << RANGE_VALUE << value.range.low << value.range.high;
            break;
        }
        case RuntimeValue::StateType: {
            *token << STATE_VALUE <<
                value.attribute.standing <<
                value.attribute.crouching <<
                value.attribute.lying <<
                value.attribute.aerial;
            break;
        }
        case RuntimeValue::AttackAttribute: {
            *token << ATTACK_VALUE; 
            for (vector<AttackType::Attribute>::const_iterator it = value.attackAttributes.begin(); it != value.attackAttributes.end(); it++){
                *token << *it;
            }
            break;
        }
        case RuntimeValue::ListOfInt: {
            *token << INTS_VALUE;
            for (vector<int>::const_iterator it = value.ints_value.begin(); it != value.ints_value.end(); it++){
                *token << *it;
            }

            break;
        }
    }

    return token;
}
    
void deserialize_RuntimeValue(RuntimeValue & out, const Token * token){
    int type = getRuntimeValueType(token->getName());
    TokenView view = token->view();

    switch (type){
        case RuntimeValue::Invalid: {
            break;
        }
        case RuntimeValue::Bool: {
            bool value = false;
            view >> value;
            out = RuntimeValue(value);
            break;
        }
        case RuntimeValue::String: {
            std::string value;
            view >> value;
            out = RuntimeValue(value);
            break;
        }
        case RuntimeValue::Double: {
            double value = 0;
            try{
                view >> value;
            } catch (const TokenException & fail){
                /* there might be no value because its 0 */
            }
            out = RuntimeValue(value);
            break;
        }
        case RuntimeValue::ListOfString: {
            std::vector<std::string> values;
            while (view.hasMore()){
                std::string value;
                view >> value;
                values.push_back(value);
            }

            out = RuntimeValue(values);
            break;
        }
        case RuntimeValue::RangeType: {
            int low = 0;
            int high = 0;
            view >> low >> high;
            out = RuntimeValue(low, high);
            break;
        }
        case RuntimeValue::StateType: {
            RuntimeValue::StateTypes value;
            view >> value.standing >> value.crouching >> value.lying >> value.aerial;
            out = RuntimeValue(value);
            break;
        }
        case RuntimeValue::AttackAttribute: {
            vector<AttackType::Attribute> values;
            while (view.hasMore()){
                int value = 0;
                view >> value;
                values.push_back(AttackType::Attribute(value));
            }

            out = RuntimeValue(values);
            break;
        }
        case RuntimeValue::ListOfInt: {
            vector<int> values;
            while (view.hasMore()){
                int value = 0;
                view >> value;
                values.push_back(value);
            }

            out = RuntimeValue(values);
            break;
        }
    }
}

void deserialize_stdstring(std::string & out, const Token * token){
    out = token->getName();
}

void deserialize_CharacterId(CharacterId & out, const Token * token){
    out = deserializeCharacterId(token);
}

void deserialize_ScreenBound(ScreenBound & out, const Token * token){
    out = deserializeScreenBound(token);
}
    
void deserialize_stdvectorCharacterId(std::vector<CharacterId> & out, const Token * token){
    TokenView view = token->view();
    const Token * use = view.next();
    while (use != NULL){
        out.push_back(deserializeCharacterId(use));
        use = view.next();
    }
}
    
void deserialize_HitOverride(HitOverride & out, const Token * token){
    out = deserializeHitOverride(token);
}

/* FIXME: probably output a full token like (AttackTypeAttribute ..data..) */
Token * serialize(const AttackType::Attribute data){
    Token * token = new Token();
    *token << data;
    return token;
}

Token * serialize(const AttackType::Animation data){
    Token * token = new Token();
    std::ostringstream out;
    out << data;
    *token << "AttackTypeAnimation" << out.str();
    return token;
}

Token * serialize(const AttackType::Ground data){
    Token * token = new Token();
    std::ostringstream out;
    out << data;
    *token << "AttackTypeGround" << out.str();
    return token;
}

Token * serialize(const TransType data){
    Token * token = new Token();
    std::ostringstream out;
    out << data;
    *token << "TransType" << out.str();
    return token;
}

Token * serialize(const CharacterId & data){
    Token * token = new Token();
    std::ostringstream out;
    out << data.intValue();
    *token << "CharacterId" << out.str();
    return token;
}
    
Token * serialize(int data){
    std::ostringstream out;
    out << data;
    return new Token(out.str());
}

Token * serialize(const Graphics::Color & color){
    Token * start = new Token();
    *start << "color" << Graphics::getRed(color) << Graphics::getGreen(color) << Graphics::getBlue(color);
    return start;
}

Graphics::Color deserializeGraphicsColor(const Token * token){
    int red, green, blue;
    if (token->match("_", red, green, blue)){
        return Graphics::makeColor(red, green, blue);
    }

    return Graphics::Color();
}
    
Token * serialize(const Physics::Type data){
    Token * token = new Token();
    std::ostringstream out;
    out << data;
    *token << "PhysicsType" << out.str();
    return token;
}

Token * serialize(const Facing data){
    std::ostringstream out;
    out << data;
    Token * token = new Token();
    *token << "Facing" << out.str();
    return token;
}

Token * serialize(const std::vector<CharacterId> & data){
    Token * token = new Token();
    *token << "ids";
    for (vector<CharacterId>::const_iterator it = data.begin(); it != data.end(); it++){
        *token << it->intValue();
    }
    return token;
}

Token * serialize(const std::string & data){
    return new Token(data);
}

static int integer(const string & what){
    int out = 0;
    std::istringstream data(what);
    data >> out;
    return out;
}
    
void deserialize_int(int & out, const Token * token){
    std::istringstream data(token->getName());
    data >> out;
}

AttackType::Attribute deserializeAttackTypeAttribute(const Token * token){
    return AttackType::Attribute(integer(token->getName()));
}

AttackType::Animation deserializeAttackTypeAnimation(const Token * token){
    int out = defaultAttackTypeAnimation();
    if (token->match("_", out)){
        return AttackType::Animation(out);
    }
    return AttackType::Animation(out);
}

AttackType::Ground deserializeAttackTypeGround(const Token * token){
    int out = defaultAttackTypeGround();
    if (token->match("_", out)){
        return AttackType::Ground(out);
    }
    return AttackType::Ground(out);
}

TransType deserializeTransType(const Token * token){
    int out = defaultTransType();
    if (token->match("_", out)){
        return TransType(out);
    }
    return TransType(out);
}

CharacterId deserializeCharacterId(const Token * token){
    int out = -1;
    if (token->match("_", out)){
        return CharacterId(out);
    }
    return CharacterId(out);
}

Physics::Type deserializePhysicsType(const Token * token){
    int out = 0;
    if (token->match("_", out)){
        return Physics::Type(out);
    }
    return Physics::None;
}

Facing deserializeFacing(const Token * token){
    int out = FacingLeft;
    if (token->match("_", out)){
        return Facing(out);
    }
    return FacingLeft;
}
 
AttackType::Animation defaultAttackTypeAnimation(){
    return AttackType::NoAnimation;
}

AttackType::Ground defaultAttackTypeGround(){
    return AttackType::None;
}

TransType defaultTransType(){
    return None;
}

CharacterId defaultCharacterId(){
    return CharacterId(-1);
}

Physics::Type defaultPhysicsType(){
    return Physics::None;
}

Facing defaultFacing(){
    return FacingLeft;
}

Graphics::Color defaultGraphicsColor(){
    return Graphics::Color();
}

}
