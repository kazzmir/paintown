#ifndef _paintown_actor_h
#define _paintown_actor_h

#include "object_nonattack.h"
#include "r-tech1/network/network.h"
#include "r-tech1/file-system.h"
#include <vector>
#include <string>

namespace Graphics{
class Bitmap;
}
class Animation;

namespace Storage{
    class AbsolutePath;
}

namespace Paintown{

class Actor: public ObjectNonAttack {
public:
	Actor( const Filesystem::AbsolutePath & filename );
	Actor( const Actor & item );
	
	virtual void act(std::vector<Object *> * others, World * world, std::vector< Object * > * add) override;
	virtual void draw(const Graphics::Bitmap & work, int rel_x, int rel_y) override;
	virtual bool isCollidable( Object * obj ) override;
	virtual bool isGettable() override;
	virtual int getWidth() const override;
	virtual int getHeight() const override;

	virtual Network::Message getCreateMessage() override;
	
	virtual Object * copy() override;
	
	virtual ~Actor();

protected:
    std::vector< Animation * > animations;
	Animation * current_animation;
};

}

#endif
