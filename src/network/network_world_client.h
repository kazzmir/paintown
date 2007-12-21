#ifndef _paintown_network_world_client_h
#define _paintown_network_world_client_h

#include <pthread.h>
#include "world.h"
#include "network.h"
#include "util/load_exception.h"

class NetworkWorldClient: public World{
public:

	NetworkWorldClient( Network::Socket server, const std::vector< Object * > & players, const string & path, unsigned int id, int screen_size = 320 ) throw ( LoadException );
	
	virtual void act();

	void addIncomingMessage( const Network::Message & message );

	inline NLint getServer(){
		return server;
	}

	inline pthread_mutex_t * getLock(){
		return &message_mutex;
	}
	
	virtual void doScene( int min_x, int max_x );
	virtual void addMessage( Network::Message m );

	virtual ~NetworkWorldClient();

	bool isRunning();
	void stopRunning();
	
	virtual const bool finished() const;

protected:

	void handleCreateCharacter( Network::Message & message );
	void handleCreateCat( Network::Message & message );
	void handleCreateBang( Network::Message & message );
	void removeObject( unsigned int id );

	void sendMessage( const Network::Message & message, NLsocket socket );

	bool uniqueObject( unsigned int id );
	void handleMessage( Network::Message & message );
	vector< Network::Message > getIncomingMessages();

private:
	NLsocket server;
	std::vector< Network::Message > incoming;
	std::vector< Network::Message > outgoing;
	pthread_mutex_t message_mutex;
	pthread_mutex_t running_mutex;
	pthread_t message_thread;

	bool world_finished;
	unsigned int id;

	bool running;
};

#endif
