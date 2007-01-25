#ifndef _animation_event_bbox_h
#define _animation_event_bbox_h

#include "animation_event.h"

class Animation;

class AnimationEventBBox: public AnimationEvent{
public:
	AnimationEventBBox( int _x1, int _y1, int _x2, int _y2 );

	virtual void Interact( Animation * animation ); 

protected:
	int x1, y1, x2, y2;

};

#endif
