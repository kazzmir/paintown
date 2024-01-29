#ifndef _paintown_projectile_h
#define _paintown_projectile_h

#include "object_attack.h"
#include "r-tech1/network/network.h"
#include <vector>
#include <string>

class Token;

namespace Paintown{
class Animation;

class Projectile: public ObjectAttack {
public:
	Projectile( Token * token );
	Projectile( const Projectile * projectile );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add ) override;
	virtual void draw(const Graphics::Bitmap & work, int rel_x, int rel_y) override;
	virtual void drawReflection(const Graphics::Bitmap & work, int rel_x, int rel_y, int intensity) override;
	virtual void grabbed(Object * obj) override;
	virtual void unGrab() override;
	virtual bool isGrabbed() override;
    virtual bool isGrabbable(Object * obj) override;
	virtual Object * copy() override;
	virtual const std::string getAttackName() override;
	virtual bool isAttacking() override;
	virtual bool collision(ObjectAttack * obj) override;
	virtual int getDamage() const override;
    virtual double getForceX() const override;
    virtual double getForceY() const override;
	virtual std::vector<ECollide*> getCollide() const override;
	virtual bool isCollidable(Object * obj) override;
	virtual bool isGettable() override;
	virtual int getWidth() const override;
	virtual int getHeight() const override;
	// virtual void getAttackCoords( int & x, int & y);
	virtual double minZDistance() const override;
	virtual void attacked(World * world, Object * something, std::vector< Object * > & objects ) override;
	virtual Network::Message getCreateMessage() override;

	virtual int getRY() const override;
	virtual int getRX() const override;

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
	
	virtual int getHealth() const override;

protected:
	Animation * mainAnimation;
	Animation * death;
	Animation * currentAnimation;

	double dx, dy;
	int life;
};

}

#endif
