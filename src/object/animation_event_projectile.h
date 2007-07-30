#ifndef _animation_event_projectile_h
#define _animation_event_projectile_h

#include "animation_event.h"
#include "util/load_exception.h"

class Animation;
class Token;
class Projectile;

class AnimationEventProjectile: public AnimationEvent {
public:
	AnimationEventProjectile( Token * token ) throw( LoadException );

	virtual void Interact( Animation * animation ); 

	virtual ~AnimationEventProjectile();

protected:
	inline void setX( int x ){
		this->x = x;
	}

	inline void setY( int y ){
		this->y = y;
	}

	inline const int getX() const {
		return x;
	}

	inline const int getY() const {
		return y;
	}

	inline void setDX( double dx ){
		this->dx = dx;
	}

	inline void setDY( double dy ){
		this->dy = dy;
	}

	inline const double getDX() const {
		return this->dx;
	}
	
	inline const double getDY() const {
		return this->dy;
	}

	inline void setLife( int life ){
		this->life = life;
	}

	inline const int getLife() const {
		return life;
	}

	Projectile * projectile;
	int x, y;
	double dx, dy;
	int life;
};

#endif
