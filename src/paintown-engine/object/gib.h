#ifndef _paintown_gib_h
#define _paintown_gib_h

#include "object_nonattack.h"
#include "util/pointer.h"
#include "util/network/network.h"
#include <string>
#include <vector>

namespace Paintown{

class Gib: public ObjectNonAttack {
public:
    /* The property that defines how many gibs to make */
    static std::string GibProperty;

    struct Point{
        Point( int x, int y, int life ):x(x), y(y), life(life){}
        int x, y;
        int life;
    };
public:
    Gib( const int x, const int y, const int z, double dx, double dy, double dz, Graphics::Bitmap * image, const Util::ReferenceCount<Graphics::Bitmap> & blood);
    Gib( const Gib & g );

    virtual void draw( Graphics::Bitmap * work, int rel_x, int rel_y );

    virtual Object * copy();
    virtual bool isCollidable( Object * obj );
    virtual bool isGettable();
    virtual int getWidth() const;
    virtual int getHeight() const;
    virtual Network::Message getCreateMessage();

    virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );

    virtual ~Gib();

protected:
    double dx, dy, dz;
    int angle;
    int fade;
    Graphics::Bitmap * image;
    Util::ReferenceCount<Graphics::Bitmap> bloodImage;
    std::vector< Point > blood;
};

}

#endif
