#ifndef _paintown_mugen_behavior_h
#define _paintown_mugen_behavior_h

#include <vector>
#include <string>
#include "util.h"
#include "input/input-map.h"

class MugenStage;

namespace Mugen{

class Command;

/* handles input and tells the character what commands to invoke */
class Behavior{
public:
    Behavior();

    virtual std::vector<std::string> currentCommands(const MugenStage & stage, const std::vector<Command*> & commands, bool reversed) = 0;

    virtual ~Behavior();
};

class HumanBehavior: public Behavior {
public:
    HumanBehavior(InputMap<Keys> left, InputMap<Keys> right);

    virtual std::vector<std::string> currentCommands(const MugenStage & stage, const std::vector<Command*> & commands, bool reversed);

    virtual ~HumanBehavior();

protected:
    InputMap<Keys> & getInput(bool facing);

protected:
    InputMap<Keys> right;
    InputMap<Keys> left;
};

class AIBehavior: public Behavior {
public:
    AIBehavior();

    virtual std::vector<std::string> currentCommands(const MugenStage & stage, const std::vector<Command*> & commands, bool reversed);

    virtual ~AIBehavior();
};

}

#endif
