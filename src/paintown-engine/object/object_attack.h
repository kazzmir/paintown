#ifndef _object_attack_h
#define _object_attack_h

#include "object.h"
#include <string>
#include <vector>

class World;

namespace Paintown{

class ObjectAttack: public Object {
public:
	ObjectAttack( int alliance );
	ObjectAttack( const int x, const int y, int alliance );
	ObjectAttack( const ObjectAttack & copy );

	virtual bool isAttacking() = 0;
	// virtual void getAttackCoords( int & x, int & y) = 0;
	virtual const std::string getAttackName() = 0;

	virtual double minZDistance() const = 0;

	/* this object just attached 'something' */
	virtual void attacked( World * world, Object * something, std::vector< Object * > & objects ) = 0;
	
	virtual inline unsigned int getTicket() const{
		return attack_ticket;
	}

	virtual void nextTicket();

protected:
	unsigned int attack_ticket;

};

}

#endif
