#ifndef _object_nonattack_h
#define _object_nonattack_h

#include <string>
#include <vector>

#include "object.h"
#include "globals.h"

using namespace std;

class ObjectAttack;

class ObjectNonAttack : public Object {
public:

	ObjectNonAttack( int alliance = ALLIANCE_NONE );
	ObjectNonAttack( const ObjectNonAttack & obj );
	
	virtual bool collision( ObjectAttack * obj );
	
	virtual void grabbed( Object * obj );
	virtual void unGrab();
	
	virtual int getDamage() const;
	
	virtual bool isAttacking();
	virtual const std::string & getAttackName();

protected:
	string no_name;

};

#endif
