#include "../util/bitmap.h"
#include "animation.h"
#include "draw-effect.h"
#include "draw-glow-effect.h"
#include "../util/funcs.h"
#include "../globals.h"
#include "character.h"
#include "object.h"
#include <math.h>

DrawGlowEffect::DrawGlowEffect(const Character * owner):
DrawEffect(owner, 1),
angle(0),
period(0.2){
}

void DrawGlowEffect::draw(int x, Bitmap * work){
    double f = sin(Util::radians(1) * angle / period);
    int max_white = 80;
    int base_blue = 120;
    int c = (int)(f * max_white);
    if ( c < 0 ) c = 0;
    int v = (int)(f * (255 - (max_white + base_blue)) + base_blue + c);
    Bitmap::transBlender(v, v, c, 50);
    Global::debug(1) << "Glow angle " << angle << " color " << v << std::endl;

    Animation * animation = owner->getCurrentMovement();
    int rx = owner->getRX() - x;
    int ry = owner->getRY();
    if (owner->getFacing() == Object::FACING_RIGHT ){
        animation->DrawLit(rx, ry, work );
    } else {
        animation->DrawLitFlipped(rx, ry, work); 
    }
}

bool DrawGlowEffect::act(){
    angle += 1;
    return false;
}

DrawEffect * DrawGlowEffect::copy(const Character * owner) const {
    return new DrawGlowEffect(owner);
}

DrawGlowEffect::~DrawGlowEffect(){
}
