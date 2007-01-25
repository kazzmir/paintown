#include "animation.h"
#include "animation_event.h"
#include "animation_event_nop.h"

AnimationEventNOP::AnimationEventNOP():
AnimationEvent(){
}

void AnimationEventNOP::Interact( Animation * animation ){
	animation->Delay();
}
