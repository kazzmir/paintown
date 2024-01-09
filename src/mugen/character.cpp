#include "r-tech1/graphics/bitmap.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>
#include <sstream>
#include <iostream>

// To aid in filesize
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "r-tech1/funcs.h"
#include "r-tech1/font.h"
#include "r-tech1/parameter.h"
#include "r-tech1/file-system.h"
#include "r-tech1/timedifference.h"
#include "r-tech1/debug.h"
#include "r-tech1/message-queue.h"
#include "factory/font_render.h"

#include "animation.h"
#include "item.h"
#include "item-content.h"
#include "section.h"
#include "character.h"
#include "sound.h"
#include "reader.h"
#include "sprite.h"
#include "util.h"
#include "stage.h"
#include "globals.h"
#include "state.h"
#include "compiler.h"
// #include "command.h"
#include "constraint.h"
#include "behavior.h"
#include "state-controller.h"
#include "helper.h"

#include "r-tech1/input/input-map.h"
#include "r-tech1/input/input-manager.h"

#include "parse-cache.h"
#include "parser/all.h"
#include "ast/all.h"


using namespace std;

static const int REGENERATE_TIME = 40;

namespace Mugen{

PaintownUtil::Parameter<Filesystem::RelativePath> stateFileParameter;

namespace StateType{

std::string Stand = "S";
std::string Crouch = "C";
std::string Air = "A";
std::string LyingDown = "L";

}

namespace Move{

std::string Attack = "A";
std::string Idle = "I";
std::string Hit = "H";

}

namespace AttackType{
    std::string Normal = "N";
    std::string Special = "S";
    std::string Hyper = "H";
}

namespace PhysicalAttack{
    std::string Normal = "A";
    std::string Throw = "T";
    std::string Projectile = "P";
}

namespace PaintownUtil = ::Util;
    
CharacterId::CharacterId():
id(-1){
}
    
CharacterId::CharacterId(int value):
id(value){
}

CharacterId::~CharacterId(){
}
    
CharacterId::CharacterId(const CharacterId & copy):
id(copy.id){
}

CharacterId& CharacterId::operator=(const CharacterId & copy){
    id = copy.id;
    return *this;
}

bool CharacterId::operator<(const CharacterId & him) const {
    return id < him.id;
}
    
bool CharacterId::operator==(const CharacterId & him) const {
    return id == him.id;
}

bool CharacterId::operator!=(const CharacterId & him) const {
    return !(*this == him);
}
    
int CharacterId::intValue() const {
    return id;
}

static string sourceLocation(const Ast::AttributeSimple & simple, const Filesystem::AbsolutePath & path){
    ostringstream out;
    out << "in " << path.path() << " at line " << simple.getLine() << " column " << simple.getColumn();
    return out.str();
}

State::State(int id):
id(id),
type(Unchanged),
animation(NULL),
changeControl(false),
control(NULL),
changeVelocity(false),
velocity_x(NULL),
velocity_y(NULL),
physics(Physics::None),
changePower(false),
powerAdd(NULL),
moveType(Move::Idle),
juggle(0),
hitDefPersist(false),
layer(0),
changeLayer(false),
spritePriority(NULL),
changeSpritePriority(false){
}
    
State * State::deepCopy() const {
    State * state = new State(id);
    state->type = this->type;
    state->animation = Compiler::copy(this->animation);
    state->changeControl = this->changeControl;
    state->control = Compiler::copy(this->control);
    state->changeVelocity = this->changeVelocity;
    state->velocity_x = Compiler::copy(this->velocity_x);
    state->velocity_y = Compiler::copy(this->velocity_y);
    state->physics = this->physics;
    state->changePower = this->changePower;
    state->powerAdd = Compiler::copy(this->powerAdd);
    state->moveType = this->moveType;
    state->juggle = Compiler::copy(this->juggle);
    state->hitDefPersist = this->hitDefPersist;
    state->layer = this->layer;
    state->changeLayer = this->changeLayer;
    state->spritePriority = Compiler::copy(this->spritePriority);
    state->changeSpritePriority = this->changeSpritePriority;
    for (vector<StateController*>::const_iterator it = controllers.begin(); it != controllers.end(); it++){
        StateController * controller = *it;
        state->addController(controller->deepCopy());
    }
    return state;
}
    
StateController * State::findControllerById(unsigned int id) const {
    for (vector<StateController*>::const_iterator it = controllers.begin(); it != controllers.end(); it++){
        StateController * controller = *it;
        if (controller->getId() == id){
            return controller;
        }
    }
    return NULL;
}

void State::addController(StateController * controller){
#if 0
    for (vector<StateController*>::iterator it = controllers.begin(); it != controllers.end(); /**/ ){
        StateController * mine = *it;

        /* controllers must have unique names */
        if (mine->getName() == controller->getName()){
            delete mine;
            it = controllers.erase(it);
        } else {
            it++;
        }
    }
#endif

    controllers.push_back(controller);
}

void State::addControllerFront(StateController * controller){
#if 0
    for (vector<StateController*>::iterator it = controllers.begin(); it != controllers.end(); /**/ ){
        StateController * mine = *it;
        /* controllers must have unique names */
        if (mine->getName() == controller->getName()){
            delete mine;
            it = controllers.erase(it);
        } else {
            it++;
        }
    }
#endif

    controllers.insert(controllers.begin(), controller);
}
    
void State::setSpritePriority(Compiler::Value * priority){
    changeSpritePriority = true;
    spritePriority = priority;
}

void State::setJuggle(Compiler::Value * juggle){
    this->juggle = juggle;
}

void State::setVelocity(Compiler::Value * x, Compiler::Value * y){
    changeVelocity = true;
    velocity_x = x;
    velocity_y = y;
}

void State::setPower(Compiler::Value * power){
    powerAdd = power;
    changePower = true;
}
    
void State::setMoveType(const std::string & type){
    moveType = type;
}
    
void State::setPhysics(Physics::Type p){
    physics = p;
}

void State::setLayer(int layer){
    this->layer = layer;
    changeLayer = true;
}

void State::transitionTo(const Mugen::Stage & stage, Character & who){
    /* TODO: handle layer */

    FullEnvironment environment(stage, who);

    if (animation != NULL){
        who.setAnimation((int) animation->evaluate(environment).toNumber());
    }

    if (changeControl){
        who.setControl(control->evaluate(environment).toBool());
    }

    if (juggle != NULL){
        who.setCurrentJuggle((int) juggle->evaluate(environment).toNumber());
    }

    if (changeSpritePriority && spritePriority != NULL){
        who.setSpritePriority((int) spritePriority->evaluate(environment).toNumber());
    }

    if (powerChanged()){
        who.addPower(getPower()->evaluate(FullEnvironment(stage, who)).toNumber());
    }

    who.setMoveType(moveType);

    /* I don't think a hit definition should persist across state changes, unless
     * hitdefpersist is set to 1. Anyway this is needed because a state could try
     * to repeatedly set the same hitdef, like:
     *   state 200
     *   type = hitdef
     *   trigger1 = animelem = 3
     * But since the animelem won't change every game tick, this trigger will be
     * activated a few times (like 3-4).
     * Resetting the hitdef to NULL allows the hitdef controller to ensure only
     * unique hitdef's get set as well as repeat the same hitdef in sequence.
     *
     * Update: 7/28/2010: I don't think this is true anymore because `animelem'
     * is only true for the first tick of an animation. `animelem = 3' won't be true
     * for as long as the animation is 3, just the first tick of the game that its 3.
     */
    if (!doesHitDefPersist()){
        who.disableHit();
        /*
        who.setHitDef(NULL);
        */
    }

    who.setReversalInactive();

    if (changeVelocity){
        who.setXVelocity(velocity_x->evaluate(FullEnvironment(stage, who)).toNumber());
        who.setYVelocity(velocity_y->evaluate(FullEnvironment(stage, who)).toNumber());
        // Global::debug(0) << "Velocity set to " << velocity_x->evaluate(FullEnvironment(stage, who)).toNumber() << ", " << velocity_y->evaluate(FullEnvironment(stage, who)).toNumber() << endl;
    }

    if (physics != Physics::Unchanged){
        who.setCurrentPhysics(physics);
    }

    switch (type){
        case Standing : {
            who.setStateType(StateType::Stand);
            break;
        }
        case Crouching : {
            who.setStateType(StateType::Crouch);
            break;
        }
        case Air : {
            who.setStateType(StateType::Air);
            break;
        }
        case LyingDown : {
            who.setStateType(StateType::LyingDown);
            break;
        }
        case Unchanged : {
            break;
        }
    }
    
    for (vector<StateController*>::iterator it = controllers.begin(); it != controllers.end(); it++){
        StateController * controller = *it;
        controller->resetPersistent();
    }
}

State::~State(){
    for (vector<StateController*>::iterator it = controllers.begin(); it != controllers.end(); it++){
        delete (*it);
    }

    delete powerAdd;
    delete animation;
    delete control;
    delete juggle;
    delete velocity_x;
    delete velocity_y;
}

/* Called when the player was hit */
static void update(HitState & state, Mugen::Stage & stage, const Character & guy, bool inAir, const HitDefinition & hit){
    /* FIXME: choose the proper ground/air/guard types */

    state.guarded = false;
    state.shakeTime = hit.pause.player2;
    state.yAcceleration = hit.yAcceleration;
    state.airType = hit.airType;
    state.groundType = hit.groundType;

    state.recoverTime = guy.getLieDownTime();

    state.chainId = hit.id;

    state.spritePriority = hit.player2SpritePriority;
    state.moveContact = 0;

    /* FIXME: set damage */
    
    /* if in the air */
    if (inAir){
        state.hitType = hit.airType;
        if (state.fall.fall){
            if (hit.animationTypeFall == AttackType::NoAnimation){
                if (hit.animationTypeAir == AttackType::Up){
                    state.animationType = AttackType::Up;
                } else {
                    state.animationType = AttackType::Back;
                }
            } else {
                state.animationType = hit.animationTypeFall;
            }

            state.hitTime = 0;
        } else {
            if (hit.animationTypeAir != AttackType::NoAnimation){
                state.animationType = hit.animationTypeAir;
            } else {
                state.animationType = hit.animationType;
            }

            state.hitTime = hit.airHitTime;
        }
        
        state.xVelocity = hit.airVelocity.x;
        state.yVelocity = hit.airVelocity.y;

        state.fall.fall |= hit.fall.fall;
        state.fall.yVelocity = hit.fall.yVelocity;
        state.fall.xVelocity = hit.fall.xVelocity;
        state.fall.changeXVelocity = hit.fall.changeXVelocity;
        int groundSlideTime = (int) hit.groundSlideTime;
        state.returnControlTime = hit.airGuardControlTime;
    } else {
        state.hitType = hit.groundType;
        int groundSlideTime = (int) hit.groundSlideTime;
        state.animationType = hit.animationType;
        state.returnControlTime = hit.guardControlTime;
        state.hitTime = hit.groundHitTime;
        state.slideTime = groundSlideTime;
        state.xVelocity = hit.groundVelocity.x;
        state.yVelocity = hit.groundVelocity.y;
        state.fall.fall = hit.fall.fall;
        state.fall.yVelocity = hit.fall.yVelocity;
        state.fall.xVelocity = hit.fall.xVelocity;
        state.fall.changeXVelocity = hit.fall.changeXVelocity;
    }

    state.fall.envShake.time = hit.fall.envShake.time;

    // Global::debug(0) << "Hit definition: shake time " << shakeTime << " hit time " << hitTime << endl;
}

/*
Character::Character( const char * location ):
ObjectAttack(0),
commonSounds(NULL),
hit(NULL){
    this->location = std::string(location);
    initialize();
}
*/
    
Object::Object(){
}

Object::Object(int alliance):
attack_ticket(0),
alliance(alliance),
ticket(0){
}

Object::Object(const Object & copy):
attack_ticket(copy.attack_ticket),
alliance(copy.alliance),
ticket(copy.ticket){
}
    
Object::~Object(){
}

/*
void Object::doMovement(const std::vector<Character*> & objects, Stage & stage){
}
*/
    
/*
unsigned int Object::getTicket() const {
    return ticket;
}

void Object::nextTicket(){
    ticket += 1;
}
*/

int Object::getAlliance() const {
    return alliance;
}

void Object::setAlliance(int alliance){
    this->alliance = alliance;
}
    
void Character::setZ(double what){
    getStateData().virtualz = what;
}
    
void Character::setX(double what){
    getStateData().virtualx = what;
}

void Character::setY(double what){
    getStateData().virtualy = what;
}

double Character::getY() const {
    return getStateData().virtualy;
}

double Character::getX() const {
    return getStateData().virtualx;
}

double Character::getZ() const {
    return getStateData().virtualz;
}
    
double Character::getRY() const {
    return getZ() + getY();
}

void Character::moveLeft(double x){
    if (getFacing() == FacingLeft){
        moveX(x);
    } else {
        moveX(-x);
    }
}

void Character::moveLeftForce(double x){
    if (getFacing() == FacingLeft){
        moveX(x, true);
    } else {
        moveX(-x, true);
    }
}

void Character::moveRight(double x){
    if (getFacing() == FacingRight){
        moveX(x);
    } else {
        moveX(-x);
    }
}

void Character::moveRightForce(double x){
    if (getFacing() == FacingRight){
        moveX(x, true);
    } else {
        moveX(-x, true);
    }
}

Facing Character::getFacing() const {
    return getStateData().facing;
}

void Character::setFacing(Facing what){
    this->getStateData().facing = what;
}

Facing Character::getOppositeFacing() const {
    switch (getFacing()){
        case FacingLeft: return FacingRight;
        case FacingRight: return FacingLeft;
    }
    return FacingRight;
}
    
void Character::reverseFacing(){
    setFacing(getOppositeFacing());
}

Character::Character(const Filesystem::AbsolutePath & s, int alliance):
Object(alliance){
    getLocalData().location = s;
    initialize();
}

Character::Character(const Filesystem::AbsolutePath & s, const int x, const int y, int alliance):
Object(alliance){
    getLocalData().location = s;
    initialize();
}

Character::Character(const Character & copy):
Object(copy),
stateControllerId(copy.stateControllerId),
localData(copy.localData),
stateData(copy.stateData){
}

Character::~Character(){
    stopRecording();
    for (vector<Command2*>::iterator it = getLocalData().commands.begin(); it != getLocalData().commands.end(); it++){
        delete (*it);
    }
}

void Character::initialize(){
    stateControllerId = 0;

    getLocalData().max_health = 0;
    getStateData().health = 0;
    getLocalData().maxChangeStates = 0;
    getStateData().currentState = Standing;
    getStateData().currentPhysics = Physics::Stand;
    setMoveType(Move::Idle);
    getStateData().wasHitCounter = 0;
    getStateData().frozen = false;
    getStateData().reversalActive = false;
    getStateData().previousState = getStateData().currentState;
    setStateType(StateType::Stand);
    getStateData().currentAnimation = Standing;
    getLocalData().ownPalette = false;
    getStateData().drawAngle = 0;
    /* FIXME: whats the default sprite priority? */
    getStateData().spritePriority = 0;
    // getLocalData().juggleRemaining = 0;
    getLocalData().koecho = false;
    getLocalData().defense = 0;
    getLocalData().fallDefenseUp = 0;
    getStateData().defenseMultiplier = 1;
    getStateData().attackMultiplier = 1;
    getLocalData().lieDownTime = 0;
    getLocalData().xscale = 1;
    getLocalData().yscale = 1;
    getLocalData().debug = false;
    getStateData().has_control = true;
    getStateData().blocking = false;
    getLocalData().airjumpnum = 0;
    getLocalData().airjumpheight = 35;
    getStateData().guarding = false;
    getLocalData().behavior = NULL;

    getLocalData().intpersistindex = 0;
    getLocalData().floatpersistindex = 0;

    getLocalData().matchWins = 0;

    getStateData().combo = 1;
    // nextCombo = 0;

    // lastTicket = 0;

    /* Load up info for the select screen */
    //loadSelectData();

    /* provide sensible defaults */
    getLocalData().walkfwd = 0;
    getLocalData().walkback = 0;
    getLocalData().runbackx = 0;
    getLocalData().runbacky = 0;
    getLocalData().runforwardx = 0;
    getLocalData().runforwardy = 0;
    getStateData().power = 0;

    getStateData().velocity_x = 0;
    getStateData().velocity_y = 0;

    getLocalData().gravity = 0.1;
    getLocalData().standFriction = 0.85;
    getLocalData().crouchFriction = 0.82;

    getStateData().stateTime = 0;

    /* Regeneration */
    getLocalData().regenerateHealth = false;
    getLocalData().regenerating = false;
    getLocalData().regenerateTime = REGENERATE_TIME;
    getLocalData().regenerateHealthDifference = 0;
}

void Character::loadSelectData(){
    /* Load up info for the select screen */
    try{
        Filesystem::AbsolutePath baseDir = getLocalData().location.getDirectory();
	Global::debug(1) << baseDir.path() << endl;
        Filesystem::RelativePath str = Filesystem::RelativePath(getLocalData().location.getFilename().path());
	const Filesystem::AbsolutePath ourDefFile = Util::fixFileName(baseDir, str.path() + ".def");
	
	if (ourDefFile.isEmpty()){
	    Global::debug(1) << "Cannot locate player definition file for: " << getLocalData().location.path() << endl;
	}
	
        AstRef parsed(Util::parseDef(ourDefFile));
	// Set name of character
	this->getLocalData().name = Mugen::Util::probeDef(parsed, "info", "name");
	this->getLocalData().displayName = Mugen::Util::probeDef(parsed, "info", "displayname");
	this->getLocalData().sffFile = Mugen::Util::probeDef(parsed, "files", "sprite");
	// Get necessary sprites 9000 & 9001 for select screen
        /* FIXME: replace 9000 with some readable constant */
        Filesystem::AbsolutePath absoluteSff = Storage::instance().lookupInsensitive(baseDir, Filesystem::RelativePath(this->getLocalData().sffFile));
        /* FIXME: use getIconAndPortrait so we only load the sff once */
	this->getLocalData().sprites[9000][0] = Mugen::Util::probeSff(absoluteSff, 9000, 0, true);
	this->getLocalData().sprites[9000][1] = Mugen::Util::probeSff(absoluteSff, 9000, 1, true);
	
    } catch (const MugenException &ex){
	Global::debug(1) << "Couldn't grab details for character!" << endl;
	Global::debug(1) << "Error was: " << ex.getReason() << endl;
    }
}
    
void Character::addCommand(Command2 * command){
    getLocalData().commands.push_back(command);
}

void Character::setAnimation(int animation, int element){
    if (getAnimation(animation) != NULL){
        getLocalData().foreignAnimation = NULL;
        getStateData().currentAnimation = animation;
        if (getCurrentAnimation() != NULL){
            getCurrentAnimation()->reset();
            getCurrentAnimation()->setPosition(element);
        }
    } else {
        Global::debug(0, getDisplayName()) << "No animation for " << animation << std::endl;
    }
}

static void mergeStates(map<int, PaintownUtil::ReferenceCount<State> > & mergeIn,
                        map<int, PaintownUtil::ReferenceCount<State> > & from){
    for (map<int, PaintownUtil::ReferenceCount<State> >::iterator it = from.begin(); it != from.end(); it++){
        mergeIn[it->first] = it->second;
    }
}

void Character::loadCmdFile(const Filesystem::RelativePath & path){
    Filesystem::AbsolutePath full = Storage::instance().lookupInsensitive(getLocalData().baseDir, path);
    map<int, PaintownUtil::ReferenceCount<State> > out;
    try{
        int defaultTime = 15;
        int defaultBufferTime = 1;

        AstRef parsed(Util::parseCmd(full));
        PaintownUtil::ReferenceCount<State> currentState;
        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            head = Util::fixCase(head);

            if (head == "command"){
                class CommandWalker: public Ast::Walker {
                public:
                    CommandWalker(Character & self, const int defaultTime, const int defaultBufferTime, const Filesystem::AbsolutePath & full):
                        self(self),
                        time(defaultTime),
                        bufferTime(defaultBufferTime),
                        key(NULL),
                        path(full){
                        }

                    Character & self;
                    int time;
                    int bufferTime;
                    string name;
                    Ast::Key * key;
                    const Filesystem::AbsolutePath & path;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "name"){
                            simple.view() >> name;
                        } else if (simple == "command"){
                            key = (Ast::Key*) simple.getValue()->copy();
                        } else if (simple == "time"){
                            simple.view() >> time;
                        } else if (simple == "buffer.time"){
                            simple.view() >> bufferTime;
                            /* Time that the command will be buffered for. If the command is done
                             * successfully, then it will be valid for this time. The simplest
                             * case is to set this to 1. That means that the command is valid
                             * only in the same tick it is performed. With a higher value, such
                             * as 3 or 4, you can get a "looser" feel to the command. The result
                             * is that combos can become easier to do because you can perform
                             * the command early.
                             */
                        }
                    }

                    virtual void onSectionFinished(const Ast::Section & section){
                        if (name == ""){
                            ostringstream out;
                            out << "No name given for command at line " << section.getLine();
                            throw MugenException(out.str(), __FILE__, __LINE__);
                        }

                        if (key == NULL){
                            Global::debug(0) << section.toString() << std::endl;
                            ostringstream out;
                            out << "No 'command' attribute given for command '" << name << "' at line " << section.getLine();
                            throw MugenException(out.str(), __FILE__, __LINE__);
                        }

                        try{
                            /* parser guarantees the key will be a KeyList */
                            self.addCommand(new Command2(name, (Ast::KeyList*) key, time, bufferTime));
                        } catch (const MugenException & fail){
                            Global::debug(0) << "Could not add command for '" << name << "': " << fail.getTrace() << std::endl;
                        }
                    }
                };

                CommandWalker walker(*this, defaultTime, defaultBufferTime, full);
                section->walk(walker);
            } else if (head == "defaults"){
                class DefaultWalker: public Ast::Walker {
                public:
                    DefaultWalker(int & time, int & buffer):
                        time(time),
                        buffer(buffer){
                        }

                    int & time;
                    int & buffer;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "command.time"){
                            simple.view() >> time;
                        } else if (simple == "command.buffer.time"){
                            simple.view() >> buffer;
                        }
                    }
                };

                DefaultWalker walker(defaultTime, defaultBufferTime);
                section->walk(walker);
            } else if (PaintownUtil::matchRegex(head, PaintownUtil::Regex("statedef"))){
                currentState = parseStateDefinition(section, full, out);
            } else if (PaintownUtil::matchRegex(head, PaintownUtil::Regex("state "))){
                if (currentState != NULL){
                    currentState->addController(parseState(section));
                } else {
                    /* not all states in the .cmd file will have a statedef */
                    StateController * controller = parseState(section);
                    if (getSelfState(controller->getState()) != NULL){
                        getSelfState(controller->getState())->addController(controller);
                    } else {
                        delete controller;
                    }
                }
            }

            /* [Defaults]
             * ; Default value for the "time" parameter of a Command. Minimum 1.
             * command.time = 15
             *
             * ; Default value for the "buffer.time" parameter of a Command. Minimum 1,
             * ; maximum 30.
             * command.buffer.time = 1
             */
        }
    } catch (const MugenException & fail){
        ostringstream out;
        out << "Could not parse " << path.path() << ": " << fail.getFullReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    } catch (const Mugen::Cmd::ParseException & e){
        /*
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
        */
        ostringstream out;
        out << "Could not parse " << path.path() << ": " << e.getReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
                
    mergeStates(getLocalData().states, out);
}

static bool isStateDefSection(string name){
    name = Util::fixCase(name);
    return PaintownUtil::matchRegex(name, PaintownUtil::Regex("state ")) ||
           PaintownUtil::matchRegex(name, PaintownUtil::Regex("statedef "));
}

static AttackType::Attribute parseAttribute(const string & kind, const string & action){
    map<string, AttackType::Attribute> attributes;
    attributes["na"] = AttackType::NormalAttack;
    attributes["nt"] = AttackType::NormalThrow;
    attributes["np"] = AttackType::NormalProjectile;
    attributes["sa"] = AttackType::SpecialAttack;
    attributes["st"] = AttackType::SpecialThrow;
    attributes["sp"] = AttackType::SpecialProjectile;
    attributes["ha"] = AttackType::HyperAttack;
    attributes["ht"] = AttackType::HyperThrow;
    attributes["hp"] = AttackType::HyperProjectile;

    string use = PaintownUtil::lowerCaseAll(kind) + PaintownUtil::lowerCaseAll(action);
    if (attributes.find(use) != attributes.end()){
        return attributes[use];
    }

    return AttackType::NoAttribute;
}

/* true if the enemy's hitdef uses an attribute that matches the given hitattribute */
static bool hasHitAttribute(Character * enemy, const HitAttributes & attribute){
    /* Check the state type */
    if (enemy->getHit().attribute.state == StateType::Crouch &&
        !attribute.crouching){
        return false;
    }

    if (enemy->getHit().attribute.state == StateType::Stand &&
        !attribute.standing){
        return false;
    }

    if (enemy->getHit().attribute.state == StateType::Air &&
        !attribute.aerial){
        return false;
    }

    /* Then check the physics type */
    AttackType::Attribute hitType = parseAttribute(enemy->getHit().attribute.attackType, enemy->getHit().attribute.physics);
    const vector<AttackType::Attribute> & attributes = attribute.attributes;

    /* The hit type of the hit definition must be in the list of attributes somewhere */
    for (vector<AttackType::Attribute>::const_iterator it = attributes.begin(); it != attributes.end(); it++){
        if (hitType == *it){
            return true;
        }
    }

    return false;
}

bool Character::canBeHit(Character * enemy){
    for (int slot = 0; slot < 2; slot++){
        /* Only check active slots */
        if (getStateData().hitByOverride[slot].time > 0){
            /* FIXME: use hasHitAttribute from above */

            /* Check the state type */
            if (enemy->getHit().attribute.state == StateType::Crouch &&
                !getStateData().hitByOverride[slot].crouching){
                return false;
            }
            
            if (enemy->getHit().attribute.state == StateType::Stand &&
                !getStateData().hitByOverride[slot].standing){
                return false;
            }
            
            if (enemy->getHit().attribute.state == StateType::Air &&
                !getStateData().hitByOverride[slot].aerial){
                return false;
            }

            /* Then check the physics type */
            AttackType::Attribute hitType = parseAttribute(enemy->getHit().attribute.attackType, enemy->getHit().attribute.physics);
            const vector<AttackType::Attribute> & attributes = getStateData().hitByOverride[slot].attributes;
            bool ok = false;

            /* The hit type of the hit definition must be in the list of attributes somewhere */
            for (vector<AttackType::Attribute>::const_iterator it = attributes.begin(); it != attributes.end(); it++){
                if (hitType == *it){
                    ok = true;
                    break;
                }
            }

            /* If not then return false immediately */
            if (!ok){
                return false;
            }
        }
    }

    return (getMoveType() != Move::Hit) ||
           (getMoveType() == Move::Hit && getStateData().juggleRemaining >= enemy->getCurrentJuggle());
}
    
void Character::setConstant(std::string name, const vector<double> & values){
    getLocalData().constants[name] = Constant(values);
}

void Character::setConstant(std::string name, double value){
    getLocalData().constants[name] = Constant(value);
}

void Character::setFloatVariable(int index, const RuntimeValue & value){
    getStateData().floatVariables[index] = value;
}

void Character::setVariable(int index, const RuntimeValue & value){
    getStateData().variables[index] = value;
}

static RuntimeValue extractVariable(const map<int, RuntimeValue> & stuff, int index){
    map<int, RuntimeValue>::const_iterator found = stuff.find(index);
    if (found != stuff.end()){
        return (*found).second;
    }
    return RuntimeValue(0);
}

RuntimeValue Character::getVariable(int index) const {
    return extractVariable(getStateData().variables, index);
}

RuntimeValue Character::getFloatVariable(int index) const {
    return extractVariable(getStateData().floatVariables, index);
}
        
void Character::setSystemVariable(int index, const RuntimeValue & value){
    getStateData().systemVariables[index] = value;
}

RuntimeValue Character::getSystemVariable(int index) const {
    return extractVariable(getStateData().systemVariables, index);
}
        
void Character::resetStateTime(){
    getStateData().stateTime = 0;
}
        
void Character::resetJugglePoints(){
    getStateData().juggleRemaining = getJugglePoints();
}
    
/*
void Character::changeState(Mugen::Stage & stage, int state){
    vector<string> empty;
    changeState(stage, state, empty);
}
*/

/* Updates the current state but doesn't activate it until the current
 * pause time reaches 0.
 */
void Character::delayChangeState(Mugen::Stage & stage, int stateNumber){
    getLocalData().maxChangeStates += 1;
    if (getLocalData().maxChangeStates > 100){
        std::ostringstream out;
        out << "ChangeState called more than 100 times, most likely this is a bug";
        Global::debug(0) << out.str() << std::endl;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
    /* dont let after images carry over to the next state
     * UPDATE: mugen actually allows this
     */
    // afterImage.show = false;

    /* reset juggle points once the player gets up */
    if (stateNumber == GetUpFromLiedown){
        resetJugglePoints();
    }

    /* FIXME: handle movehitpersist
     * Note 2: the values of the four Move* triggers reset to 0 and stop incrementing after a state transition. See "movehitpersist" parameter for StateDefs (CNS docs) for how to override this behavior.
     */
    getHitState().moveContact = 0;

    /* reset hit count */
    getStateData().hitCount = 0;

    ostringstream debug;
    debug << getDisplayName() << "-" << getId().intValue();
    Global::debug(1, debug.str()) << "Tick " << stage.getTicks() << " Change from state " << getCurrentState() << " to state " << stateNumber << endl;
    getStateData().previousState = getCurrentState();
    setCurrentState(stateNumber);
    getStateData().stateTime = -1;
    /*
    if (getState(currentState) != NULL){
        PaintownUtil::ReferenceCount<State> state = getState(currentState);
        state->transitionTo(stage, *this);
        doStates(stage, active, currentState);
    } else {
        Global::debug(0, debug.str()) << "Unknown state " << currentState << endl;
    }
    */
}

/* Immediately changes to a state and activates it */
void Character::changeState(Mugen::Stage & stage, int stateNumber){

    getLocalData().maxChangeStates += 1;
    if (getLocalData().maxChangeStates > 100){
        std::ostringstream out;
        out << "ChangeState called more than 100 times, most likely this is a bug";
        Global::debug(0) << out.str() << std::endl;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
    /* dont let after images carry over to the next state
     * UPDATE: mugen actually allows this
     */
    // afterImage.show = false;

    /* reset juggle points once the player gets up */
    if (stateNumber == GetUpFromLiedown){
        resetJugglePoints();
    }

    /* FIXME: handle movehitpersist
     * Note 2: the values of the four Move* triggers reset to 0 and stop incrementing after a state transition. See "movehitpersist" parameter for StateDefs (CNS docs) for how to override this behavior.
     */
    getHitState().moveContact = 0;

    /* reset hit count */
    getStateData().hitCount = 0;

    ostringstream debug;
    debug << getDisplayName() << "-" << getId().intValue();
    Global::debug(1, debug.str()) << "Tick " << stage.getTicks() << " Change from state " << getCurrentState() << " to state " << stateNumber << endl;
    getStateData().previousState = getCurrentState();
    setCurrentState(stateNumber);
    resetStateTime();
    if (getState(getCurrentState(), stage) != NULL){
        PaintownUtil::ReferenceCount<State> state = getState(getCurrentState(), stage);
        state->transitionTo(stage, *this);
        doStates(stage, getStateData().active, getCurrentState());
    } else {
        Global::debug(0, debug.str()) << "Unknown state " << getCurrentState() << endl;
    }
}

/* TODO: get rid of the inputs parameter */
void Character::changeOwnState(Mugen::Stage & stage, int state, const std::vector<std::string> & inputs){
    getStateData().characterData.who = CharacterId(-1);
    getStateData().characterData.enabled = false;
    changeState(stage, state);
}

void Character::loadCnsFile(const Filesystem::RelativePath & path){
    Filesystem::AbsolutePath full = Storage::instance().findInsensitive(Storage::instance().cleanse(getLocalData().baseDir).join(path));
    try{
        /* cns can use the Cmd parser */
        AstRef parsed(Util::parseCmd(full));
        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            head = Util::fixCase(head);
            if (false && !isStateDefSection(head)){
                /* I dont think this is the right way to do it */
                class AttributeWalker: public Ast::Walker {
                public:
                    AttributeWalker(Character & who):
                    self(who){
                    }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        string name = simple.idString();
                        if (simple.getValue() != 0 && simple.getValue()->hasMultiple()){
                            vector<double> values;
                            simple.view() >> values;
                            self.setConstant(name, values);
                        } else {
                            double value;
                            simple.view() >> value;
                            self.setConstant(name, value);
                        }
                    }
                };

                AttributeWalker walker(*this);
                section->walk(walker);
            } else if (head == "velocity"){
                class VelocityWalker: public Ast::Walker {
                public:
                    VelocityWalker(Character & who):
                    self(who){
                    }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "walk.fwd"){
                            double speed;
                            simple.view() >> speed;
                            self.setWalkForwardX(speed);
                        } else if (simple == "walk.back"){
                            double speed;
                            simple.view() >> speed;
                            self.setWalkBackX(speed);
                        } else if (simple == "run.fwd"){
                            double x, y;
                            simple.view() >> x >> y;
                            self.setRunForwardX(x);
                            self.setRunForwardY(y);
                        } else if (simple == "run.back"){
                            double x, y;
                            simple.view() >> x >> y;
                            self.setRunBackX(x);
                            self.setRunBackY(y);
                        } else if (simple == "jump.neu"){
                            double x, y;
                            simple.view() >> x >> y;
                            self.setNeutralJumpingX(x);
                            self.setNeutralJumpingY(y);
                        } else if (simple == "jump.back"){
                            double speed;
                            simple.view() >> speed;
                            self.setJumpBack(speed);
                        } else if (simple == "jump.fwd"){
                            double speed;
                            simple.view() >> speed;
                            self.setJumpForward(speed);
                        } else if (simple == "runjump.back"){
                            double speed;
                            simple.view() >> speed;
                            self.setRunJumpBack(speed);
                        } else if (simple == "runjump.fwd"){
                            double speed;
                            simple.view() >> speed;
                            self.setRunJumpForward(speed);
                        } else if (simple == "airjump.neu"){
                            double x, y;
                            simple.view() >> x >> y;
                            self.setAirJumpNeutralX(x);
                            self.setAirJumpNeutralY(y);
                        } else if (simple == "airjump.back"){
                            double speed;
                            simple.view() >> speed;
                            self.setAirJumpBack(speed);
                        } else if (simple == "airjump.fwd"){
                            double speed;
                            simple.view() >> speed;
                            self.setAirJumpForward(speed);
                        } else if (simple == "air.gethit.airrecover.mul"){
                            /* FIXME: what are the right defaults here? */
                            double x = 0;
                            double y = 0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & ignore){
                            }

                            self.setAirHitRecoverMultiplierX(x);
                            self.setAirHitRecoverMultiplierY(y);
                        }  else if (simple == "air.gethit.groundrecover"){
                            /* FIXME: what are the right defaults here? */
                            double x = 0;
                            double y = 0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & ignore){
                            }

                            self.setAirHitGroundRecoverX(x);
                            self.setAirHitGroundRecoverY(y);
                        }


                        /* TODO
                        air.gethit.groundrecover = -.15,-3.5
                        air.gethit.airrecover.add = 0,-4.5
                        air.gethit.airrecover.back = -1
                        air.gethit.airrecover.fwd = 0
                        air.gethit.airrecover.up = -2
                        air.gethit.airrecover.down = 1.5
                        */
                    }
                };

                VelocityWalker walker(*this);
                section->walk(walker);
            } else if (head == "data"){
                class DataWalker: public Ast::Walker {
                public:
                    DataWalker(Character & who):
                    self(who){
                    }
                    
                    Character & self;

                    ResourceEffect extractResource(const Ast::Value * value){
                        class Walker: public Ast::Walker {
                        public:
                            Walker():
                            found(false){
                            }

                            virtual void onResource(const Ast::Resource & resource){
                                this->resource.own = resource.isOwn();
                                resource.getValue()->view() >> this->resource.group;
                                found = true;
                            }

                            ResourceEffect resource;
                            bool found;
                        };

                        Walker walker;
                        value->walk(walker);
                        if (!walker.found){
                            walker.resource.own = false;
                            value->view() >> walker.resource.group;
                        }
                        return walker.resource;
                    }

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "liedown.time"){
                            int x;
                            simple.view() >> x;
                            self.setLieDownTime(x);
                        } else if (simple == "airjuggle"){
                            int x;
                            simple.view() >> x;
                            self.setJugglePoints(x);
                            self.resetJugglePoints();
                        } else if (simple == "life"){
                            int x;
                            simple.view() >> x;
                            self.setMaxHealth(x);
                            self.setHealth(x);
                        } else if (simple == "sparkno"){
                            self.setDefaultSpark(extractResource(simple.getValue()));
                        } else if (simple == "guard.sparkno"){
                            self.setDefaultGuardSpark(extractResource(simple.getValue()));
                        } else if (simple == "fall.defence_up"){
                            int x;
                            simple.view() >> x;
                            self.setFallDefenseUp(x);
                        } else if (simple == "defence"){
                            int x;
                            simple.view() >> x;
                            self.setDefense(x);
                        } else if (simple == "ko.echo"){
                            bool what;
                            simple.view() >> what;
                            self.setKoEcho(what);
                        } else if (simple == "intpersistindex"){
                            int x;
                            simple.view() >> x;
                            self.setIntPersistIndex(x);
                        } else if (simple == "floatpersistindex"){
                            int x;
                            simple.view() >> x;
                            self.setFloatPersistIndex(x);
                        }

                        /* TODO: handle all the other parameters in [Data] */
                    }

                };
                
                DataWalker walker(*this);
                section->walk(walker);
            } else if (head == "size"){
                class SizeWalker: public Ast::Walker {
                public:
                    SizeWalker(Character & self):
                        self(self){
                        }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "height"){
                            int x;
                            simple.view() >> x;
                            self.setHeight(x);
                        } else if (simple == "xscale"){
			    simple.view() >> self.getLocalData().xscale;
			} else if (simple == "yscale"){
			    simple.view() >> self.getLocalData().yscale;
			} else if (simple == "ground.back"){
			    simple.view() >> self.getLocalData().groundback;
			} else if (simple == "ground.front"){
			    simple.view() >> self.getLocalData().groundfront;
			} else if (simple == "air.back"){
			    simple.view() >> self.getLocalData().airback;
			} else if (simple == "air.front"){
			    simple.view() >> self.getLocalData().airfront;
			} else if (simple == "attack.dist"){
			    simple.view() >> self.getLocalData().attackdist;
			} else if (simple == "proj.attack.dist"){
			    simple.view() >> self.getLocalData().projattackdist;
			} else if (simple == "proj.doscale"){
			    simple.view() >> self.getLocalData().projdoscale;
			} else if (simple == "head.pos"){
			    int x=0,y=0;
			    try{
				simple.view() >> self.getLocalData().headPosition.x >> self.getLocalData().headPosition.y;
			    } catch (const Ast::Exception & e){
			    }
			} else if (simple == "mid.pos"){
			    int x=0,y=0;
			    try{
				simple.view() >> self.getLocalData().midPosition.x >> self.getLocalData().midPosition.y;
			    } catch (const Ast::Exception & e){
			    }
			} else if (simple == "shadowoffset"){
			    simple.view() >> self.getLocalData().shadowoffset;
			} else if (simple == "draw.offset"){
			    int x=0,y=0;
			    try{
				simple.view() >> self.getLocalData().drawOffset.x >> self.getLocalData().drawOffset.y;
			    } catch (const Ast::Exception & e){
			    }
			}
                    }
                };
                
                SizeWalker walker(*this);
                section->walk(walker);

            } else if (head == "movement"){
                class MovementWalker: public Ast::Walker {
                public:
                    MovementWalker(Character & who):
                    self(who){
                    }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "yaccel"){
                            double n;
                            simple.view() >> n;
                            self.setGravity(n);
                        } else if (simple == "crouch.friction"){
                            double n;
                            simple.view() >> n;
                            self.setCrouchingFriction(n);
                        } else if (simple == "crouch.friction.threshold"){
                            double n;
                            simple.view() >> n;
                            self.setCrouchingFrictionThreshold(n);
                        } else if (simple == "stand.friction"){
                            double n;
                            simple.view() >> n;
                            self.setStandingFriction(n);
                        } else if (simple == "stand.friction.threshold"){
                            double n;
                            simple.view() >> n;
                            self.setStandingFrictionThreshold(n);
                        } else if (simple == "jump.changeanim.threshold"){
                            double n;
                            simple.view() >> n;
                            self.setJumpChangeAnimationThreshold(n);
                        } else if (simple == "airjump.num"){
                            int x;
                            simple.view() >> x;
                            self.setExtraJumps(x);
                        } else if (simple == "airjump.height"){
                            double x;
                            simple.view() >> x;
                            self.setAirJumpHeight(x);
                        } else if (simple == "air.gethit.groundlevel"){
                            double x;
                            simple.view() >> x;
                            self.setAirGetHitGroundLevel(x);
                        } else if (simple == "air.gethit.groundrecover.groundlevel"){
                            double x;
                            simple.view() >> x;
                            self.setAirGetHitGroundLevel(x);
                        }
                    }
                };

                MovementWalker walker(*this);
                section->walk(walker);
            }
        }
    } catch (const Mugen::Cmd::ParseException & e){
        ostringstream out;
        out << "Could not parse " << path.path() << ": " << e.getReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    } catch (const Ast::Exception & e){
        ostringstream out;
        out << "Could not parse " << path.path() << ": " << e.getReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

PaintownUtil::ReferenceCount<State> Character::parseStateDefinition(Ast::Section * section, const Filesystem::AbsolutePath & path, map<int, PaintownUtil::ReferenceCount<State> > & stateMap){
    std::string head = section->getName();
    /* this should really be head = Mugen::Util::fixCase(head) */
    head = Util::fixCase(head);

    int state = atoi(PaintownUtil::captureRegex(head, PaintownUtil::Regex("statedef *(-?[0-9]+)"), 0).c_str());
    class StateWalker: public Ast::Walker {
        public:
            StateWalker(PaintownUtil::ReferenceCount<State> definition, const Filesystem::AbsolutePath & path):
            definition(definition),
            path(path){
            }

            PaintownUtil::ReferenceCount<State> definition;
            const Filesystem::AbsolutePath & path;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "type" || simple == "statetype"){
                    string type;
                    simple.view() >> type;
                    type = PaintownUtil::lowerCaseAll(type);
                    if (type == "s"){
                        definition->setType(State::Standing);
                    } else if (type == "c"){
                        definition->setType(State::Crouching);
                    } else if (type == "a"){
                        definition->setType(State::Air);
                    } else if (type == "l"){
                        definition->setType(State::LyingDown);
                    } else if (type == "u"){
                        definition->setType(State::Unchanged);
                    } else {
                        ostringstream out;
                        out << "Unknown statedef type: '" << type << "'";
                        throw MugenException(out.str(), __FILE__, __LINE__);
                    }
                } else if (simple == "movetype"){
                    string type;
                    simple.view() >> type;
                    definition->setMoveType(type);
                } else if (simple == "physics"){
                    string type;
                    simple.view() >> type;
                    if (type == "S"){
                        definition->setPhysics(Physics::Stand);
                    } else if (type == "N"){
                        definition->setPhysics(Physics::None);
                    } else if (type == "C"){
                        definition->setPhysics(Physics::Crouch);
                    } else if (type == "A"){
                        definition->setPhysics(Physics::Air);
                    } else if (type == "U"){
                        definition->setPhysics(Physics::Unchanged);
                    }
                } else if (simple == "anim"){
                    definition->setAnimation(Compiler::compile(simple.getValue()));
                } else if (simple == "velset"){
                    const Ast::Value * x;
                    const Ast::Value * y;
                    simple.view() >> x >> y;
                    definition->setVelocity(Compiler::compile(x), Compiler::compile(y));
                } else if (simple == "ctrl"){
                    definition->setControl(Compiler::compile(simple.getValue()));
                } else if (simple == "poweradd"){
                    definition->setPower(Compiler::compile(simple.getValue()));
                } else if (simple == "juggle"){
                    definition->setJuggle(Compiler::compile(simple.getValue()));
                } else if (simple == "facep2"){
                } else if (simple == "hitdefpersist"){
                    bool what;
                    simple.view() >> what;
                    definition->setHitDefPersist(what);
                } else if (simple == "layerno"){
                    int layer;
                    simple.view() >> layer;
                    definition->setLayer(layer);
                } else if (simple == "movehitpersist"){
                } else if (simple == "hitcountpersist"){
                } else if (simple == "sprpriority"){
                    definition->setSpritePriority(Compiler::compile(simple.getValue()));
                } else {
                    Global::debug(0) << "Unhandled attribute in Statedef " << definition->getState() << ": " << simple.toString() << " " << sourceLocation(simple, path) << endl;
                }
            }
    };

    PaintownUtil::ReferenceCount<State> definition(new State(state));
    StateWalker walker(definition, path);
    section->walk(walker);

    /* FIXME: find out if only common states can be overriden */
    if (stateMap[state] != NULL){
        /* Don't let states be overriden within a single file. */
        return definition;
        /*
        Global::debug(1) << "Overriding state " << state << endl;
        delete states[state];
        */
    }
    Global::debug(2) << "Adding state definition " << state << endl;
    stateMap[state] = definition;

    return definition;
}
        
static map<string, StateController::Type> types;
static bool typesSetup = false;

StateController * Character::parseState(Ast::Section * section){
    std::string head = section->getName();
    head = Util::fixCase(head);

    string name = PaintownUtil::captureRegex(head, PaintownUtil::Regex("state *-?[0-9]+ *, *(.*)"), 0);
    int state = atoi(PaintownUtil::captureRegex(head, PaintownUtil::Regex("state *(-?[0-9]+)"), 0).c_str());

    class StateControllerWalker: public Ast::Walker {
    public:
        StateControllerWalker():
        type(StateController::Unknown){
            if (!typesSetup){
                typesSetup = true;
                types["afterimage"] = StateController::AfterImage;
                types["afterimagetime"] = StateController::AfterImageTime;
                types["allpalfx"] = StateController::AllPalFX;
                types["angleadd"] = StateController::AngleAdd;
                types["angledraw"] = StateController::AngleDraw;
                types["anglemul"] = StateController::AngleMul;
                types["angleset"] = StateController::AngleSet;
                types["appendtoclipboard"] = StateController::AppendToClipboard;
                types["assertspecial"] = StateController::AssertSpecial;
                types["attackdist"] = StateController::AttackDist;
                types["attackmulset"] = StateController::AttackMulSet;
                types["bgpalfx"] = StateController::BGPalFX;
                types["bindtoparent"] = StateController::BindToParent;
                types["bindtoroot"] = StateController::BindToRoot;
                types["bindtotarget"] = StateController::BindToTarget;
                types["changeanim"] = StateController::ChangeAnim;
                types["changeanim2"] = StateController::ChangeAnim2;
                types["changestate"] = StateController::ChangeState;
                types["clearclipboard"] = StateController::ClearClipboard;
                types["ctrlset"] = StateController::CtrlSet;
                types["defencemulset"] = StateController::DefenceMulSet;
                types["destroyself"] = StateController::DestroySelf;
                types["displaytoclipboard"] = StateController::DisplayToClipboard;
                types["envcolor"] = StateController::EnvColor;
                types["envshake"] = StateController::EnvShake;
                types["explod"] = StateController::Explod;
                types["explodbindtime"] = StateController::ExplodBindTime;
                types["forcefeedback"] = StateController::ForceFeedback;
                types["fallenvshake"] = StateController::FallEnvShake;
                types["gamemakeanim"] = StateController::GameMakeAnim;
                types["gravity"] = StateController::Gravity;
                types["helper"] = StateController::Helper;
                types["hitadd"] = StateController::HitAdd;
                types["hitby"] = StateController::HitBy;
                types["hitdef"] = StateController::HitDef;
                types["hitfalldamage"] = StateController::HitFallDamage;
                types["hitfallset"] = StateController::HitFallSet;
                types["hitfallvel"] = StateController::HitFallVel;
                types["hitoverride"] = StateController::HitOverride;
                types["hitvelset"] = StateController::HitVelSet;
                types["lifeadd"] = StateController::LifeAdd;
                types["lifeset"] = StateController::LifeSet;
                types["makedust"] = StateController::MakeDust;
                types["modifyexplod"] = StateController::ModifyExplod;
                types["movehitreset"] = StateController::MoveHitReset;
                types["nothitby"] = StateController::NotHitBy;
                types["null"] = StateController::Null;
                types["offset"] = StateController::Offset;
                types["palfx"] = StateController::PalFX;
                types["parentvaradd"] = StateController::ParentVarAdd;
                types["parentvarset"] = StateController::ParentVarSet;
                types["pause"] = StateController::Pause;
                types["playerpush"] = StateController::PlayerPush;
                types["playsnd"] = StateController::PlaySnd;
                types["posadd"] = StateController::PosAdd;
                types["posfreeze"] = StateController::PosFreeze;
                types["posset"] = StateController::PosSet;
                types["poweradd"] = StateController::PowerAdd;
                types["powerset"] = StateController::PowerSet;
                types["projectile"] = StateController::Projectile;
                types["removeexplod"] = StateController::RemoveExplod;
                types["reversaldef"] = StateController::ReversalDef;
                types["screenbound"] = StateController::ScreenBound;
                types["selfstate"] = StateController::SelfState;
                types["sprpriority"] = StateController::SprPriority;
                types["statetypeset"] = StateController::StateTypeSet;
                types["sndpan"] = StateController::SndPan;
                types["stopsnd"] = StateController::StopSnd;
                types["superpause"] = StateController::SuperPause;
                types["targetbind"] = StateController::TargetBind;
                types["targetdrop"] = StateController::TargetDrop;
                types["targetfacing"] = StateController::TargetFacing;
                types["targetlifeadd"] = StateController::TargetLifeAdd;
                types["targetpoweradd"] = StateController::TargetPowerAdd;
                types["targetstate"] = StateController::TargetState;
                types["targetveladd"] = StateController::TargetVelAdd;
                types["targetvelset"] = StateController::TargetVelSet;
                types["trans"] = StateController::Trans;
                types["turn"] = StateController::Turn;
                types["varadd"] = StateController::VarAdd;
                types["varrandom"] = StateController::VarRandom;
                types["varrangeset"] = StateController::VarRangeSet;
                types["varset"] = StateController::VarSet;
                types["veladd"] = StateController::VelAdd;
                types["velmul"] = StateController::VelMul;
                types["velset"] = StateController::VelSet;
                types["width"] = StateController::Width;
                types["zoom"] = StateController::Zoom;
                types["debug"] = StateController::Debug;
            }
        }

        StateController::Type type;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "type"){
                string type;
                simple.view() >> type;
                type = Mugen::Util::fixCase(type);
                
                if (types.find(type) != types.end()){
                    map<string, StateController::Type>::iterator what = types.find(type);
                    this->type = (*what).second;
                } else {
                    Global::debug(0) << "Unknown state controller type " << type << endl;
                }
            }
        }
    };

    // StateController * controller = new StateController(name);
    StateControllerWalker walker;
    section->walk(walker);
    StateController::Type type = walker.type;
    if (type == StateController::Unknown){
        Global::debug(0) << "Warning: no type given for controller " << section->getName() << endl;
        return NULL;
    } else {
        StateController * controller = StateController::compile(section, name, state, nextStateControllerId(), type);
        return controller;
    }
}

static Filesystem::AbsolutePath findStateFile(const Filesystem::AbsolutePath & base, const string & path){
    try{
        return Storage::instance().findInsensitive(Storage::instance().cleanse(base).join(Filesystem::RelativePath(path)));
    } catch (const Filesystem::NotFound & fail){
        return Storage::instance().findInsensitive(Filesystem::RelativePath("mugen/data/" + path));
    }
        /*
    if (PaintownUtil::exists(base.join(Filesystem::RelativePath(path)).path())){
        return base.join(Filesystem::RelativePath(path));
    } else {
        return Filesystem::findInsensitive(Filesystem::RelativePath("mugen/data/" + path));
    }
    */

#if 0
    try{
        /* first look in the character's directory */
        // return Filesystem::find(base.join(Filesystem::RelativePath(path)));
        
    } catch (const Filesystem::NotFound & f){
        /* then look for it in the common data directory.
         * this is where common1.cns lives
         */
        return Filesystem::find(Filesystem::RelativePath("mugen/data/" + path));
    }
#endif
}
        
void Character::loadStateFile(const Filesystem::AbsolutePath & base, const string & path){
    Filesystem::AbsolutePath full = findStateFile(base, path);
    MessageQueue::info("Reading " + Storage::instance().cleanse(full).path());
    PaintownUtil::Parameter<Filesystem::RelativePath> currentFile(stateFileParameter, Storage::instance().cleanse(full));
    // string full = Filesystem::find(base + "/" + PaintownUtil::trim(path));
    /* st can use the Cmd parser */
    AstRef parsed(Util::parseCmd(full));
    map<int, PaintownUtil::ReferenceCount<State> > out;
    PaintownUtil::ReferenceCount<State> currentState;
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
        std::string head = section->getName();
        head = Util::fixCase(head);

        if (PaintownUtil::matchRegex(head, PaintownUtil::Regex("statedef"))){
            currentState = parseStateDefinition(section, full, out);
        } else if (PaintownUtil::matchRegex(head, PaintownUtil::Regex("state "))){
            if (currentState != NULL){
                StateController * controller = parseState(section);
                if (controller != NULL){
                    if (controller->getState() != currentState->getState()){
                        Global::debug(1) << "Warning: controller '" << controller->getName() << "' specified state " << controller->getState() << " which does not match the most recent state definition " << currentState->getState() << " in file " << full.path() << endl;
                    }
                    currentState->addController(controller);
                }
            } else {
                Global::debug(0) << "Warning: no statedef defined for " << head << std::endl;
            }
        }
    }

    mergeStates(getLocalData().states, out);
}
    
void Character::startRecording(int count){
    getLocalData().record = PaintownUtil::ReferenceCount<RecordingInformation>(new RecordingInformation());
    ostringstream filename;
    filename << getDisplayName() << "-" << count << ".txt";
    getLocalData().record->out.open(filename.str().c_str());
    if (!getLocalData().record->out.good()){
        Global::debug(0) << "Could not record " << filename.str() << std::endl;
    }
    getLocalData().record->ticks = 1;
}

static bool differentCommands(std::vector<std::string> list1, std::vector<std::string> list2){
    if (list1.size() != list2.size()){
        return true;
    }

    std::sort(list1.begin(), list1.end());
    std::sort(list2.begin(), list2.end());

    for (unsigned int i = 0; i < list1.size(); i++){
        if (list1[i] != list2[i]){
            return true;
        }
    }

    return false;
}

void Character::recordCommands(const std::vector<std::string> & commands){
    if (getLocalData().record != NULL){
        if (differentCommands(commands, getLocalData().record->commands)){
            getLocalData().record->out << getLocalData().record->ticks << " " << PaintownUtil::join(getLocalData().record->commands, ", ") << std::endl;
            getLocalData().record->ticks = 1;
            getLocalData().record->commands = commands;
        } else {
            getLocalData().record->ticks += 1;
        }
    }
}

void Character::stopRecording(){
    /* force the last set of commands to be written if any */
    std::vector<std::string> last;
    recordCommands(last);
    getLocalData().record = NULL;
}
    
void Character::checkStateControllers(){
    for (map<int, PaintownUtil::ReferenceCount<State> >::iterator it = getLocalData().states.begin(); it != getLocalData().states.end(); it++){
        PaintownUtil::ReferenceCount<State> state = it->second;
        if (state->getControllers().size() == 0){
            std::ostringstream out;
            out << "State " << state->getState() << " has no state controllers";
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
    }
}

/* a container for a directory and a file */
struct Location{
    Location(Filesystem::AbsolutePath base, string file):
        base(base), file(file){
        }

    Filesystem::AbsolutePath base;
    string file;
};

void Character::load(int useAct){
#if 0
    // Lets look for our def since some people think that all file systems are case insensitive
    baseDir = Filesystem::find("mugen/chars/" + location + "/");
    Global::debug(1) << baseDir << endl;
    std::string realstr = Mugen::Util::stripDir(location);
    const std::string ourDefFile = Mugen::Util::fixFileName(baseDir, std::string(realstr + ".def"));
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate player definition file for: " + location );
    }
#endif

    MessageQueue::info("Loading character '" + getLocalData().location.getFilename().path() + "'");
    
    // baseDir = Filesystem::cleanse(Mugen::Util::getFileDir(location));
    getLocalData().baseDir = getLocalData().location.getDirectory();
    // const std::string ourDefFile = location;
     
    AstRef parsed(Util::parseDef(getLocalData().location));
    try{
        /* Every character should have a [Files] section at least. Possibly [Info] as well
         * but I'm not sure yet.
         */
        if (parsed->findSection("files") == NULL){
            std::ostringstream out;
            out << "Could not find a [Files] section in " << getLocalData().location.path();
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
        /* Extract info for our first section of our stage */
        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            head = Mugen::Util::fixCase(head);

            if (head == "info"){
                class InfoWalker: public Ast::Walker {
                    public:
                        InfoWalker(Character & who):
                            self(who){
                            }

                        Character & self;

                        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                            if (simple == "name"){
                                try{
                                    simple.view() >> self.getLocalData().name;
                                } catch (const Ast::Exception & fail){
                                }
                            } else if (simple == "displayname"){
                                try{
                                    simple.view() >> self.getLocalData().displayName;
                                } catch (const Ast::Exception & fail){
                                }
                            } else if (simple == "versiondate"){
                                try{
                                    simple.view() >> self.getLocalData().versionDate;
                                } catch (const Ast::Exception & fail){
                                }
                            } else if (simple == "mugenversion"){
                                try{
                                    simple.view() >> self.getLocalData().mugenVersion;
                                } catch (const Ast::Exception & fail){
                                }
                            } else if (simple == "author"){
                                try{
                                    simple.view() >> self.getLocalData().author;
                                } catch (const Ast::Exception & fail){
                                }
                            } else if (simple == "pal.defaults"){
                                vector<int> numbers;
                                simple.view() >> numbers;
                                for (vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                                    self.getLocalData().palDefaults.push_back((*it) - 1);
                                }
                                // Global::debug(1) << "Pal" << self.palDefaults.size() << ": " << num << endl;
                            } else if (simple == "localcoord"){
                                /* TODO: mugen 1.0 thing */
                            } else {
                                throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                            }
                        }
                };

                InfoWalker walker(*this);
                Ast::Section * section = *section_it;
                section->walk(walker);
            } else if (head == "files"){
                class FilesWalker: public Ast::Walker {
                    public:
                        FilesWalker(Character & self, const Filesystem::AbsolutePath & location):
                            location(location),
                            self(self){
                            }

                        vector<Location> stateFiles;
                        const Filesystem::AbsolutePath & location;

                        Character & self;
                        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                            if (simple == "cmd"){
                                string file;
                                simple.view() >> file;
                                self.getLocalData().cmdFile = Filesystem::RelativePath(file);
                                /* loaded later after the state files */
                            } else if (simple == "cns"){
                                string file;
                                simple.view() >> file;
                                /* just loads the constants */
                                self.loadCnsFile(Filesystem::RelativePath(file));
                            } else if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(simple.idString()), PaintownUtil::Regex("st[0-9]+"))){
                                int num = atoi(PaintownUtil::captureRegex(PaintownUtil::lowerCaseAll(simple.idString()), PaintownUtil::Regex("st([0-9]+)"), 0).c_str());
                                if (num >= 0 && num <= 12){
                                    string path;
                                    simple.view() >> path;
                                    stateFiles.push_back(Location(self.getLocalData().baseDir, path));
                                    // simple >> self.stFile[num];
                                }
                            } else if (simple == "stcommon"){
                                string path;
                                simple.view() >> path;
                                stateFiles.insert(stateFiles.begin(), Location(self.getLocalData().baseDir, path));
                            } else if (simple == "st"){
                                string path;
                                simple.view() >> path;
                                stateFiles.push_back(Location(self.getLocalData().baseDir, path));
                            } else if (simple == "sprite"){
                                simple.view() >> self.getLocalData().sffFile;
                            } else if (simple == "anim"){
                                simple.view() >> self.getLocalData().airFile;
                            } else if (simple == "sound"){
                                simple.view() >> self.getLocalData().sndFile;
                                // Mugen::Util::readSounds(Mugen::Util::fixFileName(self.baseDir, self.sndFile), self.sounds);
                                Util::readSounds(Storage::instance().lookupInsensitive(self.getLocalData().baseDir, Filesystem::RelativePath(self.getLocalData().sndFile)), self.getLocalData().sounds);
                            } else if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(simple.idString()), PaintownUtil::Regex("pal[0-9]+"))){
                                int num = atoi(PaintownUtil::captureRegex(PaintownUtil::lowerCaseAll(simple.idString()), PaintownUtil::Regex("pal([0-9]+)"), 0).c_str());
                                try{
                                    string what;
                                    /* There could be no file listed, just
                                     * pal2 =
                                     */
                                    simple.view() >> what;
                                    self.getLocalData().palFile[num] = what;
                                } catch (const Ast::Exception & fail){
                                    Global::debug(1) << "No palette defined for " << num << std::endl;
                                }
                            } else {
                                Global::debug(0) << "Unhandled option in Files Section: " + simple.toString() << endl;
                            }
                        }
                };

                FilesWalker walker(*this, getLocalData().location);
                Ast::Section * section = *section_it;
                section->walk(walker);

                TimeDifference compileTime;
                compileTime.startTime();

                /* state controllers with no obvious parent. after parsing all
                 * the state files, go through this list and try to find the right parent
                 */
                for (vector<Location>::iterator it = walker.stateFiles.begin(); it != walker.stateFiles.end(); it++){
                    Location & where = *it;
                    try{
                        /* load definitions first */
                        loadStateFile(where.base, where.file);
                    } catch (const MugenException & e){
                        ostringstream out;
                        out << "Problem loading state file " << where.file << ": " << e.getFullReason();
                        throw MugenException(out.str(), __FILE__, __LINE__);
                    } catch (const Mugen::Cmd::ParseException & e){
                        ostringstream out;
                        out << "Problem loading state file " << where.file << ": " << e.getReason();
                        throw MugenException(out.str(), __FILE__, __LINE__);
                    }

                }

                loadCmdFile(getLocalData().cmdFile);

                compileTime.endTime();
                Global::debug(1) << compileTime.printTime("Compile time") << std::endl;

#if 0
                for (vector<Location>::iterator it = walker.stateFiles.begin(); it != walker.stateFiles.end(); it++){
                    Location & where = *it;
                    try{
                        /* load definitions first */
                        loadStateFile(where.base, where.file, true, false);
                    } catch (const MugenException & e){
                        ostringstream out;
                        out << "Problem loading state file " << where.file << ": " << e.getReason();
                        throw MugenException(out.str());
                    } catch (const Mugen::Cmd::ParseException & e){
                        ostringstream out;
                        out << "Problem loading state file " << where.file << ": " << e.getReason();
                        throw MugenException(out.str());
                    }
                }

                for (vector<Location>::iterator it = walker.stateFiles.begin(); it != walker.stateFiles.end(); it++){
                    Location & where = *it;
                    try{
                        /* then load controllers */
                        loadStateFile(where.base, where.file, false, true);
                    } catch (const MugenException & e){
                        ostringstream out;
                        out << "Problem loading state file " << where.file << ": " << e.getReason();
                        throw MugenException(out.str());
                    } catch (const Mugen::Cmd::ParseException & e){
                        ostringstream out;
                        out << "Problem loading state file " << where.file << ": " << e.getReason();
                        throw MugenException(out.str());
                    }
                }
#endif


#if 0
                /* now just load the state controllers */
                for (vector<Location>::iterator it = walker.stateFiles.begin(); it != walker.stateFiles.end(); it++){
                    Location & where = *it;
                    try{
                        loadStateFile(where.base, where.file, false, true);
                    } catch (const MugenException & e){
                        ostringstream out;
                        out << "Problem loading state file " << where.file << ": " << e.getReason();
                        throw MugenException(out.str());
                    }
                }
#endif

                /*
                   if (commonStateFile != ""){
                   loadStateFile("mugen/data/", commonStateFile);
                   }
                   if (stateFile != ""){
                   loadStateFile("mugen/chars/" + location, stateFile);
                   }
                   if (
                   */

            } else if (head == "arcade"){
                class ArcadeWalker: public Ast::Walker {
                    public:
                        ArcadeWalker(Character & self):
                            self(self){
                            }

                        Character & self;

                        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                            if (simple == "intro.storyboard"){
                                try{
                                    simple.view() >> self.getLocalData().introFile;
                                } catch (const Ast::Exception & fail){
                                }
                            } else if (simple == "ending.storyboard"){
                                try{
                                    simple.view() >> self.getLocalData().endingFile;
                                } catch (const Ast::Exception & fail){
                                }
                            } else {
                                throw MugenException("Unhandled option in Arcade Section: " + simple.toString(), __FILE__, __LINE__);
                            }
                        }
                };

                ArcadeWalker walker(*this);
                Ast::Section * section = *section_it;
                section->walk(walker);
            }
        }
    } catch (const Ast::Exception & e){
        ostringstream out;
        out << "Could not load " << getLocalData().location.path() << ": " << e.getReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    /* Check that all state's have at least one state controller */
    checkStateControllers();

    /* Is this just for testing? */
    if (getMaxHealth() == 0 || getHealth() == 0){
        setHealth(1000);
        setMaxHealth(1000);
    }

    // Current palette
    if (getLocalData().palDefaults.empty()){
	// Correct the palette defaults
	for (unsigned int i = 0; i < getLocalData().palFile.size(); ++i){
	    getLocalData().palDefaults.push_back(i);
	}
    }
    /*
    if (palDefaults.size() < palFile.size()){
	bool setPals[palFile.size()];
	for( unsigned int i =0;i<palFile.size();++i){
	    setPals[i] = false;
	}
	// Set the ones already set
	for (unsigned int i = 0; i < palDefaults.size(); ++i){
	    setPals[palDefaults[i]] = true;
	}
	// now add the others
	for( unsigned int i =0;i<palFile.size();++i){
	    if(!setPals[i]){
		palDefaults.push_back(i);
	    }
	}
    }
    */

    getLocalData().currentPalette = useAct;

    loadGraphics(getLocalData().currentPalette);
    
    fixAssumptions();

    /*
    State * state = states[-1];
    for (vector<StateController*>::const_iterator it = state->getControllers().begin(); it != state->getControllers().end(); it++){
        Global::debug(0) << "State -1: '" << (*it)->getName() << "'" << endl;
    }
    */
}

void Character::loadGraphics(int palette){
    std::string paletteFile = "";
    if (getLocalData().palFile.find(palette) == getLocalData().palFile.end()){
        /* FIXME: choose a default. its not just palette 1 because that palette
         * might not exist
         */
	Global::debug(1) << "Couldn't find palette: " << palette << " in palette collection. Defaulting to internal palette if available." << endl;
        if (getLocalData().palFile.size() > 0){
            paletteFile = getLocalData().palFile.begin()->second;
        }
    } else {
        if (palette < (int) getLocalData().palFile.size()){
            paletteFile = getLocalData().palFile[palette];
            Global::debug(2) << "Current pal: " << palette << " | Palette File: " << paletteFile << endl;
        }
    }
    
    Global::debug(2) << "Reading Sff (sprite) Data..." << endl; 
    Filesystem::AbsolutePath finalPalette;
    try{
        finalPalette = Storage::instance().lookupInsensitive(getLocalData().baseDir, Filesystem::RelativePath(paletteFile));
    } catch (const Filesystem::Exception & fail){
        Global::debug(0) << "Couldn't find palette for '" << paletteFile << "' because " << fail.getTrace() << endl;
        /* ignore palette */
    }

    getLocalData().sprites = SpriteMap();

    Util::readSprites(Storage::instance().lookupInsensitive(getLocalData().baseDir, Filesystem::RelativePath(getLocalData().sffFile)), finalPalette, getLocalData().sprites, true);

    Global::debug(2) << "Reading Air (animation) Data..." << endl;
    getLocalData().animations = Util::loadAnimations(Storage::instance().lookupInsensitive(getLocalData().baseDir, Filesystem::RelativePath(getLocalData().airFile)), getLocalData().sprites, true);
}

bool Character::isBound() const {
    return getStateData().bind.time > 0;
}

void Character::bindTo(const CharacterId & bound, int time, int facing, double offsetX, double offsetY){
    getStateData().bind.bound = bound;
    getStateData().bind.time = time;
    getStateData().bind.facing = facing;
    getStateData().bind.offsetX = offsetX;
    getStateData().bind.offsetY = offsetY;
}
    
const CharacterId & Character::getId() const {
    return getLocalData().id;
}
        
void Character::setId(const CharacterId & id){
    getLocalData().id = id;
}

bool Character::hasAnimation(int index) const {
    return getAnimation(index) != NULL;
}

/* completely arbitrary number, just has to be unique and unlikely to
 * be used by the system. maybe negative numbers are better?
 */
static const int JumpIndex = 234823;

class MutableCompiledInteger: public Compiler::Value {
public:
    MutableCompiledInteger(int value):
        value(value){
        }

    int value;

    virtual RuntimeValue evaluate(const Environment & environment) const {
        return RuntimeValue(value);
    }

    virtual void set(int value){
        this->value = value;
    }

    virtual int get() const {
        return this->value;
    }

    virtual ~MutableCompiledInteger(){
    }
};

/* TODO: get rid of inputs */
void Character::resetJump(Mugen::Stage & stage, const vector<string> & inputs){
    setSystemVariable(JumpIndex, RuntimeValue(0));
    changeState(stage, JumpStart);
}

/* TODO: get rid of inputs */
void Character::doubleJump(Mugen::Stage & stage, const vector<string> & inputs){
    setSystemVariable(JumpIndex, RuntimeValue(getSystemVariable(JumpIndex).toNumber() + 1));
    changeState(stage, AirJumpStart);
}

/* TODO: get rid of inputs */
void Character::stopGuarding(Mugen::Stage & stage, const vector<string> & inputs){
    getStateData().guarding = false;
    if (getStateType() == StateType::Crouch){
        changeState(stage, Crouching);
    } else if (getStateType() == StateType::Air){
        changeState(stage, 51);
    } else {
        changeState(stage, Standing);
    }
}

static StateController * parseController(const string & input, const string & name, int state, unsigned int id, StateController::Type type){
    try{
        Ast::AstParse sections((list<Ast::Section*>*) Mugen::Cmd::parse(input.c_str()));
        if (sections.getSections()->size() == 0){
            ostringstream out;
            out << "Could not parse controller: " << input;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
        Ast::Section * first = sections.getSections()->front();
        return StateController::compile(first, name, state, id, type);
    } catch (const Ast::Exception & e){
        throw MugenException(e.getReason(), __FILE__, __LINE__);
    } catch (const Mugen::Cmd::ParseException & e){
        ostringstream out;
        out << "Could not parse " << input << " because " << e.getReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

unsigned int Character::nextStateControllerId(){
    stateControllerId += 1;
    return stateControllerId;
}

void Character::fixAssumptions(){
    /* TODO: need a -1 state controller that changes to state 20 if holdfwd
     * or holdback is pressed
     */

    if (getLocalData().states[-1] != 0){
        /* walk */
        {
            ostringstream raw;
            raw << "[State -1, paintown-internal-walk]\n";
            raw << "triggerall = stateno = 0\n";
            raw << "trigger1 = command = \"holdfwd\"\n";
            raw << "trigger2 = command = \"holdback\"\n";
            raw << "value = " << WalkingForwards << "\n";
            getLocalData().states[-1]->addController(parseController(raw.str(), "paintown internal walk", -1, nextStateControllerId(), StateController::ChangeState));
        }


        /* crouch */
        {
            ostringstream raw;
            raw << "[State -1, paintown-internal-crouch]\n";
            raw << "value = " << StandToCrouch << "\n";
            raw << "triggerall = ctrl\n";
            raw << "trigger1 = stateno = 0\n";
            raw << "trigger2 = stateno = " << WalkingForwards << "\n";
            raw << "triggerall = command = \"holddown\"\n";

            getLocalData().states[-1]->addControllerFront(parseController(raw.str(), "paintown internal crouch", -1, nextStateControllerId(), StateController::ChangeState));
        }

        /* jump */
        {
            class InternalJumpController: public StateController {
            public:
                InternalJumpController(unsigned int id):
                StateController("jump", -1, id){
                }

                virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
                    guy.resetJump(stage, commands);
                }

                virtual StateController * deepCopy() const {
                    return new InternalJumpController(id);
                }
            };

            InternalJumpController * controller = new InternalJumpController(nextStateControllerId());
            controller->addTriggerAll(Compiler::compileAndDelete(new Ast::SimpleIdentifier("ctrl")));
            controller->addTriggerAll(Compiler::compileAndDelete(new Ast::ExpressionInfix(-1, -1, Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("statetype"),
                        new Ast::SimpleIdentifier("S"))));
            controller->addTrigger(1, Compiler::compileAndDelete(new Ast::ExpressionInfix(-1, -1, Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("command"),
                        new Ast::String(-1, -1, new string("holdup")))));
            getLocalData().states[-1]->addController(controller);
        }

        /* double jump */
        {
            string jumpCommand = "internal:double-jump-command";
            vector<Ast::Key*> keys;
            keys.push_back(new Ast::KeyModifier(-1, -1, Ast::KeyModifier::Release, new Ast::KeySingle(-1, -1, "U")));
            keys.push_back(new Ast::KeySingle(-1, -1, "U"));
            Command2 * doubleJumpCommand = new Command2(jumpCommand, new Ast::KeyList(-1, -1, keys), 5, 0);
            addCommand(doubleJumpCommand);

            setSystemVariable(JumpIndex, RuntimeValue(0));

            class InternalDoubleJumpController: public StateController {
            public:
                InternalDoubleJumpController(unsigned int id):
                StateController("double jump", -1, id){
                }

                virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
                    guy.doubleJump(stage, commands);
                }

                StateController * deepCopy() const {
                    return new InternalDoubleJumpController(id);
                }
            };

            InternalDoubleJumpController * controller = new InternalDoubleJumpController(nextStateControllerId());
            // controller->setDebug(true);
            controller->addTriggerAll(Compiler::compileAndDelete(new Ast::SimpleIdentifier("ctrl")));
            controller->addTriggerAll(Compiler::compileAndDelete(new Ast::ExpressionInfix(-1, -1, Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("statetype"),
                        new Ast::SimpleIdentifier("A"))));
            controller->addTriggerAll(Compiler::compileAndDelete(new Ast::ExpressionInfix(-1, -1, Ast::ExpressionInfix::GreaterThan,
                                                                                          new Ast::ExpressionUnary(-1, -1, Ast::ExpressionUnary::Minus,
                                                                                                                   new Ast::Keyword(-1, -1, "pos y")),
                                                                                          new Ast::SimpleIdentifier("internal:airjump-height"))));
            controller->addTriggerAll(Compiler::compileAndDelete(new Ast::ExpressionInfix(-1, -1, Ast::ExpressionInfix::LessThan,
                        new Ast::Function(-1, -1, "sysvar", new Ast::ValueList(new Ast::Number(-1, -1, JumpIndex))),
                        new Ast::SimpleIdentifier("internal:extra-jumps"))));
            controller->addTrigger(1, Compiler::compileAndDelete(new Ast::ExpressionInfix(-1, -1, Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("command"),
                        // new Ast::String(new string("holdup")
                        new Ast::String(-1, -1, new string(jumpCommand)
                            ))));
            getLocalData().states[-1]->addController(controller);
        }
    }

    {
        if (getLocalData().states[StopGuardStand] != 0){
            class StopGuardStandController: public StateController {
            public:
                StopGuardStandController(unsigned int id):
                StateController("stop guarding", StopGuardStand, id){
                }

                virtual void activate(Mugen::Stage & stage, Character & guy, const vector<string> & commands) const {
                    guy.stopGuarding(stage, commands);
                }

                StateController * deepCopy() const {
                    return new StopGuardStandController(id);
                }
            };

            StopGuardStandController * controller = new StopGuardStandController(nextStateControllerId());
            controller->addTrigger(1, Compiler::compileAndDelete(new Ast::ExpressionInfix(-1, -1, Ast::ExpressionInfix::Equals,
                    new Ast::SimpleIdentifier("animtime"),
                    new Ast::Number(-1, -1, 0))));
            getLocalData().states[StopGuardStand]->addController(controller);
        }
    }

    /* need a 20 state controller that changes to state 0 if holdfwd
     * or holdback is not pressed
     */
    if (getLocalData().states[20] != 0){
        ostringstream raw;
        raw << "[State 20, paintown-internal-stop-walking]\n";
        raw << "value = " << Standing << "\n";
        raw << "trigger1 = command != \"holdfwd\"\n";
        raw << "trigger1 = command != \"holdback\"\n";

        getLocalData().states[20]->addController(parseController(raw.str(), "paintown internal stop walking", 20, nextStateControllerId(), StateController::ChangeState));
    }

    if (getLocalData().states[Standing] != 0){
        getLocalData().states[Standing]->setControl(Compiler::compile(1));
    }

    if (getLocalData().states[StandToCrouch] != NULL){
        ostringstream raw;
        raw << "[State " << StandToCrouch << ", paintown-internal-stand-while-crouching]\n";
        raw << "value = " << CrouchToStand << "\n";
        raw << "trigger1 = command != \"holddown\"\n";

        getLocalData().states[StandToCrouch]->addController(parseController(raw.str(), "stand while crouching", StandToCrouch, nextStateControllerId(), StateController::ChangeState));

    }

    /* stand after crouching */
    if (getLocalData().states[11] != 0){
        ostringstream raw;
        raw << "[State 11, paintown-internal-stand-after-crouching]\n";
        raw << "value = " << CrouchToStand << "\n";
        raw << "trigger1 = command != \"holddown\"\n";

        getLocalData().states[11]->addController(parseController(raw.str(), "stand after crouching", 11, nextStateControllerId(), StateController::ChangeState));
    }

    /* get up kids */
    if (getLocalData().states[Liedown] != 0){
        ostringstream raw;
        raw << "[State " << Liedown << ", paintown-internal-get-up]\n";
        raw << "value = " << GetUpFromLiedown << "\n";
        raw << "trigger1 = time >= " << getLieDownTime() << "\n";

        getLocalData().states[Liedown]->addController(parseController(raw.str(), "get up", Liedown, nextStateControllerId(), StateController::ChangeState));
    }

    /* standing turn state */
    {
        State * turn = new State(StandTurning);
        turn->setType(State::Unchanged);
        turn->setAnimation(Compiler::compile(StandTurning));
        getLocalData().states[StandTurning] = turn;

        ostringstream raw;
        raw << "[State 5, paintown-internal-turn]\n";
        raw << "value = " << Standing << "\n";
        raw << "trigger1 = animtime = 0\n";

        turn->addController(parseController(raw.str(), "turn", StandTurning, nextStateControllerId(), StateController::ChangeState));
    }

    /* standing turn state */
    {
        State * turn = new State(CrouchTurning);
        turn->setType(State::Unchanged);
        turn->setAnimation(Compiler::compile(CrouchTurning));
        getLocalData().states[CrouchTurning] = turn;

        ostringstream raw;
        raw << "[State 5, paintown-internal-crouch-turn]\n";
        raw << "value = " << Standing << "\n";
        raw << "trigger1 = animtime = 0\n";

        turn->addController(parseController(raw.str(), "turn", CrouchTurning, nextStateControllerId(), StateController::ChangeState));
    }

    /* if y reaches 0 then auto-transition to state 52.
     * probably just add a trigger to state 50
     */
    if (getLocalData().states[50] != 0){
        /*
        ostringstream raw;
        raw << "[State 50, paintown-internal-land]\n";
        raw << "value = 52\n";
        raw << "trigger1 = pos y >= 0\n";
        raw << "trigger1 = vel y >= 0\n";

        states[50]->addController(parseController(raw.str(), "land", 50, StateController::ChangeState));
        */

        /*
        StateController * controller = new StateController("jump land");
        controller->setType(StateController::ChangeState);
        controller->setValue1(new Ast::Number(52));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::GreaterThanEquals,
                    new Ast::Keyword("pos y"),
                    new Ast::Number(0)));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::GreaterThan,
                    new Ast::Keyword("vel y"),
                    new Ast::Number(0)));
        states[50]->addController(controller);
        */

    }
}

// Render sprite
void Character::renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Graphics::Bitmap *bmp , const int flip, const double scalex, const double scaley ){
    PaintownUtil::ReferenceCount<Mugen::Sprite> sprite = getLocalData().sprites[group][image];
    if (sprite != NULL){
        Mugen::Effects effects;
        effects.facing = flip == 1;
        effects.scalex = scalex;
        effects.scaley = scaley;

        sprite->render(x, y, *bmp, effects);

        /*
        Graphics::Bitmap *bitmap = sprite->getBitmap(true);
	const int width = (int)(bitmap->getWidth() * scalex);
	const int height =(int)(bitmap->getHeight() * scaley);
	if (flip == 1){
	    bitmap->drawStretched(x,y, width, height, *bmp);
	} else if (flip == -1){
	    // temp bitmap to flip and crap
            Graphics::Bitmap temp = Graphics::Bitmap::temporaryBitmap(bitmap->getWidth(), bitmap->getHeight());
	    temp.fill(Graphics::MaskColor());
	    bitmap->drawHFlip(0,0,temp);
	    temp.drawStretched(x-width,y, width, height, *bmp);
	}
        */
    }
}
        
const std::vector<Command2 *> & Character::getCommands() const {
    return getLocalData().commands;
}
        
bool Character::canRecover() const {
    /* TODO */
    return false;
    // return true;
    // return getY() == 0;
}

void Character::nextPalette(){
    if (getLocalData().currentPalette < getLocalData().palDefaults.size()-1){
	getLocalData().currentPalette++;
    } else {
        getLocalData().currentPalette = 0;
    }
    Global::debug(1) << "Current pal: " << getLocalData().currentPalette << " | Location: " << getLocalData().palDefaults[getLocalData().currentPalette] << " | Palette File: " << getLocalData().palFile[getLocalData().palDefaults[getLocalData().currentPalette]] << endl;

    loadGraphics(getLocalData().currentPalette);
}

void Character::priorPalette(){
    if (getLocalData().currentPalette > 0){
	getLocalData().currentPalette--;
    } else {
        getLocalData().currentPalette = getLocalData().palDefaults.size() -1;
    }
    Global::debug(1) << "Current pal: " << getLocalData().currentPalette << " | Palette File: " << getLocalData().palFile[getLocalData().palDefaults[getLocalData().currentPalette]] << endl;

    loadGraphics(getLocalData().currentPalette);
}
        
/* players are their own root normally, only helpers differ */
CharacterId Character::getRoot() const {
    return getId();
}
        
const AnimationState Character::getCurrentAnimationState() const {
    if (getCurrentAnimation() != NULL){
        return getCurrentAnimation()->getState();
    }

    return AnimationState();
}

void Character::setCurrentAnimationState(const AnimationState & state){
    if (getCurrentAnimation() != NULL){
        getCurrentAnimation()->setState(state);
    }
}

const PaintownUtil::ReferenceCount<Mugen::Sprite> Character::getCurrentFrame() const {
    if (getCurrentAnimation() != NULL){
        return getCurrentAnimation()->getCurrentFrame()->getSprite();
    } else {
        return PaintownUtil::ReferenceCount<Mugen::Sprite>(NULL);
    }
}

void Character::drawReflection(Graphics::Bitmap * work, int rel_x, int rel_y, int intensity){
    if (getStateData().special.invisible){
        return;
    }

    if (getCurrentAnimation() != NULL){
        getCurrentAnimation()->renderReflection(getFacing() == FacingLeft, true, intensity, (int)(getX() - rel_x), (int)(getZ() - getY() - rel_y), *work, getLocalData().xscale, getLocalData().yscale);
    }
}
    
int Character::getAnimation() const {
    if (getLocalData().foreignAnimation != NULL){
        return getLocalData().foreignAnimationNumber;
    }
    return getStateData().currentAnimation;
}

PaintownUtil::ReferenceCount<Animation> Character::getCurrentAnimation() const {
    /* Foreign animation is set by ChangeAnim2 */
    if (getLocalData().foreignAnimation != NULL){
        return getLocalData().foreignAnimation;
    }

    return getAnimation(getStateData().currentAnimation);
    /*
    typedef std::map< int, Animation * > Animations;
    Animations::const_iterator it = getAnimations().find(currentAnimation);
    if (it != getAnimations().end()){
        Animation * animation = (*it).second;
        return animation;
    }
    return NULL;
    */
}

void Character::resetStatePersistent(){
    for (map<int, PaintownUtil::ReferenceCount<State> >::const_iterator it = getLocalData().states.begin(); it != getLocalData().states.end(); it++){
        const PaintownUtil::ReferenceCount<State> & state = it->second;
        if (state != NULL){
            const std::vector<StateController*> & controllers = state->getControllers();
            map<unsigned int, int> use;

            for (vector<StateController*>::const_iterator it2 = controllers.begin(); it2 != controllers.end(); it2++){
                StateController * controller = *it2;
                controller->resetPersistent();
            }
        }
    }
}

void Character::setStatePersistent(const std::map<int, std::map<uint32_t, int> > & statePersistent){
    resetStatePersistent();

    for (std::map<int, std::map<uint32_t, int> >::const_iterator it = statePersistent.begin(); it != statePersistent.end(); it++){
        PaintownUtil::ReferenceCount<State> state = getSelfState(it->first);
        if (state != NULL){
            const map<uint32_t, int> & persistent = it->second;

            for (map<uint32_t, int>::const_iterator it2 = persistent.begin(); it2 != persistent.end(); it2++){
                StateController * controller = state->findControllerById(it2->first);
                if (controller != NULL){
                    controller->setCurrentPersistent(it2->second);
                }
            }
        }
    }
}

std::map<int, std::map<uint32_t, int> > Character::getStatePersistent() const {
    map<int, map<uint32_t, int> > data;
        
    for (map<int, PaintownUtil::ReferenceCount<State> >::const_iterator it = getLocalData().states.begin(); it != getLocalData().states.end(); it++){
        const PaintownUtil::ReferenceCount<State> & state = it->second;
        if (state != NULL){
            const std::vector<StateController*> & controllers = state->getControllers();
            map<uint32_t, int> use;

            for (vector<StateController*>::const_iterator it2 = controllers.begin(); it2 != controllers.end(); it2++){
                StateController * controller = *it2;
                if (controller->getPersistent() != controller->getCurrentPersistent()){
                    use[controller->getId()] = controller->getCurrentPersistent();
                }
            }

            if (use.size() > 0){
                data[state->getState()] = use;
            }
        }

    }

    return data;
}

/* returns all the commands that are currently active */
vector<string> Character::doInput(const Mugen::Stage & stage){
    if (getLocalData().behavior == NULL){
        throw MugenException("Internal error: No behavior specified", __FILE__, __LINE__);
    }

    return getLocalData().behavior->currentCommands(stage, this, getLocalData().commands, getFacing() == FacingRight);
}

bool Character::isPaused() const {
    return getHitState().shakeTime > 0;
}

int Character::pauseTime() const {
    return getHitState().shakeTime;
}
    
double Character::getHealth() const {
    if (getStateData().health < 0){
        return 0;
    }
    return getStateData().health;
}

/*
InputMap<Mugen::Keys> & Character::getInput(){
    if (getFacing() == Object::FACING_RIGHT){
        return inputLeft;
    }
    return inputRight;
}
*/

static bool holdingBlock(const vector<string> & commands){
    for (vector<string>::const_iterator it = commands.begin(); it != commands.end(); it++){
        if (*it == "holdback"){
            return true;
        }
    }

    return false;
}

void Character::processAfterImages(){
    if (getLocalData().afterImage.lifetime > 0){
        getLocalData().afterImage.lifetime -= 1;
    }

    if (getLocalData().afterImage.currentTime > 0){
        getLocalData().afterImage.currentTime -= 1;
    }

    int x = (int) getX();
    int y = (int) getRY();

    /* not sure if checking for the timegap > 0 is the right thing.. */
    if (getLocalData().afterImage.timegap > 0 && getLocalData().afterImage.currentTime <= 0){
        getLocalData().afterImage.currentTime += getLocalData().afterImage.timegap;
        PaintownUtil::ReferenceCount<Animation> animation = getCurrentAnimation();
        if (animation != NULL){
            // afterImage.currentTime -= afterImage.timegap;
            Frame * currentSprite = animation->getCurrentFrame();
            getLocalData().afterImage.frames.push_front(AfterImage::Image(*currentSprite, animation->getCurrentEffects(getFacing() == FacingLeft, false, getLocalData().xscale, getLocalData().yscale), x, y, getLocalData().afterImage.lifetime > 0));
        }
    }
    if (getLocalData().afterImage.length > 0 &&
        getLocalData().afterImage.frames.size() >= getLocalData().afterImage.length){

        getLocalData().afterImage.frames.resize(getLocalData().afterImage.length - 1);
    }

#if 0
    for (deque<AfterImage::Image>::iterator it = afterImage.frames.begin(); it != afterImage.frames.end(); /**/ ){
        AfterImage::Image & frame = *it;
        frame.life -= 1;
        /* negative lifetimes mean indefinite frames */
        if (frame.life == 0){
            it = afterImage.frames.erase(it);
        } else {
            it++;
        }
    }
#endif
}
    
bool Character::withinGuardDistance(const Mugen::Character * enemy) const {
    if (enemy != NULL){
        if (enemy->getHit().alive && enemy->getHit().guardDistance >= 0){
            return fabs(getX() - enemy->getX()) < enemy->getHit().guardDistance;
        }

        return fabs(getX() - enemy->getX()) < enemy->getAttackDistance();
    }
    return false;
}
        
/* True if the state is one of the predetermined blocking states */
static bool blockingState(int state){
    return state == StartGuardStand ||
           state == StartGuardCrouch ||
           state == StartGuardAir ||
           state == GuardStand ||
           state == GuardCrouch ||
           state == GuardAir;
}
        
std::vector<std::string> Character::currentInputs() const {
    return getStateData().active;
}
        
void Character::setInputs(uint32_t tick, const std::vector<std::string> & inputs){
    /*
    while (tick <= getLocalData().inputHistory.size()){
        getLocalData().inputHistory.push_back(vector<string>());
    }
    */
    if (getLocalData().inputHistory.size() < tick){
        getLocalData().inputHistory.resize(tick);
    }
    getLocalData().inputHistory[tick - 1] = inputs;
}

/* Inherited members */
void Character::act(Stage * stage){

    getLocalData().maxChangeStates = 0;

    /* Reversals deactivate on state change or if a reversal actually occurs */
    // reversalActive = false;

    getStateData().special = SpecialStuff();
    getStateData().blocking = false;

    if (getStateData().frozen){
        getStateData().frozen = false;
    }
 
    //! Check pushable
    if (getStateData().pushPlayer > 0){
        getStateData().pushPlayer--;
    }

    /* reset some stuff */
    getStateData().widthOverride.enabled = false;
    getStateData().transOverride.enabled = false;
    getStateData().drawAngleData.enabled = false;

    processAfterImages();

    if (getLocalData().paletteEffects.time > 0){
        getLocalData().paletteEffects.counter += 1;
        getLocalData().paletteEffects.time -= 1;
    }

    for (map<int, HitOverride>::iterator it = getStateData().hitOverrides.begin(); it != getStateData().hitOverrides.end(); it++){
        HitOverride & override = it->second;
        if (override.time > 0){
            override.time -= 1;
        }
    }

    for (int slot = 0; slot < 2; slot++){
        if (getStateData().hitByOverride[slot].time > 0){
            getStateData().hitByOverride[slot].time -= 1;
        }
    }

    /* During replay the stage ticks will be less than the current size of the
     * input history vector. If the stage ticks is larger or equal then we are
     * in real-time and need to get raw inputs. Otherwise get the inputs
     * from the input history.
     */
    if (! stage->replayEnabled()){
        // stage->getTicks() - 1 >= getLocalData().inputHistory.size()){
        /* active is the current set of commands */
        getStateData().active = doInput(*stage);
        getLocalData().inputHistory.push_back(getStateData().active);

        recordCommands(getStateData().active);
    } else {
        getStateData().active = getLocalData().inputHistory[stage->getTicks() - 1];
    }

    if (getHitState().recoverTime > 0){
        getHitState().recoverTime -= 1;
    }

    getStateData().blocking = holdingBlock(getStateData().active);

    // if (hitState.shakeTime > 0 && moveType != Move::Hit){
    if (getHitState().shakeTime > 0){
        getHitState().shakeTime -= 1;

        /* Need to update the animation so it doesn't get stuck */
        PaintownUtil::ReferenceCount<Animation> animation = getCurrentAnimation();
        if (animation != NULL){
            animation->virtualTick();
        }
    } else {
        /* If statetime was -1 then we were paused due to a p1stateno or p2stateno
         * from a hitdef or reversaldef. When shaketime reaches 0 we need to activate
         * the new state that was set from delayChangeState.
         */
        if (getStateTime() == -1){
            PaintownUtil::ReferenceCount<State> state = getState(getCurrentState(), *stage);
            if (state != NULL){
                state->transitionTo(*stage, *this);
            } else {
                Global::debug(0) << "Unknown state " << getCurrentState() << endl;
            }
        }

        /* Stuff to skip if the player is shaking/paused */
        PaintownUtil::ReferenceCount<Animation> animation = getCurrentAnimation();
        if (animation != NULL){
            /* Check debug state */
            if (getLocalData().debug && !isHelper()){
                if (!animation->showingDefense()){
                    animation->toggleDefense();
                }
                if (!animation->showingOffense()){
                    animation->toggleOffense();
                }
            } else {
                if (animation->showingDefense()){
                    animation->toggleDefense();
                }
                if (animation->showingOffense()){
                    animation->toggleOffense();
                }
            }
            animation->logic();
        }

        if (getHitState().hitTime > -1){
            getHitState().hitTime -= 1;
        }

        if (isAttacking() && getHitState().moveContact > 0){
            getHitState().moveContact += 1;
        }

        /* if shakeTime is non-zero should we update stateTime? */
        getStateData().stateTime += 1;

        /* FIXME: there are a bunch more states that are considered blocking */
        if (getStateData().blocking && !blockingState(getCurrentState()) &&
            getMoveType() == Move::Idle &&
            stage->getEnemy(this)->getMoveType() == Move::Attack &&
            withinGuardDistance(stage->getEnemy(this))){

            changeState(*stage, Mugen::StartGuardStand);
        }

        /*
        if (needToGuard){
            needToGuard = false;
            / * misnamed state, but this is the first guard state and will
             * eventually transition to stand/crouch/air guard
             * /
            guarding = true;
            changeState(*stage, Mugen::StartGuardStand, active);
        }
        */
    }

    /* Check the states even if we are paused. If a controller has 'ignorehitpause = 1'
     * then it might still activate.
     */
    /* always run through the negative states unless we are borrowing another
     * players states
     */
    if (!getStateData().characterData.enabled){
        doStates(*stage, getStateData().active, -3);
        doStates(*stage, getStateData().active, -2);
        doStates(*stage, getStateData().active, -1);
    }
    doStates(*stage, getStateData().active, getCurrentState());

    /*! do regeneration if set, but only for main players */
    if (getLocalData().regenerateHealth && !isHelper()){
        if (getHealth() < 5){
            setHealth(5);
            getLocalData().regenerateTime = REGENERATE_TIME;
        }

        if (getLocalData().regenerating){

            /* avoid rounding errors */
            if (getHealth() >= getMaxHealth() - 2){
                setHealth(getMaxHealth());
            } else {
                setHealth((int) ((getMaxHealth() + getHealth()) / 2.0));
            }

            if (getHealth() == getMaxHealth()){
                getLocalData().regenerating = false;
            }
            getLocalData().regenerateTime = REGENERATE_TIME;
        } else if (getHealth() < getMaxHealth() && getLocalData().regenerateTime == REGENERATE_TIME){
            getLocalData().regenerateTime -= 1;
        } else if (getLocalData().regenerateTime <= 0){
            getLocalData().regenerating = true;
        } else {
            getLocalData().regenerateTime -= 1;
        }

        /*
        if (getHealth() < getMaxHealth()){
            regenerateTime = REGENERATE_TIME;
        } else {
            if (regenerateTime <= 0){
                setHealth((getMaxHealth() + getHealth())/2);
                regenerateHealthDifference = getHealth();
            } else {
                regenerateTime -= 1;
            }
        }
        */
    }
}
        
void Character::addPower(double d){
    getStateData().power += d;
    /* max power is 3000. is that specified somewhere or just hard coded
     * in the engine?
     */
    if (getStateData().power > 3000){
        getStateData().power = 3000;
    }

    if (getStateData().power < 0){
        getStateData().power = 0;
    }
}
        
void Character::testStates(Mugen::Stage & stage, const std::vector<std::string> & active, int stateNumber){
    if (getState(stateNumber, stage) != NULL){
        PaintownUtil::ReferenceCount<State> state = getState(stateNumber, stage);
        const vector<StateController*> & controllers = state->getControllers();
        FullEnvironment environment(stage, *this, active);
        for (vector<StateController*>::const_iterator it = controllers.begin(); it != controllers.end(); it++){
            StateController * controller = *it;
            try{
                controller->canTrigger(environment);
            } catch (const MugenNormalRuntimeException & me){
                Global::debug(1, getName()) << "Error while processing state " << stateNumber << ", " << controller->getName() << ". Error with trigger: " << me.getReason() << endl;
            } catch (const MugenFatalRuntimeException & me){
                Global::debug(0, getName()) << "Fatal error while processing state " << stateNumber << ", " << controller->getName() << ". Error with trigger: " << me.getReason() << endl;
            } catch (const MugenException & me){
                Global::debug(0, getName()) << "Abnormal error while processing state " << stateNumber << ", " << controller->getName() << ". Error with trigger: " << me.getReason() << endl;
            }
        }
    }
}

void Character::addCombo(int combo){
    getStateData().hitCount += combo;
    if (getStateData().hitCount < 0){
        getStateData().hitCount = 0;
    }
}

void Character::maybeTurn(Stage & stage){
    if (canTurn()){
        Mugen::Character * enemy = stage.getEnemy(this);
        if ((enemy->getX() > getX() && getFacing() != Mugen::FacingRight) ||
            (enemy->getX() < getX() && getFacing() != Mugen::FacingLeft)){
            doTurn(stage);
        }

        /*
        for (vector<Character*>::const_iterator enem = objects.begin(); enem != objects.end(); ++enem){
            Mugen::Character * enemy = *enem;
            if (stage.isaPlayer(enemy) && enemy->getAlliance() != getAlliance()){
                if ((enemy->getX() > getX() && getFacing() != Mugen::FacingRight) ||
                    (enemy->getX() < getX() && getFacing() != Mugen::FacingLeft)){
                    doTurn(stage);
                }
            }
        }
        */
    }
}
    
void Character::destroyed(Stage & stage){
    stage.unbind(this);
    stage.removeEffects(this, -1);
}

void Character::unbind(Character * who){
    if (getStateData().bind.bound == who->getId()){
        getStateData().bind.bound = CharacterId(-1);
        getStateData().bind.time = 0;
    }

    if (getStateData().characterData.who == who->getId()){
        getStateData().characterData.who = CharacterId(-1);
        getStateData().characterData.enabled = false;
    }

    for (map<int, vector<CharacterId> >::iterator it = getStateData().targets.begin(); it != getStateData().targets.end(); it++){
        vector<CharacterId> & objects = it->second;
        for (vector<CharacterId>::iterator it2 = objects.begin(); it2 != objects.end(); /**/){
            if (*it2 == who->getId()){
                it2 = objects.erase(it2);
            } else {
                it2++;
            }
        }
    }
}

void Character::doMovement(Stage & stage){
    if (getStateData().bind.time > 0 && getStateData().bind.bound != CharacterId(-1)){
        getStateData().bind.time -= 1;
        const Character * bound = stage.getCharacter(getStateData().bind.bound);
        setX(bound->getX() + getStateData().bind.offsetX * (bound->getFacing() == FacingLeft ? -1 : 1));
        setY(bound->getY() + getStateData().bind.offsetY);
        switch (getStateData().bind.facing){
            case -1: setFacing(bound->getOppositeFacing()); break;
            case 0: maybeTurn(stage); break;
            case 1: setFacing(bound->getFacing()); break;
        }
    } else {
        /* TODO: ensure that if shaketime > 0 that binding should still happen */
        if (getHitState().shakeTime > 0){
            return;
        }

        if (getCurrentPhysics() == Mugen::Physics::Air){
            if (getY() > 0){
                /* change to the landing state */
                // mugen->setXVelocity(0);
                /* FIXME: replace 52 with a constant */
                changeState(stage, 52);
            }
        }

#if 0
        if (getCurrentPhysics() == Mugen::Physics::Air){
            /* gravity */
            if (getY() < 0){
                double gravity = getGravity();
                setYVelocity(getYVelocity() + gravity);
            } else if (getYVelocity() > 0){
                /* change to the landing state */
                // mugen->setXVelocity(0);
                vector<string> inputs;
                /* FIXME: replace 52 with a constant */
                changeState(stage, 52, inputs);
            }
        } else if (getCurrentPhysics() == Mugen::Physics::Stand){
            setY(0);
        }
#endif

        moveX(getXVelocity());
        moveY(getYVelocity());
        /*
           if (mugen->getY() < 0){
           mugen->setY(0);
           }
           */

        maybeTurn(stage);
    }
}

void Character::setTargetId(int id, Character * enemy){
    vector<CharacterId> & objects = getStateData().targets[id];

    /* Don't add a duplicate target */
    for (vector<CharacterId>::iterator it = objects.begin(); it != objects.end(); it++){
        if (*it == enemy->getId()){
            return;
        }
    }
    objects.push_back(enemy->getId());
}
        
CharacterId Character::getTargetId(int id) const {
    if (getStateData().targets.find(id) != getStateData().targets.end()){
        const vector<CharacterId> & objects = getStateData().targets.find(id)->second;
        if (objects.size() > 0){
            /* FIXME: return a random target? */
            return objects[PaintownUtil::rnd(objects.size())];
        }
    }
    return CharacterId(-1);
}
        
void Character::didHitGuarded(Character * enemy, Mugen::Stage & stage){
    /* TODO */
    getHitState().shakeTime = getHit().guardPause.player1;
    getHitState().spritePriority = getHit().player1SpritePriority;
    getHitState().moveContact = 1;
    addPower(getHit().getPower.guarded);
    getStateData().characterData.who = CharacterId(-1);
    getStateData().characterData.enabled = false;

    disableHit();
}

void Character::didHit(Character * enemy, Mugen::Stage & stage){
    getStateData().characterData.who = CharacterId(-1);
    getStateData().characterData.enabled = false;
    getHitState().shakeTime = getHit().pause.player1;
    getHitState().spritePriority = getHit().player1SpritePriority;
    getHitState().moveContact = 1;
    addPower(getHit().getPower.hit);

    /* Once a hitdef hits, it cannot hit again */
    disableHit();

    if (getHit().id != 0){
        setTargetId(getHit().id, enemy);
    }

    /* if he is already in a Hit state then increase combo */
    if (enemy->getMoveType() == Move::Hit){
        getStateData().combo += 1;
    } else {
        getStateData().combo = 1;
    }

    // nextCombo = 15;

    getStateData().hitCount += 1;

    /* Mainly used for AI so it can tell if a hit succeeded and thus learn which moves to do */
    if (getLocalData().behavior != NULL){
        getLocalData().behavior->hit(enemy);
    }

    /* FIXME: handle p1getp2facing */
    if (getHit().player1Facing == -1){
        setFacing(getOppositeFacing());
    }

    if (getHit().player1State != -1){
        delayChangeState(stage, getHit().player1State);
    }
}

void Character::takeDamage(Stage & world, Object * obj, double amount, double forceX, double forceY){
    hurt(amount);
}

void Character::hurt(double x){
    setHealth(getHealth() - x);
}

void Character::takeDamage(Stage & world, Object * obj, double amount, bool kill, bool defense){
    /* TODO: use getDefense() here somehow */
    if (defense){
        takeDamage(world, obj, amount / getStateData().defenseMultiplier, 0.0, 0.0);
    } else {
        takeDamage(world, obj, amount, 0.0, 0.0);
    }

    if (!kill){
        if (getHealth() < 1){
            setHealth(1);
        }
    }
}
        
void Character::takeDamage(Stage & world, Object * obj, int amount){
    takeDamage(world, obj, amount, true, true);
}

void Character::wasReversed(Mugen::Stage & stage, Character * enemy, const ReversalData & data){
    disableHit();
    getHitState().shakeTime = data.player2Pause;
    /* FIXME: what should the sprite priority of a reversal be? */
    getHitState().spritePriority = 0;
    if (data.player2State != -1){
        useCharacterData(enemy->getRoot());
        delayChangeState(stage, data.player2State);
    }
}

void Character::didReverse(Mugen::Stage & stage, Character * enemy, const ReversalData & data){
    setReversalInactive();
    getHitState().shakeTime = data.player1Pause;
    /* FIXME: what should the sprite priority of a reversal be? */
    getHitState().spritePriority = 0;
    if (data.player1State != -1){
        delayChangeState(stage, data.player1State);
    }
}

void Character::wasHit(Mugen::Stage & stage, Character * enemy, const HitDefinition & hisHit){
    getStateData().characterData.who = CharacterId(-1);
    getStateData().characterData.enabled = false;

    getStateData().wasHitCounter += 1;
    update(getHitState(), stage, *this, getY() < 0, hisHit);
    
    addPower(hisHit.givePower.hit);

    /* I guess HitVelSet in the 50X0 states will set the velocity
     * based on the hitstate.
     */
    /*
    switch (enemy->getFacing()){
        case FacingRight: {
            setXVelocity(-hitState.xVelocity);
            break;
        }
        case FacingLeft: {
            setXVelocity(hitState.xVelocity);
            break;
        }
    }
    setYVelocity(hitState.yVelocity);
    */

    // lastTicket = enemy->getTicket();

    if (hisHit.damage.damage != 0){
        takeDamage(stage, enemy, hisHit.damage.damage, true, true);
    }

    switch (hisHit.player2Facing){
        case -1: setFacing(enemy->getFacing()); break;
        case 1: setFacing(enemy->getOppositeFacing()); break;
        default: break;
    }

    setControl(false);

    /*
    if (getHealth() <= 0){
        hitState.fall.fall = true;
    }
    */
    
    /* FIXME: not sure if disabling afterimage's is the right thing */
    getLocalData().afterImage.lifetime = 0;

    getStateData().juggleRemaining -= enemy->getCurrentJuggle() + hisHit.airJuggle;

    for (map<int, HitOverride>::iterator it = getStateData().hitOverrides.begin(); it != getStateData().hitOverrides.end(); it++){
        HitOverride & override = it->second;
        /* Time can be -1 (infinity) or some positive number (counting down) */
        if (override.time != 0 && hasHitAttribute(enemy, override.attributes)){
            /* FIXME: handle override.forceAir */
            if (override.state != -1){
                changeState(stage, override.state);
            }

            /* I believe that if any hit override matches that all the others
             * are ignored and the rest of the logic about checking player2State
             * and whatever is skipped.
             */
            return;
        }
    }

    if (hisHit.player2State != -1){
        /* Use the state data from the enemy until we are hit or call SelfState
         * Always get the data from root so we don't bind to a helper which may
         * be deleted soon.
         */
        useCharacterData(enemy->getRoot());
        changeState(stage, hisHit.player2State);
    } else {
        /* FIXME: replace 50*0 with some constant */
        if (getStateType() == StateType::Air){
            changeState(stage, 5020);
        } else if (getStateType() == StateType::Crouch){
            changeState(stage, 5010);
        } else if (getStateType() == StateType::Stand){
            changeState(stage, 5000);
        }
    }
}

/* returns true if a state change occured */
bool Character::doStates(Mugen::Stage & stage, const vector<string> & active, int stateNumber){
    int oldState = getCurrentState();
    if (getState(stateNumber, stage) != NULL){
        PaintownUtil::ReferenceCount<State> state = getState(stateNumber, stage);
        // Global::debug(0) << getDisplayName() << " evaluating state " << stateNumber << " states " << state->getControllers().size() << std::endl;
        const vector<StateController*> & controllers = state->getControllers();
        FullEnvironment environment(stage, *this, active);
        for (vector<StateController*>::const_iterator it = controllers.begin(); it != controllers.end(); it++){
            StateController * controller = *it;
            Global::debug(2 * !controller->getDebug()) << "State " << stateNumber << " check state controller " << controller->getName() << endl;

#if 0
            /* more debugging */
            bool hasFF = false;
            for (vector<string>::const_iterator it = active.begin(); it != active.end(); it++){
                if (*it == "holdup"){
                    hasFF = true;
                }
            }
            if (stateNumber == -1 && controller->getName() == "jump" && hasFF){
            if (controller->getName() == "run fwd"){
                int x = 2;
            }
            /* for debugging
            if (stateNumber == 20 && controller->getName() == "3"){
                int x = 2;
            }
            */
#endif

            try{
                if (controller->canTrigger(environment)){
                    /* check if the controller's persistent values allow it
                     * to be activated.
                     */
                    if (controller->persistentOk()){
                        Global::debug(2, getDisplayName()) << "Activate controller " << controller->getName() << std::endl;
                        /* activate may modify the current state */
                        controller->activate(stage, *this, active);

                        /* 8/27/2012 - the mugen docs say this about negative states:
                         *   For each tick of game-time, MUGEN makes a single pass through each of the special states, from top to bottom, in order of increasing state number (-3, -2, then -1). For each state controller encountered, its condition-type triggers are evaluated and, if they are satisfied, the controller is executed. Then processing proceeds to the next state controller in the state. A state transition (ChangeState) in any of the special states will update the player's current state number, but will not abort processing of the special states. After all the state controllers in the special states have been checked, the player's current state is processed, again from top to bottom. If a state transition is made out of the current state, the rest of the state controllers (if any) in the current state are skipped, and processing continues from the beginning of the new state. When the end of the current state is reached and no state transition is made, processing halts for this tick.
                         *
                         * But in testing mugen it seems like a ChangeState in a special state does abort the rest of the controllers. I tested this in kfm.cmd
                         *
                         * [State -1, xx1]
                         * type = ChangeState
                         * value = 0
                         * trigger1 = 1 
                         *
                         * [State -1, debugx2]
                         * type = DisplayToClipboard
                         * text = "testing!"
                         * params = random
                         * trigger1 = 1
                         *
                         * The clipboard is never updated with the text "testing!". What really happens is that MUGEN will process states -3, -2, -1 no matter what followed by the current state but if any state (negative or normal) does a ChangeState controller then the rest of the controllers in that state are skipped.
                         *
                         * So it doesn't matter what the state number is, we always return immediately if the current state was changed.
                         */
                        /*
                        if (stateNumber >= 0 && getCurrentState() != oldState){
                            return true;
                        }
                        */

                        if (getCurrentState() != oldState){
                            return true;
                        }
                    }
                }
            } catch (const MugenNormalRuntimeException & me){
                Global::debug(1, getName()) << "Error while processing state " << stateNumber << ", " << controller->getName() << ". Error with trigger: " << me.getReason() << endl;
            } catch (const MugenFatalRuntimeException & me){
                Global::debug(0, getName()) << "Fatal error while processing state " << stateNumber << ", " << controller->getName() << ". Error with trigger: " << me.getReason() << endl;
            } catch (const MugenException & me){
                Global::debug(0, getName()) << "Abnormal error while processing state " << stateNumber << ", " << controller->getName() << ". Error with trigger: " << me.getReason() << endl;
            }
        }
    }
    return false;
}

void Character::drawAfterImage(const AfterImage & afterImage, const AfterImage::Image & frame, int index, int x, int y, const Graphics::Bitmap & work){
    class AfterImageFilter: public Graphics::Bitmap::Filter {
    public:
        /* all the components have this formula
         * x' = (x + bright) * contrast / 256 + post
         * = x * contrast / 256 + bright * constrast / 256 + post
         * So we will call 'contrast/256' multiplier and
         * 'bright * contrast / 256 + post' adder
         */
        struct Save{
            Save(double bright, double contrast, double post):
                multiplier(contrast / 256.0),
                adder(bright * contrast / 256.0 + post){
                }
            double multiplier;
            double adder;
        };

        AfterImageFilter(const AfterImage::RGBx & bright, const AfterImage::RGBx & contrast, const AfterImage::RGBx & post, const AfterImage::RGBx & extraAdd, const AfterImage::RGBx & extraMultiplier, int extra):
            bright(bright),
            contrast(contrast),
            post(post),
            extraAdd(extraAdd),
            extraMultiplier(extraMultiplier),
            extra(extra),
            redPart(bright.red, contrast.red, post.red),
            greenPart(bright.green, contrast.green, post.green),
            bluePart(bright.blue, contrast.blue, post.blue){

                redExtraAdd = extraAdd.red * extra;
                redExtraMultiply = pow(extraMultiplier.red, extra);
                greenExtraAdd = extraAdd.green * extra;
                greenExtraMultiply = pow(extraMultiplier.green, extra);
                blueExtraAdd = extraAdd.blue * extra;
                blueExtraMultiply = pow(extraMultiplier.blue, extra);
            }

        const AfterImage::RGBx & bright;
        const AfterImage::RGBx & contrast;
        const AfterImage::RGBx & post;
        const AfterImage::RGBx & extraAdd;
        const AfterImage::RGBx & extraMultiplier;
        const int extra;
        mutable map<Graphics::Color, Graphics::Color> cache;

        double redExtraAdd;
        double redExtraMultiply;
        double greenExtraAdd;
        double greenExtraMultiply;
        double blueExtraAdd;
        double blueExtraMultiply;

        Save redPart;
        Save greenPart;
        Save bluePart;

        Graphics::Color doFilter(int red, int green, int blue) const {
            double red_out = red;
            double green_out = green;
            double blue_out = blue;

            // red_out = (red_out + bright.red) * contrast.red / 256 + post.red;
            // green_out = (green_out + bright.green) * contrast.green / 256 + post.green;
            // blue_out = (blue_out + bright.blue) * contrast.blue / 256 + post.blue;
            red_out = red_out * redPart.multiplier + redPart.adder;
            green_out = green_out * greenPart.multiplier + greenPart.adder;
            blue_out = blue_out * bluePart.multiplier + bluePart.adder;

            /* the mugen docs lied:
             * "In one application of these palette effects, first the paladd components are added to the afterimage palette, then the components are multiplied by the palmul multipliers. These effects are applied zero times to the most recent afterimage frame, once to the  second-newest afterimage frame, twice in succession to the third-newest afterimage frame, etc."
             * This would lead you to believe that you should do an add and
             * multiply operation for some number of times but in fact what
             * you are supposed to do is do all the add operations first
             * and then do all the multiply operations second.
             */

            red_out += redExtraAdd;
            red_out *= redExtraMultiply;
            green_out += greenExtraAdd;
            green_out *= greenExtraMultiply;
            blue_out += blueExtraAdd;
            blue_out *= blueExtraMultiply;

            /*
            red_out += extraAdd.red * extra;
            red_out *= pow(extraMultiplier.red, extra);
            green_out += extraAdd.green * extra;
            green_out *= pow(extraMultiplier.green, extra);
            blue_out += extraAdd.blue * extra;
            blue_out *= pow(extraMultiplier.blue, extra);
            */

            /* This is what the code would look like if the mugen docs
             * were telling the truth.
             */
            /*
               for (int i = 0; i < extra; i++){
               red_out += extraAdd.red;
               red_out *= extraMultiplier.red;
               green_out += extraAdd.green;
               green_out *= extraMultiplier.green;
               blue_out += extraAdd.blue;
               blue_out *= extraMultiplier.blue;
               }
               */

            if (red_out < 0){
                red_out = 0;
            }
            if (red_out > 255){
                red_out = 255;
            }
            if (green_out < 0){
                green_out = 0;
            }
            if (green_out > 255){
                green_out = 255;
            }
            if (blue_out < 0){
                blue_out = 0;
            }
            if (blue_out > 255){
                blue_out = 255;
            }

            Graphics::Color out = Graphics::makeColor((int) red_out, (int) green_out, (int) blue_out);
            return out;
        }

        PaintownUtil::ReferenceCount<Graphics::Shader> shader;

        static PaintownUtil::ReferenceCount<Graphics::Shader> create(){
            PaintownUtil::ReferenceCount<Graphics::Shader> out;

#ifdef USE_ALLEGRO5
            std::ostringstream vertex;
            // vertex << "#version 110\n";
            vertex << Graphics::defaultVertexShader();
            out = Graphics::create_shader("110", vertex.str(), Storage::readFile(Storage::instance().find(Filesystem::RelativePath("shaders/mugen-after-effect.fragment.glsl"))));
#endif

            return out;
        }

        PaintownUtil::ReferenceCount<Graphics::Shader> getShader(){
            if (shader == NULL){
                PaintownUtil::ReferenceCount<Graphics::ShaderManager> manager = Graphics::shaderManager.current();
                shader = manager->getShader("mugen-after-effect", create);
            }

            return shader;
        }

        void setupShader(const PaintownUtil::ReferenceCount<Graphics::Shader> & what){
#ifdef USE_ALLEGRO5
            Graphics::setShaderFloat(shader->getShader(), "extra", extra);
            Graphics::setShaderVec4(shader->getShader(), "bright", bright.red / 255.0, bright.green / 255.0, bright.blue / 255.0, 0);
            Graphics::setShaderVec4(shader->getShader(), "contrast", contrast.red / 255.0, contrast.green / 255.0,  contrast.blue / 255.0, 0);
            Graphics::setShaderVec4(shader->getShader(), "post", post.red / 255.0, post.green / 255.0, post.blue / 255.0, 0);
            Graphics::setShaderVec4(shader->getShader(), "extraAdd", extraAdd.red / 255.0, extraAdd.green / 255.0, extraAdd.blue / 255.0, 0);
            Graphics::setShaderVec4(shader->getShader(), "extraMultiplier", extraMultiplier.red, extraMultiplier.green, extraMultiplier.blue, 0);
#endif
        }

        Graphics::Color filter(Graphics::Color pixel) const {
            if (cache.find(pixel) != cache.end()){
                return cache[pixel];
            }

            int red = Graphics::getRed(pixel);
            int green = Graphics::getGreen(pixel);
            int blue = Graphics::getBlue(pixel);
            Graphics::Color out = doFilter(red, green, blue);
            cache[pixel] = out;
            return out;
        }
    };

    /* TODO: handle afterImage.color and afterImage.invert */
    AfterImageFilter filter(afterImage.bright, afterImage.contrast, afterImage.postBright, afterImage.add, afterImage.multiply, index);

    Effects total = frame.effects + afterImage.translucent;
    total.filter = &filter;

    // Sprite::draw(frame.sprite->getSprite()->getFinalBitmap(frame.effects), x, y, frame.sprite->getSprite()->getX(), frame.sprite->getSprite()->getY(), work, total);
    frame.sprite.render(x, y, work, total);

    // frame.sprite->render(frame.x - cameraX + drawOffset.x, frame.y - cameraY + drawOffset.y, *work, frame.effects + afterImage.translucent + blender);
    /*
    Bitmap original = frame.sprite->getSprite()->getFinalBitmap(frame.effects);
    Bitmap fixed = Bitmap::temporaryBitmap2(original.getWidth(), original.getHeight());
    original.draw(0, 0, filter, fixed);
    */

    /* trade time for space -- but allocating a bitmap is sort
     * of expensive..
     */
    // return Bitmap(fixed, true);
    // frame.cache = Bitmap(fixed, true);
}

class PaletteFilter: public Graphics::Bitmap::Filter {
    public:
        PaletteFilter(int time, int addRed, int addGreen, int addBlue, int multiplyRed, int multiplyGreen, int multiplyBlue, int sinRed, int sinGreen, int sinBlue, int period, int invert, int color):
        time(time),
        addRed(addRed),
        addGreen(addGreen),
        addBlue(addBlue),
        multiplyRed(multiplyRed),
        multiplyGreen(multiplyGreen),
        multiplyBlue(multiplyBlue),
        sinRed(sinRed),
        sinGreen(sinGreen),
        sinBlue(sinBlue),
        period(period),
        invert(invert),
        color(color){
        }

        int time;
        int addRed;
        int addGreen;
        int addBlue;
        int multiplyRed;
        int multiplyGreen;
        int multiplyBlue;
        int sinRed;
        int sinGreen;
        int sinBlue;
        int period;
        int invert;
        int color;
        
        mutable map<Graphics::Color, Graphics::Color> cache;

        Graphics::Color doFilter(int red, int green, int blue) const {
            int newRed = red;
            int newGreen = green;
            int newBlue = blue;

            if (color < 255){
                double greyRed = (1 - 0.299) * color / 255 + 0.299;
                double greyGreen = (1 - 0.587) * color / 255 + 0.587;
                double greyBlue = (1 - 0.114) * color / 255 + 0.114;
                red = (int)(red * greyRed + 0.5 + 16);
                green = (int)(green * greyGreen + 0.5 + 16);
                blue = (int)(blue * greyBlue + 0.5 + 16);
            }

            if (invert){
                red = 255 - red;
                green = 255 - green;
                blue = 255 - blue;
            }

            if (period > 0){
                newRed = (red + addRed + sinRed * sin(2 * PaintownUtil::pi * time / period)) * multiplyRed / 256;
                newGreen = (green + addGreen + sinGreen * sin(2 * PaintownUtil::pi * time / period)) * multiplyGreen / 256;
                newBlue = (blue + addBlue + sinBlue * sin(2 * PaintownUtil::pi * time / period)) * multiplyBlue / 256;
            } else {
                newRed = (red + addRed) * multiplyRed / 256;
                newGreen = (green + addGreen) * multiplyGreen / 256;
                newBlue = (blue + addBlue) * multiplyBlue / 256;
            }

            if (newRed > 255){
                newRed = 255;
            }

            if (newRed < 0){
                newRed = 0;
            }

            if (newGreen > 255){
                newGreen = 255;
            }

            if (newGreen < 0){
                newGreen = 0;
            }

            if (newBlue > 255){
                newBlue = 255;
            }

            if (newBlue < 0){
                newBlue = 0;
            }

            return Graphics::makeColor(newRed, newGreen, newBlue);
        }

        PaintownUtil::ReferenceCount<Graphics::Shader> shader;

        static PaintownUtil::ReferenceCount<Graphics::Shader> create(){
            PaintownUtil::ReferenceCount<Graphics::Shader> out;

#ifdef USE_ALLEGRO5
            std::ostringstream vertex;
            // vertex << "#version 110\n";
            vertex << Graphics::defaultVertexShader();
            out = Graphics::create_shader("110", vertex.str(), Storage::readFile(Storage::instance().find(Filesystem::RelativePath("shaders/mugen-palette-effect.fragment.glsl"))));
#endif

            return out;
        }

        PaintownUtil::ReferenceCount<Graphics::Shader> getShader(){
            if (shader == NULL){
                PaintownUtil::ReferenceCount<Graphics::ShaderManager> manager = Graphics::shaderManager.current();
                shader = manager->getShader("mugen-palfx", create);
            }

            return shader;
        }

        void setupShader(const PaintownUtil::ReferenceCount<Graphics::Shader> & what){
#ifdef USE_ALLEGRO5
            Graphics::setShaderBool(shader->getShader(), "invert", invert > 0);
            Graphics::setShaderInt(shader->getShader(), "time", time);
            Graphics::setShaderInt(shader->getShader(), "period", period);
            Graphics::setShaderFloat(shader->getShader(), "color", (float) color / 255.0);
            Graphics::setShaderVec4(shader->getShader(), "add", (float) addRed / 255.0, (float) addGreen / 255.0, (float) addBlue / 255.0, 0);
            Graphics::setShaderVec4(shader->getShader(), "multiply", (float) multiplyRed / 256.0, (float) multiplyGreen / 256.0, (float) multiplyBlue / 256.0, 0);
            Graphics::setShaderVec4(shader->getShader(), "sin_", (float) sinRed / 255.0, (float) sinGreen / 255.0, (float) sinBlue / 255.0, 0);
#endif
        }

        Graphics::Color filter(Graphics::Color pixel) const {
            
            if (cache.find(pixel) != cache.end()){
                return cache[pixel];
            }

            int red = Graphics::getRed(pixel);
            int green = Graphics::getGreen(pixel);
            int blue = Graphics::getBlue(pixel);
            Graphics::Color out = doFilter(red, green, blue);
            cache[pixel] = out;
            return out;
        }
    };

Graphics::Bitmap::Filter * Character::getPaletteEffects(unsigned int time){
    return new PaletteFilter(time, getLocalData().paletteEffects.addRed,
                    getLocalData().paletteEffects.addGreen, getLocalData().paletteEffects.addBlue,
                    getLocalData().paletteEffects.multiplyRed, getLocalData().paletteEffects.multiplyGreen,
                    getLocalData().paletteEffects.multiplyBlue, getLocalData().paletteEffects.sinRed,
                    getLocalData().paletteEffects.sinGreen, getLocalData().paletteEffects.sinBlue,
                    getLocalData().paletteEffects.period, getLocalData().paletteEffects.invert,
                    getLocalData().paletteEffects.color);
}

/* FIXME: reimplement this method */
void Character::drawMugenShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh){
#if 0
    if (animation_current != NULL){
        const Graphics::Bitmap *bmp = animation_current->getCurrentFrame();
        const double newheight = bmp->getHeight() * scale;
        Graphics::Bitmap shade = Graphics::Bitmap::temporaryBitmap(bmp->getWidth(), (int) fabs(newheight));
        bmp->Stretch(shade);

        /* Could be slow, but meh, lets do it for now to make it look like a real shadow */
        for (int h = 0; h < shade.getHeight(); ++h){
            for (int w = 0; w < shade.getWidth(); ++w){
                Graphics::Color pix = shade.getPixel(w,h);
                if (pix != Graphics::MaskColor()){
                    shade.putPixel(w,h, Graphics::makeColor(0,0,0));
                }
            }
        }

        int i = ((Graphics::getRed(color) * 77 + intensity) + (Graphics::getGreen(color) * 154 + intensity) + (Graphics::getBlue(color) * 25 + intensity))/256;
        i = 255 - i;
        // Bitmap::transBlender(Bitmap::getRed(color), Bitmap::getGreen(color), Bitmap::getBlue(color), i);
        Graphics::Bitmap::multiplyBlender((Graphics::getRed(color) * 77 + intensity), (Graphics::getGreen(color) * 154 + intensity), (Graphics::getBlue(color) * 25 + intensity), i);

        if (scale > 0){
            int x = (int)(getRX() - rel_x - bmp->getWidth()/2);
            int y = (int)(getRZ() + getY() * scale);
            if (getFacing() == FACING_RIGHT){ 
                shade.translucent().drawVFlip( x, y, *work );
            } else { 
                shade.translucent().drawHVFlip(x, y, *work );
            }
        } else if (scale < 0){
            int x = (int)((getRX() - rel_x) - bmp->getWidth()/2);
            int y = (int)((getRZ() - fabs(newheight)) + (getY() * scale));
            if (getFacing() == FACING_RIGHT){ 
                shade.translucent().draw(x + 3, y, *work );
            } else { 
                shade.translucent().drawHFlip(x - 3, y, *work );
            }
        }
    }
#endif
}
        
int Character::getStateTime() const {
    return getStateData().stateTime;
}

void Character::draw(Graphics::Bitmap * work, int cameraX, int cameraY){
    /*
    int color = Bitmap::makeColor(255,255,255);
    font.printf( x, y, color, *work, "State %d Animation %d", 0,  getCurrentState(), currentAnimation);
    font.printf( x, y + font.getHeight() + 1, color, *work, "X %f Y %f", 0, (float) getXVelocity(), (float) getYVelocity());
    font.printf( x, y + font.getHeight() * 2 + 1, color, *work, "Time %d", 0, getStateTime());
    */

    if (getStateData().special.invisible){
        return;
    }

    PaintownUtil::ReferenceCount<Animation> animation = getCurrentAnimation();
    /* this should never be NULL... */
    if (animation != NULL){
        int x = getX() - cameraX + getLocalData().drawOffset.x;
        int y = getRY() - cameraY + getLocalData().drawOffset.y;

        /* shake, but only if we are the one being hit */
        if (isPaused() && getMoveType() == Move::Hit){
            x += PaintownUtil::rnd(3) - 1;
        }

        for (unsigned int index = 0; index < getLocalData().afterImage.frames.size(); index += getLocalData().afterImage.framegap){
            AfterImage::Image & frame = getLocalData().afterImage.frames[index];
            if (frame.show){
                int x = frame.x - cameraX + getLocalData().drawOffset.x + frame.sprite.xoffset;
                int y = frame.y - cameraY + getLocalData().drawOffset.y + frame.sprite.yoffset;
                drawAfterImage(getLocalData().afterImage, frame, index, x, y, *work);
            }
        }

        Mugen::Effects effects;

        effects.facing = getFacing() == FacingLeft;
        effects.vfacing = false;
        effects.scalex = getLocalData().xscale;
        effects.scaley = getLocalData().yscale;

        if (getStateData().transOverride.enabled){
            effects.alphaSource = getStateData().transOverride.alphaSource;
            effects.alphaDest = getStateData().transOverride.alphaDestination;
            effects.trans = getStateData().transOverride.type;
        }

        if (getStateData().drawAngleData.enabled){
            effects.scalex *= getStateData().drawAngleData.scaleX;
            effects.scaley *= getStateData().drawAngleData.scaleY;
            if (getFacing() == FacingRight){
                /* Counter clock wise if facing right */
                effects.rotation = getStateData().drawAngleData.angle;
            } else {
                effects.rotation = -getStateData().drawAngleData.angle;
            }
        }

        PaintownUtil::ReferenceCount<Graphics::Bitmap::Filter> filter;
        if (getLocalData().paletteEffects.time > 0){
            filter = getPaletteEffects(getLocalData().paletteEffects.counter);
            effects.filter = filter.raw();
        }

        animation->render(x, y, *work, effects);
    }

    if (getLocalData().debug && !isHelper()){
        const ::Font & font = ::Font::getDefaultFont(16, 16);
        int x = 1;
        if (getAlliance() == Mugen::Stage::Player2Side){
            x = 640 - font.textLength("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") - 1;
        }
        int y = 1;
        Graphics::Color color = Graphics::makeColor(255, 255, 255);
        Graphics::Color backgroundColor = Graphics::MaskColor();
        FontRender * render = FontRender::getInstance();
        render->addMessage(font, x, y, color, backgroundColor, "State %d Animation %d", getCurrentState(), getStateData().currentAnimation);
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "Vx %f Vy %f", getXVelocity(), getYVelocity());
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "X %f Y %f", getX(), getY());
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "Time %d", getStateTime());
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "State type %s", getStateType().c_str());
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "Attack type %s", getMoveType().c_str());
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "Hit enabled %d", getHit().alive);
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "Control %d", hasControl());
        y += font.getHeight();
        render->addMessage(font, x, y, color, backgroundColor, "Pause time %d", getHitState().shakeTime);
        y += font.getHeight();
        if (getMoveType() == Move::Hit){
            render->addMessage(font, x, y, color, backgroundColor, "HitShake %d HitTime %d", getHitState().shakeTime, getHitState().hitTime);
            y += font.getHeight();
            render->addMessage(font, x, y, color, backgroundColor, "Hit velocity x %f y %f", getHitState().xVelocity, getHitState().yVelocity);
        }

        int wx = 1;
        int wy = 1;
        int width = work->getWidth();
        int height = 110;
        if (getAlliance() == Mugen::Stage::Player2Side){
            wx = work->getWidth() - width - 1;
        }
        // Graphics::Bitmap::transBlender(0, 0, 0, 128);
        work->translucent(128).rectangleFill(wx, wy, wx+width, wy+height, Graphics::makeColor(0, 0, 0));
        work->translucent(128).line(0, wy+height, wx+width, wy+height, Graphics::makeColor(64, 64, 64));
    }
}

bool Character::canTurn() const {
    return getCurrentState() == Standing ||
           getCurrentState() == WalkingForwards ||
           getCurrentState() == WalkingBackwards ||
           getCurrentState() == Crouching;
}
        
void Character::setTransOverride(TransType type, int alphaFrom, int alphaTo){
    getStateData().transOverride.enabled = true;
    getStateData().transOverride.type = type;
    getStateData().transOverride.alphaSource = alphaFrom;
    getStateData().transOverride.alphaDestination = alphaTo;
}

static PaintownUtil::ReferenceCount<Mugen::Sound> findSound(const Mugen::SoundMap & sounds, int group, int item){
    Mugen::SoundMap::const_iterator findGroup = sounds.find(group);
    if (findGroup != sounds.end()){
        const map< unsigned int, PaintownUtil::ReferenceCount<Mugen::Sound> > & found = (*findGroup).second;
        map<unsigned int, PaintownUtil::ReferenceCount<Mugen::Sound> >::const_iterator sound = found.find(item);
        if (sound != found.end()){
            return sound->second;
        }
    }
    return PaintownUtil::ReferenceCount<Mugen::Sound>(NULL);
}

PaintownUtil::ReferenceCount<Mugen::Sound> Character::getCommonSound(int group, int item) const {
    if (getCommonSounds() == NULL){
        return PaintownUtil::ReferenceCount<Mugen::Sound>(NULL);
    }
    return findSound(*getCommonSounds(), group, item);
}
        
PaintownUtil::ReferenceCount<Mugen::Sound> Character::getSound(int group, int item) const {
    return findSound(getSounds(), group, item);
}

void Character::doTurn(Mugen::Stage & stage){
    if (getCurrentState() != Mugen::Crouching){
        changeState(stage, Mugen::StandTurning);
    } else {
        changeState(stage, Mugen::CrouchTurning);
    }
    if (getLocalData().behavior != NULL){
        getLocalData().behavior->flip();
    }
    reverseFacing();
}

const vector<Area> Character::getAttackBoxes() const {
    if (getCurrentAnimation() != NULL){
        return getCurrentAnimation()->getAttackBoxes(getFacing() == FacingLeft, getLocalData().xscale, getLocalData().yscale);
    }
    return vector<Area>();
}

const vector<Area> Character::getDefenseBoxes() const {
    if (getCurrentAnimation() != NULL){
        return getCurrentAnimation()->getDefenseBoxes(getFacing() == FacingLeft, getLocalData().xscale, getLocalData().yscale);
    }
    return vector<Area>();
}

const std::string Character::getAttackName(){
    return getName();
}

int Character::getDamage() const {
    return 0;
}
    
double Character::getForceX() const {
    return 0;
}

double Character::getForceY() const {
    return 0;
}

/* FIXME: I think the width/backwidth/backx things should take into account
 * if the player is in the air. If so it should return airBack/front/etc..
 */
int Character::getWidth() const {
    if (getStateData().widthOverride.enabled){
        return getStateData().widthOverride.playerFront;
    }
    return getLocalData().groundfront;
}

int Character::getBackWidth() const {
    if (getStateData().widthOverride.enabled){
        return getStateData().widthOverride.playerBack;
    }
    return getLocalData().groundback;
}
        
int Character::getBackX() const {
    int width = getBackWidth();
    if (getStateData().widthOverride.enabled){
        width = getStateData().widthOverride.edgeBack;
    }
    if (getFacing() == FacingLeft){
        return getX() + width;
    }
    return getX() - width;
}

int Character::getFrontX() const {
    int width = getWidth();
    if (getStateData().widthOverride.enabled){
        width = getStateData().widthOverride.edgeFront;
    }
    if (getFacing() == FacingLeft){
        return getX() - width;
    }
    return getX() + width;
}

int Character::getCurrentCombo() const {
    return getStateData().combo;
}

/* TODO: implement these */
void Character::setUnhurtable(){
}

void Character::setHurtable(){
}
        
void Character::addWin(WinGame win){
    getLocalData().wins.push_back(win);
}

void Character::addMatchWin(){
    getLocalData().matchWins += 1;
}

void Character::resetPlayer(){
    clearWins();
    getStateData().power = 0;
    setHealth(getMaxHealth());
}
        
bool Character::isBlocking(const HitDefinition & hit){
    /* FIXME: can only block if in the proper state relative to the hit def */
    return hasControl() && getStateData().blocking;
}
        
void Character::resetHitFlag(){
    /* FIXME: not sure if this is right */
    getStateData().guarding = false;
}

bool Character::isGuarding() const {
    return getStateData().guarding;
}
        
void Character::guarded(Mugen::Stage & stage, Object * enemy, const HitDefinition & hit){
    /* FIXME: call hitState.updateGuard */
    getHitState().guarded = true;
    // lastTicket = enemy->getTicket();
    getHitState().shakeTime = hit.guardPause.player2;
    getHitState().spritePriority = hit.player2SpritePriority;
    addPower(hit.givePower.guarded);
    bool inAir = getY() > 0;
    if (inAir){
    } else {
        setXVelocity(hit.guardVelocity);
    }
    
    if (getStateType() == StateType::Air){
        changeState(stage, Mugen::GuardAir);
    } else if (getStateType() == StateType::Crouch){
        changeState(stage, Mugen::GuardCrouch);
    } else if (getStateType() == StateType::Stand){
        changeState(stage, Mugen::GuardStand);
    }
}

void Character::setAfterImage(int time, int length, int timegap, int framegap, TransType translucent, int paletteColor, bool invertColor, const AfterImage::RGBx & bright, const AfterImage::RGBx & contrast, const AfterImage::RGBx & postBright, const AfterImage::RGBx & add, const AfterImage::RGBx & multiply){
    getLocalData().afterImage.currentTime = 0;
    getLocalData().afterImage.timegap = timegap;
    getLocalData().afterImage.framegap = framegap;
    getLocalData().afterImage.lifetime = time;
    getLocalData().afterImage.length = length;
    getLocalData().afterImage.translucent = translucent;
    getLocalData().afterImage.frames.clear();
    getLocalData().afterImage.paletteColor = paletteColor;
    getLocalData().afterImage.invertColor = invertColor;
    getLocalData().afterImage.bright = bright;
    getLocalData().afterImage.contrast = contrast;
    getLocalData().afterImage.postBright = postBright;
    getLocalData().afterImage.add = add;
    getLocalData().afterImage.multiply = multiply;
}
        
void Character::setAfterImageTime(int time){
    getLocalData().afterImage.lifetime = time;
}
        
void Character::updateAngleEffect(double angle){
    getStateData().drawAngle = angle;
}

double Character::getAngleEffect() const {
    return getStateData().drawAngle;
}
        
void Character::drawAngleEffect(double angle, bool setAngle, double scaleX, double scaleY){
    if (setAngle){
        getStateData().drawAngle = angle;
    }
    getStateData().drawAngleData.angle = getStateData().drawAngle;
    getStateData().drawAngleData.enabled = true;
    getStateData().drawAngleData.scaleX = scaleX;
    getStateData().drawAngleData.scaleY = scaleY;
}
        
void Character::assertSpecial(Specials special){
    if (special == Invisible){
        this->getStateData().special.invisible = true;
    }
    if (special == Intro){
        this->getStateData().special.intro = true;
    }
    /* FIXME: handle other specials */
}

void Character::enableHit(){
    getHit().alive = true;
}

void Character::disableHit(){
    getHit().alive = false;
}
        
void Character::setWidthOverride(int edgeFront, int edgeBack, int playerFront, int playerBack){
    getStateData().widthOverride.enabled = true;
    getStateData().widthOverride.edgeFront = edgeFront;
    getStateData().widthOverride.edgeBack = edgeBack;
    getStateData().widthOverride.playerFront = playerFront;
    getStateData().widthOverride.playerBack = playerBack;
}
        
/* FIXME: it may be easier to put the foreign animation stuff here rather than keep track of
 * a separate set of foreign data, that way everything will just work, such as hasAnimation()
 * and whatnot.
 */
PaintownUtil::ReferenceCount<Animation> Character::getAnimation(int id) const {
    std::map<int, PaintownUtil::ReferenceCount<Animation> >::const_iterator where = getAnimations().find(id);
    if (where != getAnimations().end()){
        return where->second;
    }
    return PaintownUtil::ReferenceCount<Animation>(NULL);
}
        
void Character::setHitByOverride(int slot, int time, bool standing, bool crouching, bool aerial, const std::vector<AttackType::Attribute> & attributes){
    getStateData().hitByOverride[slot].time = time;
    getStateData().hitByOverride[slot].standing = standing;
    getStateData().hitByOverride[slot].crouching = crouching;
    getStateData().hitByOverride[slot].aerial = aerial;
    getStateData().hitByOverride[slot].attributes = attributes;
}
        
void Character::setDefenseMultiplier(double defense){
    getStateData().defenseMultiplier = defense;
}

void Character::setAttackMultiplier(double attack){
    getStateData().attackMultiplier = attack;
}
        
void Character::doFreeze(){
    getStateData().frozen = true;
}
        
void Character::moveX(double x, bool force){
    if (force || !getStateData().frozen){
	if (getFacing() == FacingLeft){
            getStateData().virtualx -= x;
	} else {
            getStateData().virtualx += x;
	}
    }
}

void Character::moveY(double y, bool force){
    if (force || !getStateData().frozen){
        getStateData().virtualy += y;
    }
}
        
Point Character::getMidPosition() const {
    return getLocalData().midPosition;
}

Point Character::getHeadPosition() const {
    return getLocalData().headPosition;
}

Point Character::getDrawOffset() const {
    return getLocalData().drawOffset;
}

void Character::setMaxHealth(double health){
    getLocalData().max_health = health;
    if (this->getStateData().health > getLocalData().max_health){
        this->getStateData().health = getLocalData().max_health;
    }
}

void Character::setHealth(double health){
    this->getStateData().health = health;
    if (this->getStateData().health < 0){
        this->getStateData().health = 0;
    }
    if (this->getStateData().health > getMaxHealth()){
        this->getStateData().health = getMaxHealth();
    }
}

void Character::setSpritePriority(int priority){
    getStateData().spritePriority = priority;
}
        
int Character::getSpritePriority() const {
    /* FIXME: figure out how long the hitdef's sprite priority should take effect. */
    if (getHitState().shakeTime > 0){
        return getHitState().spritePriority;
    } else {
        return getStateData().spritePriority;
    }
}

/* Replace the sprites from the given animation with sprites from our own sff file */
PaintownUtil::ReferenceCount<Animation> Character::replaceSprites(const PaintownUtil::ReferenceCount<Animation> & animation){
    PaintownUtil::ReferenceCount<Animation> update = PaintownUtil::ReferenceCount<Animation>(new Animation(*animation));
    const vector<Frame*> & frames = update->getFrames();
    for (vector<Frame*>::const_iterator it = frames.begin(); it != frames.end(); it++){
        Frame * frame = *it;
        PaintownUtil::ReferenceCount<Mugen::Sprite> sprite = frame->getSprite();
        PaintownUtil::ReferenceCount<Mugen::Sprite> mySprite = getSprite(sprite->getGroupNumber(), sprite->getImageNumber());
        if (mySprite != NULL){
            frame->setSprite(mySprite);
        }
    }
    return update;
}

void Character::setForeignAnimation(PaintownUtil::ReferenceCount<Animation> animation, int number){
    getLocalData().foreignAnimation = replaceSprites(animation);
    getLocalData().foreignAnimationNumber = number;
}
        
bool Character::isHelper() const {
    return false;
}

double Character::getGroundFriction() const {
    if (getCurrentPhysics() == Mugen::Physics::Stand){
        return getStandingFriction();
    } else if (getCurrentPhysics() == Mugen::Physics::Crouch){
        return getCrouchingFriction();
    }

    return getStandingFriction();
}
        
void Character::setReversalActive(){
    getStateData().reversalActive = true;
}

void Character::setReversalInactive(){
    getStateData().reversalActive = false;
}
        
bool Character::isReversalActive(){
    return getStateData().reversalActive;
}

bool Character::canReverse(Character * enemy){
    if (!isReversalActive()){
        return false;
    }

    const ReversalData & reversal = getReversal();

    if (enemy->getHit().attribute.state == StateType::Crouch &&
        !reversal.crouching){
        return false;
    }
            
    if (enemy->getHit().attribute.state == StateType::Stand &&
        !reversal.standing){
        return false;
    }
            
    if (enemy->getHit().attribute.state == StateType::Air &&
        !reversal.aerial){
        return false;
    }

    AttackType::Attribute hitType = parseAttribute(enemy->getHit().attribute.attackType, enemy->getHit().attribute.physics);
    const vector<AttackType::Attribute> & attributes = getReversal().attributes;
    bool ok = false;

    /* The hit type of the enemy must be in the list of reversal attributes somewhere */
    for (vector<AttackType::Attribute>::const_iterator it = attributes.begin(); it != attributes.end(); it++){
        if (hitType == *it){
            ok = true;
            break;
        }
    }

    return ok;
}

ReversalData & Character::getReversal(){
    return getStateData().reversal;
}
        
double Character::forwardPoint(double x) const {
    if (getFacing() == FacingRight){
        return getX() + x;
    } else {
        return getX() - x;
    }
}
        
void Character::disablePushCheck(){
    getStateData().pushPlayer = 1;
}

bool Character::isPushable(){
    return getStateData().pushPlayer == 0;
}
        
void Character::setHitOverride(int slot, const HitAttributes & attribute, int state, int time, bool air){
    HitOverride & override = getStateData().hitOverrides[slot];
    override.attributes = attribute;
    override.state = state;
    override.time = time;
}

void Character::setDrawOffset(double x, double y){
    getLocalData().drawOffset.set(x, y);
}

PaintownUtil::ReferenceCount<State> Character::getSelfState(int id) const {
    if (getLocalData().states.find(id) != getLocalData().states.end()){
        return getLocalData().states.find(id)->second;
    }
    return PaintownUtil::ReferenceCount<State>(NULL);
}

PaintownUtil::ReferenceCount<State> Character::getState(int id, Stage & stage) const {
    if (getStateData().characterData.enabled &&
        getStateData().characterData.who != CharacterId(-1)){
        Character * guy = stage.getCharacter(getStateData().characterData.who);
        if (guy != NULL){
            return guy->getState(id, stage);
        }
    }

    return getSelfState(id);
}

void Character::setState(int id, PaintownUtil::ReferenceCount<State> what){
    getLocalData().states[id] = what;
}
        
void Character::setPaletteEffects(int time, int addRed, int addGreen, int addBlue, int multiplyRed, int multiplyGreen, int multiplyBlue, int sinRed, int sinGreen, int sinBlue, int period, int invert, int color){
    getLocalData().paletteEffects.time = time;
    getLocalData().paletteEffects.addRed = addRed;
    getLocalData().paletteEffects.addGreen = addGreen;
    getLocalData().paletteEffects.addBlue = addBlue;
    getLocalData().paletteEffects.multiplyRed = multiplyRed;
    getLocalData().paletteEffects.multiplyGreen = multiplyGreen;
    getLocalData().paletteEffects.multiplyBlue = multiplyBlue;
    getLocalData().paletteEffects.sinRed = sinRed;
    getLocalData().paletteEffects.sinGreen = sinGreen;
    getLocalData().paletteEffects.sinBlue = sinBlue;
    getLocalData().paletteEffects.period = period;
    getLocalData().paletteEffects.invert = invert;
    getLocalData().paletteEffects.color = color;
    getLocalData().paletteEffects.counter = 0;
}
        
unsigned int Character::getWasHitCount() const {
    return getStateData().wasHitCounter;
}
        
void Character::roundEnd(Stage & stage){
    /* TODO: reset int and float arrays, use intpersist and floatpersist.
     * maybe kill helpers or projectiles.
     */
    vector<Helper*> helpers = stage.findHelpers(this);
    for (vector<Helper*>::iterator it = helpers.begin(); it != helpers.end(); it++){
        Helper * helper = *it;
        stage.removeHelper(helper);
    }
}

bool Character::isAttacking() const { 
    return getMoveType() == Move::Attack;
}
    
void Character::useCharacterData(const CharacterId & who){
    getStateData().characterData.who = who;
    getStateData().characterData.enabled = true;
}
    
bool Character::compatibleHitFlag(const HitFlags & flags){
    bool ok = false;
    ok = ok || (getStateType() == StateType::Air && flags.air);
    ok = ok || (getStateType() == StateType::Stand && flags.high);
    ok = ok || (getStateType() == StateType::Crouch && flags.low);
    ok = ok || (getStateType() == StateType::LyingDown && flags.down);
    /* FIXME: handle fall */
    if (flags.getHitState){
        ok = ok && (getMoveType() == Move::Hit);
    }
    if (flags.notGetHitState){
        ok = ok && (getMoveType() != Move::Hit);
    }

    return ok;
}
        
std::map<int, vector<CharacterId> > & Character::getTargets(){
    return getStateData().targets;
}

const std::map<int, vector<CharacterId> > & Character::getTargets() const {
    return getStateData().targets;
}
        
bool Character::isAssertIntro(){
    return getStateData().special.intro;
}
    
bool Character::isOwnPalette() const {
    return getLocalData().ownPalette;
}

void Character::setOwnPalette(bool what){
    getLocalData().ownPalette = what;
}
        
double Character::getAirHitRecoverMultiplierX() const {
    return getLocalData().velocity_air_gethit_airrecover_mul_x;
}

double Character::getAirHitRecoverMultiplierY() const {
    return getLocalData().velocity_air_gethit_airrecover_mul_y;
}
        
void Character::setAirHitRecoverMultiplierX(double x){
    getLocalData().velocity_air_gethit_airrecover_mul_x = x;
}

void Character::setAirHitRecoverMultiplierY(double y){
    getLocalData().velocity_air_gethit_airrecover_mul_y = y;
}
        
double Character::getAirHitGroundRecoverX() const {
    return getLocalData().velocity_air_gethit_groundrecover_x;
}

double Character::getAirHitGroundRecoverY() const {
    return getLocalData().velocity_air_gethit_groundrecover_y;
}
        
void Character::setAirHitGroundRecoverX(double x){
    getLocalData().velocity_air_gethit_groundrecover_x = x;
}

void Character::setAirHitGroundRecoverY(double y){
    getLocalData().velocity_air_gethit_groundrecover_y = y;
}

double Character::getAirHitRecoverAddX() const {
    return getLocalData().velocity_air_gethit_recover_add_x;
}

double Character::getAirHitRecoverAddY() const {
    return getLocalData().velocity_air_gethit_recover_add_y;
}

void Character::setAirHitRecoverAddX(double x){
    getLocalData().velocity_air_gethit_recover_add_x = x;
}

void Character::setAirHitRecoverAddY(double y){
    getLocalData().velocity_air_gethit_recover_add_y = y;
}

double Character::getAirHitRecoverUp() const {
    return getLocalData().velocity_air_gethit_recover_up;
}

void Character::setAirHitRecoverUp(double up){
    getLocalData().velocity_air_gethit_recover_up = up;
}

double Character::getAirHitRecoverDown() const {
    return getLocalData().velocity_air_gethit_recover_down;
}

void Character::setAirHitRecoverDown(double down){
    getLocalData().velocity_air_gethit_recover_down = down;
}
 
double Character::getAirHitRecoverForward() const {
    return getLocalData().velocity_air_gethit_recover_forward;
}

void Character::setAirHitRecoverForward(double forward){
    getLocalData().velocity_air_gethit_recover_forward = forward;
}

double Character::getAirHitRecoverBack() const {
    return getLocalData().velocity_air_gethit_recover_back;
}

void Character::setAirHitRecoverBack(double back){
    getLocalData().velocity_air_gethit_recover_back = back;
}
        
double Character::getAirHitRecoverYAccel() const {
    return getLocalData().air_gethit_recover_yaccel;
}

void Character::setAirHitRecoverYAccel(double yaccel){
    getLocalData().air_gethit_recover_yaccel = yaccel;
}

double Character::getXScale() const {
    return getLocalData().xscale;
}

double Character::getYScale() const {
    return getLocalData().yscale;
}

int Character::getGroundBack() const {
    return getLocalData().groundback;
}

int Character::getGroundFront() const {
    return getLocalData().groundfront;
}

int Character::getAirBack() const {
    return getLocalData().airback;
}

int Character::getAirFront() const {
    return getLocalData().airfront;
}

void Character::setStateData(const StateData & data){
    this->stateData = data;
    const std::map<std::string, std::string > & serializedCommands = data.commandState;;

    std::map<std::string, Command2*> commandMap;
    const vector<Command2*> & commands = getCommands();
    for (vector<Command2*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        Command2 * command = *it;
        commandMap[command->getName()] = command;
    }

    for (map<string, string>::const_iterator it = serializedCommands.begin(); it != serializedCommands.end(); it++){
        string name = it->first;
        string value = it->second;

        Command2 * command = commandMap[name];
        if (command != NULL){
            TokenReader reader;
            command->deserialize(reader.readTokenFromString(value));
        }
    }
}

Character::LocalData::LocalData(){
#define Z(x) x = 0
    /* TODO: add all the variables here */
    Z(projdoscale);
    
    // Z(pushPlayer);

    Z(velocity_air_gethit_airrecover_mul_x);
    Z(velocity_air_gethit_airrecover_mul_y);
    Z(velocity_air_gethit_groundrecover_x);
    Z(velocity_air_gethit_groundrecover_y);
    Z(velocity_air_gethit_recover_add_x);
    Z(velocity_air_gethit_recover_add_y);
    Z(velocity_air_gethit_recover_up);
    Z(velocity_air_gethit_recover_down);
    Z(velocity_air_gethit_recover_forward);
    Z(velocity_air_gethit_recover_back);
    Z(air_gethit_recover_yaccel);
#undef Z
}

/* FIXME: need to copy more attributes */
Character::LocalData::LocalData(const Character::LocalData & copy){
    /* simple macro to copy fields */
#define C(field) field = copy.field
    C(commonSounds);
    debug = false;
    // frozen = false;
    // pushPlayer = 0;
    maxChangeStates = 0;
    C(xscale);
    C(yscale);
    // C(currentState);
    // C(previousState);
    // C(currentAnimation);
    // C(velocity_x);
    // C(velocity_y);
    // C(currentPhysics);
    // C(lastTicket);
    C(regenerateHealth);
    C(regenerating);
    C(regenerateTime);
    C(max_health);
    // C(health);
    // C(spritePriority);
    C(location);
    C(baseDir);
    C(name);
    C(displayName);
    C(versionDate);
    C(mugenVersion);
    C(author);
    C(palDefaults);
    C(currentPalette);
    C(cmdFile);
    C(ownPalette);
    C(constantsFile);
    C(sffFile);
    C(airFile);
    C(sndFile);
    C(palFile);
    C(introFile);
    C(endingFile);
    C(attack);
    C(defense);
    C(fallDefenseUp);
    C(lieDownTime);
    C(airjuggle);
    // C(juggleRemaining);
    // C(currentJuggle);
    C(spark);
    C(guardSpark);
    C(koecho);
    C(volumeoffset);
    C(intpersistindex);
    C(floatpersistindex);
    C(xscale);
    C(yscale);
    C(groundback);
    C(groundfront);
    C(airback);
    C(airfront);
    C(height);
    C(attackdist);
    C(projattackdist);
    C(projdoscale);
    C(headPosition);
    C(midPosition);
    C(shadowoffset);
    C(drawOffset);
    C(walkfwd);
    C(walkback);
    C(runforwardx);
    C(runforwardy);
    C(runbackx);
    C(runbacky);
    C(jumpneux);
    C(jumpneuy);
    C(jumpback);
    C(jumpfwd);
    C(runjumpback);
    C(runjumpfwd);
    C(airjumpneux);
    C(airjumpneuy);
    C(airjumpback);
    C(airjumpfwd);
    C(airjumpnum);
    C(airjumpheight);
    C(yaccel);
    C(crouchFriction);
    C(crouchFrictionThreshold);
    C(constants);
    /*
    C(currentState);
    C(previousState);
    C(currentAnimation);
    */
    C(debug);
    // C(velocity_x);
    // C(velocity_y);
    // C(has_control);
    // C(stateTime);
    // C(variables);
    // C(floatVariables);
    // C(systemVariables);
    // C(currentPhysics);
    C(gravity);
    C(standFriction);
    C(standFrictionThreshold);
    // C(stateType);
    // C(moveType);
    // lastTicket = 0;
    // combo = 0;
    // hitCount = 0;
    // blocking = false;
    // C(drawAngle);
    C(wins);
    C(matchWins);
    C(regenerateHealth);
    C(regenerating);
    C(regenerateTime);
    C(regenerateHealthDifference);
    // guarding = false;
    C(afterImage);
    // C(widthOverride);
    // C(hitByOverride);
    // C(defenseMultiplier);
    // C(attackMultiplier);
    // C(frozen);
    // C(reversal);
    // C(reversalActive);
    // C(transOverride);
    // C(special);
    C(paletteEffects);
    C(max_health);
    // C(health);
    // C(targets);
    // C(spritePriority);
    // wasHitCounter = 0;
    C(jumpChangeAnimationThreshold);
    C(airGetHitGroundLevel);
    C(velocity_air_gethit_airrecover_mul_x);
    C(velocity_air_gethit_airrecover_mul_y);
    C(velocity_air_gethit_groundrecover_x);
    C(velocity_air_gethit_groundrecover_y);
    C(velocity_air_gethit_recover_add_x);
    C(velocity_air_gethit_recover_add_y);
    C(velocity_air_gethit_recover_up);
    C(velocity_air_gethit_recover_down);
    C(velocity_air_gethit_recover_forward);
    C(velocity_air_gethit_recover_back);
    C(air_gethit_recover_yaccel);
#undef C
}

}
