#include "object/animation.h"
#include "animation.h"

namespace Bor{

Animation::Animation(Token * token, Character * const owner):
::Animation(token, owner){
}
    
Animation::Animation(const Animation & animation, Character * const owner):
::Animation(animation, owner){
}

int Animation::getOffsetX() const {
    return getWidth() / 2 - ::Animation::getOffsetX();
}

int Animation::getOffsetY() const {
    return getHeight() - ::Animation::getOffsetY();
}
    
::Animation * Animation::copy(Character * owner) const {
    return new Animation(*this, owner);
}

}
