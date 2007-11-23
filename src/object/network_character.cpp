#include "network_character.h"
#include "object.h"
#include "animation.h"
#include "globals.h"
#include "world.h"
#include <vector>

using namespace std;

NetworkCharacter::NetworkCharacter( int alliance ):
Character( alliance ){
}

NetworkCharacter::NetworkCharacter( const char * filename, int alliance ) throw( LoadException ):
Character( filename, alliance ){
}

NetworkCharacter::NetworkCharacter( const string & filename, int alliance ) throw ( LoadException ):
Character( filename, alliance ){
}

NetworkCharacter::NetworkCharacter( const Character & chr ) throw( LoadException ):
Character( chr ){
}
	
NetworkCharacter::~NetworkCharacter(){
}
	
Object * NetworkCharacter::copy(){
	return new NetworkCharacter( *this );
}

void NetworkCharacter::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	Character::act( others, world, add );
	if ( getStatus() == Status_Ground && animation_current->Act() ){
		animation_current = getMovement( "idle" );
		animation_current->reset();
	}
}
