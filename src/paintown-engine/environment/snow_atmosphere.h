#ifndef _paintown_snow_atmosphere_h
#define _paintown_snow_atmosphere_h

#include <vector>

namespace Graphics{
class Bitmap;
}
class Token;

#include "atmosphere.h"

enum FlakeType{
    Small,
    Medium
};

struct Flake{
    Flake(int x, int y, FlakeType type, int angle ):
        x(x),
        y(y),
        dx(x),
        dy(y),
        angle(angle),
        type(type),
        dir(0),
        spin(0){}
    int x, y;
    double dx, dy;
    int angle;
    FlakeType type;
    int dir;
    int spin;
};

class SnowAtmosphere: public Atmosphere {
public:

    SnowAtmosphere();	
    virtual ~SnowAtmosphere();

    virtual void drawBackground(const Graphics::Bitmap & work, int x);
    virtual void drawForeground(const Graphics::Bitmap & work, int x);
    virtual void drawFront(const Graphics::Bitmap & work, int x);
    virtual void drawScreen(const Graphics::Bitmap & work, int x);
    virtual void act(const Scene & level, const std::vector<Paintown::Object*> *);
    virtual void interpret(const Token * message);

protected:
    std::vector< Flake * > flakes;
};

#endif
