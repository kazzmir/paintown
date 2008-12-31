#ifndef _paintown_projectile_h
#define _paintown_projectile_h

#include "object_attack.h"
#include "util/load_exception.h"
#include "network/network.h"

class Token;
class Animation;

class Projectile: public ObjectAttack {
public:
	Projectile( Token * token ) throw( LoadException );
	Projectile( const Projectile * projectile );
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x );
	virtual void grabbed( Object * obj );
	virtual void unGrab();
	virtual bool isGrabbed();
        virtual bool isGrabbable(Object * obj);
	virtual Object * copy();
	virtual const std::string & getAttackName();
	virtual bool isAttacking();
	virtual bool collision( ObjectAttack * obj );
	virtual int getDamage() const;
	virtual ECollide * getCollide() const;
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual const int getWidth() const;
	virtual const int getHeight() const;
	virtual void getAttackCoords( int & x, int & y);
	virtual const double minZDistance() const;
	virtual void attacked(World * world, Object * something, vector< Object * > & objects );
	virtual Network::Message getCreateMessage();

	virtual const int getRY() const;
	virtual const int getRX() const;

	virtual ~Projectile();

	inline void setDX( const double dx ){
		this->dx = dx;
	}

	inline void setDY( const double dy ){
		this->dy = dy;
	}

	inline const double getDX() const {
		return dx;
	}

	inline const double getDY() const {
		return dy;
	}

	inline void setLife( int life ){
		this->life = life;
	}

	inline const int getLife() const {
		return life;
	}

	inline void decreaseLife(){
		life -= 1;
	}
	
	virtual const int getHealth() const;

protected:
	Animation * main;
	Animation * death;
	Animation * currentAnimation;

	double dx, dy;
	int life;
};

#endif
