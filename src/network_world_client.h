#ifndef _paintown_network_world_client_h
#define _paintown_network_world_client_h

#include <pthread.h>
#include "world.h"
#include "network.h"
#include "util/load_exception.h"

class NetworkWorldClient: public World{
public:

	NetworkWorldClient( NLsocket server, const std::vector< Object * > & players, const string & path, int screen_size = 320 ) throw ( LoadException );
	
	virtual void act();

	void addIncomingMessage( const Network::Message & message );

	inline NLint getServer(){
		return server;
	}

	inline pthread_mutex_t * getLock(){
		return &message_mutex;
	}

protected:

	void handleMessage( Network::Message & message );
	vector< Network::Message > getIncomingMessages();

private:
	NLsocket server;
	std::vector< Network::Message > incoming;
	pthread_mutex_t message_mutex;
	pthread_t message_thread;
};

#endif
