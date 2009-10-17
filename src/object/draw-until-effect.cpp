#include "draw-effect.h"
#include "draw-until-effect.h"
#include "../globals.h"

DrawUntilEffect::DrawUntilEffect(DrawEffect * const effect, until end):
DrawEffect(),
effect(effect),
end(end){
}

void DrawUntilEffect::draw(int x, Bitmap * work){
    effect->draw(x, work);
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
