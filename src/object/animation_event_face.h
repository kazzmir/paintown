#ifndef _animation_event_face_h
#define _animation_event_face_h

#include "animation_event.h"

class Animation;

class AnimationEventFace: public AnimationEvent{
public:
	AnimationEventFace( int direction );

	virtual void Interact( Animation * animation ); 

protected:
	int direction;

};

#endif
