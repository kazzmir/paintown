#ifndef _paintown_actor_h
#define _paintown_actor_h

#include "object_nonattack.h"
#include "util/network/network.h"
#include <vector>
#include <string>

class Bitmap;
class Animation;

namespace Filesystem{
    class AbsolutePath;
}

namespace Paintown{

class Actor: public ObjectNonAttack {
public:
	Actor( const Filesystem::AbsolutePath & filename ) throw( LoadException );
	Actor( const Actor & item );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x, int rel_y );
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual int getWidth() const;
	virtual int getHeight() const;

	virtual Network::Message getCreateMessage();
	
	virtual Object * copy();
	
	virtual ~Actor();

protected:
    std::vector< Animation * > animations;
	Animation * current_animation;
};

}

#endif
