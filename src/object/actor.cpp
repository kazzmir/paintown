#include "object_nonattack.h"
#include "util/bitmap.h"
#include "actor.h"

Actor::Actor( const string & filename, Stimulation * const stimulation ) throw( LoadException ):
ObjectNonAttack( 0, 0 ){
}

Actor::Actor( const Actor & actor ):
ObjectNonAttack( actor ){
}
	
void Actor::act( vector< Object * > * others, World * world, vector< Object * > * add ){
}

void Actor::draw( Bitmap * work, int rel_x ){
}

bool Actor::isCollidable( Object * obj ){
	return false;
}

bool Actor::isGettable(){
	return false;
}

const int Actor::getWidth() const {
	return 0;
}

const int Actor::getHeight() const {
	return 0;
}

Object * Actor::copy(){
	return new Actor( *this );
}

Actor::~Actor(){
}

