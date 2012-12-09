#ifndef _paintown_cat_h
#define _paintown_cat_h

#include <map>
#include <string>
#include <vector>
#include "object_nonattack.h"
#include "util/sound/sound.h"
#include "util/network/network.h"
#include "util/file-system.h"

namespace Graphics{
class Bitmap;
}

namespace Paintown{

class Animation;

class Cat: public ObjectNonAttack {
public:
	Cat(const Filesystem::AbsolutePath & filename );
	Cat( const Cat & cat );
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	virtual void draw( Graphics::Bitmap * work, int rel_x, int rel_y );
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

        Filesystem::AbsolutePath path;

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

}

#endif
