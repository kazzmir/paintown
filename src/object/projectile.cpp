#include "projectile.h"
#include "object_attack.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "globals.h"
#include "animation.h"

/* the alliance must be set by someone else at some point */
Projectile::Projectile( Token * token ) throw( LoadException ):
ObjectAttack( ALLIANCE_NONE ),
main( NULL ),
death( NULL ){
}
	
Projectile::Projectile( const Projectile * projectile ):
ObjectAttack( projectile->getAlliance() ),
main( NULL ),
death( NULL ){
}
	
Projectile::~Projectile(){
	if ( main ){
		delete main;
	}
	if ( death ){
		delete death;
	}
}

void Projectile::act( vector< Object * > * others, World * world, vector< Object * > * add ){
}

void Projectile::draw( Bitmap * work, int rel_x ){
}

void Projectile::grabbed( Object * obj ){
}

void Projectile::unGrab(){
}

Object * Projectile::copy(){
	return new Projectile( this );
}

const std::string & Projectile::getAttackName(){
	return name;
}

bool Projectile::isAttacking(){
	return true;
}

bool Projectile::collision( ObjectAttack * obj ){
	return false;
}

int Projectile::getDamage() const {
	return 0;
}

bool Projectile::isCollidable( Object * obj ){
	return true;
}

bool Projectile::isGettable(){
	return false;
}

const int Projectile::getWidth() const {
	return 0;
}

const int Projectile::getHeight() const {
	return 0;
}

void Projectile::getAttackCoords( int & x, int & y){
}

const double Projectile::minZDistance() const {
	return 0;
}

void Projectile::attacked( Object * something, vector< Object * > & objects ){
}
