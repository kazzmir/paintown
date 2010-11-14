#include "object/animation.h"
#include "animation.h"

namespace Bor{

Animation::Animation(Token * token, Paintown::Character * const owner):
Paintown::Animation(token, owner){
}
    
Animation::Animation(const Animation & animation, Paintown::Character * const owner):
Paintown::Animation(animation, owner){
}

int Animation::getOffsetX() const {
    return getWidth() / 2 - Paintown::Animation::getOffsetX();
}

int Animation::getOffsetY() const {
    return getHeight() - Paintown::Animation::getOffsetY();
}
    
Paintown::Animation * Animation::copy(Paintown::Character * owner) const {
    return new Animation(*this, owner);
}

}
