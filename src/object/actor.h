#ifndef _paintown_actor_h
#define _paintown_actor_h

#include "object_nonattack.h"
#include "network/network.h"

class Bitmap;
class Animation;

class Actor: public ObjectNonAttack {
public:
	Actor( const string & filename ) throw( LoadException );
	Actor( const Actor & item );
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x );
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual const int getWidth() const;
	virtual const int getHeight() const;

	virtual Network::Message getCreateMessage();
	
	virtual Object * copy();
	
	virtual ~Actor();

protected:
	vector< Animation * > animations;
	Animation * current_animation;
};

#endif
