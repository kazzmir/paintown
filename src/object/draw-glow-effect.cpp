#include "../util/bitmap.h"
#include "animation.h"
#include "draw-effect.h"
#include "draw-glow-effect.h"
#include "../util/funcs.h"
#include "../globals.h"
#include "character.h"
#include "object.h"
#include <math.h>

DrawGlowEffect::DrawGlowEffect(const Character * owner, int startColor, int endColor, double period):
DrawEffect(owner, 1),
angle(0),
period(period),
startColor(startColor),
endColor(endColor){
}

void DrawGlowEffect::draw(int x, Bitmap * work){
    double f = fabs(sin(Util::radians(180) * angle / period));
    int alpha = 50;

    int color_r = (Bitmap::getRed(endColor) - Bitmap::getRed(startColor)) * f + Bitmap::getRed(startColor);
    int color_g = (Bitmap::getGreen(endColor) - Bitmap::getGreen(startColor)) * f + Bitmap::getGreen(startColor);
    int color_b = (Bitmap::getBlue(endColor) - Bitmap::getBlue(startColor)) * f + Bitmap::getBlue(startColor);

    Bitmap::transBlender(color_r, color_g, color_b, alpha);

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
    return new DrawGlowEffect(owner, startColor, endColor, period);
}

DrawGlowEffect::~DrawGlowEffect(){
}
