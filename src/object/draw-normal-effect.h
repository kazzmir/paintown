#ifndef _paintown_draw_normal_effect_h
#define _paintown_draw_normal_effect_h

#include "draw-effect.h"

class DrawNormalEffect: public DrawEffect {
public:
    DrawNormalEffect(const Character * owner);
    virtual void draw(int x, Bitmap * work);
    virtual bool act();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual ~DrawNormalEffect();
};

#endif
