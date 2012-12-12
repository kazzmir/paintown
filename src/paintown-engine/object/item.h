#ifndef _paintown_item_h
#define _paintown_item_h

#include "util/load_exception.h"
#include "util/file-system.h"
#include "object_nonattack.h"
#include "util/sound/sound.h"
#include "util/graphics/bitmap.h"
#include "util/network/network.h"
#include "stimulation.h"
#include "util/pointer.h"

#include <string>
#include <vector>

class World;

namespace Paintown{

class Stimulation;
class Object;

class Item: public ObjectNonAttack {
public:
    Item(const Filesystem::AbsolutePath & filename, const Util::ReferenceCount<Stimulation> & stimulation);
    Item(const Item & item);

    virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
    virtual void draw( Graphics::Bitmap * work, int rel_x, int rel_y );
    virtual bool isCollidable( Object * obj );
    virtual std::vector<ECollide*> getCollide() const;
    virtual bool collision( ObjectAttack * obj );
    virtual bool isGettable();
    virtual void touch( Object * obj );
    virtual int getWidth() const;
    virtual int getHeight() const;
    virtual Network::Message getCreateMessage();
    virtual void setStimulation(const Util::ReferenceCount<Stimulation> & stimulation);

    virtual Object * copy();

    virtual ~Item();

protected:
    inline const Filesystem::AbsolutePath & getPath() const {
        return path;
    }

    inline Util::ReferenceCount<Stimulation> getStimulation() const {
        return stimulation;
    }

    Graphics::Bitmap picture;
    ECollide * collide;
    Util::ReferenceCount<Stimulation> stimulation;
    Sound sound;
    Filesystem::AbsolutePath path;
};

}

#endif
