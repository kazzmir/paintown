#ifndef _paintown_item_h
#define _paintown_item_h

#include "util/load_exception.h"
#include "object/object_nonattack.h"
#include "util/sound.h"
#include "util/bitmap.h"
#include "network/network.h"

#include <string>
#include <vector>

class Stimulation;
class Object;
class World;

class Item: public ObjectNonAttack {
public:
	Item( const std::string & filename, Stimulation * const stimulation ) throw( LoadException );
	Item( const Item & item );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x, int rel_y );
	virtual bool isCollidable( Object * obj );
	virtual ECollide * getCollide() const;
	virtual bool collision( ObjectAttack * obj );
	virtual bool isGettable();
	virtual void touch( Object * obj );
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual Network::Message getCreateMessage();
	
	virtual Object * copy();
	
	virtual ~Item();

protected:
	Stimulation * copyStimulation() const;

	inline const std::string getPath() const {
		return path;
	}

	inline Stimulation * getStimulation() const {
		return stimulation;
	}

	Bitmap picture;
	ECollide * collide;
	Stimulation * const stimulation;
	Sound sound;
	std::string path;
};

#endif
