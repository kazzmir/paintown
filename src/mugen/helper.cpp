// #include "game/world.h"
#include "helper.h"
#include <vector>
#include <string>

using namespace std;

namespace Mugen{

Helper::Helper(Character * owner, const Character * root, int id, const string & name):
Character(*owner),
owner(owner->getId()),
root(root->getId()),
id(id),
name(owner->getName() + " " + name + " (helper)"){
    getLocalData().behavior = &dummy;
    getLocalData().states = owner->getStates();
    getLocalData().animations = owner->getAnimations();
    getLocalData().sounds = owner->getSounds();
    getLocalData().commonSounds = owner->getCommonSounds();
}

Helper::~Helper(){
}
    
void Helper::destroyed(Stage & stage){
    Character::destroyed(stage);
    stage.removeHelper(this);
}
    
void Helper::reParent(const CharacterId & parent){
    this->owner = parent;
}
    
void Helper::roundEnd(Mugen::Stage & stage){
    /* Anything to do here? */
}
    
/*
bool Helper::canBeHit(Character * enemy){
    / * Pretty sure a helper can never be hit * /
    return false;
}
*/

typedef PaintownUtil::ReferenceCount<State> RefState;

RefState Helper::getState(int id, Stage & stage) const {
    /* states -3 and -2 are disabled for helpers */
    if (id == -3 || id == -2){
        return RefState(NULL);
    }
    /* state -1 should be disabled unless the helper was specifically created
     * with input enabled. TODO */
    if (id == -1){
        return RefState(NULL);
    }
    map<int, RefState>::const_iterator findIt = proxyStates.find(id);
    if (findIt == proxyStates.end()){
        RefState dad = Character::getState(id, stage);
        if (dad != NULL){
            /* this is why proxyStates has to be mutable */
            proxyStates[id] = RefState(dad->deepCopy());
            return proxyStates[id];
        }
    } else {
        return findIt->second;
    }
    return RefState(NULL);

}
    
const std::string Helper::getName() const {
    return name;
}
    
const std::string Helper::getDisplayName() const {
    return name;
}

/*
bool Helper::doStates(MugenStage & stage, const std::vector<string> & active, int stateNumber){
    if (getState(stateNumber) == NULL){
        State * state = owner.getState(stateNumber);
        if (state != NULL){
            setState(stateNumber, state->deepCopy());
        }
    }
    return Character::doStates(stage, active, stateNumber);
}
*/
    
PaintownUtil::ReferenceCount<Animation> Helper::getAnimation(int id) const {
    map<int, PaintownUtil::ReferenceCount<Animation> >::const_iterator findIt = proxyAnimations.find(id);
    if (findIt == proxyAnimations.end()){
        PaintownUtil::ReferenceCount<Animation> originalAnimation = Character::getAnimation(id);
        if (originalAnimation != NULL){
            /* We have to copy the animation because the animation stores state about
             * which frame is being shown and we don't want to mess up the
             * original characters animations.
             * this is why proxyAnimations has to be mutable */
            proxyAnimations[id] = PaintownUtil::ReferenceCount<Animation>(new Animation(*originalAnimation));
            return proxyAnimations[id];
        }
    } else {
        return findIt->second;
    }
    return PaintownUtil::ReferenceCount<Animation>(NULL);
}
    
bool Helper::isHelper() const {
    return true;
}

}
