#ifndef _paintown_actor_h
#define _paintown_actor_h

#include "object_nonattack.h"

class Bitmap;

class Actor: public ObjectNonAttack {
public:
	Actor( const string & filename, Stimulation * const stimulation ) throw( LoadException );
	Actor( const Actor & item );
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x );
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual const int getWidth() const;
	virtual const int getHeight() const;
	
	virtual Object * copy();
	
	virtual ~Actor();
};

#endif
