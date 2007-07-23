#include "animation.h"
#include "animation_event.h"
#include "animation_event_jump.h"

AnimationEventJump::AnimationEventJump( double vx, double vy, double vz ):
vx( vx ),
vy( vy ),
vz( vz ){
}

void AnimationEventJump::Interact( Animation * animation ){
	animation->jump( vx, vy, vz );
}
