#ifndef _animation_event_status_h
#define _animation_event_status_h

#include "animation_event.h"

class Animation;

class AnimationEventStatus: public AnimationEvent{
public:
	AnimationEventStatus( int _stat );

	virtual void Interact( Animation * animation ); 

protected:
	int status;

};

#endif
