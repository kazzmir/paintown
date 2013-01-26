#ifndef _paintown_night_atmosphere_h
#define _paintown_night_atmosphere_h

namespace Graphics{
class Bitmap;
}

#include "util/token_exception.h"
#include "util/graphics/color.h"
#include "atmosphere.h"
#include <vector>

class Token;
class Sound;

struct Light{
    Light(const int x, const int y, const int lower_width, const int upper_width, const Graphics::Color color, const int alpha):
        x(x), y(y), lower_width(lower_width), upper_width(upper_width),
        alpha(alpha), color(color){
    }

    int x, y;
    int lower_width, upper_width;
    int alpha;
    Graphics::Color color;
};

class NightAtmosphere: public Atmosphere {
public:

    NightAtmosphere();	
    virtual ~NightAtmosphere();

    virtual void drawBackground(Graphics::Bitmap * work, int x);
    virtual void drawForeground(Graphics::Bitmap * work, int x);
    virtual void drawFront(Graphics::Bitmap * work, int x);
    virtual void drawScreen(Graphics::Bitmap * work, int x);
    virtual void act(const Scene & level, const std::vector<Paintown::Object*>*);
    virtual void addLight(const int x, const int y, const int lower_width, const int upper_width, const Graphics::Color color, const int alpha);
    virtual void interpret(const Token * message);

protected:

    void drawLight(Graphics::Bitmap * original, Graphics::Bitmap * work, const int x, const int y, const int lower_width, const int upper_width, const Graphics::Color black, const int black_alpha, const Graphics::Color light, const int light_alpha, bool draw_light);
    void processLight(const Token * token);
    Graphics::Color getSkyColor() const;
    int getSkyDarkness() const;

    std::vector<Light*> lights;

    /* alpha between 0-255, 255 is completely dark */
    int darkness;

    Graphics::Bitmap * lamp;
    Util::ReferenceCount<Sound> thunder;
    bool lightning;
    int lightningFade;
    int thunderPause;
};

#endif
