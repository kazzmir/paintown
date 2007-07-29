#include "configuration.h"
#include "util/keyboard.h"
#include "object/animation.h"
#include "object/object.h"

static int right = Keyboard::Key_RIGHT;
static int left = Keyboard::Key_LEFT;
static int up = Keyboard::Key_UP;
static int down = Keyboard::Key_DOWN;
static int attack1 = Keyboard::Key_A;
static int attack2 = Keyboard::Key_S;
static int attack3 = Keyboard::Key_D;
static int jump = Keyboard::Key_SPACE;

int Configuration::getKey( int which, int facing ){
	switch( which ){
		case PAIN_KEY_FORWARD : {
			if ( facing == Object::FACING_LEFT ) 
				return left;
			else 	return right;
		}
		case PAIN_KEY_BACK : {
			if ( facing == Object::FACING_LEFT )
				return right;
			else	return left;
		}
		case PAIN_KEY_UP : return up;
		case PAIN_KEY_DOWN : return down;
		case PAIN_KEY_ATTACK1 : return attack1;
		case PAIN_KEY_ATTACK2 : return attack2;
		case PAIN_KEY_ATTACK3 : return attack3;
		case PAIN_KEY_JUMP : return jump;
		default : return -1;
	}
}


void Configuration::setRight( int i ){
	right = i;
}

void Configuration::setLeft( int i ){
	left = i;
}

void Configuration::setUp( int i ){
	up = i;
}

void Configuration::setDown( int i ){
	down = i;	
}

void Configuration::setAttack1( int i ){
	attack1 = i;
}

void Configuration::setAttack2( int i ){
	attack2 = i;
}

void Configuration::setAttack3( int i ){
	attack3 = i;
}

void Configuration::setJump( int i ){
	jump = i;
}

int Configuration::getRight(){
	return right;
}

int Configuration::getLeft(){
	return left;
}

int Configuration::getUp(){
	return up;
}

int Configuration::getDown(){
	return down;
}

int Configuration::getAttack1(){
	return attack1;
}

int Configuration::getAttack2(){
	return attack2;
}

int Configuration::getAttack3(){
	return attack3;
}

int Configuration::getJump(){
	return jump;
}
