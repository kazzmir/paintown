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
MugenFrame::~MugenFrame(){
}

/*
Holds mugen animations, ie: player.air
*/
MugenAnimation::MugenAnimation():
loopPosition(0),
position(0){
}
MugenAnimation::MugenAnimation( const MugenAnimation &copy ){
    this->frames = copy.frames;
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


