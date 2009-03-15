#include "mugen_animation.h"

#include <string>
#include <vector>

#include "init.h"
#include "mugen_sprite.h"
#include "mugen_util.h"

#include "util/bitmap.h"

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
time(0),
flipHorizontal(false),
flipVertical(false),
colorAdd(NO),
colorSource(255),
colorDestination(255),
bmp(0){
}
MugenFrame::MugenFrame( const MugenFrame &copy ){
    this->loopstart = copy.loopstart;
    this->sprite = copy.sprite;
    this->xoffset = copy.xoffset;
    this->yoffset = copy.yoffset;
    this->time = copy.time;
    this->flipHorizontal = copy.flipHorizontal;
    this->flipVertical = copy.flipVertical;
    this->colorAdd = copy.colorAdd;
    this->colorSource = copy.colorSource;
    this->colorDestination = copy.colorDestination;
    if( copy.bmp )this->bmp = new Bitmap(*copy.bmp);

}

MugenFrame & MugenFrame::operator=( const MugenFrame &copy ){
    this->loopstart = copy.loopstart;
    this->sprite = copy.sprite;
    this->xoffset = copy.xoffset;
    this->yoffset = copy.yoffset;
    this->time = copy.time;
    this->flipHorizontal = copy.flipHorizontal;
    this->flipVertical = copy.flipVertical;
    this->colorAdd = copy.colorAdd;
    this->colorSource = copy.colorSource;
    this->colorDestination = copy.colorDestination;
    if( copy.bmp )this->bmp = new Bitmap(*copy.bmp);
    
    return *this;

}

MugenFrame::~MugenFrame(){
    // Kill bitmap
    if( bmp )delete bmp;
}

/*
Holds mugen animations, ie: player.air
*/
MugenAnimation::MugenAnimation():
loopPosition(0),
position(0),
type(UNKNOWN),
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
    // This gets deleted by frame, so don't worry, be happy
    if( !frame->bmp ){
	if( frame->sprite )frame->bmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) frame->sprite->pcx, frame->sprite->newlength));
	else frame->bmp = new Bitmap();
    }
    if( frame->loopstart ) loopPosition = frames.size();
    frames.push_back( frame );
}

const MugenFrame *MugenAnimation::getNext(){
    if( position < frames.size() -1 )position++;
    else position = loopPosition;
    
    return frames[position];
}

void MugenAnimation::logic(){
    if( frames[position]->time != -1 ){
	ticks++;
	if(ticks >= frames[position]->time * Global::LOGIC_MULTIPLIER){
		ticks = 0;
		if( position < frames.size() -1 )position++;
		else position = loopPosition;
	}
    }
}

void MugenAnimation::render( int xaxis, int yaxis, Bitmap &work ){
    if (frames[position]->sprite == 0){
	return;
    }
    const int spritex = frames[position]->sprite ? frames[position]->sprite->x : 0;
    const int spritey = frames[position]->sprite ? frames[position]->sprite->y : 0;
    const int placex = (xaxis - spritex ) + frames[position]->xoffset;
    const int placey = (yaxis - spritey ) + frames[position]->yoffset;
    
    if (frames[position]->flipHorizontal && ! frames[position]->flipVertical){
	switch (frames[position]->colorAdd){
	    case ADD:
		Bitmap::addBlender( 255, 255, 255, frames[position]->colorSource );
		frames[position]->bmp->drawTransHFlip(placex + frames[position]->bmp->getWidth(), placey, work);
		break;
	    case SUB:
		Bitmap::differenceBlender( 128, 128, 128, frames[position]->colorSource );
		frames[position]->bmp->drawTransHFlip(placex + frames[position]->bmp->getWidth(), placey, work);
		break;
	    case NO:
	    default:
		frames[position]->bmp->drawHFlip(placex + frames[position]->bmp->getWidth(), placey, work);
		break;
	}
    } else if (frames[position]->flipVertical && ! frames[position]->flipHorizontal){
	switch (frames[position]->colorAdd){
	    case ADD:
		Bitmap::addBlender( 255, 255, 255, frames[position]->colorSource );
		frames[position]->bmp->drawTransVFlip(placex, placey - frames[position]->bmp->getHeight(), work);
		break;
	    case SUB:
		Bitmap::differenceBlender( 128, 128, 128, frames[position]->colorSource );
		frames[position]->bmp->drawTransVFlip(placex, placey - frames[position]->bmp->getHeight(), work);
		break;
	    case NO:
	    default:
		frames[position]->bmp->drawVFlip(placex, placey - frames[position]->bmp->getHeight(), work);
		break;
	}
    } else if ( frames[position]->flipVertical && frames[position]->flipHorizontal ){
	switch (frames[position]->colorAdd){
	    case ADD:
		Bitmap::addBlender( 255, 255, 255, frames[position]->colorSource );
		frames[position]->bmp->drawTransHVFlip(placex + frames[position]->bmp->getWidth(), placey - frames[position]->bmp->getHeight(), work);
		break;
	    case SUB:
		Bitmap::differenceBlender( 128, 128, 128, frames[position]->colorSource );
		frames[position]->bmp->drawTransHVFlip(placex + frames[position]->bmp->getWidth(), placey - frames[position]->bmp->getHeight(), work);
		break;
	    case NO:
	    default:
		frames[position]->bmp->drawHVFlip(placex + frames[position]->bmp->getWidth(), placey - frames[position]->bmp->getHeight(), work);
		break;
	}
    } else{
	switch (frames[position]->colorAdd){
	    case ADD:
		Bitmap::addBlender( 255, 255, 255, frames[position]->colorSource );
		frames[position]->bmp->drawTrans(placex, placey, work);
		break;
	    case SUB:
		Bitmap::differenceBlender( 128, 128, 128, frames[position]->colorSource );
		frames[position]->bmp->drawTrans(placex, placey, work);
		break;
	    case NO:
	    default:
		frames[position]->bmp->draw(placex, placey, work);
		break;
	}
    }
    
    if( showDefense )renderCollision( frames[position]->defenseCollision, work, xaxis, yaxis, Bitmap::makeColor( 0,255,0 ) );
    if( showOffense )renderCollision( frames[position]->attackCollision, work, xaxis, yaxis,  Bitmap::makeColor( 255,0,0 ) );
}

void MugenAnimation::forwardFrame(){
    if( position < frames.size() -1 )position++;
    else position = loopPosition;
}

void MugenAnimation::backFrame(){
    if( position > loopPosition )position--;
    else position = position = frames.size() - 1;
}

// Get name of type of animation
const std::string MugenAnimation::getName(const MugenAnimationType t){
    switch( t ){
	case STANDING: return "Standing";break;
	case STAND_TURNING: return "Stand turning";break;
	case CROUCH_TURNING: return "Crouch turning";break;
	case STAND_TO_CROUCH: return "Stand to crouch";break;
	case CROUCHING: return "Crouching";break;
	case CROUCH_TO_STAND: return "Crouch to stand";break;
	case WALKING_FORWARDS: return "Walking forwards";break;
	case WALKING_BACKWARDS: return "Walking backwards";break;
	case JUMP_START: return "Jump start (on ground)";break;
	case JUMP_NEUTRAL_UP: return "Jump neutral (upwards)";break;
	case JUMP_FORWARDS_UP: return "Jump forwards (upwards)";break;
	case JUMP_BACK_UP: return "Jump back (upwards)";break;
	case JUMP_NEUTRAL_DOWN: return "Jump neutral (downwards)";break;
	case JUMP_FORWARDS_DOWN: return "Jump fwd (downwards)";break;
	case JUMP_BACK_DOWN: return "Jump back (downwards)";break;
	case JUMP_LANDING: return "Jump landing";break;
	case RUN_FORWARDS: return "Run fwd/hop forward";break;
	case HOP_BACK: return "Hop back";break;
	case START_GUARD_STAND: return "Start guarding (stand)";break;
	case START_GUARD_CROUCH: return "Start guarding (crouch)";break;
	case START_GUARD_AIR: return "Start guarding (air)";break;
	case GUARD_STAND: return "Guard (stand)";break;
	case GUARD_CROUCH: return "Guard (crouch)";break;
	case GUARD_AIR: return "Guard (air)";break;
	case STOP_GUARD_STAND: return "Stop guarding (stand)";break;
	case STOP_GUARD_CROUCH: return "Stop guarding (crouch)";break;
	case STOP_GUARD_AIR: return "Stop guarding (air)";break;
	case GUARD_HIT_STAND: return "Guarding a hit (stand)";break;
	case GUARD_HIT_CROUCH: return "Guarding a hit (crouch)";break;
	case GUARD_HIT_AIR: return "Guarding a hit (air)";break;
	case LOSE: return "opt   Lose";break;
	case DRAW: return "opt   Time Over drawgame";break;
	case WIN: return "opt   Win";break;
	case INTRO: return "opt   Intro";break;
	case TAUNT: return "opt   Taunt";break;
	case HIT_HIGH_LIGHT: return "Stand/Air Hit high (light)";break;
	case HIT_HIGH_MEDIUM: return "Stand/Air Hit high (medium)";break;
	case HIT_HIGH_HARD: return "Stand/Air Hit high (hard)";break;
	case STAND_RECOVER_HIGH_LIGHT: return "Stand Recover high (light)";break;
	case STAND_RECOVER_HIGH_MEDIUM: return "Stand Recover high (medium)";break;
	case STAND_RECOVER_HIGH_HARD: return "Stand Recover high (hard)";break;
	case HIT_LOW_LIGHT: return "Stand/Air Hit low (light)";break;
	case HIT_LOW_MEDIUM: return "Stand/Air Hit low (medium)";break;
	case HIT_LOW_HARD: return "Stand/Air Hit low (hard)";break;
	case STAND_RECOVER_LOW_LIGHT: return "Stand Recover low (light)";break;
	case STAND_RECOVER_LOW_MEDIUM: return "Stand Recover low (medium)";break;
	case STAND_RECOVER_LOW_HARD: return "Stand Recover low (hard)";break;
	case CROUCH_HIT_LIGHT: return "Crouch Hit (light)";break;
	case CROUCH_HIT_MEDIUM: return "Crouch Hit (medium)";break;
	case CROUCH_HIT_HARD: return "Crouch Hit (hard)";break;
	case CROUCH_RECOVER_LIGHT: return "Crouch Recover (light)";break;
	case CROUCH_RECOVER_MEDIUM: return "Crouch Recover (medium)";break;
	case CROUCH_RECOVER_HARD: return "Crouch Recover (hard)";break;
	case HIT_BACK: return "Stand/Air Hit back";break;
	case HIT_TRANSITION: return "opt  Stand/Air Hit transition";break;
	case AIR_RECOVER: return "Air Recover";break;
	case AIR_FALL: return "Air Fall";break;
	case AIR_FALL_COMING_DOWN: return "opt  Air Fall (coming down)";break;
	case TRIPPED: return "Tripped";break;
	case LIEDOWN_HIT_STAYDOWN: return "LieDown Hit (stay down)";break;
	case LIEDOWN_HIT_INTOAIR: return "LieDown Hit (hit up into air)";break;
	case HIT_GROUND_FROM_FALL: return "Hitting ground from fall";break;
	case BOUNCE_INTO_AIR: return "Bounce into air";break;
	case HIT_GROUND_FROM_BOUNCE: return "Hit ground from bounce";break;
	case LIEDOWN: return "LieDown";break;
	case GET_UP_FROM_LIEDOWN: return "Get up from LieDown";break;
	case LIEDEAD_FIRST_ROUNDS: return "opt  LieDead (first rounds)";break;
	case LIEDEAD_FINAL_ROUND: return "opt  LieDead (final round)";break;
	case FALL_RECOVERY_NEAR_GROUND: return "Fall-recovery near ground";break;
	case FALL_RECOVERY_MIDAIR: return "Fall-recovery in mid-air";break;
	case DIZZY: return "Dizzy";break;
	case CONTINUE: return "opt  \"Continue?\" screen";break;
	case YES_CONTINUE: return "opt  \"Yes\" to \"Continue\"";break;
	case NO_CONTINUE: return "opt  \"No\" to \"Continue\"";break;
	case AIR_FALL_HIT_UP: return "opt  Air fall -- hit up";break;
	case FALL_FROM_HIT_UP: return "opt  Coming down from hit up";break;
	case LIEDOWN_HIT_STAYDOWN2: return "opt  LieDown Hit (stay down)";break;
	case BOUNCE_FROM_GROUND_INTO_AIR: return "opt  Bounce from ground into air";break;
	case BOUNCE_INTO_AIR2: return "opt  Bounce into air";break;
	case HIT_GROUND_FROM_BOUNCE2: return "opt  Hit ground from bounce";break;
	case LIEDOWN2: return "opt  LieDown";break;
	case GET_UP_FROM_LIEDOWN2: return "opt  Get up from LieDown";break;
	case LIEDEAD_FIRST_ROUNDS2: return "opt  LieDead (first rounds)";break;
	case LIEDEAD_FINAL_ROUND2: return "opt  LieDead (final round)";break;
	case AIR_FALL_HIT_UP2: return "opt  Air fall -- hit up";break;
	case FALL_FROM_HIT_UP2: return "opt  Coming down from hit up";break;
	case LIEDOWN_HIT_STAYDOWN3: return "opt  LieDown Hit (stay down)";break;
	case BOUNCE_FROM_GROUND_INTO_AIR2: return "opt  Bounce from ground into air";break;
	case BOUNCE_INTO_AIR3: return "opt  Bounce into air";break;
	case HIT_GROUND_FROM_BOUNCE3: return "opt  Hit ground from bounce";break;
	case LIEDOWN3: return "opt  LieDown";break;
	case GET_UP_FROM_LIEDOWN3: return "opt  Get up from LieDown";break;
	case LIEDEAD_FIRST_ROUNDS3: return "opt  LieDead (first rounds)";break;
	case LIEDEAD_FINAL_ROUND3: return "opt  LieDead (final round)";break;
	case UNKNOWN:
	default: return "Custom or Optional Animation";break;
    }
}



