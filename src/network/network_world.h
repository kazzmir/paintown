#ifndef _paintown_network_world_h
#define _paintown_network_world_h

#include "network.h"
#include "game/adventure_world.h"
#include "util/load_exception.h"
#include "object/object.h"
#include "input/text-input.h"
#include <pthread.h>
#include <vector>
#include <string>
#include <deque>
#include <sstream>
#include <map>

struct Packet{
	Packet(const Network::Message & m, Network::Socket s, Network::Socket to = 0):
            message(m),
            socket(s),
            to(to){}

	Network::Message message;
	Network::Socket socket;
        /* if you want to specify a receipient, make `to' non-null */
	Network::Socket to;
};

class NetworkWorld: public AdventureWorld {
public:
	NetworkWorld( std::vector<Network::Socket> & sockets, const std::vector< Object * > & players, const std::map<Object*, Network::Socket> & characterToClient, const std::string & path, const std::map<Object::networkid_t, std::string> & clientNames, int screen_size = 320 ) throw ( LoadException );
	
	virtual void addMessage( Network::Message m, Network::Socket from = 0, Network::Socket to = 0);
	virtual void act();
        void draw(Bitmap * work);
	virtual void doScene( int min_x, int max_x );
        
        virtual void changePause();
	
	void addIncomingMessage( const Network::Message & message, Network::Socket from );

	Network::Message finishMessage();

        /* starts the threads for each client */
        void startMessageHandlers();
        void waitForHandlers();

	bool isRunning();

	void flushOutgoing();
	
	void stopRunning();
	
	virtual void addObject( Object * o );

	virtual ~NetworkWorld();

public:
        void endChatLine();

protected:
	Object * findNetworkObject( Object::networkid_t id );
	void sendMessage( const Network::Message & message, Network::Socket socket );
        std::vector< Network::Message > getIncomingMessages();
	void handleMessage( Network::Message & message );
        void handlePing(Network::Message & message);

	Network::Message nextBlockMessage( int block );

        void removePlayer(Object * player);
        void removeSocket(Network::Socket socket);
        Object * findPlayerFromSocket(Network::Socket socket);

	inline unsigned int nextId(){
            unsigned int i = id;
            id += 1;
            return i;
	}

        enum Keys{
            Talk,
        };

private:
	std::vector<Network::Socket> & sockets;
	std::vector< Packet > outgoing;
	std::vector< Network::Message > incoming;
	std::vector< pthread_t > threads;
        std::deque<std::string> chatMessages;
        std::map<Object::networkid_t, std::string> clientNames;
        int removeChatTimer;
	std::map<Object*, Network::Socket> characterToClient;
        Object::networkid_t id;

	unsigned int sent_messages;

	pthread_mutex_t message_mutex;
	pthread_mutex_t running_mutex;
	bool running;
        InputMap<Keys> input;

        bool enable_chat;
        TextInput chatInput;
};

#endif
