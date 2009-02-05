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

static void doParallax(Bitmap &bmp, Bitmap &work, int xscroll, int y1, float z1, int y2, float z2){
   if(y2 <= y1) return;

   // trade down the centre of the screen
   float z = 1.0 / z1;
   float z_add = ((1.0 / z2) - z) / (y2 - y1);
   float src_y = 0;
   float src_y_add = ((float)bmp.getHeight() / z2) / (y2 - y1);

   while(y1 < y2)
   {
      float y = src_y / z;
      int start_x = (work.getWidth() >> 1) - z*((bmp.getWidth() >> 1) + xscroll);
      int width = bmp.getWidth()*z;

      //stretch_blit(source, dest, 0, y, source->w, 1, start_x, y1, width, 1);
      bmp.Stretch( work, 0, y, bmp.getWidth(), 1, start_x, y1, width, 1 );

      y1++;
      z +=  z_add;
      src_y += src_y_add;
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
actionno(-1),
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
siny_offset(0),
xoffset(0),
yoffset(0),
movex(0),
movey(0),
sprite(0),
spriteBmp(0),
action(0){
}
MugenBackground::MugenBackground( const MugenBackground &copy ){
}
MugenBackground::~MugenBackground(){
    // Kill the bmp
    if( spriteBmp )delete spriteBmp;
}
MugenBackground & MugenBackground::operator=( const MugenBackground &copy ){
    
    return *this;
}
    
void MugenBackground::logic( const int &x, const int &y ){
    movex = movey = 0;
    movex += x * deltax;
    movey += y * deltay;
    if( type == Anim ) action->logic();
    Global::debug(1) << "x move: " << x << " | y move: " << y << endl;
}
    
void MugenBackground::render( const int &totalLength, const int &totalHeight, Bitmap *work ){
    const int x = xoffset + movex;
    const int y = yoffset + movey;
    switch( type ){
	case Normal:{
	    // Normal is a sprite
	    // see if we need to tile this beyatch
	    int tilexloc = x;
	    const int width = spriteBmp->getWidth();
	    const int height = spriteBmp->getHeight();
	    bool dirx = false, diry = false;
	    // Figure out total we need to tile (this crap doesn't work needs fix)
	    int repeath = (tilex > 0 ? (tilex > 1 ? tilex : ( calculateTile( totalLength, width ) ) ) : 1 );
	    int repeatv = ( tiley > 0 ? (tiley > 1 ? tiley : ( calculateTile( totalLength, height ) ) ) : 1 );
	    // We need to repeat and wrap
	    for( int h = 0; h < repeath; h++ ){
		int tileyloc = y;
		for( int v = 0; v < repeatv; v++ ){
		    draw( tilexloc, tileyloc, *work );
		    if( !diry )tileyloc += height + tilespacingy;
		    else tileyloc -= height + tilespacingy;
		    if( tileyloc >= work->getHeight() ){
			diry = true;
			tileyloc = y - height + tilespacingy;
		    }
		}
		if( !dirx )tilexloc += width + tilespacingx;
		else tilexloc -= width + tilespacingx;
		if( tilexloc >= work->getWidth() ){
		    dirx = true;
		    tilexloc = x - width + tilespacingx;
		}
	    }
	    break;
	}
	case Parallax:{
	    // This is also a sprite but we must parallax it across the top and bottom to give the illusion of depth
	    doParallax( *spriteBmp, *work, x, y - (((yscalestart + yscaledelta)/100)*movey), xscaletop * 1 , y + spriteBmp->getHeight() + (((yscalestart + yscaledelta)/100)*movey), xscaletop * 1.75);
	    break;
	}
	case Anim:{
	    // there is no sprite use our action!
	    action->render( x, y, *work );
	    break;
	}
	case Dummy:
	    // Do nothing
	default:
	    break;
    }
    
}

void MugenBackground::preload( const int &xaxis, const int &yaxis ){
    // Lets load our sprite
    if(sprite){
	spriteBmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength));
    }
    // Set our initial offsets
    xoffset = 160 + (xaxis - sprite->x) + startx;
    yoffset = (yaxis - sprite->y) + starty;
}

void MugenBackground::draw( const int &x, const int &y, Bitmap &work ){
    // This needs to be a switch trans = None, Add, Add1, Sub1, Addalpha
    switch( trans ){
	case Addalpha:{
	    // Need to figure out blend correctly addalpha is given to two locations low and high ?
	    Bitmap::transBlender( 0, 0, 0, alphalow );
            Bitmap::drawingMode( Bitmap::MODE_TRANS );
	    spriteBmp->drawTrans( x, y, work);
            Bitmap::drawingMode( Bitmap::MODE_SOLID );
	    break;
	}
	case Add:
	case Add1:
	case Sub:
	case None:
	default:{
	    if( mask )spriteBmp->draw( x,y, work );
	    else spriteBmp->Blit( x, y, work );
	    break;
	}
    }
}
