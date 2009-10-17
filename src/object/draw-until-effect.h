#ifndef _paintown_draw_until_effect_h
#define _paintown_draw_until_effect_h

#include "draw-effect.h"

class DrawUntilEffect: public DrawEffect {
public:
    typedef bool (*until)(const Character * const character);
    DrawUntilEffect(DrawEffect * const effect, until end);
    virtual void draw(int x, Bitmap * work);
    virtual bool act();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual int getLevel() const;
    virtual ~DrawUntilEffect();
protected:
    DrawEffect * const effect;
    until end;
};

#endif
