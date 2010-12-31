#ifndef _paintown_openbor_animation_h
#define _paintown_openbor_animation_h

#include "paintown-engine/object/animation.h"

class Token;

namespace Paintown{
    class Character;
}

namespace Bor{

class Animation: public Paintown::Animation{
public:
    Animation(Token * token, Paintown::Character * const owner);
    Animation(const Animation & animation, Paintown::Character * const owner);

    virtual int getOffsetX() const;
    virtual int getOffsetY() const;
        
    virtual Paintown::Animation * copy(Paintown::Character * owner) const;
};

}

#endif
