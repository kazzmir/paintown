#include "util/bitmap.h"
#include "mugen_animation.h"
#include "state.h"

#include "globals.h"
#include <string>
#include <vector>

#include "init.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "util/load_exception.h"

using namespace std;

static void renderCollision( const std::vector< MugenArea > &vec, Bitmap &bmp, int x, int y, int color ){
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
MugenFrame::MugenFrame():
loopstart(false),
sprite(0),
xoffset(0),
yoffset(0),
time(0){
}
MugenFrame::MugenFrame( const MugenFrame &copy ){
    this->loopstart = copy.loopstart;
    this->sprite = copy.sprite;
    this->xoffset = copy.xoffset;
    this->yoffset = copy.yoffset;
    this->time = copy.time;
    this->effects = copy.effects;

}

MugenFrame & MugenFrame::operator=( const MugenFrame &copy ){
    this->loopstart = copy.loopstart;
    this->sprite = copy.sprite;
    this->xoffset = copy.xoffset;
    this->yoffset = copy.yoffset;
    this->time = copy.time;
    this->effects = copy.effects;
    
    return *this;

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
type(Mugen::Unknown),
showDefense(false),
showOffense(false),
ticks(0){
}

MugenAnimation::MugenAnimation( const MugenAnimation &copy ){
    this->frames = copy.frames;
    this->loopPosition = copy.loopPosition;
    this->position = copy.position;
    this->type = copy.type;
    this->showDefense = copy.showDefense;
    this->showOffense = copy.showOffense;
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

/* time left in the animation */
int MugenAnimation::animationTime() const {
    return (int) position - (int) frames.size() + 1;
}

const std::vector<MugenArea> & MugenAnimation::getDefenseBoxes() const {
    MugenFrame * frame = frames[position];
    return frame->getDefenseBoxes();
}

const std::vector<MugenArea> & MugenAnimation::getAttackBoxes() const {
    MugenFrame * frame = frames[position];
    return frame->getAttackBoxes();
}

void MugenAnimation::logic(){
    if (position < frames.size()){
        if (frames[position]->time != -1){
            ticks += 1;
            if (ticks >= frames[position]->time){
                ticks = 0;
                if (position < frames.size() -1){
                    position += 1;
                } else {
                    position = loopPosition;
                }
            }
        }
    }
}

void MugenAnimation::renderFrame(MugenFrame * frame, int xaxis, int yaxis, Bitmap & work, double scalex, double scaley, const Mugen::Effects & effects){
    const int placex = xaxis+frames[position]->xoffset;
    const int placey = yaxis+frames[position]->yoffset;
    frame->sprite->render(placex, placey, work, effects);

    if (showDefense){
        renderCollision(frame->defenseCollision, work, xaxis, yaxis, Bitmap::makeColor(0, 255, 0));
    }

    if (showOffense){
        renderCollision(frame->attackCollision, work, xaxis, yaxis, Bitmap::makeColor(255,0,0 ));
    }
}

void MugenAnimation::render(int xaxis, int yaxis, Bitmap &work, double scalex, double scaley){
    if (position >= frames.size()){
        return;
    }

    if (frames[position]->sprite == 0){
	return;
    }

    renderFrame(frames[position], xaxis, yaxis, work, scalex, scaley, frames[position]->effects);

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

void MugenAnimation::render(bool facing, bool vfacing, const int xaxis, const int yaxis, Bitmap &work, const double scalex, const double scaley ){
    if (position >= frames.size()){
        return;
    }

    if (frames[position]->sprite == 0){
	return;
    }

    MugenFrame * frame = frames[position];
    Mugen::Effects effects = frame->effects;
    effects.facing = (facing ? -1 : 1);
    effects.vfacing = (vfacing ? -1 : 1);

    renderFrame(frame, xaxis, yaxis, work, scalex, scaley, effects);
    /*
    // Override flip and set back to original when done
    const int horizontal = frames[position]->effects.facing;
    const int vertical = frames[position]->effects.vfacing;
    frames[position]->effects.facing = (facing ? -1 : 1);
    frames[position]->effects.vfacing = (vfacing ? -1 : 1);
    // Now render
    render(xaxis,yaxis,work,scalex,scaley);
    // Set original setting
    frames[position]->effects.facing = horizontal;
    frames[position]->effects.vfacing = vertical;
    */
}

void MugenAnimation::forwardFrame(){
    if( position < frames.size() -1 )position++;
    else position = loopPosition;
}

void MugenAnimation::backFrame(){
    if( position > loopPosition )position--;
    else position = position = frames.size() - 1;
}

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
	case Mugen::JumpForwardsDown : return "Jump fwd (downwards)";break;
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
