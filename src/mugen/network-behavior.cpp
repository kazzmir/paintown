#include "network-behavior.h"
#include "util/timedifference.h"

using std::vector;
using std::string;

/* Artificial delay:
 *  commands will be sent with a stage tick timestamp.
 *  a command sent at timestamp X will be consumed at timestamp X+delay where
 *  delay is some constant, like 2.
 *
 *  or commands can just be sent and pushed onto a stack. when the behavior starts
 *  up it will start with X things on the stack where X is the amount of delay to have.
 *  commands will be sent every tick by local and read by remote where its placed on the stack.
 *  the remote should read the commands in a thread so it doesn't block the main thread.
 *  if the remote runs out of stack space because local is too slow then it will 
 *  wait for more commands to be sent.
 */

namespace Mugen{

/* Send the number of commands followed by (for each command)
 *   length of the command string
 *   the command string
 */
static void sendCommands(const vector<string> & commands, Network::Socket socket){
    /*
    TimeDifference timer;
    timer.startTime();
    */
#ifdef HAVE_NETWORKING
    int total = sizeof(uint16_t);
    for (vector<string>::const_iterator it = commands.begin(); it != commands.end(); it++){
        const string & command = *it;
        total += sizeof(uint16_t);
        total += command.size() + 1;
    }

    /* Just send one packet. Add an extra 2 bytes for the packet size */
    char * buffer = new char[total + sizeof(uint16_t)];
    char * position = buffer;
    position = Network::dump16(position, total);
    position = Network::dump16(position, commands.size());
    for (vector<string>::const_iterator it = commands.begin(); it != commands.end(); it++){
        const string & command = *it;
        position = Network::dump16(position, command.size());
        position = Network::dumpStr(position, command);
    }

    Network::sendBytes(socket, (uint8_t*) buffer, total + sizeof(uint16_t));

    /*
    Network::send16(socket, commands.size());
    for (vector<string>::const_iterator it = commands.begin(); it != commands.end(); it++){
        const string & command = *it;
        Network::send16(socket, command.size());
        Network::sendStr(socket, command);
    }
    */
    delete[] buffer;
#endif
    /*
    timer.endTime();
    Global::debug(0) << timer.printTime("send") << std::endl;
    */
}

NetworkLocalBehavior::NetworkLocalBehavior(Behavior * local, Network::Socket socket):
local(local),
socket(socket){
}

void NetworkLocalBehavior::begin(){
    const int delay = 4;
    /* send empty commands */
    for (int i = 0; i < delay; i++){
        commands.push_back(vector<string>());
        sendCommands(vector<string>(), socket);
    }
}

void NetworkLocalBehavior::start(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    vector<string> current = local->currentCommands(stage, owner, commands, reversed);
    this->commands.push_back(current);
    sendCommands(current, socket);
}

std::vector<std::string> NetworkLocalBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    vector<string> out = this->commands.front();
    this->commands.pop_front();
    return out;
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

static vector<string> readCommands(Network::Socket socket){
    vector<string> out;
    /*
    TimeDifference timer;
    timer.startTime();
    */
#ifdef HAVE_NETWORKING
    int16_t total = Network::read16(socket);
    char * buffer = new char[total];
    Network::readBytes(socket, (uint8_t*) buffer, total);
    char * position = buffer;
    uint16_t commands = 0;
    position = Network::parse16(position, &commands);
    for (int i = 0; i < commands; i++){
        uint16_t length = 0;
        position = Network::parse16(position, &length);
        string command;
        position = Network::parseString(position, &command, length + 1);
        out.push_back(command);
    }
    delete[] buffer;

    /*
    int16_t commands = Network::read16(socket);
    for (int i = 0; i < commands; i++){
        int16_t length = Network::read16(socket) + 1;
        out.push_back(Network::readStr(socket, length));
    }
    */
#endif
    /*
    timer.endTime();
    Global::debug(0) << timer.printTime("read") << std::endl;
    */
    return out;
}

void NetworkRemoteBehavior::pollCommands(){
    try{
        while (polling){
            vector<string> more = readCommands(socket);
            lock.acquire();
            commands.push_back(more);
            // lock.signal();
            lock.release();
        }
    } catch (const Network::NetworkException & fail){
    }
}

static void * launch_thread(void * arg){
    NetworkRemoteBehavior * behavior = (NetworkRemoteBehavior*) arg;
    behavior->pollCommands();
    return NULL;
}

NetworkRemoteBehavior::NetworkRemoteBehavior(Network::Socket socket):
socket(socket),
polling(true){
}

void NetworkRemoteBehavior::begin(){
    ::Util::Thread::createThread(&thread, NULL, (::Util::Thread::ThreadFunction) launch_thread, this);
}
    
vector<string> NetworkRemoteBehavior::nextCommand(){
    bool ok = false;
    while (!ok){
        lock.acquire();
        if (commands.size() == 0){
            // Global::debug(0) << "Waiting for more commands" << std::endl;
            // lock.wait();
            lock.release();
            PaintownUtil::rest(1);
        } else {
            ok = true;
        }
    }

    vector<string> out = commands.front();
    commands.pop_front();
    lock.release();
    return out;
}

std::vector<std::string> NetworkRemoteBehavior::currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
    return nextCommand();
}

void NetworkRemoteBehavior::flip(){
}

void NetworkRemoteBehavior::hit(Object * enemy){
}

NetworkRemoteBehavior::~NetworkRemoteBehavior(){
    polling = false;
    ::Util::Thread::joinThread(thread);
}

}
