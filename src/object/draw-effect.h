#ifndef _paintown_draw_effect_h
#define _paintown_draw_effect_h

class Character;
class Bitmap;

class DrawEffect{
public:
    DrawEffect(const Character * owner, const int level);
    virtual void draw(int x, Bitmap * work) = 0;
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

#endif
