#ifndef _paintown_network_world_h
#define _paintown_network_world_h

#include "network.h"
#include "world.h"
#include "util/load_exception.h"
#include <pthread.h>
#include <vector>

class NetworkWorld: public World{
public:
	NetworkWorld( const std::vector< NLsocket > & sockets, const std::vector< Object * > & players, const string & path, int screen_size = 320 ) throw ( LoadException );
	
	virtual void addMessage( Network::Message m );
	virtual void act();
	virtual void doScene( int min_x, int max_x );
	
	void addIncomingMessage( const Network::Message & message );

	Network::Message finishMessage();

	bool isRunning();

	void flushOutgoing();

	virtual ~NetworkWorld();

protected:
	void sendMessage( const Network::Message & message, NLsocket socket );
	vector< Network::Message > getIncomingMessages();
	void handleMessage( Network::Message & message );

	Network::Message nextBlockMessage( int block );

	void stopRunning();

	inline unsigned int nextId(){
		unsigned int i = id;
		id += 1;
		return i;
	}

private:
	std::vector< NLsocket > sockets;
	std::vector< Network::Message > outgoing;
	std::vector< Network::Message > incoming;
	std::vector< pthread_t > threads;
	unsigned int id;

	unsigned int sent_messages;

	pthread_mutex_t message_mutex;
	pthread_mutex_t running_mutex;
	bool running;
};

#endif
