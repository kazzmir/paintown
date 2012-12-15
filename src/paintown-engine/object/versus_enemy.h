#ifndef _paintown_versus_enemy_h
#define _paintown_versus_enemy_h

#include "enemy.h"
#include "character.h"
#include "util/exceptions/load_exception.h"

class World;

namespace Paintown{
class Object;

class VersusEnemy: public Enemy {
public:
	VersusEnemy( const Character & chr ) throw( LoadException );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	
	virtual void hurt( int x );

	virtual ~VersusEnemy();

protected:
	int what_to_do;
};

}

#endif
