// #include "game/world.h"
#include "helper.h"
#include <vector>
#include <string>

using namespace std;

namespace Mugen{

Helper::Helper(Character * owner, const Character * root, int id, const string & name):
Character(*owner),
owner(owner),
root(root),
id(id),
name(owner->getName() + " " + name + " (helper)"){
    behavior = &dummy;
}

Helper::~Helper(){
}
    
void Helper::destroyed(Stage & stage){
    Character::destroyed(stage);
    stage.removeHelper(this);
}
    
void Helper::reParent(Character * parent){
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

RefState Helper::getState(int id) const {
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
    if (findIt == proxyStates.end() && owner != NULL){
        RefState dad = owner->getState(id);
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
    
PaintownUtil::ReferenceCount<Mugen::Sound> Helper::getSound(int group, int item) const {
    if (owner != NULL){
        return owner->getSound(group, item);
    }
    return PaintownUtil::ReferenceCount<Mugen::Sound>(NULL);
}

PaintownUtil::ReferenceCount<Mugen::Sound> Helper::getCommonSound(int group, int item) const {
    if (owner != NULL){
        return owner->getCommonSound(group, item);
    }
    return PaintownUtil::ReferenceCount<Mugen::Sound>(NULL);
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
    if (findIt == proxyAnimations.end() && owner != NULL){
        if (owner->hasAnimation(id)){
            PaintownUtil::ReferenceCount<Animation> dad = owner->getAnimation(id);
            /* this is why proxyAnimations has to be mutable */
            proxyAnimations[id] = PaintownUtil::ReferenceCount<Animation>(new Animation(*dad));
            return proxyAnimations[id];
        }
    } else {
        if (findIt != proxyAnimations.end()){
            return findIt->second;
        }
    }
    return PaintownUtil::ReferenceCount<Animation>(NULL);
}
    
bool Helper::isHelper() const {
    return true;
}

}
