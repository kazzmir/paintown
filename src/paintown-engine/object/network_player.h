#ifndef _paintown_network_player_h
#define _paintown_network_player_h

#include <string>
#include <vector>
#include <map>
#include "util/load_exception.h"
#include "util/graphics/bitmap.h"
#include "character.h"
#include "network_character.h"

namespace Paintown{

class NetworkPlayer: public NetworkCharacter {
public:

	NetworkPlayer(const Filesystem::AbsolutePath & filename, int alliance);
	NetworkPlayer(const Character & chr);
	
	virtual ~NetworkPlayer();
	
	virtual Object * copy();

public:
	virtual void drawFront( Graphics::Bitmap * work, int rel_x );
	virtual void interpretMessage(World * world, Network::Message & m );
        virtual void attacked( World * world, Object * something, std::vector< Object * > & objects );

	/* logic */
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

        virtual inline bool confirm() const {
            return need_confirm;
        }

        void needConfirm();
	
        virtual void collided(World * world, ObjectAttack * obj, std::vector< Object * > & objects );
	
protected:
        void initializeAttackGradient();
	virtual Network::Message scoreMessage();

protected:
        unsigned int score;
        double attack_bonus;
        static const int num_attack_gradient = 50;
        Graphics::Color attack_gradient[num_attack_gradient];
        bool need_confirm;
        bool need_confirm_message;
};

}

#endif
