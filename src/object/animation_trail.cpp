#include "util/bitmap.h"
#include "animation_trail.h"
#include "object.h"

AnimationTrail::AnimationTrail(const int x, const int y, const int facing, const int life, const Bitmap & sprite):
sprite(sprite),
x(x),
y(y),
facing(facing),
max_life(life),
life(life){
}

bool AnimationTrail::act(){
    life -= 1;
    return life <= 0;
}

void AnimationTrail::draw(int rel_x, Bitmap * work) const {
    int w = sprite.getWidth() / 2;
    int h = sprite.getHeight();

    Bitmap::transBlender(0, 0, 0, life * 255 / max_life);
    if (facing == Object::FACING_RIGHT){
        sprite.drawTrans(x-w - rel_x, y-h, *work);
    } else {
        sprite.drawTransHFlip(x-w - rel_x, y-h, *work);
    }

    /*
    if (facing == Object::FACING_RIGHT){
        sprite.draw(x-w - rel_x, y-h, *work);
    } else {
        sprite.drawHFlip(x-w - rel_x, y-h, *work);
    }
    */
}

AnimationTrail::~AnimationTrail(){
}
