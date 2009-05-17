#ifndef _paintown_network_player_h
#define _paintown_network_player_h

#include <string>
#include <vector>
#include <map>
#include "util/load_exception.h"
#include "character.h"
#include "network_character.h"

class NetworkPlayer: public NetworkCharacter {
public:

	NetworkPlayer(const std::string & filename, int alliance) throw ( LoadException );
	NetworkPlayer(const Character & chr) throw( LoadException );
	
	virtual ~NetworkPlayer();
	
	virtual Object * copy();

public:
	virtual void drawFront( Bitmap * work, int rel_x );
	virtual void interpretMessage( Network::Message & m );
        virtual void attacked( World * world, Object * something, std::vector< Object * > & objects );

	/* logic */
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

protected:
        void initializeAttackGradient();
	virtual Network::Message scoreMessage();

protected:
        unsigned int score;
        double attack_bonus;
        static const int num_attack_gradient = 50;
        int attack_gradient[num_attack_gradient];
};

#endif
