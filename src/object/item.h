#ifndef _paintown_item_h
#define _paintown_item_h

#include "util/load_exception.h"
#include "object/object_nonattack.h"
#include "util/sound.h"
#include "util/bitmap.h"
#include "network.h"

#include <string>
#include <vector>

class Stimulation;
class Object;
class World;

using namespace std;

class Item: public ObjectNonAttack {
public:
	Item( const string & filename, Stimulation * const stimulation ) throw( LoadException );
	Item( const Item & item );
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x );
	virtual bool isCollidable( Object * obj );
	virtual ECollide * getCollide() const;
	virtual bool collision( ObjectAttack * obj );
	virtual bool isGettable();
	virtual void touch( Object * obj );
	virtual const int getWidth() const;
	virtual const int getHeight() const;
	virtual Network::Message getCreateMessage();
	
	virtual Object * copy();
	
	virtual ~Item();

protected:
	Stimulation * const copyStimulation() const;

	inline Stimulation * const getStimulation() const {
		return stimulation;
	}

	Bitmap picture;
	ECollide * collide;
	Stimulation * const stimulation;
	Sound sound;
};

#endif
