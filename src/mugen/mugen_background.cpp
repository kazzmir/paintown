#include "mugen_background.h"

#include <math.h>
#include <ostream>
#include <cstring>
#include <string>
#include <algorithm>
#include "globals.h"
#include "mugen_sprite.h"
#include "util/bitmap.h"

//static double pi = 3.14159265;

using namespace std;

static double interpolate(double f1, double f2, double p){
    return (f1 * (1.0 - p)) + (f2 * p);
}

static int calculateTile( int length, int width ){
    int loc = 0;
    for( int i = 1; ; ++i ){
	if( loc > length )return i;
	loc+=width;
    }
}

static void doParallax(Bitmap &bmp, Bitmap &work, int leftx, int lefty, int xoffset, double top, double bot, int yscalestart, double yscaledelta, int yoffset, bool mask){
    const int height = bmp.getHeight();
    const int w = bmp.getWidth();
    int movex = 0;
    //double z = 1.0 / z1;
    //const double z_add = ((1.0 / z2) - z) / (y2 - y1);


    Global::debug(3) << "background leftx " << leftx << endl;

    for (int localy = 0; localy < height; ++localy ){
	//int width = bmp.getWidth()*z;
        const double range = (double)localy / (double)height;
	const double scale = interpolate(top, bot, range) - top;
	const int newHeight = height*((yscalestart+(yoffset*yscaledelta))/100);
	const double yscale = (newHeight/height);
	movex = (int)(leftx + (leftx - xoffset) * scale);
	bmp.Stretch(work, 0, localy, w, 1,movex, lefty+localy, w,1);
	//z +=  z_add;
	Global::debug(1) << "Height: " << height << " | yscalestart: " << yscalestart << " | yscaledelta: " << yscaledelta << " | yoffset: " << yoffset << " | New Height: " << newHeight << " | yscale: " << yscale << endl;	
    }
}

static int idCounter = -9999999;

BgController::BgController( ControlType ctrl, std::vector<int>ids ):
type(ctrl),
timestart(0),
endtime(0),
looptime(-1),
ctrlID(ids){
}
BgController::~BgController(){
}
void BgController::act( std::map<int, MugenBackground *> &bgs ){
    // nothing
}

MugenBgController::MugenBgController( std::string &n, std::vector<int>ids ):
name(n),
looptime(-1),
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
visible(true),
enabled(true),
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
    if (enabled){
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
}
    
void MugenBackground::render( const int &totalLength, const int &totalHeight, Bitmap *work ){
    if (visible){
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
		doParallax( *spriteBmp, *work, x, y, xoffset, xscaletop, xscalebot, yscalestart, yscaledelta, (movey-deltay), mask);
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


