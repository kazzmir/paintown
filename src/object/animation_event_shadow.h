#ifndef _animation_event_shadow_h
#define _animation_event_shadow_h

#include "animation_event.h"

class Animation;

class AnimationEventShadow: public AnimationEvent {
public:
	AnimationEventShadow( int x, int y );

	virtual void Interact( Animation * animation ); 

protected:

	int x, y;
};

#endif
