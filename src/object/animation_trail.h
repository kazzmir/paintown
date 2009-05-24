#ifndef _paintown_animation_trail_h
#define _paintown_animation_trail_h

class Bitmap;

class AnimationTrail{
public:
    AnimationTrail(const int x, const int y, const int facing, const int life, const Bitmap & sprite);

    virtual void draw(const int rel_x, Bitmap * work) const;
    virtual bool act();

    virtual ~AnimationTrail();

protected:
    const Bitmap & sprite;
    const int x, y;
    const int facing;
    const int max_life;
    int life;

};

#endif
