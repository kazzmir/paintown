#ifndef _animation_event_ticket_h
#define _animation_event_ticket_h

#include "animation_event.h"

class Animation;

class AnimationEventTicket: public AnimationEvent{
public:
	AnimationEventTicket();

	virtual void Interact( Animation * animation ); 

};

#endif
