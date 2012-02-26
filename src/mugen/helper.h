#include "stage.h"
#include "character.h"
#include "behavior.h"

class MugenSound;

namespace Mugen{

/* copy all data from the parent somehow, maybe lazily. to speed things up */
class Helper: public Character {
public:
    Helper(Character * parent, const Character * root, int id);
    virtual ~Helper();

    virtual inline int getHelperId() const {
        return id;
    }
        
    using Character::destroyed;
    virtual void destroyed(Stage & stage);
    
    virtual bool canBeHit(Character * enemy);

    virtual bool isHelper() const;
    virtual const std::string getName() const;
    virtual const std::string getDisplayName() const;
    using Character::getAnimation;
    virtual PaintownUtil::ReferenceCount<MugenAnimation> getAnimation(int id) const;
    using Character::getState;
    virtual State * getState(int id) const;

    void reParent(Character * parent);

    inline Character * getParent() const {
        return owner;
    }
        
    virtual void roundEnd(Mugen::Stage & stage);

    virtual const Character * getRoot() const {
        return root;
    }
        
    virtual MugenSound * getSound(int group, int item) const;
    virtual MugenSound * getCommonSound(int group, int item) const;

protected:
    /* the character that spawned this helper */
    Character * owner;
    /* the root character. player -> helper A -> helper B
     * the root of B is the player
     */ 
    const Character * root;
    HitDefinition hit;
    DummyBehavior dummy;
    mutable std::map< int, PaintownUtil::ReferenceCount<MugenAnimation> > proxyAnimations;
    mutable std::map< int, State *> proxyStates;
    int id;
    std::string name;
};

}
