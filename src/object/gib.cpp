#include "util/bitmap.h"
#include <iostream>
#include "object_nonattack.h"
#include "gib.h"
#include "util/funcs.h"
#include "globals.h"
#include <math.h>

Gib::Gib( const int x, const int y, const int z, double dx, double dy, double dz, Bitmap * image ):
ObjectNonAttack( x, z ),
dx(dx),
dy(dy),
dz(dz),
angle( 0 ),
fade( 0 ),
image( image ){
	setY( y );
	setMaxHealth( 1 );
	setHealth( 1 );
}

Gib::Gib( const Gib & g ):
ObjectNonAttack( g ){
}

void Gib::draw( Bitmap * work, int rel_x ){
	if ( fade > 0 ){
		// Bitmap::dissolveBlender( 0, 0, 0, 255 - fade );
		Bitmap::transBlender( 0, 0, 0, 255 - fade );
		image->drawTrans( getRX() - rel_x - image->getWidth() / 2, getRY() - image->getHeight() / 2, *work );
	} else {
		for ( std::vector< Point >::iterator it = blood.begin(); it != blood.end(); it++ ){
			const Point & p = *it;
			int l = 200 + p.life * 15;
			int red = Bitmap::makeColor( l > 255 ? 255 : l, 0, 0 );
			work->circleFill( p.x - rel_x, p.y, 1, red );
			// work->putPixel( p.x - rel_x, p.y, red );
		}
		// image->draw( getRX() - rel_x - image->getWidth() / 2, getRY() - image->getHeight() / 2, *work );
		image->drawRotate( getRX() - rel_x - image->getWidth() / 2, getRY() - image->getHeight() / 2, angle, *work );
	}
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

int Gib::getWidth() const {
	return 1;
}

int Gib::getHeight() const {
	return 1;
}
	
Network::Message Gib::getCreateMessage(){
	Network::Message m;

	return m;
}
	
void Gib::act( std::vector< Object * > * others, World * world, std::vector< Object * > * add ){
	if ( fade > 0 ){
		fade += 2;
		if ( fade > 255 ){
			setHealth( -1 );
		}
	} else {

		moveX(dx);
		moveY(dy);
                moveZ(dz);

		dy -= 0.1;
		if ( getY() <= 0 ){
			dy = -dy / 2;
			dx = dx / 2;
                        dz = dz / 2;
			if ( fade == 0 && fabs( dy ) < 0.1 ){
				fade = 1;
			}

			/*
			   if ( fabs( dy ) < 0.1 ){
			   setHealth( -1 );
			   }
			   */
		}

		for ( int i = 0; i < 3; i++ ){
			int x = getRX() + Util::rnd( 5 ) - 2;
			int y = getRY() + Util::rnd( 5 ) - 2;
			blood.push_back( Point( x, y, 10 ) );
		}

		for ( std::vector< Point >::iterator it = blood.begin(); it != blood.end(); ){
			Point & p = *it;
			p.life -= 1;
			if ( p.life <= 0 ){
				it = blood.erase( it );
			} else {
				it++;
			}
		}

		angle += (int) sqrt( dx * dx + dy * dy ) * 3;
	}
}

Gib::~Gib(){
}
