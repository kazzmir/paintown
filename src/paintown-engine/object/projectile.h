#ifndef _paintown_projectile_h
#define _paintown_projectile_h

#include "object_attack.h"
#include "util/load_exception.h"
#include "util/network/network.h"
#include <vector>
#include <string>

class Token;

namespace Paintown{
class Animation;

class Projectile: public ObjectAttack {
public:
	Projectile( Token * token );
	Projectile( const Projectile * projectile );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	virtual void draw( Graphics::Bitmap * work, int rel_x, int rel_y );
	virtual void drawReflection(Graphics::Bitmap * work, int rel_x, int rel_y, int intensity);
	virtual void grabbed( Object * obj );
	virtual void unGrab();
	virtual bool isGrabbed();
    virtual bool isGrabbable(Object * obj);
	virtual Object * copy();
	virtual const std::string getAttackName();
	virtual bool isAttacking();
	virtual bool collision( ObjectAttack * obj );
	virtual int getDamage() const;
        virtual double getForceX() const;
        virtual double getForceY() const;
	virtual std::vector<ECollide*> getCollide() const;
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual int getWidth() const;
	virtual int getHeight() const;
	// virtual void getAttackCoords( int & x, int & y);
	virtual double minZDistance() const;
	virtual void attacked(World * world, Object * something, std::vector< Object * > & objects );
	virtual Network::Message getCreateMessage();

	virtual int getRY() const;
	virtual int getRX() const;

	virtual ~Projectile();

	inline void setDX( const double dx ){
		this->dx = dx;
	}

	inline void setDY( const double dy ){
		this->dy = dy;
	}

	inline double getDX() const {
		return dx;
	}

	inline double getDY() const {
		return dy;
	}

	inline void setLife( int life ){
		this->life = life;
	}

	inline int getLife() const {
		return life;
	}

	inline void decreaseLife(){
		life -= 1;
	}
	
	virtual int getHealth() const;

protected:
	Animation * main;
	Animation * death;
	Animation * currentAnimation;

	double dx, dy;
	int life;
};

}

#endif
