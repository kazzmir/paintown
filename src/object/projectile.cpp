#include "projectile.h"
#include "object_attack.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/ebox.h"
#include "util/funcs.h"
#include "world.h"
#include "globals.h"
#include "animation.h"
#include <iostream>

using namespace std;

/* the alliance must be set by someone else at some point */
Projectile::Projectile( Token * token ) throw( LoadException ):
ObjectAttack( ALLIANCE_NONE ),
main( NULL ),
death( NULL ),
dx( 0 ),
dy( 0 ),
life( 0 ){

	if ( *token != "projectile" ){
		throw LoadException( "Token does not start with 'projectile'. Instead it starts with " + token->getName() );
	}

	Token * current;
	while ( token->hasTokens() ){
		try{
			*token >> current;
			if ( *current == "anim" ){
				Animation * animation = new Animation( current, NULL );
				if ( animation->getName() == "main" ){
					main = animation;
				} else if ( animation->getName() == "death" ){
					death = animation;
				} else {
					cout << "Unknown animation for projectile: " + animation->getName() + ". Must be either 'main' or 'death'." << endl;
					delete animation;
				}
			}
		} catch ( const TokenException & e ){
			throw LoadException( "Could not load projectile because " + e.getReason() );
		}
	}

	if ( main == NULL ){
		throw LoadException( "No 'main' animation given" );
	}
	
	currentAnimation = main;

	// nextTicket();
}
	
Projectile::Projectile( const Projectile * const projectile ):
ObjectAttack( projectile->getAlliance() ),
main( new Animation( *projectile->main, NULL ) ),
death( NULL ),
dx( projectile->getDX() ),
dy( projectile->getDY() ),
life( projectile->getLife() ){
	if ( projectile->death != NULL ){
		death = new Animation( *projectile->death, NULL );
	}

	currentAnimation = main;
	
	nextTicket();
}
	
Projectile::~Projectile(){
	if ( main ){
		delete main;
	}
	if ( death ){
		delete death;
	}
}
	
const int Projectile::getHealth() const {
	if ( death ){
		return getLife() <= 0 && death->empty() ? 0 : 1;
	}
	return getLife() <= 0 ? 0 : 1;
}

const int Projectile::getRX() const {
	if ( getFacing() == FACING_LEFT ){
		return Object::getRX() - currentAnimation->getOffsetX();
	} else {
		return Object::getRX() + currentAnimation->getOffsetX();
	}
}

const int Projectile::getRY() const {
	return Object::getRY() + currentAnimation->getOffsetY();
}

void Projectile::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	moveX( getDX() );
	moveY( getDY() );
	if ( currentAnimation->Act() ){
		if ( currentAnimation != death ){
			currentAnimation->reset();
		}
	}

	decreaseLife();
	if ( getLife() <= 0 && death ){
		currentAnimation = death;
		setDX( 0 );
		setDY( 0 );
	}
}

void Projectile::draw( Bitmap * work, int rel_x ){
	if ( getFacing() == Object::FACING_RIGHT ){
		currentAnimation->Draw( getRX() - rel_x, getRY(), work );
	} else {
		currentAnimation->DrawFlipped( getRX() - rel_x, getRY(), work ); 
	}
}

void Projectile::grabbed( Object * obj ){
}

void Projectile::unGrab(){
}
		
ECollide * Projectile::getCollide() const {
	return currentAnimation->getCollide( getFacing() );
}

Object * Projectile::copy(){
	return new Projectile( this );
}
	
Network::Message Projectile::getCreateMessage(){
	Network::Message message;

	message.id = 0;
	message << World::CREATE_PROJECTILE;

	return message;
}

const std::string & Projectile::getAttackName(){
	return currentAnimation->getName(); 
}

bool Projectile::isAttacking(){
	return true;
}

bool Projectile::collision( ObjectAttack * obj ){
	return false;
}

int Projectile::getDamage() const {
	return main->getDamage();
}

bool Projectile::isCollidable( Object * obj ){
	return true;
}

bool Projectile::isGettable(){
	return false;
}

const int Projectile::getWidth() const {
	return currentAnimation->getWidth();
}

const int Projectile::getHeight() const {
	return currentAnimation->getHeight();
}

void Projectile::getAttackCoords( int & x, int & y){
	currentAnimation->getAttackCoords( x, y );
	if ( getFacing() == FACING_LEFT ){
		x = getRX() - x + getWidth() / 2;
	} else {
		x += getRX() - getWidth()/2;
	}

	y += getRY() - getHeight();
}

const double Projectile::minZDistance() const {
	return currentAnimation->getMinZDistance();
}

void Projectile::attacked( Object * something, vector< Object * > & objects ){
	setLife( 0 );
}
