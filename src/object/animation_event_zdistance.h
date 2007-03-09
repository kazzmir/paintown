#ifndef _animation_event_zdistance_h
#define _animation_event_zdistance_h

#include "animation_event.h"

class Animation;

class AnimationEventZDistance: public AnimationEvent {
public:
	AnimationEventZDistance( const double d );

	virtual void Interact( Animation * animation ); 

protected:
	double d;
};

#endif
