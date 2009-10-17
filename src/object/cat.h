#ifndef _paintown_cat_h
#define _paintown_cat_h

#include <map>
#include <string>
#include <vector>
#include "object_nonattack.h"
#include "util/sound.h"
#include "network/network.h"

class Bitmap;
class Animation;

class Cat: public ObjectNonAttack {
public:
	Cat( const std::string & filename ) throw( LoadException );
	Cat( const Cat & cat );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	virtual void draw( Bitmap * work, int rel_x );
	virtual bool isCollidable( Object * obj );
	virtual bool isGettable();
	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual Network::Message getCreateMessage();
	
	virtual Object * copy();

	virtual int getRX() const;
	
	virtual ~Cat();

protected:
    std::map< std::string, Animation * > animations;
	Animation * current_animation;

    /* meow meow! */
	Sound meow;

    std::string path;

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
