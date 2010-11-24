#include "stage.h"
#include "character.h"
#include "behavior.h"

namespace Mugen{
/* copy all data from the parent somehow, maybe lazily. to speed things up */
class Helper: public Character {
public:
    Helper(Character & parent);
    virtual ~Helper();
        
    virtual bool isHelper() const;
    using Character::getAnimation;
    virtual MugenAnimation * getAnimation(int id) const;
    using Character::getState;
    virtual State * getState(int id) const;

protected:
    Character & owner;
    HitDefinition hit;
    DummyBehavior dummy;
    mutable std::map< int, MugenAnimation * > proxyAnimations;
    mutable std::map< int, State *> proxyStates;
};

}
