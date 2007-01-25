#ifndef _animation_event_move_h
#define _animation_event_move_h

#include "animation_event.h"

class Animation;

class AnimationEventMove: public AnimationEvent{
public:
	AnimationEventMove( int _x, int _y, int _z );

	virtual void Interact( Animation * animation ); 

protected:
	int x, y, z;

};

#endif
