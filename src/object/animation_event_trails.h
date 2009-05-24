#ifndef _animation_event_trails_h
#define _animation_event_trails_h

#include "animation_event.h"

class Animation;

class AnimationEventTrail: public AnimationEvent {
public:
    AnimationEventTrail(const int produce, const int life);
    virtual void Interact( Animation * animation ); 

protected:
	const int produce;
        const int life;
};

#endif
