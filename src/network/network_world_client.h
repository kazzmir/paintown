#ifndef _paintown_network_world_client_h
#define _paintown_network_world_client_h

#include <pthread.h>
#include "game/adventure_world.h"
#include "network.h"
#include "object/object.h"
#include "util/load_exception.h"

class NetworkWorldClient: public AdventureWorld {
public:

	NetworkWorldClient( Network::Socket server, const std::vector< Object * > & players, const string & path, Object::networkid_t id, int screen_size = 320 ) throw ( LoadException );
	
	virtual void act();

	void addIncomingMessage( const Network::Message & message );

	inline NLint getServer(){
		return server;
	}

	inline pthread_mutex_t * getLock(){
		return &message_mutex;
	}

        /* start thread to handle messages */
        void startMessageHandler();
	
	virtual void doScene( int min_x, int max_x );
	virtual void addMessage( Network::Message m, Network::Socket from = 0 );

	virtual ~NetworkWorldClient();

	bool isRunning();
	void stopRunning();
	
	virtual const bool finished() const;

protected:

	Object * findNetworkObject( Object::networkid_t id );
	void handleCreateCharacter( Network::Message & message );
	void handleCreateCat( Network::Message & message );
	void handleCreateBang( Network::Message & message );
	void handleCreateItem( Network::Message & message );
	Object * removeObject( Object::networkid_t id );

	void sendMessage( const Network::Message & message, NLsocket socket );
        void sendMessages(const vector<Network::Message> & messages, Network::Socket socket);

	bool uniqueObject( Object::networkid_t id );
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
        Object::networkid_t id;

	bool running;
};

#endif
