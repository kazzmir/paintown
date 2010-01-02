#include "util/bitmap.h"
#include "mugen_animation.h"

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
}

/*
Holds mugen animations, ie: player.air
*/
MugenAnimation::MugenAnimation():
loopPosition(0),
position(0),
type(Unknown),
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
    for( std::vector< MugenFrame * >::iterator i = frames.begin() ; i != frames.end() ; ++i ){
	if( (*i) )delete (*i);
    }
}

void MugenAnimation::addFrame( MugenFrame *frame ){
    if (frame->loopstart){
	loopPosition = frames.size();
    }
    frames.push_back( frame );
}

const MugenFrame *MugenAnimation::getNext(){
    if( position < frames.size() -1 )position++;
    else position = loopPosition;
    
    return frames[position];
}

void MugenAnimation::logic(){
    if (position < frames.size()){
        if( frames[position]->time != -1 ){
            ticks++;
            if(ticks >= frames[position]->time * Global::LOGIC_MULTIPLIER){
                ticks = 0;
                if( position < frames.size() -1 )position++;
                else position = loopPosition;
            }
        }
    }
}

void MugenAnimation::render( int xaxis, int yaxis, Bitmap &work, double scalex, double scaley ){
    if (position >= frames.size()){
        return;
    }

    if (frames[position]->sprite == 0){
	return;
    }
    // Modify with frame adjustment
    const int placex = xaxis+frames[position]->xoffset;
    const int placey = yaxis+frames[position]->yoffset;
    try{
        frames[position]->sprite->render(placex,placey,work,frames[position]->effects);
        
        if( showDefense )renderCollision( frames[position]->defenseCollision, work, xaxis, yaxis, Bitmap::makeColor( 0,255,0 ) );
        if( showOffense )renderCollision( frames[position]->attackCollision, work, xaxis, yaxis,  Bitmap::makeColor( 255,0,0 ) );
    } catch (const LoadException & e){
        Global::debug(0) << "Error loading sprite: " << e.getReason() << endl;
        /* FIXME: do something sensible here */
        frames[position]->sprite = 0;
    }
}

void MugenAnimation::render( const int facing, const int vfacing, const int xaxis, const int yaxis, Bitmap &work, const double scalex, const double scaley ){
    if (position >= frames.size()){
        return;
    }

    if (frames[position]->sprite == 0){
	return;
    }
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
const std::string MugenAnimation::getName(const MugenAnimationType t){
    switch( t ){
	case Standing : return "Standing";break;
	case StandTurning : return "Stand turning";break;
	case CrouchTurning : return "Crouch turning";break;
	case StandToCrouch : return "Stand to crouch";break;
	case Crouching : return "Crouching";break;
	case CrouchToStand : return "Crouch to stand";break;
	case WalkingForwards : return "Walking forwards";break;
	case WalkingBackwards : return "Walking backwards";break;
	case JumpStart : return "Jump start (on ground)";break;
	case JumpNeutralUp : return "Jump neutral (upwards)";break;
	case JumpForwardsUp : return "Jump forwards (upwards)";break;
	case JumpBackUp : return "Jump back (upwards)";break;
	case JumpNeutralDown : return "Jump neutral (downwards)";break;
	case JumpForwardsDown : return "Jump fwd (downwards)";break;
	case JumpBackDown : return "Jump back (downwards)";break;
	case JumpLanding : return "Jump landing";break;
	case RunForwards : return "Run fwd/hop forward";break;
	case HopBack : return "Hop back";break;
	case StartGuardStand : return "Start guarding (stand)";break;
	case StartGuardCrouch : return "Start guarding (crouch)";break;
	case StartGuardAir : return "Start guarding (air)";break;
	case GuardStand : return "Guard (stand)";break;
	case GuardCrouch : return "Guard (crouch)";break;
	case GuardAir : return "Guard (air)";break;
	case StopGuardStand : return "Stop guarding (stand)";break;
	case StopGuardCrouch : return "Stop guarding (crouch)";break;
	case StopGuardAir : return "Stop guarding (air)";break;
	case GuardHitStand : return "Guarding a hit (stand)";break;
	case GuardHitCrouch : return "Guarding a hit (crouch)";break;
	case GuardHitAir : return "Guarding a hit (air)";break;
	case Lose : return "opt   Lose";break;
	case Draw : return "opt   Time Over drawgame";break;
	case Win : return "opt   Win";break;
	case Intro : return "opt   Intro";break;
	case Taunt : return "opt   Taunt";break;
	case HitHighLight : return "Stand/Air Hit high (light)";break;
	case HitHighMedium : return "Stand/Air Hit high (medium)";break;
	case HitHighHard : return "Stand/Air Hit high (hard)";break;
	case StandRecoverHighLight : return "Stand Recover high (light)";break;
	case StandRecoverHighMedium : return "Stand Recover high (medium)";break;
	case StandRecoverHighHard : return "Stand Recover high (hard)";break;
	case HitLowLight : return "Stand/Air Hit low (light)";break;
	case HitLowMedium : return "Stand/Air Hit low (medium)";break;
	case HitLowHard : return "Stand/Air Hit low (hard)";break;
	case StandRecoverLowLight : return "Stand Recover low (light)";break;
	case StandRecoverLowMedium : return "Stand Recover low (medium)";break;
	case StandRecoverLowHard : return "Stand Recover low (hard)";break;
	case CrouchHitLight : return "Crouch Hit (light)";break;
	case CrouchHitMedium : return "Crouch Hit (medium)";break;
	case CrouchHitHard : return "Crouch Hit (hard)";break;
	case CrouchRecoverLight : return "Crouch Recover (light)";break;
	case CrouchRecoverMedium : return "Crouch Recover (medium)";break;
	case CrouchRecoverHard : return "Crouch Recover (hard)";break;
	case HitBack : return "Stand/Air Hit back";break;
	case HitTransition : return "opt  Stand/Air Hit transition";break;
	case AirRecover : return "Air Recover";break;
	case AirFall : return "Air Fall";break;
	case AirFallComingDown : return "opt  Air Fall (coming down)";break;
	case Tripped : return "Tripped";break;
	case LiedownHitStaydown : return "LieDown Hit (stay down)";break;
	case LiedownHitIntoair : return "LieDown Hit (hit up into air)";break;
	case HitGroundFromFall : return "Hitting ground from fall";break;
	case BounceIntoAir : return "Bounce into air";break;
	case HitGroundFromBounce : return "Hit ground from bounce";break;
	case Liedown : return "LieDown";break;
	case GetUpFromLiedown : return "Get up from LieDown";break;
	case LiedeadFirstRounds : return "opt  LieDead (first rounds)";break;
	case LiedeadFinalRound : return "opt  LieDead (final round)";break;
	case FallRecoveryNearGround : return "Fall-recovery near ground";break;
	case FallRecoveryMidair : return "Fall-recovery in mid-air";break;
	case Dizzy : return "Dizzy";break;
	case Continue : return "opt  \"Continue?\" screen";break;
	case YesContinue : return "opt  \"Yes\" to \"Continue\"";break;
	case NoContinue : return "opt  \"No\" to \"Continue\"";break;
	case AirFallHitUp : return "opt  Air fall -- hit up";break;
	case FallFromHitUp : return "opt  Coming down from hit up";break;
	case LiedownHitStaydown2 : return "opt  LieDown Hit (stay down)";break;
	case BounceFromGroundIntoAir : return "opt  Bounce from ground into air";break;
	case BounceIntoAir2 : return "opt  Bounce into air";break;
	case HitGroundFromBounce2 : return "opt  Hit ground from bounce";break;
	case Liedown2 : return "opt  LieDown";break;
	case GetUpFromLiedown2 : return "opt  Get up from LieDown";break;
	case LiedeadFirstRounds2 : return "opt  LieDead (first rounds)";break;
	case LiedeadFinalRound2 : return "opt  LieDead (final round)";break;
	case AirFallHitUp2 : return "opt  Air fall -- hit up";break;
	case FallFromHitUp2 : return "opt  Coming down from hit up";break;
	case LiedownHitStaydown3 : return "opt  LieDown Hit (stay down)";break;
	case BounceFromGroundIntoAir2 : return "opt  Bounce from ground into air";break;
	case BounceIntoAir3 : return "opt  Bounce into air";break;
	case HitGroundFromBounce3 : return "opt  Hit ground from bounce";break;
	case Liedown3 : return "opt  LieDown";break;
	case GetUpFromLiedown3 : return "opt  Get up from LieDown";break;
	case LiedeadFirstRounds3 : return "opt  LieDead (first rounds)";break;
	case LiedeadFinalRound3 : return "opt  LieDead (final round)";break;
	case Unknown :
	default: return "Custom or Optional Animation";break;
    }
}
