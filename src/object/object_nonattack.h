#ifndef _object_nonattack_h
#define _object_nonattack_h

#include <string>

#include "object.h"

class ObjectAttack;

class ObjectNonAttack : public Object {
public:

	ObjectNonAttack( int alliance );
	ObjectNonAttack( const ObjectNonAttack & obj );
	
	virtual bool collision( ObjectAttack * obj );
	virtual void collided( ObjectAttack * obj );
	
	virtual void grabbed( Object * obj );
	virtual void unGrab();
	
	virtual int getDamage() const;
	
	virtual bool isAttacking();
	virtual const std::string & getAttackName();

protected:
	std::string no_name;

};

#endif
