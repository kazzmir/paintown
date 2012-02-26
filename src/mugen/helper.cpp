// #include "game/world.h"
#include "helper.h"
#include <vector>
#include <string>

using namespace std;

namespace Mugen{

Helper::Helper(Character * owner, const Character * root, int id):
Character(*owner),
owner(owner),
root(root),
id(id),
name(owner->getName() + " (helper)"){
    behavior = &dummy;
}

Helper::~Helper(){
    for (map<int, State*>::iterator it = proxyStates.begin(); it != proxyStates.end(); it++){
        State * state = (*it).second;
        delete state;
    }
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

State * Helper::getState(int id) const {
    /* states -3 and -2 are disabled for helpers */
    if (id == -3 || id == -2){
        return NULL;
    }
    /* state -1 should be disabled unless the helper was specifically created
     * with input enabled. TODO */
    if (id == -1){
        return NULL;
    }
    map<int, State*>::const_iterator findIt = proxyStates.find(id);
    if (findIt == proxyStates.end() && owner != NULL){
        State * dad = owner->getState(id);
        if (dad != NULL){
            /* this is why proxyStates has to be mutable */
            proxyStates[id] = dad->deepCopy();
            return proxyStates[id];
        }
    } else {
        return findIt->second;
    }
    return NULL;

}
    
const std::string Helper::getName() const {
    return name;
}
    
const std::string Helper::getDisplayName() const {
    return name;
}
    
MugenSound * Helper::getSound(int group, int item) const {
    if (owner != NULL){
        return owner->getSound(group, item);
    }
    return NULL;
}

MugenSound * Helper::getCommonSound(int group, int item) const {
    if (owner != NULL){
        return owner->getCommonSound(group, item);
    }
    return NULL;
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
    
PaintownUtil::ReferenceCount<MugenAnimation> Helper::getAnimation(int id) const {
    map<int, PaintownUtil::ReferenceCount<MugenAnimation> >::const_iterator findIt = proxyAnimations.find(id);
    if (findIt == proxyAnimations.end() && owner != NULL){
        if (owner->hasAnimation(id)){
            PaintownUtil::ReferenceCount<MugenAnimation> dad = owner->getAnimation(id);
            /* this is why proxyAnimations has to be mutable */
            proxyAnimations[id] = PaintownUtil::ReferenceCount<MugenAnimation>(new MugenAnimation(*dad));
            return proxyAnimations[id];
        }
    } else {
        return findIt->second;
    }
    return PaintownUtil::ReferenceCount<MugenAnimation>(NULL);
}
    
bool Helper::isHelper() const {
    return true;
}

#if 0
/* FIXME */
bool Helper::isPaused(){
    return false;
}

/* FIXME */
Physics::Type Helper::getCurrentPhysics() const {
    return Physics::None;
}

/* FIXME */
double Helper::getGravity() const {
    return 0;
}

/* FIXME */
void Helper::setYVelocity(double y){
}

/* FIXME */
double Helper::getYVelocity() const {
    return 0;
}

/* FIXME */
void Helper::changeState(MugenStage & stage, int state, const std::vector<std::string> & inputs){
}

/* FIXME */
double Helper::getXVelocity() const {
    return 0;
}

/* FIXME */
void Helper::setXVelocity(double x){
}

/* FIXME */
bool Helper::canTurn() const {
    return owner.canTurn();
}

/* FIXME */
void Helper::doTurn(MugenStage & stage){
}

/* FIXME */
double Helper::getStandingFriction() const {
    return owner.getStandingFriction();
}

/* FIXME */
const string & Helper::getMoveType() const {
    return "";
}

HitDefinition & Helper::getHit(){
    return hit;
}

const HitDefinition & Helper::getHit() const {
    return hit;
}

/* FIXME */
int Helper::getCurrentJuggle() const {
    return owner.getCurrentJuggle();
}

/* FIXME */
const vector<MugenArea> Helper::getAttackBoxes() const {
    vector<MugenArea> out;
    return out;
}

/* FIXME */
const vector<MugenArea> Helper::getDefenseBoxes() const {
    vector<MugenArea> out;
    return out;
}

/* FIXME */
int Helper::getDefaultSpark() const {
    return owner.getDefaultSpark();
}

/* FIXME */
int Helper::getDefaultGuardSpark() const {
    return owner.getDefaultGuardSpark();
}

/* FIXME */
int Helper::getAttackDistance() const {
    return 0;
}

/* FIXME */
void Helper::guarded(Object * enemy, const HitDefinition & hit){
}

/* FIXME */
void Helper::addPower(double d){
}

/* FIXME */
void Helper::wasHit(MugenStage & stage, Object * enemy, const HitDefinition & hit){
}

/* FIXME */
void Helper::didHit(Object * enemy, MugenStage & stage){
}
    
/* FIXME */
void Helper::act( std::vector< Paintown::Object * > * others, World * world, std::vector< Paintown::Object * > * add ){
}

/* FIXME */
void Helper::draw(Bitmap * work, int rel_x, int rel_y){
}

/* FIXME */
void Helper::grabbed(Paintown::Object * obj){
}

/* FIXME */
void Helper::unGrab(){
}

/* FIXME */
bool Helper::isGrabbed(){
    return false;
}

/* FIXME */
Paintown::Object * Helper::copy(){
    return new Helper(owner);
}

/* FIXME */
const string & Helper::getAttackName(){
    return "";
}

/* FIXME */
bool Helper::collision( ObjectAttack * obj ){
    return false;
}

/* FIXME */
int Helper::getDamage() const {
    return 0;
}

/* FIXME */
bool Helper::isCollidable(Paintown::Object * obj ){
    return false;
}

/* FIXME */
bool Helper::isGettable(){
    return false;
}

/* FIXME */
bool Helper::isGrabbable(Paintown::Object * obj){
    return false;
}

/* FIXME */
bool Helper::isAttacking(){
    return false;
}

/* FIXME */
int Helper::getWidth() const {
    return 0;
}

/* FIXME */
int Helper::getHeight() const {
    return 0;
}

/* FIXME */
Network::Message Helper::getCreateMessage(){
    return owner.getCreateMessage();
}

/* FIXME */
void Helper::getAttackCoords( int & x, int & y){
}

/* FIXME */
double Helper::minZDistance() const {
    return 0;
}

/* FIXME */
void Helper::attacked( World * world, Paintown::Object * something, std::vector< Paintown::Object * > & objects ){
}
#endif

}
