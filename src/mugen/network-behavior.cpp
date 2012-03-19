#include "network-behavior.h"

using std::vector;
using std::string;

namespace Mugen{

NetworkLocalBehavior::NetworkLocalBehavior(Behavior * local, Network::Socket socket):
local(local),
socket(socket){
}

/* Send the number of commands followed by (for each command)
 *   length of the command string
 *   the command string
 */
static void sendCommands(const vector<string> & commands, Network::Socket socket){
#ifdef HAVE_NETWORKING
    Network::send16(socket, commands.size());
    for (vector<string>::const_iterator it = commands.begin(); it != commands.end(); it++){
        const string & command = *it;
        Network::send16(socket, command.size());
        Network::sendStr(socket, command);
    }
#endif
}

void NetworkLocalBehavior::start(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    this->commands = local->currentCommands(stage, owner, commands, reversed);
    sendCommands(this->commands, socket);
}

std::vector<std::string> NetworkLocalBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    return this->commands;
}

void NetworkLocalBehavior::flip(){
    local->flip();
}
    
void NetworkLocalBehavior::hit(Object * enemy){
    local->hit(enemy);
}

NetworkLocalBehavior::~NetworkLocalBehavior(){
    /* we do not own local, don't delete it */
}

NetworkRemoteBehavior::NetworkRemoteBehavior(Network::Socket socket):
socket(socket){
}

static vector<string> readCommands(Network::Socket socket){
    vector<string> out;
#ifdef HAVE_NETWORKING
    int16_t commands = Network::read16(socket);
    for (int i = 0; i < commands; i++){
        int16_t length = Network::read16(socket) + 1;
        out.push_back(Network::readStr(socket, length));
    }
#endif
    return out;
}

std::vector<std::string> NetworkRemoteBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    return readCommands(socket);
}

void NetworkRemoteBehavior::flip(){
}

void NetworkRemoteBehavior::hit(Object * enemy){
}

NetworkRemoteBehavior::~NetworkRemoteBehavior(){
}

}
