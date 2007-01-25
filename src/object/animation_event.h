#ifndef _animation_event_h
#define _animation_event_h

class Animation;

class AnimationEvent{
public:
	AnimationEvent();

	virtual void Interact( Animation * animation );

};

#endif
