#include "network_character.h"
#include "object.h"
#include "animation.h"
#include "globals.h"
#include "world.h"
#include <vector>
#include <math.h>

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
	
Network::Message NetworkCharacter::grabMessage( unsigned int from, unsigned int who ){
	Network::Message message;
	message.id = 0;
	message << World::IGNORE;
	return message;
}

void NetworkCharacter::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	Global::debug( 2 ) << getId() << " status is " << getStatus() << endl;
	Character::act( others, world, add );
	if ( (getStatus() == Status_Ground ||
	      getStatus() == Status_Jumping) && animation_current->Act() ){
	      	// Global::debug( 0 ) << "Reset animation" << endl;
		if ( animation_current->getName() != "idle" &&
		     animation_current->getName() != "walk" ){
			animation_current = getMovement( "idle" );
		}
		animation_current->reset();
	}
}
	
void NetworkCharacter::landed( World * world ){
	setThrown( false );
	switch( getStatus() ){
		case Status_Falling : {
			if ( landed_sound ){
				landed_sound->play();
			}

			world->Quake( (int)fabs(getYVelocity()) );

			break;
		}
		case Status_Fell : {
			world->Quake( (int)fabs(getYVelocity()) );
			break;
		}
	}

}
	
void NetworkCharacter::deathReset(){
	setY( 200 );
	setMoving( true );
	setStatus( Status_Falling );
	setHealth( getMaxHealth() );
	setInvincibility( 400 );
	setDeath( 0 );
	animation_current = getMovement( "idle" );

}
