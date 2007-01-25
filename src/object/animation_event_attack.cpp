#include "animation_event_attack.h"
#include "animation_event.h"
#include "animation.h"
#include "attack.h"

AnimationEventAttack::AnimationEventAttack( const Attack & a ){
	attack = a;
}

/*
AnimationEventAttack::AnimationEventAttack( int _x1, int _y1, int _x2, int _y2, int _damage, int _force ):
AnimationEvent(),
x1( _x1 ),
y1( _y1 ),
x2( _x2 ),
y2( _y2 ),
damage( _damage ),
force( _force ){
}
*/

void AnimationEventAttack::Interact( Animation * animation ){
	animation->setAttack( attack );
}
/*
void AnimationEventAttack::Interact( Animation * animation ){
	animation->setAttack( x1, y1, x2, y2 );
	animation->setDamage( damage );
}
*/
