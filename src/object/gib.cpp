#include <iostream>
#include "object_nonattack.h"
#include "gib.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "globals.h"
#include <math.h>

Gib::Gib( const int x, const int y, const int z, double dx, double dy, Bitmap * image ):
ObjectNonAttack( x, z ),
dx( dx ),
dy( dy ),
image( image ){
	setY( y );
	setMaxHealth( 1 );
	setHealth( 1 );
}

Gib::Gib( const Gib & g ):
ObjectNonAttack( g ){
}

void Gib::draw( Bitmap * work, int rel_x ){
	for ( vector< Point >::iterator it = blood.begin(); it != blood.end(); it++ ){
		const Point & p = *it;
		int l = 200 + p.life * 15;
		int red = Bitmap::makeColor( l > 255 ? 255 : l, 0, 0 );
		work->circleFill( p.x - rel_x, p.y, 1, red );
		// work->putPixel( p.x - rel_x, p.y, red );
	}
	image->draw( getRX() - rel_x - image->getWidth() / 2, getRY() - image->getHeight() / 2, *work );
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

	for ( int i = 0; i < 3; i++ ){
		int x = getRX() + Util::rnd( 5 ) - 2;
		int y = getRY() + Util::rnd( 5 ) - 2;
		blood.push_back( Point( x, y, 10 ) );
	}

	for ( vector< Point >::iterator it = blood.begin(); it != blood.end(); ){
		Point & p = *it;
		p.life -= 1;
		if ( p.life <= 0 ){
			it = blood.erase( it );
		} else {
			it++;
		}
	}
}

Gib::~Gib(){
}
