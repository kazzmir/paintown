#include "mugen_background.h"

#include <ostream>
#include <cstring>
#include <string>
#include "globals.h"
#include "mugen_sprite.h"
#include "util/bitmap.h"

using namespace std;

static int calculateTile( int length, int width ){
    int loc = 0;
    for( int i = 1; ; ++i ){
	if( loc > length )return i;
	loc+=width;
    }
}

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
trans(None),
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
    switch( type ){
	case Normal:{
	    // We have a sprite
	    if( imageNumber != -1 && groupNumber != -1 ){
		MugenSprite *sprite = sprites[(unsigned int)groupNumber][(unsigned int)imageNumber];
		// const int x = (xaxis - sprite->x) + startx;
		// const int y = (yaxis - sprite->y) + starty;
		const int x = (xaxis) + startx;
		const int y = (yaxis) + starty;
		if (sprite != 0){
		    Bitmap bmp = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength);
		    // see if we need to tile this beyatch
		    int tilexloc = x;
		    const int width = bmp.getWidth();
		    const int height = bmp.getHeight();
		    // Figure out total we need to tile (this crap doesn't work needs fix)
		    int repeath = (tilex > 0 ? (tilex > 1 ? tilex : ( calculateTile( work->getWidth(), width ) ) ) : 1 );
		    int repeatv = ( tiley > 0 ? (tiley > 1 ? tiley : ( calculateTile( work->getHeight(), height ) ) ) : 1 );
		     // We need to repeat and wrap
                    Global::debug(1) << id << " tilex is " << tilex << ". Repeat h is " << repeath << endl;
		    for( int h = 0; h < repeath; h++ ){
			int tileyloc = y;
			for( int v = 0; v < repeatv; v++ ){
			    // This needs to be a switch trans = None, Add, Add1, Sub1, Addalpha
			    if( trans == None ){
				if( mask )bmp.draw( tilexloc,tileyloc, *work );
				else bmp.Blit( tilexloc, tileyloc, *work );
			    }
			    else{
				// Need to figure out blend correctly addalpha is given to two locations ?
				Bitmap::transBlender( 0, 0, 0, alphalow );
				bmp.drawingMode( Bitmap::MODE_TRANS );
				bmp.drawTrans( tilexloc, tileyloc, *work);
				work->drawingMode( Bitmap::MODE_SOLID );
			    }
			    tileyloc += height + tilespacingy;
			    if( tileyloc >= work->getHeight() )tileyloc = work->getHeight() - tileyloc;
			}
			tilexloc += width + tilespacingx;
			if( tilexloc >= work->getWidth() )tilexloc = work->getWidth() - tilexloc;
		    }
		}
	    }
	    break;
	}
	case Parallax:{
	    // We have a sprite
	    if( imageNumber != -1 && groupNumber != -1 ){
		MugenSprite *sprite = sprites[(unsigned int)groupNumber][(unsigned int)imageNumber];
		const int x = (xaxis - sprite->x) + startx;
		const int y = (yaxis - sprite->y) + starty;
		if (sprite != 0){
		    Bitmap bmp = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength);
		    bmp.Blit( x, y, *work );
		}
	    }
	    break;
	}
	case Anim:{
	    break;
	}
	case Dummy:
	default:
	    break;
    }
    
}
