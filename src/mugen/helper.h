#include "stage.h"
#include "character.h"
#include "behavior.h"

namespace Mugen{
/* copy all data from the parent somehow, maybe lazily. to speed things up */
class Helper: public Character {
public:
    Helper(Character & parent, int id);
    virtual ~Helper();

    virtual inline int getHelperId() const {
        return id;
    }
        
    virtual bool isHelper() const;
    virtual const std::string getName() const;
    using Character::getAnimation;
    virtual MugenAnimation * getAnimation(int id) const;
    using Character::getState;
    virtual State * getState(int id) const;

    inline Character & getParent() const {
        return owner;
    }

protected:
    Character & owner;
    HitDefinition hit;
    DummyBehavior dummy;
    mutable std::map< int, MugenAnimation * > proxyAnimations;
    mutable std::map< int, State *> proxyStates;
    int id;
};

}
