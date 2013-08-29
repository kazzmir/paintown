#ifndef _paintown_mugen_serialize_h
#define _paintown_mugen_serialize_h

#include "common.h"
#include <string>
#include <vector>

class Token;

namespace Mugen{
    struct RuntimeValue;
    struct HitOverride;
    struct ScreenBound;

    Token * serialize(const AttackType::Attribute);
    Token * serialize(const AttackType::Animation);
    Token * serialize(const AttackType::Ground);
    Token * serialize(const TransType);
    Token * serialize(const CharacterId &);
    Token * serialize(const std::vector<CharacterId> &);
    Token * serialize(const std::string &);
    Token * serialize(const RuntimeValue &);
    Token * serialize(const Physics::Type);
    Token * serialize(const Facing);
    Token * serialize(int);
    Token * serialize(const Graphics::Color &);

    AttackType::Attribute deserializeAttackTypeAttribute(const Token * token);
    AttackType::Animation deserializeAttackTypeAnimation(const Token * token);
    AttackType::Ground deserializeAttackTypeGround(const Token * token);
    TransType deserializeTransType(const Token * token);
    CharacterId deserializeCharacterId(const Token * token);
    Physics::Type deserializePhysicsType(const Token * token);
    Facing deserializeFacing(const Token * token);
    Graphics::Color deserializeGraphicsColor(const Token * token);

    void deserialize_HitOverride(HitOverride & out, const Token * token);
    void deserialize_stdstring(std::string & out, const Token * token);
    void deserialize_CharacterId(CharacterId & out, const Token * token);
    void deserialize_ScreenBound(ScreenBound & out, const Token * token);

    void deserialize_int(int & out, const Token * token);
    void deserialize_RuntimeValue(RuntimeValue & out, const Token * token);

    void deserialize_stdvectorCharacterId(std::vector<CharacterId> & out, const Token * token);

    AttackType::Animation defaultAttackTypeAnimation();
    AttackType::Ground defaultAttackTypeGround();
    TransType defaultTransType();
    CharacterId defaultCharacterId();
    Physics::Type defaultPhysicsType();
    Facing defaultFacing();
    Graphics::Color defaultGraphicsColor();
}

#endif
