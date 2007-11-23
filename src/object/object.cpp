#include "object.h"
#include "object_attack.h"
#include "stimulation.h"
#include "util/ebox.h"
#include <math.h>
#include <exception>
#include <iostream>

using namespace std;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

/*
const int Object::FACING_LEFT = 0; 
const int Object::FACING_RIGHT = 1;
*/

Object::Object( int alliance ):
actualx( 0 ),
actualy( 0 ),
actualz( 0 ),
virtualx( 0 ),
virtualy( 0 ),
virtualz( 0 ),
health( 0 ),
max_health( 0 ),
damage( 0 ),
facing( FACING_RIGHT ),
alliance( alliance ),
id( 0 ){
}

Object::Object( const int x, const int y, int _alliance ):
actualx( x ),
actualy( 0 ),
actualz( y ),
virtualx( (double)x ),
virtualy( 0 ),
virtualz( (double)y ),
health( 0 ),
max_health( 0 ),
damage( 0 ),
facing( FACING_RIGHT ),
alliance( _alliance ),
id( 0 ){
}

Object::Object( const Object & copy ):
damage( 0 ){
	actualx = copy.actualx;
	actualy = copy.actualy;
	actualz = copy.actualz;
	virtualx = copy.virtualx;
	virtualy = copy.virtualy;
	virtualz = copy.virtualz;
	facing = copy.facing;
	health = copy.health;
	hit = copy.hit;
	id = copy.id;
	setMaxHealth( copy.getMaxHealth() );
	setAlliance( copy.getAlliance() );
}
	
double Object::ZDistance( const Object * obj ){
	return fabs( obj->getZ() - getZ() );
	/*
	if ( obj->getZ() > getZ() )
		return obj->getZ() - getZ();
	return getZ() - obj->getZ();
	*/
}
	
double Object::XDistance( const Object * obj ){
	if ( obj->getX() > getX() )
		return obj->getX() - getX();
	return getX() - obj->getX();
}

/* move in an absolute direction */
void Object::moveRight( const int x ){
	moveX( FACING_RIGHT, x );
}

void Object::moveLeft( const int x ){
	moveX( FACING_LEFT, x );
}
	
void Object::stimulate( const Stimulation & stim ){
	stim.stimulate( *this );
}
	
void Object::moveX( int dir, const int x ){
	if ( dir == getFacing() ){
		moveX( x );
	} else {
		moveX( -x );
	}
}

void Object::faceObject( const Object * o ){
	if ( o->getX() < getX() ){
		setFacing( Object::FACING_LEFT );
	} else {
		setFacing( Object::FACING_RIGHT );
	}
}

void Object::thrown(){
}
	
void Object::moveX( const int x ){
	moveX( (double)x );
}

void Object::died( vector< Object * > & objects ){
}

void Object::moveY( const int y ){
	moveY( (double)y );
}

void Object::moveZ( const int z ){
	moveZ( (double)z );
}

void Object::moveX( double x ){

	if ( getFacing() == FACING_LEFT ){
		virtualx -= x;
	} else {
		virtualx += x;
	}

	actualx = (int)virtualx;
}

void Object::moveY( double y ){
	virtualy += y;
	actualy = (int)virtualy;
	if ( actualy < 0 ){
		virtualy = 0;
		actualy = 0;
	}
}
	
const int Object::getAlliance() const{
	return alliance;
}
	
void Object::collided( ObjectAttack * obj, vector< Object * > & objects ){
	hit.play();
}

void Object::moveZ( double z ){
	virtualz += z;
	actualz = (int)virtualz;
}

void Object::print() const{
}
	
void Object::fall( double x_vel, double y_vel ){
}
	
void Object::hurt( int x ){
	setHealth( getHealth() - x );
}
	
void Object::reduceDamage( const double much ){
	damage -= much;
	if ( damage < 0 ){
		damage = 0;
	}
}
	
void Object::takeDamage( ObjectAttack * obj, int x ){
	this->hurt( x );
	damage += x;
	// health -= x;
}
	
void Object::touch( Object * obj ){
}

ECollide * Object::getCollide() const {
	return 0;
}

Object::~Object(){
}
