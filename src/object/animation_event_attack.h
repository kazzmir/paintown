#ifndef _animation_event_attack_h
#define _animation_event_attack_h

#include "animation_event.h"
#include "attack.h"

class Animation;

class AnimationEventAttack: public AnimationEvent{
public:
	AnimationEventAttack( const Attack & a );
	// AnimationEventAttack( int _x1, int _y1, int _x2, int _y2, int _damage, int _force );

	virtual void Interact( Animation * animation ); 

protected:

	Attack attack;
	/*
	int x1, y1, x2, y2;
	int damage;
	int force;
	*/

};

#endif
