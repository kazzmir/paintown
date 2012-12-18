#ifndef _paintown_item_h
#define _paintown_item_h

#include "util/file-system.h"
#include "object_nonattack.h"
#include "util/sound/sound.h"
#include "util/network/network.h"
#include "stimulation.h"
#include "util/pointer.h"
#include "animation.h"

#include <string>
#include <vector>

class World;

namespace Graphics{
    class Bitmap;
}

namespace Paintown{

class Stimulation;
class Object;

class Item: public ObjectNonAttack {
public:
    Item(const Filesystem::AbsolutePath & filename, const Util::ReferenceCount<Stimulation> & stimulation);
    Item(const Item & item);

    virtual void act(std::vector< Object * > * others, World * world, std::vector< Object * > * add);
    virtual void draw(Graphics::Bitmap * work, int rel_x, int rel_y);
    virtual bool isCollidable(Object * obj);
    virtual std::vector<ECollide*> getCollide() const;
    virtual bool collision( ObjectAttack * obj );
    virtual bool isGettable();
    virtual void touch(Object * obj);
    virtual int getWidth() const;
    virtual int getHeight() const;
    virtual Network::Message getCreateMessage();
    virtual void setStimulation(const Util::ReferenceCount<Stimulation> & stimulation);

    virtual Object * copy();

    virtual ~Item();

protected:
    void drawShadow(Graphics::Bitmap * work, int cameraX, int cameraY, double scale);

    inline const Filesystem::AbsolutePath & getPath() const {
        return path;
    }

    inline Util::ReferenceCount<Stimulation> getStimulation() const {
        return stimulation;
    }

    Util::ReferenceCount<Animation> animation;
    // ECollide * collide;
    Util::ReferenceCount<Stimulation> stimulation;
    Sound sound;
    Filesystem::AbsolutePath path;

    /* Items can float above the ground like in quake3 */
    struct Floating{
        Floating():
        enabled(false),
        height(20),
        amplitude(10),
        period(100),
        time(0){
        }

        bool enabled;
        double height;
        double amplitude;
        int period;
        unsigned int time;
    } floating;
};

class BreakableItem: public Item {
public:
    BreakableItem(const Filesystem::AbsolutePath & filename, const Util::ReferenceCount<Stimulation> & stimulation);
    BreakableItem(const BreakableItem & item);
    
    virtual bool isCollidable(Object * obj);
    virtual bool isGettable();
    virtual void touch(Object * obj);
    virtual void died(std::vector< Object * > & objects);

    virtual ~BreakableItem();
};

}

#endif
