#include "mugen_animation.h"

#include <string>
#include <vector>

#include "mugen_sprite.h"

/// area
MugenArea::MugenArea():
x1(0),
y1(0),
x2(0),
y2(0){ 
}

MugenArea::MugenArea( const MugenArea &copy ){
    this->x1 = copy.x1;
    this->x1 = copy.y1;
    this->x1 = copy.x2;
    this->x1 = copy.y2;
}

MugenArea & MugenArea::operator=( const MugenArea &copy ){
    this->x1 = copy.x1;
    this->x1 = copy.y1;
    this->x1 = copy.x2;
    this->x1 = copy.y2;
    
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
colorAdd(""){
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
type(UNKNOWN){
}
MugenAnimation::MugenAnimation( const MugenAnimation &copy ){
    this->frames = copy.frames;
    this->loopPosition = copy.loopPosition;
    this->position = copy.position;
    this->type = copy.type;
}
MugenAnimation::~MugenAnimation(){
    for( std::vector< MugenFrame * >::iterator i = frames.begin() ; i != frames.end() ; ++i ){
	delete (*i);
    }
}

void MugenAnimation::addFrame( MugenFrame *frame ){
    if( frame->loopstart ) loopPosition = frames.size();
    frames.push_back( frame );
}

const MugenFrame *MugenAnimation::getNext(){
    if( position < frames.size() )position++;
    else position = loopPosition;
    
    return frames[position];
}

// Get name of type of animation
const std::string MugenAnimation::getName(const int t){
    if(t == 0)return "Standing";
    else if(t == 5)return "Stand turning";
    else if(t == 6)return "Crouch turning";
    else if(t == 10)return "Stand to crouch";
    else if(t == 11)return "Crouching";
    else if(t == 12)return "Crouch to stand";
    else if(t == 20)return "Walking forwards";
    else if(t == 21)return "Walking backwards";
    else if(t == 40)return "Jump start (on ground)";
    else if(t == 41)return "Jump neutral (upwards)";
    else if(t == 42)return "Jump forwards (upwards)";
    else if(t == 43)return "Jump back (upwards)";
    else if(t == 44)return "Jump neutral (downwards)";
    else if(t == 45)return "Jump fwd (downwards)";
    else if(t == 46)return "Jump back (downwards)";
    else if(t == 47)return "Jump landing";
    else if(t == 100)return "Run fwd/hop forward";
    else if(t == 105)return "Hop back";
    else if(t == 120)return "Start guarding (stand)";
    else if(t == 121)return "Start guarding (crouch)";
    else if(t == 122)return "Start guarding (air)";
    else if(t == 130)return "Guard (stand)";
    else if(t == 131)return "Guard (crouch)";
    else if(t == 132)return "Guard (air)";
    else if(t == 140)return "Stop guarding (stand)";
    else if(t == 141)return "Stop guarding (crouch)";
    else if(t == 142)return "Stop guarding (air)";
    else if(t == 150)return "Guarding a hit (stand)";
    else if(t == 151)return "Guarding a hit (crouch)";
    else if(t == 152)return "Guarding a hit (air)";
    else if(t == 170)return "opt   Lose";
    else if(t == 175)return "opt   Time Over drawgame";
    else if(t == 180)return "opt   Win";
    else if(t == 190)return "opt   Intro";
    else if(t == 195)return "opt   Taunt";
    else if(t == 5000)return "Stand/Air Hit high (light)";
    else if( t == 5001)return "Stand/Air Hit high (medium)";
    else if( t == 5002)return "Stand/Air Hit high (hard)";
    else if( t == 5005)return "Stand Recover high (light)";
    else if( t == 5006)return "Stand Recover high (medium)";
    else if( t == 5007)return "Stand Recover high (hard)";
    else if( t == 5010)return "Stand/Air Hit low (light)";
    else if( t == 5011)return "Stand/Air Hit low (medium)";
    else if( t == 5012)return "Stand/Air Hit low (hard)";
    else if( t == 5015)return "Stand Recover low (light)";
    else if( t == 5016)return "Stand Recover low (medium)";
    else if( t == 5017)return "Stand Recover low (hard)";
    else if( t == 5020)return "Crouch Hit (light)";
    else if( t == 5021)return "Crouch Hit (medium)";
    else if( t == 5022)return "Crouch Hit (hard)";
    else if( t == 5025)return "Crouch Recover (light)";
    else if( t == 5026)return "Crouch Recover (medium)";
    else if( t == 5027)return "Crouch Recover (hard)";
    else if( t == 5030)return "Stand/Air Hit back";
    else if( t == 5035)return "opt  Stand/Air Hit transition";
    else if( t == 5040)return "Air Recover";
    else if( t == 5050)return "Air Fall";
    else if( t == 5060)return "opt  Air Fall (coming down)";
    else if( t == 5070)return "Tripped";
    else if( t == 5080)return "LieDown Hit (stay down)";
    else if( t == 5090)return "LieDown Hit (hit up into air)";
    else if( t == 5100)return "Hitting ground from fall";
    else if( t == 5160)return "Bounce into air";
    else if( t == 5170)return "Hit ground from bounce";
    else if( t == 5110)return "LieDown";
    else if( t == 5120)return "Get up from LieDown";
    else if( t == 5140)return "opt  LieDead (first rounds)";
    else if( t == 5150)return "opt  LieDead (final round)";
    else if( t == 5200)return "Fall-recovery near ground";
    else if( t == 5210)return "Fall-recovery in mid-air";
    else if( t == 5300)return "Dizzy";
    else if( t == 5500)return "opt  \"Continue?\" screen";
    else if( t == 5510)return "opt  \"Yes\" to \"Continue\"";
    else if( t == 5520)return "opt  \"No\" to \"Continue\"";
    else if( t == 5051)return "opt  Air fall -- hit up";
    else if( t == 5061)return "opt  Coming down from hit up";
    else if( t == 5081)return "opt  LieDown Hit (stay down)";
    else if( t == 5101)return "opt  Bounce from ground into air";
    else if( t == 5161)return "opt  Bounce into air";
    else if( t == 5171)return "opt  Hit ground from bounce";
    else if( t == 5111)return "opt  LieDown";
    else if( t == 5121)return "opt  Get up from LieDown";
    else if( t == 5151)return "opt  LieDead (first rounds)";
    else if( t == 5156)return "opt  LieDead (final round)";
    else if( t == 5052)return "opt  Air fall -- hit up";
    else if( t == 5062)return "opt  Coming down from hit up";
    else if( t == 5082)return "opt  LieDown Hit (stay down)";
    else if( t == 5102)return "opt  Bounce from ground into air";
    else if( t == 5162)return "opt  Bounce into air";
    else if( t == 5172)return "opt  Hit ground from bounce";
    else if( t == 5112)return "opt  LieDown";
    else if( t == 5122)return "opt  Get up from LieDown";
    else if( t == 5152)return "opt  LieDead (first rounds)";
    else if( t == 5157)return "opt  LieDead (final round)";
    else return "Custom or Optional Animation";
}



