#ifndef _paintown_openbor_animation_h
#define _paintown_openbor_animation_h

#include "object/animation.h"

class Token;
class Character;

namespace Bor{

class Animation: public ::Animation{
public:
    Animation(Token * token, Character * const owner);
    Animation(const Animation & animation, Character * const owner);

    virtual int getOffsetX() const;
    virtual int getOffsetY() const;
        
    virtual ::Animation * copy(Character * owner);
};

}

#endif
