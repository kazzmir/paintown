#include "object.h"
#include "object_attack.h"

ObjectAttack::ObjectAttack( int alliance ):
Object( alliance ),
attack_ticket( 0 ){
}

ObjectAttack::ObjectAttack( const int x, const int y, int alliance ):
Object( x, y, alliance ),
attack_ticket( 0 ){
}


ObjectAttack::ObjectAttack( const ObjectAttack & copy ):
Object( copy ),
attack_ticket( 0 ){
}

void ObjectAttack::nextTicket(){
	attack_ticket++;
}
