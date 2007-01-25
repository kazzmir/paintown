#include "animation.h"
#include "animation_event.h"
#include "animation_event_move.h"

AnimationEventMove::AnimationEventMove( int _x, int _y, int _z ):
x( _x ),
y( _y ),
z( _z ){
}

void AnimationEventMove::Interact( Animation * animation ){
	animation->moveX( x );
	animation->moveY( y );
	animation->moveZ( z );
}
