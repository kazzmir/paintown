#ifndef _paintown_draw_until_effect_h
#define _paintown_draw_until_effect_h

#include "draw-effect.h"

class DrawCountdownEffect: public DrawEffect {
public:
    DrawCountdownEffect(DrawEffect * const effect, int countdown);
    virtual void draw(int x, Bitmap * work);
    virtual bool act();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual int getLevel() const;
    virtual ~DrawCountdownEffect();
protected:
    DrawEffect * const effect;
    int countdown;
};

#endif
