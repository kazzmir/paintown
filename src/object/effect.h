#ifndef _effect_h
#define _effect_h

#include "object_nonattack.h"

#include <string>
#include "util/load_exception.h"

using namespace std;

class Bitmap;
class Animation;

extern const int ALLIANCE_NONE;

class Effect: public ObjectNonAttack {
public:
	Effect( int alliance = ALLIANCE_NONE );
	Effect( const Effect & ef );
	Effect( const char * _filename, int alliance = ALLIANCE_NONE ) throw( LoadException );
	
	virtual const int getWidth() const;
	virtual const int getHeight() const;
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();

	virtual void act( vector< Object * > * others, World * world );
	virtual void draw( Bitmap * work, int rel_x );
	virtual Object * copy();

	virtual ~Effect();

protected:

	Animation * image;

};

#endif
