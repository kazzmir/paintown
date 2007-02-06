#ifndef _paintown_item_h
#define _paintown_item_h

#include "util/load_exception.h"
#include "object/object_nonattack.h"
#include "heart.h"
#include "util/bitmap.h"

#include <string>
#include <vector>

class Object;
class World;

using namespace std;

class Item: public ObjectNonAttack {
public:
	Item( const string & filename ) throw( LoadException );
	Item( const Item & item );
	
	virtual void act( vector< Object * > * others, World * world );
	virtual void draw( Bitmap * work, int rel_x );
	virtual bool isCollidable( Object * obj );
	virtual const int getWidth() const;
	virtual const int getHeight() const;
	
	virtual Object * copy();
	
	virtual ~Item();

protected:
	Bitmap picture;
};

#endif
