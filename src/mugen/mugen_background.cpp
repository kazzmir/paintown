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

struct Tile {
    int start;
    int total;
};

static Tile getTileData( int location, int length, int spacing, int total ){
    Tile tile;
    if (total == 0){
	tile.start = location;
	tile.total = 1;
	return tile;
    } else if (total > 1){
	tile.start = location;
	tile.total = total;
	return tile;
    } else if (total == 1){
	// Infinite tiling.. just tile on the board
	if (location < 0){
	    // Less than the board itself lets get location up to that point
	    while (location < 0){
		location+=spacing;
	    }
	    // Now backup 1 so we get the wrap effect 
	    location-=spacing;
	} else{
	    // Either Larger than the board or inside seek back to beginning
	    while (location > 0){
		location-=spacing;
	    }
	}
	// Now figure out how many we need to do
	int temp = location;
	// Reuse total
	total = 0;
	while (temp < length){
	    total++;
	    temp+=spacing;
	}
	// Blammo
	tile.start = location;
	tile.total = total;
	return tile;
    }
    tile.start = 0;
    tile.total = 0;
    return tile;
}

static void doParallax(Bitmap &bmp, Bitmap &work, int leftx, int lefty, int xoffset, double top, double bot, int yscalestart, double yscaledelta, double yoffset, bool mask){
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
	//const double newHeight = height*((yscalestart+(yoffset*yscaledelta))/100);
	//const double yscale = (newHeight/height);
	movex = (int)(leftx + (leftx - xoffset) * scale);
	bmp.Stretch(work, 0, localy, w, 1,movex, lefty+localy, w,1);
	//z +=  z_add;
	//Global::debug(1) << "Height: " << height << " | yscalestart: " << yscalestart << " | yscaledelta: " << yscaledelta << " | yoffset: " << yoffset << " | New Height: " << newHeight << " | yscale: " << yscale << endl;	
    }
}

// mugen background
MugenBackground::MugenBackground( const unsigned long int &ticker ):
type(Normal),
groupNumber(-1),
imageNumber(-1),
actionno(-1),
id(0),
layerno(0),
startx(0),
starty(0),
deltax(1),
deltay(1),
trans(None),
alphalow(0),
alphahigh(0),
mask(false),
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
x(0),
y(0),
visible(true),
enabled(true),
controller_offsetx(0),
controller_offsety(0),
sprite(0),
spriteBmp(0),
action(0),
linked(0),
runLink(false){
    // Set window to default
    window.x1 = 0;
    window.y1 = 0;
    window.x2 = 319;
    window.y2 = 239;
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
    
void MugenBackground::logic( const double x, const double y, const double placementx, const double placementy ){
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
	
	if( type == Anim ) action->logic();
	
	this->x = (int)(placementx + xoffset + movex + velx + controller_offsetx + sinx_amp * sin(sinx_angle*sinx_period + sinx_offset));
	this->y = (int)(placementy + yoffset + movey + vely + controller_offsety + siny_amp * sin(siny_angle*siny_period + siny_offset));
    }
}
    
void MugenBackground::render( const double windowx, const double windowy, const int totalLength, const int totalHeight, Bitmap *work ){
    if (visible){
	// Set clipping rect
	work->setClipRect(windowx + window.x1, windowy + window.y1,windowx + window.x2,windowy + window.y2);
	switch( type ){
	    case Normal:{
		// Normal is a sprite
		// Tile it
		const int addw = spriteBmp->getWidth() + tilespacingx;
		const int addh = spriteBmp->getHeight() + tilespacingy;
		Tile tilev = getTileData(y, totalHeight, addh, tiley);
		for (int v = 0; v < tilev.total; ++v){
		    Tile tileh = getTileData(x, totalLength, addw, tilex);
		    for (int h = 0; h < tileh.total; ++h){
			draw( tileh.start, tilev.start, *work);
			tileh.start+=addw;
		    }
		    tilev.start+=addh;
		}
		break;
	    }
	    case Parallax:{
		// This is also a sprite but we must parallax it across the top and bottom to give the illusion of depth
		const int addw = spriteBmp->getWidth() + tilespacingx;
		const int addh = spriteBmp->getHeight() + tilespacingy;
		Tile tilev = getTileData(y, totalHeight, addh, tiley);
		for (int v = 0; v < tilev.total; ++v){
		    Tile tileh = getTileData(x, totalLength, addw, tilex);
		    for (int h = 0; h < tileh.total; ++h){
			doParallax( *spriteBmp, *work, tileh.start, tilev.start, xoffset+((totalLength)/2), xscaletop, xscalebot, yscalestart, yscaledelta, (movey-deltay), mask);
			tileh.start+=addw;
		    }
		    tilev.start+=addh;
		}
		break;
	    }
	    case Anim:{
		// there is no sprite use our action!
		// Tiling action
		const int addw = tilespacingx;
		const int addh = tilespacingy;
		Tile tilev = getTileData(y, totalHeight, addh, tiley);
		for (int v = 0; v < tilev.total; ++v){
		    Tile tileh = getTileData(x, totalLength, addw, tilex);
		    for (int h = 0; h < tileh.total; ++h){
			action->render( tileh.start, tilev.start, *work);
			tileh.start+=addw;
		    }
		    tilev.start+=addh;
		}
		break;
	    }
	    case Dummy:
		// Do nothing
	    default:
		break;
	}
	// Reset clip
	work->setClipRect(0, 0,work->getWidth(),work->getHeight());
    }
}

void MugenBackground::preload( const int xaxis, const int yaxis ){
    // Do positionlink crap
    if (positionlink && !runLink){
	if (linked){
	    linked->setPositionLink(this);
	}
	runLink = true;
    }
    
    if (sprite){
	// Lets load our sprite
	Global::debug(1) << "Name: " << name << " | Mask: " << mask << endl;
	if( spriteBmp == 0 ){
	    spriteBmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength, mask));
	}
	// Set our initial offsets
	xoffset = (xaxis - sprite->x) + startx;
	yoffset = (yaxis - sprite->y) + starty; 
	velx = vely = 0;
	Global::debug(1) << "Using sprite. Name: " << name << " | X: " << sprite->x << " | Y: " << sprite->y << endl;
    } else {
	// Set our initial offsets
	xoffset = (xaxis) + startx;
	yoffset = (yaxis) + starty;
	velx = vely = 0;
    }
}

void MugenBackground::draw( const int ourx, const int oury, Bitmap &work ){
    // This needs to be a switch trans = None, Add, Add1, Sub1, Addalpha
    switch( trans ){
	case Addalpha:{
	    // Need to figure out blend correctly addalpha is given to two locations low and high ?
	    Bitmap::transBlender( 255, 255, 255, alphalow );
	    spriteBmp->drawTrans( ourx, oury, work);
	    break;
	}
	case Add:{
	    // this additive 100% I assume... not totally sure
	    Bitmap::addBlender( 255, 255, 255, 255 );
	    spriteBmp->drawTrans( ourx, oury, work);
	    break;
	}
	case Add1:{
	    // 50%
	    Bitmap::addBlender( 128, 128, 128, 255 );
	    spriteBmp->drawTrans( ourx, oury, work);
	    break;
	}
	case Sub:{
	    // Shadow effect
	    Bitmap::differenceBlender( 128, 128, 128, 255 );
	    spriteBmp->drawTrans( ourx, oury, work);
	    break;
	}
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
    
    //Global::debug(1) << "Positionlinked bg: " << bg->name << " set to x: " << bg->startx << " y: " << bg->starty << endl;
}


