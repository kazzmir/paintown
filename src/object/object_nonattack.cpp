#include "object.h"
#include "globals.h"
#include "object_attack.h"
#include "object_nonattack.h"
#include <string>

ObjectNonAttack::ObjectNonAttack( int alliance = ALLIANCE_NONE ):
Object( alliance ){
	no_name = "";
}
	
ObjectNonAttack::ObjectNonAttack( const ObjectNonAttack & obj ):
Object( obj ){
}
	
bool ObjectNonAttack::collision( ObjectAttack * obj ){
	return false;
}

void ObjectNonAttack::collided( ObjectAttack * obj ){
}
	
void ObjectNonAttack::grabbed( Object * obj ){
}
	
void ObjectNonAttack::unGrab(){
}

int ObjectNonAttack::getDamage() const{
	return 0;
}

bool ObjectNonAttack::isAttacking(){
	return false;
}

const std::string & ObjectNonAttack::getAttackName(){
	return no_name;
}
