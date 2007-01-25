#include "animation.h"
#include "animation_event.h"
#include "animation_event_offset.h"

AnimationEventOffset::AnimationEventOffset( int _x, int _y ):
AnimationEvent(),
x( _x ),
y( _y ){
}

void AnimationEventOffset::Interact( Animation * animation ){
	animation->setOffsetX( x );
	animation->setOffsetY( y );
}
