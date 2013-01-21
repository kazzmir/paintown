#ifndef _paintown_mugen_serialize_h
#define _paintown_mugen_serialize_h

#include "common.h"
#include <string>
#include <vector>

class Token;
class RuntimeValue;

namespace Mugen{
    Token * serialize(const AttackType::Attribute);
    Token * serialize(const AttackType::Animation);
    Token * serialize(const AttackType::Ground);
    Token * serialize(const TransType);
    Token * serialize(const CharacterId &);
    Token * serialize(const std::vector<CharacterId> &);
    Token * serialize(const std::string &);
    Token * serialize(const RuntimeValue &);

    AttackType::Attribute deserializeAttackTypeAttribute(const Token * token);
    AttackType::Animation deserializeAttackTypeAnimation(const Token * token);
    AttackType::Ground deserializeAttackTypeGround(const Token * token);
    TransType deserializeTransType(const Token * token);
    CharacterId deserializeCharacterId(const Token * token);
    Physics::Type deserializePhysicsType(const Token * token);
    Facing deserializeFacing(const Token * token);
        
    AttackType::Animation defaultAttackTypeAnimation();
    AttackType::Ground defaultAttackTypeGround();
    TransType defaultTransType();
    CharacterId defaultCharacterId();
    Physics::Type defaultPhysicsType();
    Facing defaultFacing();
}

#endif
