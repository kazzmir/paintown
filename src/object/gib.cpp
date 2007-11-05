#include "object_nonattack.h"
#include "gib.h"
#include "util/bitmap.h"
#include "globals.h"
#include <math.h>

Gib::Gib( const int x, const int y, const int z, double dx, double dy ):
ObjectNonAttack( x, z ),
dx( dx ),
dy( dy ){
	setY( y );
	setMaxHealth( 1 );
	setHealth( 1 );
}

Gib::Gib( const Gib & g ):
ObjectNonAttack( g ){
}

void Gib::draw( Bitmap * work, int rel_x ){
	work->circleFill( getRX() - rel_x, getRY(), 2, Bitmap::makeColor( 255, 0, 0 ) );
}
	
Object * Gib::copy(){
	return new Gib( *this );
}

bool Gib::isCollidable( Object * obj ){
	return false;
}

bool Gib::isGettable(){
	return false;
}

const int Gib::getWidth() const {
	return 1;
}

const int Gib::getHeight() const {
	return 1;
}
	
void Gib::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	moveX( dx );
	moveY( dy );
	
	dy -= 0.1;
	if ( getY() <= 0 ){
		dy = -dy / 2;
		dx = dx / 2;
		if ( fabs( dy ) < 0.1 ){
			setHealth( -1 );
		}
	}
}

Gib::~Gib(){
}
