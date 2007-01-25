#ifndef _animation_event_nop_h
#define _animation_event_nop_h

#include "animation_event.h"

class Animation;

class AnimationEventNOP: public AnimationEvent{
public:
	AnimationEventNOP();

	virtual void Interact( Animation * animation ); 

};

#endif
