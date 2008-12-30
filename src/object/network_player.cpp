#include "util/bitmap.h"
#include "network_character.h"
#include "network_player.h"
#include "object.h"
#include "object_messages.h"
#include "animation.h"
#include "util/font.h"
#include "util/funcs.h"
#include "factory/font_factory.h"
#include "factory/font_render.h"
#include "nameplacer.h"
#include "globals.h"
#include "world.h"
#include <vector>
#include <math.h>

using namespace std;

NetworkPlayer::NetworkPlayer(const string & filename, int alliance) throw ( LoadException ):
NetworkCharacter( filename, alliance ){
}

NetworkPlayer::NetworkPlayer(const Character & chr) throw( LoadException ):
NetworkCharacter( chr ){
}

NetworkPlayer::~NetworkPlayer(){
}
	
Object * NetworkPlayer::copy(){
	return new NetworkPlayer(*this);
}

void NetworkPlayer::interpretMessage( Network::Message & message ){
	int type;
	message >> type;
        message.reset();
        NetworkCharacter::interpretMessage(message);
}

void NetworkPlayer::draw( Bitmap * work, int rel_x ){
    NetworkCharacter::draw(work, rel_x);
}

/* just performs the current animation */
void NetworkPlayer::act( vector< Object * > * others, World * world, vector< Object * > * add ){
    NetworkCharacter::act(others, world, add);
}
