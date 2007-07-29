#include "configuration.h"
#include "util/keyboard.h"
#include "object/animation.h"
#include "object/object.h"

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

int Configuration::right = Keyboard::Key_RIGHT;
int Configuration::left = Keyboard::Key_LEFT;
int Configuration::up = Keyboard::Key_UP;
int Configuration::down = Keyboard::Key_DOWN;
int Configuration::attack1 = Keyboard::Key_A;
int Configuration::attack2 = Keyboard::Key_S;
int Configuration::attack3 = Keyboard::Key_D;
int Configuration::jump = Keyboard::Key_SPACE;
