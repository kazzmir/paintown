#include "util/bitmap.h"
#include "animation.h"
#include "state.h"

#include "globals.h"
#include <string>
#include <vector>

#include "sprite.h"
#include "util.h"
#include "exception.h"
#include "util/load_exception.h"
#include <sstream>

using namespace std;

static void renderCollision( const std::vector< MugenArea > &vec, const Graphics::Bitmap &bmp, int x, int y, Graphics::Color color ){
    for( unsigned int i = 0; i < vec.size(); ++i ){
	bmp.rectangle( x + vec[i].x1, y + vec[i].y1, x + vec[i].x2, y + vec[i].y2, color );
    }
}

/// area
MugenArea::MugenArea():
x1(0),
y1(0),
x2(0),
y2(0){ 
}

MugenArea::MugenArea( const MugenArea &copy ){
    this->x1 = copy.x1;
    this->y1 = copy.y1;
    this->x2 = copy.x2;
    this->y2 = copy.y2;
}

MugenArea & MugenArea::operator=( const MugenArea &copy ){
    this->x1 = copy.x1;
    this->y1 = copy.y1;
    this->x2 = copy.x2;
    this->y2 = copy.y2;
    
    return *this;
}

MugenArea::~MugenArea(){
}

bool MugenArea::collision(int mx, int my, const MugenArea & area, int ax, int ay) const {

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
MugenFrame::MugenFrame(bool mask):
loopstart(false),
sprite(0),
xoffset(0),
yoffset(0),
time(0){
    effects.mask = mask;
}

MugenFrame::MugenFrame(const MugenFrame &copy){
    this->loopstart = copy.loopstart;
    this->sprite = copy.sprite;
    this->xoffset = copy.xoffset;
    this->yoffset = copy.yoffset;
    this->time = 0;
    this->effects = copy.effects;
    this->defenseCollision = copy.defenseCollision;
    this->attackCollision = copy.attackCollision;
}

MugenFrame & MugenFrame::operator=(const MugenFrame &copy){
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

void MugenFrame::render(int x, int y, const Graphics::Bitmap & work, const Mugen::Effects & effects){
    if (sprite != NULL){
        const int placex = x + xoffset;
        const int placey = y + yoffset;
        sprite->render(placex, placey, work, effects);
    }
}

MugenFrame::~MugenFrame(){
    /* the sprite is not deleted because it should be referenced from some
     * Mugen::SpriteMap.
     */
}

/*
Holds mugen animations, ie: player.air
*/
MugenAnimation::MugenAnimation():
loopPosition(0),
position(0),
playOnce(false),
looped(false),
started(false),
type(Mugen::Unknown),
showDefense(false),
showOffense(false),
ticks(0),
virtual_ticks(0){
}

MugenAnimation::MugenAnimation(const MugenAnimation &copy):
playOnce(copy.playOnce),
looped(false){
    this->loopPosition = copy.loopPosition;
    this->position = 0;
    this->type = copy.type;
    this->showDefense = copy.showDefense;
    this->showOffense = copy.showOffense;
    for (vector<MugenFrame*>::const_iterator it = copy.frames.begin(); it != copy.frames.end(); it++){
        MugenFrame * old = *it;
        this->frames.push_back(new MugenFrame(*old));
    }
    this->ticks = 0;
    this->virtual_ticks = 0;
    this->started = false;
}

MugenAnimation::~MugenAnimation(){
    for (std::vector< MugenFrame * >::iterator i = frames.begin() ; i != frames.end() ; ++i){
	if (*i){
            delete (*i);
        }
    }
}

void MugenAnimation::addFrame( MugenFrame *frame ){
    if (frame->loopstart){
	loopPosition = frames.size();
    }
    frames.push_back(frame);
}

const MugenFrame * MugenAnimation::getNext(){
    if (position < frames.size() - 1){
        position += 1;
    } else {
        position = loopPosition;
    }
    
    return frames[position];
}

/* time elapsed since a given element has been shown. negative if the
 * element hasn't been shown yet, 0 if its being shown right now
 *
 * position should be between 1 and frames.size() + 1, i.e. the index
 * starts at 1 instead of 0.
 */
int MugenAnimation::animationElementElapsed(int position) const {
    int total = 0;
    if (position < 1 || position > (int) frames.size() + 1){
        ostringstream out;
        out << "Invalid animation position: " << position << ". Position must be in the range 1 to " << (frames.size() + 1);
        throw MugenNormalRuntimeException(out.str());
    }

    for (int from = (int) this->position; from < position - 1; from++){
        total -= frames[from]->time;
    }

    if (position - 1 == (int) this->position){
        total += ticks;
        total += virtual_ticks;
    }

    for (int from = position - 1; from < (int) this->position; from++){
        total += frames[from]->time;
    }

    return total;
}

/* time left in the animation */
int MugenAnimation::animationTime() const {
    // return (int) position - (int) frames.size() + 1;
    if (frames[position]->time == -1){
        return -1;
    }

    int left = frames[position]->time - ticks - 1;
    /* FIXME: might need to add virtual_ticks here for loop time */
    for (unsigned int rest = position + 1; rest < frames.size(); rest++){
        if (frames[rest]->time == -1){
            return -1;
        }
        left += frames[rest]->time;
    }
    // Global::debug(0) << "Animation time " << left << endl;
    return left;
}

/* reverses through the y-axis (just the x coordinates */
static MugenArea reverseBox(const MugenArea & area){
    MugenArea reversed(area);
    reversed.x1 = -reversed.x1;
    reversed.x2 = -reversed.x2;
    return reversed;
}

static vector<MugenArea> reverseBoxes(const vector<MugenArea> & boxes){
    vector<MugenArea> out;
    for (vector<MugenArea>::const_iterator it = boxes.begin(); it != boxes.end(); it++){
        out.push_back(reverseBox(*it));
    }

    return out;
}

const std::vector<MugenArea> MugenAnimation::getDefenseBoxes(bool reverse) const {
    MugenFrame * frame = frames[position];
    if (reverse){
        return reverseBoxes(frame->getDefenseBoxes());
    }
    return frame->getDefenseBoxes();
}

const std::vector<MugenArea> MugenAnimation::getAttackBoxes(bool reverse) const {
    MugenFrame * frame = frames[position];
    if (reverse){
        return reverseBoxes(frame->getAttackBoxes());
    }
    return frame->getAttackBoxes();
}

void MugenAnimation::logic(){
    if (position < frames.size()){
        if (frames[position]->time != -1){
            ticks += 1;
            if (ticks >= frames[position]->time){
                ticks = 0;
                virtual_ticks = 0;
                if (position < frames.size() - 1){
                    position += 1;
                } else {
		    if (!playOnce){
			position = loopPosition;
                        looped = true;
		    }
                }
            }
        } else {
            virtual_ticks += 1;
        }
    }
}

void MugenAnimation::renderFrame(MugenFrame * frame, int xaxis, int yaxis, const Graphics::Bitmap & work, const Mugen::Effects & effects){
    
    frame->render(xaxis, yaxis, work, effects);

    if (showDefense){
        renderCollision(getDefenseBoxes(effects.facing == -1), work, xaxis, yaxis, Graphics::makeColor(0, 255, 0));
    }

    if (showOffense){
        renderCollision(getAttackBoxes(effects.facing == -1), work, xaxis, yaxis, Graphics::makeColor(255,0,0 ));
    }
}

void MugenAnimation::render(int xaxis, int yaxis, const Graphics::Bitmap & work, const Mugen::Effects & effects){
    if (position >= frames.size()){
        return;
    }

    Mugen::Effects combined = frames[position]->effects + effects;
    renderFrame(frames[position], xaxis, yaxis, work, combined);
}
        
MugenAnimation * MugenAnimation::copy() const {
    return new MugenAnimation(*this);
}

void MugenAnimation::render(int xaxis, int yaxis, const Graphics::Bitmap &work, double scalex, double scaley){
    if (position >= frames.size()){
        return;
    }

    Mugen::Effects effects = frames[position]->effects;
    effects.scalex = scalex;
    effects.scaley = scaley;

    renderFrame(frames[position], xaxis, yaxis, work, effects);

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

Mugen::Effects MugenAnimation::getCurrentEffects(bool facing, bool vfacing, double scalex, double scaley){
    MugenFrame * frame = frames[position];
    Mugen::Effects effects = frame->effects;
    effects.scalex = scalex;
    effects.scaley = scaley;
    effects.facing = (facing ? -1 : 1);
    effects.vfacing = (vfacing ? -1 : 1);
    return effects;
}

void MugenAnimation::render(bool facing, bool vfacing, const int xaxis, const int yaxis, const Graphics::Bitmap &work, const double scalex, const double scaley, Graphics::Bitmap::Filter * filter){
    if (position >= frames.size()){
        return;
    }

    MugenFrame * frame = frames[position];
    Mugen::Effects effects = frame->effects;
    effects.scalex = scalex;
    effects.scaley = scaley;
    effects.facing = (facing ? -1 : 1);
    effects.vfacing = (vfacing ? -1 : 1);
    effects.filter = filter;

    renderFrame(frame, xaxis, yaxis, work, effects);
}

void MugenAnimation::renderReflection(bool facing, bool vfacing, int alpha, const int xaxis, const int yaxis, const Graphics::Bitmap &work, const double scalex, const double scaley){
    if (position >= frames.size()){
        return;
    }

    MugenFrame * frame = frames[position];
    Mugen::Effects effects = frame->effects;
    effects.facing = (facing ? -1 : 1);
    effects.vfacing = (vfacing ? -1 : 1);
    effects.trans = Mugen::Translucent;
    effects.alphaSource = alpha;
    effects.scalex = scalex;
    effects.scaley = scaley;

    renderFrame(frame, xaxis, yaxis, work, effects);
}

void MugenAnimation::forwardFrame(){
    if( position < frames.size() -1 )position++;
    else position = loopPosition;
}

void MugenAnimation::backFrame(){
    if( position > loopPosition )position--;
    else position = frames.size() - 1;
}

/* who uses this function? */
void MugenAnimation::reloadBitmaps(){
    for( std::vector< MugenFrame * >::iterator i = frames.begin() ; i != frames.end() ; ++i ){
	MugenFrame *frame = *i;
	if (frame->sprite){
	   // if (frame->bmp) delete frame->bmp;
	    //frame->bmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) frame->sprite->pcx, frame->sprite->newlength));
	    frame->sprite->reload();
	}
    }
}

// Get name of type of animation
const std::string MugenAnimation::getName(const Mugen::AnimationType t){
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
