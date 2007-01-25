#include "animation.h"
#include "animation_event.h"
#include "animation_event_status.h"

AnimationEventStatus::AnimationEventStatus( int _stat ):
AnimationEvent(),
status( _stat ){
}

void AnimationEventStatus::Interact( Animation * animation ){
	animation->setStatus( status );
}
