#include "object.h"
#include "globals.h"
#include "object_attack.h"
#include "object_nonattack.h"
#include <string>

namespace Paintown{

ObjectNonAttack::ObjectNonAttack( const int x, const int y, int alliance ):
Object( x, y, alliance ){
	no_name = "";
}
	
ObjectNonAttack::ObjectNonAttack( const ObjectNonAttack & obj ):
Object( obj ){
}
	
bool ObjectNonAttack::collision( ObjectAttack * obj ){
	return false;
}

void ObjectNonAttack::grabbed( Object * obj ){
}

bool ObjectNonAttack::isGrabbable(Object * obj){
    return false;
}
	
bool ObjectNonAttack::isGrabbed(){
	return false;
}
	
void ObjectNonAttack::unGrab(){
}

int ObjectNonAttack::getDamage() const {
    return 0;
}

double ObjectNonAttack::getForceX() const {
    return 0;
}

double ObjectNonAttack::getForceY() const {
    return 0;
}

bool ObjectNonAttack::isAttacking(){
	return false;
}

const std::string ObjectNonAttack::getAttackName(){
    return no_name;
}

}
