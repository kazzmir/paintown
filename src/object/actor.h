#ifndef _paintown_actor_h
#define _paintown_actor_h

#include "object_nonattack.h"
#include "network/network.h"
#include <vector>
#include <string>

class Bitmap;
class Animation;

class Actor: public ObjectNonAttack {
public:
	Actor( const std::string & filename ) throw( LoadException );
	Actor( const Actor & item );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x );
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

#endif
