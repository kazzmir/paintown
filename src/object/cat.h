#ifndef _paintown_cat_h
#define _paintown_cat_h

#include <map>
#include "object_nonattack.h"
#include "util/sound.h"

class Bitmap;
class Animation;

class Cat: public ObjectNonAttack {
public:
	Cat( const string & filename ) throw( LoadException );
	Cat( const Cat & cat );
	
	virtual void act( vector< Object * > * others, World * world, vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x );
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual const int getWidth() const;
	virtual const int getHeight() const;
	
	virtual Object * copy();

	virtual const int getRX() const;
	
	virtual ~Cat();

protected:
	map< std::string, Animation * > animations;
	Animation * current_animation;

	Sound meow;

	enum State{
		IDLE1,
		IDLE2,
		YAWN,
		WALK,
		TURN,
		RUN,
		SIT,
	};

	State state;
};

#endif
