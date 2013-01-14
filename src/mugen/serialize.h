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
}

#endif
