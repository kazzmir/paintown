#ifndef _object_nonattack_h
#define _object_nonattack_h

#include <string>
#include <vector>

#include "object.h"
#include "globals.h"

class ObjectAttack;

class ObjectNonAttack : public Object {
public:

	ObjectNonAttack( const int x, const int y, int alliance = ALLIANCE_NONE );
	ObjectNonAttack( const ObjectNonAttack & obj );
	
	virtual bool collision( ObjectAttack * obj );
	
	virtual void grabbed( Object * obj );
	virtual void unGrab();
	virtual bool isGrabbed();
	
	virtual int getDamage() const;
	
	virtual bool isAttacking();
	virtual const std::string & getAttackName();

protected:
	std::string no_name;
};

#endif
