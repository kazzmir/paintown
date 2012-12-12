#include "animation.h"
#include "character.h"
#include "draw-effect.h"
#include "globals.h"
#include <math.h>
#include "object.h"
#include "util/graphics/bitmap.h"
#include "util/funcs.h"

namespace Paintown{

DrawEffect::DrawEffect(const Character * owner, const int level):
owner(owner),
level(level){
}

DrawEffect::DrawEffect():
owner(0),
level(0){
}

DrawEffect::~DrawEffect(){
}

bool DrawEffect::compare(const DrawEffect * a, const DrawEffect * b){
    return a->getLevel() < b->getLevel();
}

DrawCountdownEffect::DrawCountdownEffect(DrawEffect * const effect, int countdown):
effect(effect),
countdown(countdown){
}

void DrawCountdownEffect::draw(int x, Remap * remap, Graphics::Bitmap * work){
    effect->draw(x, remap, work);
}

bool DrawCountdownEffect::act(){
    effect->act();
    countdown -= 1;
    if (countdown < 0){
        countdown = 0;
    }
    return countdown <= 0;
}

DrawEffect * DrawCountdownEffect::copy(const Character * owner) const {
    return new DrawCountdownEffect(effect->copy(owner), countdown);
}

int DrawCountdownEffect::getLevel() const {
    return effect->getLevel();
}

DrawCountdownEffect::~DrawCountdownEffect(){
    delete effect;
}

DrawGlowEffect::DrawGlowEffect(const Character * owner, Graphics::Color startColor, Graphics::Color endColor, double period):
DrawEffect(owner, 1),
dead(false),
angle(0),
period(period),
startColor(startColor),
endColor(endColor){
}

void DrawGlowEffect::draw(int x, Remap * remap, Graphics::Bitmap * work){
    double f = fabs(sin(Util::radians(180) * angle / period));
    int alpha = 50;

    int color_r = (int)((Graphics::getRed(endColor) - Graphics::getRed(startColor)) * f + Graphics::getRed(startColor));
    int color_g = (int)((Graphics::getGreen(endColor) - Graphics::getGreen(startColor)) * f + Graphics::getGreen(startColor));
    int color_b = (int)((Graphics::getBlue(endColor) - Graphics::getBlue(startColor)) * f + Graphics::getBlue(startColor));

    Graphics::Bitmap::transBlender(color_r, color_g, color_b, alpha);

    Util::ReferenceCount<Animation> animation = owner->getCurrentMovement();
    int rx = owner->getRX() - x;
    int ry = owner->getRY();
    if (owner->getFacing() == Object::FACING_RIGHT ){
        animation->DrawLit(rx, ry, remap, work);
    } else {
        animation->DrawLitFlipped(rx, ry, remap, work); 
    }
}

bool DrawGlowEffect::act(){
    angle += 1;
    return dead;
}

void DrawGlowEffect::kill(){
    dead = true;
}

DrawEffect * DrawGlowEffect::copy(const Character * owner) const {
    return new DrawGlowEffect(owner, startColor, endColor, period);
}

DrawGlowEffect::~DrawGlowEffect(){
}


DrawNormalEffect::DrawNormalEffect(const Character * owner):
DrawEffect(owner, 0){
}
    
void DrawNormalEffect::draw(int x, Remap * remap, Graphics::Bitmap * work){
    Util::ReferenceCount<Animation> animation = owner->getCurrentMovement();
    int rx = owner->getRX() - x;
    int ry = owner->getRY();
    if (owner->getFacing() == Object::FACING_RIGHT ){
        animation->Draw(rx, ry, remap, work);
    } else {
        animation->DrawFlipped(rx, ry, remap, work);
    }
}
    
bool DrawNormalEffect::act(){
    return false;
}

DrawEffect * DrawNormalEffect::copy(const Character * owner) const {
    return new DrawNormalEffect(owner);
}

DrawNormalEffect::~DrawNormalEffect(){
}

DrawUntilEffect::DrawUntilEffect(DrawEffect * const effect, until end):
DrawEffect(),
effect(effect),
end(end){
}

void DrawUntilEffect::draw(int x, Remap * remap, Graphics::Bitmap * work){
    effect->draw(x, remap,work);
}

bool DrawUntilEffect::act(){
    effect->act();
    return end(effect->getOwner());
}

int DrawUntilEffect::getLevel() const {
    return effect->getLevel();
}

DrawEffect * DrawUntilEffect::copy(const Character * owner) const {
    return new DrawUntilEffect(effect->copy(owner), end);
}

DrawUntilEffect::~DrawUntilEffect(){
    delete effect;
}

}
