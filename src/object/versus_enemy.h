#ifndef _paintown_versus_enemy_h
#define _paintown_versus_enemy_h

#include "enemy.h"
#include "character.h"
#include "util/load_exception.h"

class Object;
class World;

class VersusEnemy: public Enemy{
public:
	VersusEnemy( const Character & chr ) throw( LoadException );
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
	
	virtual void hurt( int x );

	virtual ~VersusEnemy();

protected:
	int what_to_do;
};

#endif
