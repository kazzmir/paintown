#include "behavior.h"
#include <vector>
#include <string>
#include "input/input-map.h"
#include "command.h"
#include "util/funcs.h"

using namespace std;

namespace Mugen{

namespace PaintownUtil = ::Util;

Behavior::Behavior(){
}

Behavior::~Behavior(){
}

HumanBehavior::HumanBehavior(InputMap<Mugen::Keys> right, InputMap<Mugen::Keys> left):
right(right),
left(left){
}
    
InputMap<Keys> & HumanBehavior::getInput(bool facingRight){
    if (facingRight){
        return right;
    }
    return left;
}

vector<string> HumanBehavior::currentCommands(const vector<Command*> & commands, bool reversed){
    vector<string> out;
    
    InputMap<Mugen::Keys>::Output output = InputManager::getMap(getInput(reversed));

    for (vector<Command*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        Command * command = *it;
        if (command->handle(output)){
            Global::debug(2) << "command: " << command->getName() << endl;
            out.push_back(command->getName());
        }
    }

    return out;

}

HumanBehavior::~HumanBehavior(){
}

AIBehavior::AIBehavior(){
}

static string randomCommand(const vector<Command*> & commands){
    if (commands.size() == 0){
        return "";
    }

    int choice = PaintownUtil::rnd(commands.size());
    return commands[choice]->getName();
}

vector<string> AIBehavior::currentCommands(const vector<Command*> & commands, bool reversed){
    vector<string> out;
    if (PaintownUtil::rnd(100) > 90){
        out.push_back(randomCommand(commands));
    }
    return out;
}

AIBehavior::~AIBehavior(){
}

}
