#ifndef _animation_event_jump_h
#define _animation_event_jump_h

#include "animation_event.h"

class Animation;

class AnimationEventJump: public AnimationEvent{
public:
	AnimationEventJump( double vx, double vy, double vz );

	virtual void Interact( Animation * animation ); 

protected:
	double vx, vy, vz;

};

#endif
