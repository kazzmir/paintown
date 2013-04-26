#include "util/graphics/bitmap.h"
#include "animation.h"
#include "state.h"

#include "globals.h"
#include <string>
#include <vector>

#include "sprite.h"
#include "util.h"
#include "exception.h"
#include <sstream>

using namespace std;

namespace Mugen{

static void renderCollision( const std::vector< Area > &vec, const Graphics::Bitmap &bmp, int x, int y, Graphics::Color color ){
    for( unsigned int i = 0; i < vec.size(); ++i ){
	bmp.rectangle( x + vec[i].x1, y + vec[i].y1, x + vec[i].x2, y + vec[i].y2, color );
    }
}

/// area
Area::Area():
x1(0),
y1(0),
x2(0),
y2(0){ 
}

Area::Area( const Area &copy ){
    this->x1 = copy.x1;
    this->y1 = copy.y1;
    this->x2 = copy.x2;
    this->y2 = copy.y2;
}

Area & Area::operator=( const Area &copy ){
    this->x1 = copy.x1;
    this->y1 = copy.y1;
    this->x2 = copy.x2;
    this->y2 = copy.y2;
    
    return *this;
}

Area::~Area(){
}

bool Area::collision(int mx, int my, const Area & area, int ax, int ay) const {

    const int x1 = this->x1 + mx;
    const int x2 = this->x2 + mx;
    const int y1 = this->y1 + my;
    const int y2 = this->y2 + my;

    const int x3 = area.x1 + ax;
    const int x4 = area.x2 + ax;
    const int y3 = area.y1 + ay;
    const int y4 = area.y2 + ay;

    if (x1 < x3 && x1 < x4 &&
        x2 < x3 && x2 < x4) return false;
    if (x1 > x3 && x1 > x4 &&
        x2 > x3 && x2 > x4) return false;
    if (y1 < y3 && y1 < y4 &&
        y2 < y3 && y2 < y4) return false;
    if (y1 > y3 && y1 > y4 &&
        y2 > y3 && y2 > y4) return false;

    return true;
}

/*
Frame
*/
Frame::Frame(bool mask):
loopstart(false),
sprite(PaintownUtil::ReferenceCount<Mugen::Sprite>(NULL)),
xoffset(0),
yoffset(0),
time(0){
    effects.mask = mask;
}

Frame::Frame(const PaintownUtil::ReferenceCount<Mugen::Sprite> & sprite, bool mask):
loopstart(false),
sprite(sprite),
xoffset(0),
yoffset(0),
time(0){
    effects.mask = mask;
}

Frame::Frame(const Frame &copy){
    this->loopstart = copy.loopstart;
    this->sprite = copy.sprite;
    this->xoffset = copy.xoffset;
    this->yoffset = copy.yoffset;
    this->time = copy.time;
    this->effects = copy.effects;
    this->defenseCollision = copy.defenseCollision;
    this->attackCollision = copy.attackCollision;
}

Frame & Frame::operator=(const Frame &copy){
    this->loopstart = copy.loopstart;
    this->sprite = copy.sprite;
    this->xoffset = copy.xoffset;
    this->yoffset = copy.yoffset;
    this->time = copy.time;
    this->effects = copy.effects;
    this->defenseCollision = copy.defenseCollision;
    this->attackCollision = copy.attackCollision;
    
    return *this;
}

void Frame::render(int x, int y, const Graphics::Bitmap & work, const Mugen::Effects & effects) const {
    if (sprite != NULL){
        /* Only flip the X offset, not Y */
        const int placex = x + (xoffset * (effects.facing ? -1 : 1) * effects.scalex);
        const int placey = y + (yoffset * effects.scaley);
        sprite->render(placex, placey, work, effects);
    }
}
        
void Frame::setSprite(PaintownUtil::ReferenceCount<Mugen::Sprite> sprite){
    this->sprite = sprite;
}

Frame::~Frame(){
    /* the sprite is not deleted because it should be referenced from some
     * Mugen::SpriteMap.
     */
}

/*
Holds mugen animations, ie: player.air
*/
Animation::Animation():
loopPosition(0),
playOnce(false),
type(Mugen::Unknown),
showDefense(false),
showOffense(false){
}
        
Animation::Animation(const PaintownUtil::ReferenceCount<Sprite> & sprite, bool mask):
loopPosition(0),
playOnce(false),
type(Mugen::Unknown),
showDefense(false),
showOffense(false){
    frames.push_back(new Frame(sprite, mask));
}

Animation::Animation(const Animation &copy):
playOnce(copy.playOnce),
state(copy.state){
    this->loopPosition = copy.loopPosition;
    this->getState().position = 0;
    this->type = copy.type;
    this->showDefense = copy.showDefense;
    this->showOffense = copy.showOffense;
    for (vector<Frame*>::const_iterator it = copy.frames.begin(); it != copy.frames.end(); it++){
        Frame * old = *it;
        this->frames.push_back(new Frame(*old));
    }
    this->getState().ticks = 0;
    this->getState().virtual_ticks = 0;
    this->getState().started = false;
}

Animation::~Animation(){
    for (std::vector< Frame * >::iterator i = frames.begin() ; i != frames.end() ; ++i){
	if (*i){
            delete (*i);
        }
    }
}

void Animation::addFrame(Frame *frame){
    if (frame->loopstart){
	loopPosition = frames.size();
    }
    frames.push_back(frame);
}

const Frame * Animation::getNext(){
    if (getState().position < frames.size() - 1){
        getState().position += 1;
    } else {
        getState().position = loopPosition;
    }
    
    return frames[getState().position];
}

/* time elapsed since a given element has been shown. negative if the
 * element hasn't been shown yet, 0 if its being shown right now
 *
 * position should be between 1 and frames.size() + 1, i.e. the index
 * starts at 1 instead of 0.
 */
int Animation::animationElementElapsed(int position) const {
    int total = 0;
    if (position < 1 || position > (int) frames.size() + 1){
        ostringstream out;
        out << "Invalid animation position: " << position << ". Position must be in the range 1 to " << (frames.size() + 1);
        throw MugenNormalRuntimeException(out.str());
    }

    for (int from = (int) this->getState().position; from < position - 1; from++){
        total -= frames[from]->time;
    }

    if (position - 1 == (int) this->getState().position){
        total += getState().ticks;
        total += getState().virtual_ticks;
    }

    for (int from = position - 1; from < (int) this->getState().position; from++){
        total += frames[from]->time;
    }

    return total;
}

/* time left in the animation */
int Animation::animationTime() const {
    // return (int) position - (int) frames.size() + 1;
    if (frames[getState().position]->time == -1){
        return -1;
    }

    int left = frames[getState().position]->time - getState().ticks - 1;
    /* FIXME: might need to add virtual_ticks here for loop time */
    for (unsigned int rest = getState().position + 1; rest < frames.size(); rest++){
        if (frames[rest]->time == -1){
            return -1;
        }
        left += frames[rest]->time;
    }
    // Global::debug(0) << "Animation time " << left << endl;
    return left;
}

/* reverses through the y-axis (just the x coordinates */
static Area reverseBox(const Area & area){
    Area reversed(area);
    reversed.x1 = -reversed.x1;
    reversed.x2 = -reversed.x2;
    return reversed;
}

static Area scaleBox(const Area & area, double x, double y){
    Area scaled(area);
    scaled.x1 *= x;
    scaled.x2 *= x;
    scaled.y1 *= y;
    scaled.y2 *= y;
    return scaled;
}

static vector<Area> reverseBoxes(const vector<Area> & boxes){
    vector<Area> out;
    for (vector<Area>::const_iterator it = boxes.begin(); it != boxes.end(); it++){
        out.push_back(reverseBox(*it));
    }

    return out;
}

static vector<Area> scaleBoxes(const vector<Area> & boxes, double x, double y){
    vector<Area> out;
    for (vector<Area>::const_iterator it = boxes.begin(); it != boxes.end(); it++){
        out.push_back(scaleBox(*it, x, y));
    }

    return out;
}


const std::vector<Area> Animation::getDefenseBoxes(bool reverse, double xscale, double yscale) const {
    Frame * frame = frames[getState().position];
    if (reverse){
        return scaleBoxes(reverseBoxes(frame->getDefenseBoxes()), xscale, yscale);
    }
    return scaleBoxes(frame->getDefenseBoxes(), xscale, yscale);
}

const std::vector<Area> Animation::getAttackBoxes(bool reverse, double xscale, double yscale) const {
    Frame * frame = frames[getState().position];
    if (reverse){
        return scaleBoxes(reverseBoxes(frame->getAttackBoxes()), xscale, yscale);
    }
    return scaleBoxes(frame->getAttackBoxes(), xscale, yscale);
}
        
void Animation::virtualTick(){
    getState().virtual_ticks += 1;
}

void Animation::logic(){
    if (getState().position < frames.size()){
        if (frames[getState().position]->time != -1){
            getState().ticks += 1;
            if (getState().ticks >= frames[getState().position]->time){
                getState().ticks = 0;
                getState().virtual_ticks = 0;
                if (getState().position < frames.size() - 1){
                    getState().position += 1;
                } else {
		    if (!playOnce){
			getState().position = loopPosition;
                        getState().looped = true;
		    }
                }
            }
        } else {
            getState().virtual_ticks += 1;
        }
    }
}
        
void Animation::setPosition(int position){
    if (position < (int) frames.size() && position >= 0){
        this->getState().position = position;
        this->getState().ticks = 0;
        this->getState().virtual_ticks = 0;
    }
}

void Animation::reset(){ 
    this->getState().position = 0; 
    if (this->playOnce){
        this->getState().started = false;
    }
    this->getState().looped = false;
    this->getState().ticks = 0;
    this->getState().virtual_ticks = 0;
}

void Animation::renderFrame(Frame * frame, int xaxis, int yaxis, const Graphics::Bitmap & work, const Mugen::Effects & effects){
    
    frame->render(xaxis, yaxis, work, effects);

    if (showDefense){
        renderCollision(getDefenseBoxes(effects.facing, effects.scalex, effects.scaley), work, xaxis, yaxis, Graphics::makeColor(0, 255, 0));
    }

    if (showOffense){
        renderCollision(getAttackBoxes(effects.facing, effects.scalex, effects.scaley), work, xaxis, yaxis, Graphics::makeColor(255,0,0 ));
    }
}

void Animation::render(int xaxis, int yaxis, const Graphics::Bitmap & work, const Mugen::Effects & effects){
    if (getState().position >= frames.size()){
        return;
    }

    Mugen::Effects combined = frames[getState().position]->effects + effects;
    renderFrame(frames[getState().position], xaxis, yaxis, work, combined);
}
        
Animation * Animation::copy() const {
    return new Animation(*this);
}

void Animation::render(int xaxis, int yaxis, const Graphics::Bitmap &work, double scalex, double scaley){
    if (getState().position >= frames.size()){
        return;
    }

    Mugen::Effects effects = frames[getState().position]->effects;
    effects.scalex = scalex;
    effects.scaley = scaley;

    renderFrame(frames[getState().position], xaxis, yaxis, work, effects);

#if 0
    // Modify with frame adjustment
    const int placex = xaxis+frames[position]->xoffset;
    const int placey = yaxis+frames[position]->yoffset;
    try{
        frames[position]->sprite->render(placex,placey,work,frames[position]->effects);
        
        if (showDefense){
            renderCollision( frames[position]->defenseCollision, work, xaxis, yaxis, Bitmap::makeColor( 0,255,0 ) );
        }

        if (showOffense){
            renderCollision( frames[position]->attackCollision, work, xaxis, yaxis,  Bitmap::makeColor( 255,0,0 ) );
        }
    } catch (const LoadException & e){
        Global::debug(0) << "Error loading sprite: " << e.getReason() << endl;
        /* FIXME: do something sensible here */
        frames[position]->sprite = 0;
    }
#endif
}

Mugen::Effects Animation::getCurrentEffects(bool facing, bool vfacing, double scalex, double scaley){
    Frame * frame = frames[getState().position];
    Mugen::Effects effects = frame->effects;
    effects.scalex = scalex;
    effects.scaley = scaley;
    effects.facing = facing;
    effects.vfacing = vfacing;
    return effects;
}

void Animation::render(bool facing, bool vfacing, const int xaxis, const int yaxis, const Graphics::Bitmap &work, const double scalex, const double scaley, Graphics::Bitmap::Filter * filter){
    if (getState().position >= frames.size()){
        return;
    }

    Frame * frame = frames[getState().position];
    Mugen::Effects effects = frame->effects;
    effects.scalex = scalex;
    effects.scaley = scaley;
    /* FIXME: should horizontal facing follow the same logic as vfacing below? */
    effects.facing = facing;

    /* case 1: explode vfacing is true and frame is normal. vfacing = -1
     * case 2: explode vfacing is true and frame is flipped. vfacing = 1
     * case 3: explode vfacing is false and frame is normal. vfacing = 1
     * case 4: explode vfacing is false and frame is flipped. vfacing = -1
     */
    /* TODO: verify case 2 */
    if (!effects.vfacing && vfacing){
        effects.vfacing = true;
    } else if (effects.vfacing && vfacing){
        effects.vfacing = false;
    } else if (!effects.vfacing && !vfacing){
        effects.vfacing = false;
    } else {
        effects.vfacing = true;
    }
    effects.filter = filter;

    renderFrame(frame, xaxis, yaxis, work, effects);
}

void Animation::renderReflection(bool facing, bool vfacing, int alpha, const int xaxis, const int yaxis, const Graphics::Bitmap &work, const double scalex, const double scaley){
    if (getState().position >= frames.size()){
        return;
    }

    Frame * frame = frames[getState().position];
    Mugen::Effects effects = frame->effects;
    effects.facing = facing;
    effects.vfacing = vfacing;
    effects.trans = Mugen::Translucent;
    effects.alphaSource = alpha;
    effects.scalex = scalex;
    effects.scaley = scaley;

    frame->render(xaxis, yaxis, work, effects);
}

void Animation::forwardFrame(){
    if (getState().position < frames.size() -1){
        getState().position++;
    } else {
        getState().position = loopPosition;
    }
}

void Animation::backFrame(){
    if (getState().position > loopPosition){
        getState().position--;
    } else {
        getState().position = frames.size() - 1;
    }
}
	
Frame *Animation::getCurrentFrame(){
    return frames[getPosition()];
}
        
const AnimationState & Animation::getState() const {
    return state;
}

AnimationState & Animation::getState() {
    return state;
}
        
void Animation::setState(const AnimationState & state){
    this->state = state;
}

/* who uses this function? */
/*
void Animation::reloadBitmaps(){
    for( std::vector< Frame * >::iterator i = frames.begin() ; i != frames.end() ; ++i ){
	Frame *frame = *i;
	if (frame->sprite != NULL){
	   // if (frame->bmp) delete frame->bmp;
	    //frame->bmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) frame->sprite->pcx, frame->sprite->newlength));
	    frame->sprite->reload();
	}
    }
}
*/

// Get name of type of animation
const std::string Animation::getName(const Mugen::AnimationType t){
    switch( t ){
        case Mugen::Standing : return "Standing";break;
	case Mugen::StandTurning : return "Stand turning";break;
	case Mugen::CrouchTurning : return "Crouch turning";break;
	case Mugen::StandToCrouch : return "Stand to crouch";break;
	case Mugen::Crouching : return "Crouching";break;
	case Mugen::CrouchToStand : return "Crouch to stand";break;
	case Mugen::WalkingForwards : return "Walking forwards";break;
	case Mugen::WalkingBackwards : return "Walking backwards";break;
	case Mugen::JumpStart : return "Jump start (on ground)";break;
	case Mugen::JumpNeutralUp : return "Jump neutral (upwards)";break;
	case Mugen::JumpForwardsUp : return "Jump forwards (upwards)";break;
	case Mugen::JumpBackUp : return "Jump back (upwards)";break;
	case Mugen::JumpNeutralDown : return "Jump neutral (downwards)";break;
	case Mugen::AirJumpStart : return "AirJumpStart";break;
	case Mugen::JumpBackDown : return "Jump back (downwards)";break;
	case Mugen::JumpLanding : return "Jump landing";break;
	case Mugen::RunForwards : return "Run fwd/hop forward";break;
	case Mugen::HopBack : return "Hop back";break;
	case Mugen::StartGuardStand : return "Start guarding (stand)";break;
	case Mugen::StartGuardCrouch : return "Start guarding (crouch)";break;
	case Mugen::StartGuardAir : return "Start guarding (air)";break;
	case Mugen::GuardStand : return "Guard (stand)";break;
	case Mugen::GuardCrouch : return "Guard (crouch)";break;
	case Mugen::GuardAir : return "Guard (air)";break;
	case Mugen::StopGuardStand : return "Stop guarding (stand)";break;
	case Mugen::StopGuardCrouch : return "Stop guarding (crouch)";break;
	case Mugen::StopGuardAir : return "Stop guarding (air)";break;
	case Mugen::GuardHitStand : return "Guarding a hit (stand)";break;
	case Mugen::GuardHitCrouch : return "Guarding a hit (crouch)";break;
	case Mugen::GuardHitAir : return "Guarding a hit (air)";break;
	case Mugen::Lose : return "opt   Lose";break;
	case Mugen::Draw : return "opt   Time Over drawgame";break;
	case Mugen::Win : return "opt   Win";break;
	case Mugen::Intro : return "opt   Intro";break;
	case Mugen::Taunt : return "opt   Taunt";break;
	case Mugen::HitHighLight : return "Stand/Air Hit high (light)";break;
	case Mugen::HitHighMedium : return "Stand/Air Hit high (medium)";break;
	case Mugen::HitHighHard : return "Stand/Air Hit high (hard)";break;
	case Mugen::StandRecoverHighLight : return "Stand Recover high (light)";break;
	case Mugen::StandRecoverHighMedium : return "Stand Recover high (medium)";break;
	case Mugen::StandRecoverHighHard : return "Stand Recover high (hard)";break;
	case Mugen::HitLowLight : return "Stand/Air Hit low (light)";break;
	case Mugen::HitLowMedium : return "Stand/Air Hit low (medium)";break;
	case Mugen::HitLowHard : return "Stand/Air Hit low (hard)";break;
	case Mugen::StandRecoverLowLight : return "Stand Recover low (light)";break;
	case Mugen::StandRecoverLowMedium : return "Stand Recover low (medium)";break;
	case Mugen::StandRecoverLowHard : return "Stand Recover low (hard)";break;
	case Mugen::CrouchHitLight : return "Crouch Hit (light)";break;
	case Mugen::CrouchHitMedium : return "Crouch Hit (medium)";break;
	case Mugen::CrouchHitHard : return "Crouch Hit (hard)";break;
	case Mugen::CrouchRecoverLight : return "Crouch Recover (light)";break;
	case Mugen::CrouchRecoverMedium : return "Crouch Recover (medium)";break;
	case Mugen::CrouchRecoverHard : return "Crouch Recover (hard)";break;
	case Mugen::HitBack : return "Stand/Air Hit back";break;
	case Mugen::HitTransition : return "opt  Stand/Air Hit transition";break;
	case Mugen::AirRecover : return "Air Recover";break;
	case Mugen::AirFall : return "Air Fall";break;
	case Mugen::AirFallComingDown : return "opt  Air Fall (coming down)";break;
	case Mugen::Tripped : return "Tripped";break;
	case Mugen::LiedownHitStaydown : return "LieDown Hit (stay down)";break;
	case Mugen::LiedownHitIntoair : return "LieDown Hit (hit up into air)";break;
	case Mugen::HitGroundFromFall : return "Hitting ground from fall";break;
	case Mugen::BounceIntoAir : return "Bounce into air";break;
	case Mugen::HitGroundFromBounce : return "Hit ground from bounce";break;
	case Mugen::Liedown : return "LieDown";break;
	case Mugen::GetUpFromLiedown : return "Get up from LieDown";break;
	case Mugen::LiedeadFirstRounds : return "opt  LieDead (first rounds)";break;
	case Mugen::LiedeadFinalRound : return "opt  LieDead (final round)";break;
	case Mugen::FallRecoveryNearGround : return "Fall-recovery near ground";break;
	case Mugen::FallRecoveryMidair : return "Fall-recovery in mid-air";break;
	case Mugen::Dizzy : return "Dizzy";break;
	case Mugen::Continue : return "opt  \"Continue?\" screen";break;
	case Mugen::YesContinue : return "opt  \"Yes\" to \"Continue\"";break;
	case Mugen::NoContinue : return "opt  \"No\" to \"Continue\"";break;
	case Mugen::AirFallHitUp : return "opt  Air fall -- hit up";break;
	case Mugen::FallFromHitUp : return "opt  Coming down from hit up";break;
	case Mugen::LiedownHitStaydown2 : return "opt  LieDown Hit (stay down)";break;
	case Mugen::BounceFromGroundIntoAir : return "opt  Bounce from ground into air";break;
	case Mugen::BounceIntoAir2 : return "opt  Bounce into air";break;
	case Mugen::HitGroundFromBounce2 : return "opt  Hit ground from bounce";break;
	case Mugen::Liedown2 : return "opt  LieDown";break;
	case Mugen::GetUpFromLiedown2 : return "opt  Get up from LieDown";break;
	case Mugen::LiedeadFirstRounds2 : return "opt  LieDead (first rounds)";break;
	case Mugen::LiedeadFinalRound2 : return "opt  LieDead (final round)";break;
	case Mugen::AirFallHitUp2 : return "opt  Air fall -- hit up";break;
	case Mugen::FallFromHitUp2 : return "opt  Coming down from hit up";break;
	case Mugen::LiedownHitStaydown3 : return "opt  LieDown Hit (stay down)";break;
	case Mugen::BounceFromGroundIntoAir2 : return "opt  Bounce from ground into air";break;
	case Mugen::BounceIntoAir3 : return "opt  Bounce into air";break;
	case Mugen::HitGroundFromBounce3 : return "opt  Hit ground from bounce";break;
	case Mugen::Liedown3 : return "opt  LieDown";break;
	case Mugen::GetUpFromLiedown3 : return "opt  Get up from LieDown";break;
	case Mugen::LiedeadFirstRounds3 : return "opt  LieDead (first rounds)";break;
	case Mugen::LiedeadFinalRound3 : return "opt  LieDead (final round)";break;
	case Mugen::Unknown :
	default: return "Custom or Optional Animation";break;
    }
}

}
