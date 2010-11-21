#include "game/world.h"
#include "helper.h"
#include <vector>
#include <string>

using namespace std;

namespace Mugen{

Helper::Helper(Character & owner):
Object(owner),
owner(owner){
}

Helper::~Helper(){
}

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

}
