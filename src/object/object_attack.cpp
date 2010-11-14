#include "object.h"
#include "object_attack.h"

static unsigned int ticket = 1;

namespace Paintown{

ObjectAttack::ObjectAttack( int alliance ):
Object( alliance ),
attack_ticket( ticket++ ){
}

ObjectAttack::ObjectAttack( const int x, const int y, int alliance ):
Object( x, y, alliance ),
attack_ticket( ticket++ ){
}


ObjectAttack::ObjectAttack( const ObjectAttack & copy ):
Object( copy ),
attack_ticket( ticket++ ){
}

void ObjectAttack::nextTicket(){
	attack_ticket++;
}

}
