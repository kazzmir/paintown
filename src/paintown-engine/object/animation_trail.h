#ifndef _paintown_animation_trail_h
#define _paintown_animation_trail_h

namespace Graphics{
class Bitmap;
}

namespace Paintown{

class Remap;

class AnimationTrail{
public:
    AnimationTrail(const int x, const int y, const int facing, const int life, const Graphics::Bitmap & sprite);

    virtual void draw(const int rel_x, Remap * remap, const Graphics::Bitmap & work) const;
    virtual bool act();

    virtual ~AnimationTrail();

protected:
    const Graphics::Bitmap & sprite;
    const int x, y;
    const int facing;
    const int max_life;
    int life;

};

}

#endif
