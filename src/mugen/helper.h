#include "stage.h"
#include "character.h"
#include "behavior.h"

namespace Mugen{

class Sound;

/* copy all data from the parent somehow, maybe lazily. to speed things up */
class Helper: public Character {
public:
    Helper(Character * parent, const Character * root, int id, const std::string & name);
    virtual ~Helper();

    virtual inline int getHelperId() const {
        return id;
    }
        
    using Character::destroyed;
    virtual void destroyed(Stage & stage);
    
    // virtual bool canBeHit(Character * enemy);

    virtual bool isHelper() const;
    virtual const std::string getName() const;
    virtual const std::string getDisplayName() const;
    using Character::getAnimation;
    virtual PaintownUtil::ReferenceCount<Animation> getAnimation(int id) const;
    using Character::getState;
    virtual PaintownUtil::ReferenceCount<State> getState(int id, Stage & stage) const;

    void reParent(Character * parent);

    inline Character * getParent() const {
        return owner;
    }
        
    virtual void roundEnd(Mugen::Stage & stage);

    virtual const Character * getRoot() const {
        return root;
    }
        
    virtual PaintownUtil::ReferenceCount<Mugen::Sound> getSound(int group, int item) const;
    virtual PaintownUtil::ReferenceCount<Mugen::Sound> getCommonSound(int group, int item) const;

protected:
    /* the character that spawned this helper */
    Character * owner;
    /* the root character. player -> helper A -> helper B
     * the root of B is the player
     */ 
    const Character * root;
    HitDefinition hit;
    DummyBehavior dummy;
    mutable std::map< int, PaintownUtil::ReferenceCount<Animation> > proxyAnimations;
    mutable std::map< int, PaintownUtil::ReferenceCount<State> > proxyStates;
    int id;
    std::string name;
};

}
