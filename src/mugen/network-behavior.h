#ifndef _paintown_mugen_network_behavior_h
#define _paintown_mugen_network_behavior_h

#include <vector>
#include <string>
#include <deque>
#include "util/network/network.h"
#include "util/thread.h"
#include "behavior.h"

namespace Mugen{

/* Gets its commands from some other behavior and sends it over the network to a
 * waiting NetworkRemoteBehavior
 */
class NetworkLocalBehavior: public Behavior {
public:
    NetworkLocalBehavior(Behavior * local, Network::Socket socket);

    /* Start some stuff */
    virtual void begin();

    virtual void start(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    virtual void flip();
    
    virtual void hit(Object * enemy);

    virtual ~NetworkLocalBehavior();

protected:
    Behavior * local;
    Network::Socket socket;
    // std::vector<std::string> commands;
    std::deque<std::vector<std::string> > commands;
};

class NetworkRemoteBehavior: public Behavior {
public:
    NetworkRemoteBehavior(Network::Socket socket);
    
    virtual void begin();

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed);
    virtual void flip();
    
    virtual void hit(Object * enemy);

    virtual ~NetworkRemoteBehavior();

    void pollCommands();

protected:
    std::vector<std::string> nextCommand();

    Network::Socket socket;
    ::Util::Thread::LockObject lock;
    ::Util::Thread::Id thread;
    std::deque<std::vector<std::string> > commands;
    bool polling;
};

}

#endif
