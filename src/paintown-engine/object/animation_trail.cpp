#include "util/graphics/bitmap.h"
#include "util/graphics/trans-bitmap.h"
#include "animation_trail.h"
#include "character.h"
#include "object.h"

namespace Paintown{

AnimationTrail::AnimationTrail(const int x, const int y, const int facing, const int life, const Graphics::Bitmap & sprite):
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

void AnimationTrail::draw(int rel_x, Remap * remap, Graphics::Bitmap * work) const {
    int w = sprite.getWidth() / 2;
    int h = sprite.getHeight();

    Graphics::Bitmap::transBlender(0, 0, 0, life * 255 / max_life);
    if (facing == Object::FACING_RIGHT){
        sprite.translucent().draw(x-w - rel_x, y-h, remap, *work);
    } else {
        sprite.translucent().draw(x-w - rel_x, y-h, remap, *work);
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

}
