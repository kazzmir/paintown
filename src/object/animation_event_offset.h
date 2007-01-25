#ifndef _animation_event_offset_h
#define _animation_event_offset_h

#include "animation_event.h"

class Animation;

class AnimationEventOffset: public AnimationEvent{
public:
	AnimationEventOffset( int _x, int _y );

	virtual void Interact( Animation * animation ); 

protected:
	int x, y;

};

#endif
