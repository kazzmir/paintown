#include "network-behavior.h"

namespace Mugen{

NetworkLocalBehavior::NetworkLocalBehavior(Behavior * local, Network::Socket socket):
local(local),
socket(socket){
}

std::vector<std::string> NetworkLocalBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    return this->commands;
}

void NetworkLocalBehavior::flip(){
}
    
void NetworkLocalBehavior::hit(Object * enemy){
}

NetworkLocalBehavior::~NetworkLocalBehavior(){
    /* we do not own local, don't delete it */
}

NetworkRemoteBehavior::NetworkRemoteBehavior(Network::Socket socket):
socket(socket){
}

std::vector<std::string> NetworkRemoteBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    return this->commands;
}

void NetworkRemoteBehavior::flip(){
}

void NetworkRemoteBehavior::hit(Object * enemy){
}

NetworkRemoteBehavior::~NetworkRemoteBehavior(){
}

}
