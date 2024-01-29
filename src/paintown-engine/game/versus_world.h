#ifndef _paintown_versus_world_h
#define _paintown_versus_world_h

#include "adventure_world.h"

namespace Graphics{
class Bitmap;
}

namespace Paintown{
class Character;
}

class VersusWorld: public AdventureWorld {
public:
	VersusWorld( int z, Paintown::Character * player1, Paintown::Character * player2 );
	virtual ~VersusWorld();

	virtual void draw(const Graphics::Bitmap & work );

	virtual void act();
	
	virtual bool finished() const;
	
	virtual int getMaximumZ();
	virtual int getMinimumZ();

protected:
        Paintown::Character * const player1;
	Paintown::Character * const player2;

	int z;
};

#endif
