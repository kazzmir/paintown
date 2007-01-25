#ifndef _animation_event_delay_h
#define _animation_event_delay_h

#include "animation_event.h"

class AnimationEventDelay: public AnimationEvent{
public:
	AnimationEventDelay( int _delay );

	virtual void Interact( Animation * animation );

protected:
	int delay;
};

#endif
