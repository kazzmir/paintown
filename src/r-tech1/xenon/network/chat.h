#ifndef _util_network_chat_h
#define _util_network_chat_h

#include "network.h"
#include "r-tech1/pointer.h"
#include "r-tech1/thread.h"

#include <string>
#include <vector>
#include <queue>

namespace Network{
namespace Chat{

class Message{
public:
    enum Type{
        Ping,
        Chat,
        Command,
        Unknown,
    };
    Message();
    Message(const Type &);
    Message(Socket socket);
    Message(const Type &, const std::string &, const std::string &);
    Message(const Message &);
    virtual ~Message();
    
    const Message & operator=(const Message &);
    
    virtual void read(Socket socket);
    
    virtual void send(Socket socket);
    
    virtual const Type & getType() const;
    
    virtual const std::string & getName() const;
    
    virtual const std::string & getMessage() const;
    
    virtual void setParameters(const std::vector<std::string> &);
    
    virtual const std::vector<std::string> & getParameters() const;
    
protected:
    Type type;
    std::string sender;
    std::string message;
    std::vector<std::string> parameters;
};

class Threadable{
public:
    Threadable();
    virtual ~Threadable();
    
    virtual void start();
    virtual void run() = 0;
    virtual void join();
protected:
    ::Util::Thread::Id thread;
    ::Util::Thread::LockObject lock;
};
   
class Client : public Threadable{
public:
    Client(int id, Network::Socket socket);
    
    virtual ~Client();
    
    virtual void run();
    
    virtual int getId() const;
    
    virtual void sendMessage(Message &);
    
    virtual bool hasMessages() const;
    
    virtual Message nextMessage();
    
    virtual void shutdown();
    
    virtual bool isValid() const;
    
private:
    int id;
    Network::Socket socket;
    bool end;
    mutable std::queue<Message> messages;
    bool valid;
};

class Server : public Threadable{
public:
    Server(int port);
    virtual ~Server();
    
    virtual void run();
    
    virtual void poll();
    
    virtual void cleanup();
    
    virtual bool hasMessages() const;
    
    virtual Message nextMessage();
    
    virtual void global(Message &);
    
    virtual void relay(int id, Message &);
    
    virtual void shutdown();
    
private:
    Network::Socket remote;
    std::vector< Util::ReferenceCount<Client> > clients;
    mutable std::queue<Message> messages;
    bool end;
};

}// end chat
}
#endif
