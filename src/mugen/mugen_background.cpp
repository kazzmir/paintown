#include "mugen_background.h"

#include <math.h>
#include <ostream>
#include <cstring>
#include <string>
#include "globals.h"
#include "mugen_sprite.h"
#include "util/bitmap.h"

//static double pi = 3.14159265;

using namespace std;

static int calculateTile( int length, int width ){
    int loc = 0;
    for( int i = 1; ; ++i ){
	if( loc > length )return i;
	loc+=width;
    }
}

static void doParallax(Bitmap &bmp, Bitmap &work, int xscroll, int y1, float z1, int y2, float z2, bool mask){
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
//const int sourceX, const int sourceY, const int sourceWidth, const int sourceHeight, const int destX, const int destY, const int destWidth, const int destHeight
      //stretch_blit(source, dest, 0, y, source->w, 1, start_x, y1, width, 1);
      // For now until we can stretch mask
      mask = false;
      if( mask )bmp.drawStretched(  0, y, width, 1, work );
      else bmp.Stretch( work, 0, y, bmp.getWidth(), 1, start_x - 160, y1, width + 320, 1 );
   
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
MugenBackground::MugenBackground( const unsigned long int &ticker ):
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
yscalestart(100),
yscaledelta(100),
positionlink(false),
velocityx(0),
velocityy(0),
sinx_amp(0),
sinx_period(0),
sinx_offset(0),
sinx_angle(0),
siny_amp(0),
siny_period(0),
siny_offset(0),
siny_angle(0),
xoffset(0),
yoffset(0),
movex(0),
movey(0),
velx(0),
vely(0),
stageTicker( ticker ),
ticks(0),
x(0),
y(0),
sprite(0),
spriteBmp(0),
action(0),
linked(0),
runLink(false){
}
MugenBackground::MugenBackground( const MugenBackground &copy ):
stageTicker( copy.stageTicker ){
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
    velx += velocityx;
    vely += velocityy;
    /* how much should sin_angle be incremented by each frame?
     * I think the total angle should be (ticks % sin_period) * 2pi / sin_period
     * M (decimal) is the magnitude in pixels (amp)
     * P (decimal) is the period in game ticks (period) 
     * O (decimal) is the time offset in game ticks (offset)
     * From updates.txt:  M * sine ((ticks+O)/P * 2 * Pi)
     * sinx_amp * sin((stageTicker+sinx_offset)/sinx_period * 2 * pi)) ? useless it seems
     */
    //sin_angle += 0.00005;
    sinx_angle += 0.00005;
    siny_angle += 0.00005;
    
    ticks++;
    if( ticks >= 1 ){
	// Do frame or whatever movement etc
	// Reset
	ticks = 0;
    }
    if( type == Anim ) action->logic();
    
    this->x = (int)(xoffset + movex + velx + sinx_amp * sin(sinx_angle*sinx_period + sinx_offset));
    this->y = (int)(yoffset + movey + vely + siny_amp * sin(siny_angle*siny_period + siny_offset));
    
}
    
void MugenBackground::render( const int &totalLength, const int &totalHeight, Bitmap *work ){
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
	    //doParallax( *spriteBmp, *work, x + 160, y - (((yscalestart + yscaledelta)/100)*movey), xscaletop * 1 , y + spriteBmp->getHeight() + (((yscalestart + yscaledelta)/100)*movey), xscaletop * 1.75);
	    //doParallax( *spriteBmp, *work, x, y - (((yscalestart + yscaledelta)/550)*movey), xscalebot * 1 , y + spriteBmp->getHeight() + (((yscalestart + yscaledelta)/550)*movey), xscaletop * 1.75, mask);
	    draw( x, y, *work );
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
    // Do positionlink crap
    if (positionlink && !runLink){
	if (linked){
	    linked->setPositionLink(this);
	}
	runLink = true;
    }
    
    if(sprite){
	// Lets load our sprite
	spriteBmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength));
	// Set our initial offsets
	xoffset = 160 + (xaxis - sprite->x) + startx;
	yoffset = (yaxis - sprite->y) + starty;
	velx = vely = 0;
    }
    else{
	// Set our initial offsets
	xoffset = 160 + (xaxis) + startx;
	yoffset = (yaxis) + starty;
	velx = vely = 0;
    }
}

void MugenBackground::draw( const int &ourx, const int &oury, Bitmap &work ){
    // This needs to be a switch trans = None, Add, Add1, Sub1, Addalpha
    switch( trans ){
	case Addalpha:{
	    // Need to figure out blend correctly addalpha is given to two locations low and high ?
	    Bitmap::transBlender( 0, 0, 0, alphalow );
            Bitmap::drawingMode( Bitmap::MODE_TRANS );
	    spriteBmp->drawTrans( ourx, oury, work);
            Bitmap::drawingMode( Bitmap::MODE_SOLID );
	    break;
	}
	case Add:
	case Add1:
	case Sub:
	case None:
	default:{
	    if( mask )spriteBmp->draw( ourx,oury, work );
	    else spriteBmp->Blit( ourx, oury, work );
	    break;
	}
    }
}


    // Lets do our positionlink stuff
void MugenBackground::setPositionLink(MugenBackground *bg){
    if (positionlink){
	if (linked){
	    linked->setPositionLink(bg);
	    return;
	}
    }
    bg->startx += startx;
    bg->starty += starty;
    bg->deltax = deltax;
    bg->deltay = deltay;
    bg->sinx_amp = sinx_amp;
    bg->sinx_offset = sinx_offset;
    bg->sinx_period = sinx_period;
    bg->siny_amp = siny_amp;
    bg->siny_offset = siny_offset;
    bg->siny_period = siny_period;
    bg->velocityx = velocityx;
    bg->velocityy = velocityy;
    
    Global::debug(1) << "Positionlinked bg: " << bg->name << " set to x: " << bg->startx << " y: " << bg->starty << endl;
}


