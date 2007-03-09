#include "animation.h"
#include "animation_event.h"
#include "animation_event_shadow.h"

AnimationEventShadow::AnimationEventShadow( int x, int y ):
AnimationEvent(),
x( x ),
y( y ){
}

void AnimationEventShadow::Interact( Animation * animation ){
	animation->setShadowX( x );
	animation->setShadowY( y );
}
