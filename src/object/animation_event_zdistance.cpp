#include "animation.h"
#include "animation_event.h"
#include "animation_event_zdistance.h"

AnimationEventZDistance::AnimationEventZDistance( const double d ):
d( d ){
}

void AnimationEventZDistance::Interact( Animation * animation ){
	animation->setMinZDistance( d );
}
