#include "behavior.h"
#include <vector>
#include <string>
#include "input/input-map.h"
#include "command.h"

using namespace std;

namespace Mugen{

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

vector<string> AIBehavior::currentCommands(const vector<Command*> & commands, bool reversed){
    vector<string> out;
    return out;
}

AIBehavior::~AIBehavior(){
}

}
