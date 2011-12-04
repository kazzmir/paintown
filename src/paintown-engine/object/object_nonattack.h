#ifndef _object_nonattack_h
#define _object_nonattack_h

#include <string>
#include <vector>

#include "object.h"
#include "alliance.h"

namespace Paintown{

class ObjectAttack;

class ObjectNonAttack : public Object {
public:
	ObjectNonAttack( const int x, const int y, int alliance = ALLIANCE_NONE );
	ObjectNonAttack( const ObjectNonAttack & obj );
	
	virtual bool collision( ObjectAttack * obj );
	
	virtual void grabbed( Object * obj );
	virtual void unGrab();
	virtual bool isGrabbed();
        virtual bool isGrabbable(Object * obj);
	
	virtual int getDamage() const;
        virtual double getForceX() const;
        virtual double getForceY() const;
	
	virtual bool isAttacking();
	virtual const std::string getAttackName();

protected:
	std::string no_name;
};

}

#endif
