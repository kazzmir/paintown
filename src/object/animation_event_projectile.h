#ifndef _animation_event_projectile_h
#define _animation_event_projectile_h

#include "animation_event.h"

class Animation;
class Projectile;

class AnimationEventProjectile: public AnimationEvent {
public:
	AnimationEventProjectile( Projectile * projectile );

	virtual void Interact( Animation * animation ); 

	virtual ~AnimationEventProjectile();

protected:
	Projectile * projectile;
};

#endif
