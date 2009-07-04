#include "draw-effect.h"
#include "draw-normal-effect.h"
#include "animation.h"
#include "character.h"
#include "object.h"

DrawNormalEffect::DrawNormalEffect(const Character * owner):
DrawEffect(owner, 0){
}
    
void DrawNormalEffect::draw(int x, Bitmap * work){
    Animation * animation = owner->getCurrentMovement();
    int rx = owner->getRX() - x;
    int ry = owner->getRY();
    if (owner->getFacing() == Object::FACING_RIGHT ){
        animation->Draw(rx, ry, work);
    } else {
        animation->DrawFlipped(rx, ry, work);
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
