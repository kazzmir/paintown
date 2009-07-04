#ifndef _paintown_draw_glow_effect_h
#define _paintown_draw_glow_effect_h

#include "draw-effect.h"

class DrawGlowEffect: public DrawEffect {
public:
    DrawGlowEffect(const Character * owner, int startColor, int endColor, double period);
    virtual void draw(int x, Bitmap * work);
    virtual bool act();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual ~DrawGlowEffect();
protected:
    int angle;
    double period;
    int startColor, endColor;
};

#endif
