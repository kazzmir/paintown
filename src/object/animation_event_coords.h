#ifndef _animation_event_coords_h
#define _animation_event_coords_h

#include "animation_event.h"

class Animation;

class AnimationEventCoords: public AnimationEvent{
public:
	AnimationEventCoords( int _x, int _y, int _z );

	virtual void Interact( Animation * animation ); 

protected:
	int x, y, z;

};

#endif
