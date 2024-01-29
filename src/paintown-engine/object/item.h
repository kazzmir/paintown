#ifndef _paintown_item_h
#define _paintown_item_h

#include "object_nonattack.h"
#include "r-tech1/file-system.h"
#include "r-tech1/sound/sound.h"
#include "r-tech1/network/network.h"
#include "r-tech1/pointer.h"
#include "stimulation.h"
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
    Item(const Filesystem::AbsolutePath & filename, const Util::ReferenceCount<Stimulation> & stimulation, const std::string & checkName = "item");
    Item(const Item & item);

    virtual void act(std::vector< Object * > * others, World * world, std::vector< Object * > * add) override;
    virtual void draw(const Graphics::Bitmap & work, int rel_x, int rel_y) override;
    virtual bool isCollidable(Object * obj) override;
    virtual std::vector<ECollide*> getCollide() const override;
    virtual bool collision( ObjectAttack * obj ) override;
    virtual bool isGettable() override;
    virtual void touch(Object * obj) override;
    virtual int getWidth() const override;
    virtual int getHeight() const override;
    virtual Network::Message getCreateMessage() override;
    virtual void setStimulation(const Util::ReferenceCount<Stimulation> & stimulation);

    virtual Object * copy() override;

    virtual ~Item();

protected:
    void drawShadow(const Graphics::Bitmap & work, int cameraX, int cameraY, double scale);

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
    virtual bool collision(ObjectAttack * obj);
    virtual bool isGettable();
    virtual void collided(World * world, ObjectAttack * obj, std::vector< Object * > & objects);
    virtual void touch(Object * obj);
    virtual void died(const Util::ReferenceCount<Scene> & scene, std::vector< Object * > & objects);
    
    virtual Object * copy();

    virtual ~BreakableItem();

protected:
    Sound breakSound;
    Sound hitSound;
    std::map<Object *, unsigned long> collision_objects;
};

}

#endif
