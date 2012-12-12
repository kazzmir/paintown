#ifndef _paintown_draw_effect_h
#define _paintown_draw_effect_h

#include "util/graphics/bitmap.h"

namespace Paintown{

class Remap;
class Character;

class DrawEffect{
public:
    DrawEffect(const Character * owner, const int level);
    virtual void draw(int x, Remap * remap, Graphics::Bitmap * work) = 0;
    virtual DrawEffect * copy(const Character * owner) const = 0;
    virtual bool act() = 0;
    
    virtual ~DrawEffect();

    virtual inline int getLevel() const {
        return level;
    }

    virtual inline const Character * getOwner(){
        return owner;
    }

    static bool compare(const DrawEffect * a, const DrawEffect * b);
protected:
    DrawEffect();

    const Character * owner;
    int level;
};

class DrawCountdownEffect: public DrawEffect {
public:
    DrawCountdownEffect(DrawEffect * const effect, int countdown);
    virtual void draw(int x, Remap * remap, Graphics::Bitmap * work);
    virtual bool act();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual int getLevel() const;
    virtual ~DrawCountdownEffect();
protected:
    DrawEffect * const effect;
    int countdown;
};

class DrawGlowEffect: public DrawEffect {
public:
    DrawGlowEffect(const Character * owner, Graphics::Color startColor, Graphics::Color endColor, double period);
    virtual void draw(int x, Remap * remap, Graphics::Bitmap * work);
    virtual bool act();
    virtual void kill();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual ~DrawGlowEffect();
protected:
    bool dead;
    int angle;
    double period;
    Graphics::Color startColor, endColor;
};

class DrawNormalEffect: public DrawEffect {
public:
    DrawNormalEffect(const Character * owner);
    virtual void draw(int x, Remap * remap, Graphics::Bitmap * work);
    virtual bool act();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual ~DrawNormalEffect();
};

class DrawUntilEffect: public DrawEffect {
public:
    typedef bool (*until)(const Character * const character);
    DrawUntilEffect(DrawEffect * const effect, until end);
    virtual void draw(int x, Remap * remap, Graphics::Bitmap * work);
    virtual bool act();
    virtual DrawEffect * copy(const Character * owner) const;
    virtual int getLevel() const;
    virtual ~DrawUntilEffect();
protected:
    DrawEffect * const effect;
    until end;
};

}

#endif
