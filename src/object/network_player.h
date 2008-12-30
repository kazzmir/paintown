#ifndef _paintown_network_player_h
#define _paintown_network_player_h

#include <string>
#include <vector>
#include <map>
#include "util/load_exception.h"
#include "character.h"

class NetworkPlayer: public NetworkCharacter {
public:

	NetworkPlayer(const std::string & filename, int alliance) throw ( LoadException );
	NetworkPlayer(const Character & chr) throw( LoadException );
	
	virtual ~NetworkPlayer();
	
	virtual Object * copy();

public:
	virtual void draw( Bitmap * work, int rel_x );
	virtual void interpretMessage( Network::Message & m );

	/* logic */
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
protected:
        unsigned int score;
};

#endif
