#include "mugen_background.h"

#include <ostream>
#include <cstring>
#include <string>
#include "globals.h"
#include "mugen_sprite.h"
#include "util/bitmap.h"

using namespace std;

static int idCounter = -9999999;

BgController::BgController( ControlType ctrl, std::vector<int>ids ):
type(ctrl),
timestart(0),
endtime(0),
looptime(0),
ctrlID(ids){
}
BgController::~BgController(){
}
void BgController::act( std::map<int, MugenBackground *> &bgs ){
    // nothing
}

MugenBgController::MugenBgController( std::string &n, std::vector<int>ids ):
name(n),
looptime(0),
ctrlID(ids){
}
MugenBgController::~MugenBgController(){
}
void MugenBgController::addControl( BgController *ctrl ){
    controls.push_back(ctrl);
}
void MugenBgController::act( std::map<int, MugenBackground *> &bgs ){
}

// mugen background
MugenBackground::MugenBackground():
type(Normal),
groupNumber(-1),
imageNumber(-1),
actionno(0),
action(0),
id(++idCounter),
layerno(0),
startx(0),
starty(0),
deltax(0),
deltay(0),
trans(""),
alphalow(0),
alphahigh(0),
mask(true),
tilex(0),
tiley(0),
tilespacingx(0),
tilespacingy(0),
windowdeltax(0),
windowdeltay(0),
xscaletop(0),
xscalebot(0),
yscalestart(0),
yscaledelta(0),
positionlink(false),
velocityx(0),
velocityy(0),
sinx_amp(0),
sinx_period(0),
sinx_offset(0),
siny_amp(0),
siny_period(0),
siny_offset(0){
}
MugenBackground::MugenBackground( const MugenBackground &copy ){
}
MugenBackground::~MugenBackground(){
}
MugenBackground & MugenBackground::operator=( const MugenBackground &copy ){
    
    return *this;
}
    
void MugenBackground::logic(){
}
    
void MugenBackground::render( int xaxis, int yaxis, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites, Bitmap *work ){
    // We have a sprite
    if( imageNumber != -1 && groupNumber != -1 ){
	MugenSprite *sprite = sprites[(unsigned int)groupNumber][(unsigned int)imageNumber];
	if (sprite != 0){
	    const int x = (xaxis - startx ) - sprite->x;
	    const int y = (yaxis - starty ) - sprite->y;
	    
	    Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength).draw( x,y, *work );
	}
    }
    // Else we should have an animation
    else{
    }
}
    
  
  