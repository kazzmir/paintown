#ifndef _paintown_versus_world_h
#define _paintown_versus_world_h

#include "world.h"

class Character;
class Bitmap;

class VersusWorld: public World {
public:
	VersusWorld( Character * player1, Character * player2 );
	virtual ~VersusWorld();

	virtual void draw( Bitmap * work );

	virtual void act();
	
	virtual const bool finished() const;
	
	virtual int getMaximumZ();
	virtual int getMinimumZ();

protected:
	Character * const player1;
	Character * const player2;
};

#endif
