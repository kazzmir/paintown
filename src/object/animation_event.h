#ifndef _paintown_animation_event_h
#define _paintown_animation_event_h

#include "attack.h"

class Animation;

class AnimationEvent{
public:
    AnimationEvent();
    virtual void Interact( Animation * animation );
    virtual ~AnimationEvent();
};

class Animation;

class AnimationEventAttack: public AnimationEvent{
public:
	AnimationEventAttack( const Attack & a );
	// AnimationEventAttack( int _x1, int _y1, int _x2, int _y2, int _damage, int _force );

	virtual void Interact( Animation * animation ); 

protected:
        Attack attack;
};

#endif
