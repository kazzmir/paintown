#include "animation.h"
#include "animation_event.h"
#include "animation_event_face.h"

AnimationEventFace::AnimationEventFace( int direction ):
direction( direction ){
}

void AnimationEventFace::Interact( Animation * animation ){
	animation->setFacing( direction );
}
