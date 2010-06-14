#ifndef _paintown_night_atmosphere_h
#define _paintown_night_atmosphere_h

class Bitmap;

#include "util/token_exception.h"
#include "atmosphere.h"
#include <vector>

class Token;
class Sound;

struct Light{
    Light(const int x, const int y, const int lower_width, const int upper_width, const int color, const int alpha):
        x(x), y(y), lower_width(lower_width), upper_width(upper_width),
        alpha(alpha), color(color){
    }

    int x, y;
    int lower_width, upper_width;
    int alpha, color;
};

class NightAtmosphere: public Atmosphere {
public:

    NightAtmosphere();	
    virtual ~NightAtmosphere();

    virtual void drawBackground(Bitmap * work, int x);
    virtual void drawForeground(Bitmap * work, int x);
    virtual void drawFront(Bitmap * work, int x);
    virtual void drawScreen(Bitmap * work, int x);
    virtual void act(const Scene & level);
    virtual void addLight(const int x, const int y, const int lower_width, const int upper_width, const int color, const int alpha);
    virtual void interpret(Token * message);

protected:

    void drawLight(Bitmap * original, Bitmap * work, const int x, const int y, const int lower_width, const int upper_width, const int black, const int black_alpha, const int light, const int light_alpha, bool draw_light);
    void processLight(Token * token);
    int getSkyColor() const;
    int getSkyDarkness() const;

    std::vector<Light*> lights;

    /* alpha between 0-255, 255 is completely dark */
    int darkness;

    Bitmap * lamp;
    Sound * thunder;
    bool lightning;
    int lightningFade;
    int thunderPause;
};

#endif
