#include "draw-effect.h"
#include "draw-countdown-effect.h"

DrawCountdownEffect::DrawCountdownEffect(DrawEffect * const effect, int countdown):
effect(effect),
countdown(countdown){
}

void DrawCountdownEffect::draw(int x, Bitmap * work){
    effect->draw(x, work);
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
