#ifndef _paintown_network_world_h
#define _paintown_network_world_h

#include "network.h"
#include "game/adventure_world.h"
#include "util/load_exception.h"
#include "object/object.h"
#include <pthread.h>
#include <vector>

struct Packet{
	Packet( const Network::Message & m, Network::Socket s ):message(m),socket(s){}
	Network::Message message;
	Network::Socket socket;
};

class NetworkWorld: public AdventureWorld {
public:
	NetworkWorld( const std::vector< NLsocket > & sockets, const std::vector< Object * > & players, const string & path, int screen_size = 320 ) throw ( LoadException );
	
	virtual void addMessage( Network::Message m, Network::Socket from = 0 );
	virtual void act();
	virtual void doScene( int min_x, int max_x );
	
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

protected:
	Object * findNetworkObject( Object::networkid_t id );
	void sendMessage( const Network::Message & message, NLsocket socket );
	vector< Network::Message > getIncomingMessages();
	void handleMessage( Network::Message & message );

	Network::Message nextBlockMessage( int block );


	inline unsigned int nextId(){
		unsigned int i = id;
		id += 1;
		return i;
	}

private:
	std::vector< NLsocket > sockets;
	std::vector< Packet > outgoing;
	std::vector< Network::Message > incoming;
	std::vector< pthread_t > threads;
        Object::networkid_t id;

	unsigned int sent_messages;

	pthread_mutex_t message_mutex;
	pthread_mutex_t running_mutex;
	bool running;
};

#endif
