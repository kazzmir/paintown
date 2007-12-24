#include "animation_event_ticket.h"
#include "animation_event.h"
#include "animation.h"

AnimationEventTicket::AnimationEventTicket():
AnimationEvent(){
}

void AnimationEventTicket::Interact( Animation * animation ){
	animation->nextTicket();
}
