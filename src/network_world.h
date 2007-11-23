#ifndef _paintown_network_world_h
#define _paintown_network_world_h

#include "network.h"
#include "world.h"
#include "util/load_exception.h"
#include <vector>

class NetworkWorld: public World{
public:
	NetworkWorld( const std::vector< NLsocket > & sockets, const std::vector< Object * > & players, const string & path, int screen_size = 320 ) throw ( LoadException );
	
	virtual void addMessage( Network::Message m );
	virtual void act();
	virtual void doScene( int min_x, int max_x );

protected:
	void sendMessage( const Network::Message & message, NLsocket socket );

	inline unsigned int nextId(){
		unsigned int i = id;
		id += 1;
		return i;
	}

private:
	std::vector< NLsocket > sockets;
	std::vector< Network::Message > messages;
	unsigned int id;

};

#endif
