#ifndef _object_attack_h
#define _object_attack_h

#include "object.h"
#include <string>

class ObjectAttack: public Object{
public:
	ObjectAttack( int alliance );
	ObjectAttack( const int x, const int y, int alliance );
	ObjectAttack( const ObjectAttack & copy );

	virtual bool isAttacking() = 0;
	virtual void getAttackCoords( int & x, int & y) = 0;
	virtual const std::string & getAttackName() = 0;

	virtual void attacked( Object * something ) = 0;
	
	virtual inline const unsigned int getTicket() const{
		return attack_ticket;
	}

protected:
	virtual void nextTicket();

protected:
	unsigned int attack_ticket;

};

#endif
