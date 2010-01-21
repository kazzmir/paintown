#include "util/bitmap.h"
#include "mugen/background.h"
#include <math.h>
#include <ostream>
#include <cstring>
#include <string>
#include <list>
#include <algorithm>
#include "globals.h"
#include "mugen_sprite.h"
#include "util/regex.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

using namespace std;
namespace PaintownUtil = ::Util;
using namespace Mugen;

static double interpolate(double f1, double f2, double p){
    return (f1 * (1.0 - p)) + (f2 * p);
}

#if 0
static int ff = 0;
static void doParallax2(const Bitmap &bmp, const Bitmap &work, int leftx, int lefty, int xoffset, double top, double bot, int yscalestart, double yscaledelta, double yoffset, bool mask){
    const int height = bmp.getHeight();
    const int width = bmp.getWidth();
    //double z = 1.0 / z1;
    //const double z_add = ((1.0 / z2) - z) / (y2 - y1);


    Global::debug(3) << "background leftx " << leftx << endl;
    xoffset = 0;

    leftx -= 320;

    // leftx = 0;
    // leftx = ff;
    // xoffset = 50;
    // xoffset = ff - 300;
    ff = (ff + 1) % 600;

    for (int localy = 0; localy < height; ++localy ){
        int movex = 0;
	//int width = bmp.getWidth()*z;
        const double range = (double)localy / (double)height;
	const double scale = interpolate(top, bot, range);
	//const double newHeight = height*((yscalestart+(yoffset*yscaledelta))/100);
	//const double yscale = (newHeight/height);
	// movex = (int)(leftx + (leftx - xoffset) * (scale - top));
	movex = (int)(xoffset + (xoffset - leftx) * (scale - top));
	bmp.Stretch(work, 0, localy, width, 1, movex, lefty+localy, width, 1);
        // bmp.BlitMasked(0, localy, width, 1, movex, lefty + localy, work);
        // bmp.Blit(0, localy, width, 1, movex, lefty + localy, work);
	//z +=  z_add;
	//Global::debug(1) << "Height: " << height << " | yscalestart: " << yscalestart << " | yscaledelta: " << yscaledelta << " | yoffset: " << yoffset << " | New Height: " << newHeight << " | yscale: " << yscale << endl;	
    }
}
#endif


BackgroundElement::BackgroundElement():
deltaX(1),
deltaY(1),
window(0,0,319,239),
windowDeltaX(0),
windowDeltaY(0),
positionLink(false),
velocityX(0),
velocityY(0),
linkedElement(0){
}

BackgroundElement::BackgroundElement(const BackgroundElement &copy){
    this->setID(copy.getID());
    this->setLayer(copy.getLayer());
    this->setName(copy.getName());
    this->start = copy.start;
    this->deltaX = copy.deltaX;
    this->deltaY = copy.deltaY;
    this->effects = copy.effects;
    this->tile = copy.tile;
    this->tileSpacing = copy.tileSpacing;
    this->window = copy.window;
    this->windowDeltaX = copy.windowDeltaX;
    this->windowDeltaY = copy.windowDeltaY;
    this->positionLink = copy.positionLink;
    this->velocityX = copy.velocityX;
    this->velocityY = copy.velocityY;
    this->sinX = copy.sinX;
    this->sinY = copy.sinY;
    this->linkedElement = copy.linkedElement;
}
BackgroundElement::~BackgroundElement(){
}

/* Copy the contents of this into element
 * If this element is linked then we will call it recursively until we've come to the end of the chain
 * and then copy the contents of the last link of the chain into our passed element
 */
void BackgroundElement::setLink(BackgroundElement *element){
    element->linkedElement = this;
    if (positionLink){
	if (linkedElement){
	    linkedElement->setLink(element);
	    return;
	}
    }
    Mugen::Point newStart = element->getStart();
    newStart.x += start.x;
    newStart.y += start.y;
    element->setStart(newStart);
    element->setDelta(deltaX, deltaY);
    element->setVelocity(velocityX, velocityY);
    element->setSinX(sinX);
    element->setSinY(sinY);
}

// Copy operator so we can make an initial copy of are starting values to restore on a reset
const BackgroundElement & BackgroundElement::operator=(const BackgroundElement &copy){
    this->setID(copy.getID());
    this->setLayer(copy.getLayer());
    this->setName(copy.getName());
    this->start = copy.start;
    this->deltaX = copy.deltaX;
    this->deltaY = copy.deltaY;
    this->effects = copy.effects;
    this->tile = copy.tile;
    this->tileSpacing = copy.tileSpacing;
    this->window = copy.window;
    this->windowDeltaX = copy.windowDeltaX;
    this->windowDeltaY = copy.windowDeltaY;
    this->positionLink = copy.positionLink;
    this->velocityX = copy.velocityX;
    this->velocityY = copy.velocityY;
    this->sinX = copy.sinX;
    this->sinY = copy.sinY;
    this->linkedElement = copy.linkedElement;
    return *this;
}


NormalElement::NormalElement():
sprite(0){
}
NormalElement::~NormalElement(){
}
void NormalElement::act(){
    getSinX().act();
    getSinY().act();
}
void NormalElement::render(int cameraX, int cameraY, const Bitmap &bmp){
    const int addw = sprite->getWidth() + getTileSpacing().x;
    const int addh = sprite->getHeight() + getTileSpacing().y;
    const int currentX = (bmp.getWidth()/2) + (int) ((getStart().x - cameraX + getVelocityX() + getSinX().get()) + cameraX * (1 - getDeltaX()));
    const int currentY = (int) ((getStart().y - cameraY + getVelocityY() + getSinY().get()) + cameraY * (1 - getDeltaY()));
    const int windowAddX = (int) (getWindowDeltaX() * cameraX);
    const int windowAddY = (int) (getWindowDeltaY() * cameraY);

    // const int currentX = (bmp.getWidth()/2) + int((getStart().x + cameraX + getVelocityX() + getSinX().get()) * getDeltaX());
    // const int currentY =  int((getStart().y + y + getVelocityY() + getSinY().get()) * getDeltaY());
    // const int currentY = (int) (getStart().y - cameraY);
    
    // Set the clipping window
    bmp.setClipRect( getWindow().x + windowAddX, getWindow().y + windowAddY, getWindow().getX2() + windowAddX, getWindow().getY2() + windowAddY );
    // Render initial sprite
    sprite->render(currentX, currentY, bmp, getEffects());
    // Do tiling
    if (getTile().x > 1){
	int next = currentX+addw;
	// Tile set amount of times but only forward
	for (int t = 0; t < getTile().x; ++t){
	    sprite->render(next, currentY, bmp, getEffects());
	    next+=addw;
	}
    } else if (getTile().x == 1){
	// infinite tiling goes in both directions
	int next = currentX+addw;
	int prev = currentX-addw;
	bool prevDone = false;
	bool nextDone = false;
	while (!prevDone && !nextDone){
	    if (!nextDone){
		sprite->render(next, currentY, bmp, getEffects());
	    }
	    if (!prevDone){
		sprite->render(prev, currentY, bmp, getEffects());
	    }
	    next+=addw;
	    prev-=addw;
	    // If we have gone passed the viewport, no need to draw
	    // *FIXME Not so correct, need to figure this one out
	    if (next >= bmp.getWidth()){
		nextDone = true;
	    }
	    if (prev <= 0){
		prevDone = true;
	    }
	}
    }
    if (getTile().y > 1){
	int next = currentY+addh;
	// Tile set amount of times but only forward
	for (int t = 0; t < getTile().y; ++t){
	    sprite->render(currentX, next, bmp, getEffects());
	    next+=addh;
	}
    } else if (getTile().y == 1){
	// infinite tiling goes in both directions
	int next = currentY+addh;
	int prev = currentY-addh;
	bool prevDone = false;
	bool nextDone = false;
	while (!prevDone && !nextDone){
	    if (!nextDone){
		sprite->render(currentX, next, bmp, getEffects());
	    }
	    if (!prevDone){
		sprite->render(currentX, prev, bmp, getEffects());
	    }
	    next+=addh;
	    prev-=addh;
	    // If we have gone passed the viewport, no need to draw
	    // *FIXME Not so correct, need to figure this one out
	    if (next >= bmp.getHeight()){
		nextDone = true;
	    }
	    if (prev <= 0){
		prevDone = true;
	    }
	}
    }
    // Reset clip state
    bmp.setClipRect(0, 0,bmp.getWidth(),bmp.getHeight());
}

AnimationElement::AnimationElement(std::map< int, MugenAnimation * >  & animations):
animation(0),
animations(animations){
}
AnimationElement::~AnimationElement(){
}
void AnimationElement::act(){
    animations[animation]->logic();
    getSinX().act();
    getSinY().act();
}
void AnimationElement::render(int cameraX, int cameraY, const Bitmap &bmp){
    const int addw = getTileSpacing().x;
    const int addh = getTileSpacing().y;

    // const int currentX = (bmp.getWidth()/2) + int((getStart().x - cameraX + getVelocityX() + getSinX().get()) * getDeltaX());
    // const int currentY =  int((getStart().y - cameraY + getVelocityY() + getSinY().get()) * getDeltaY());

    const int currentX = (bmp.getWidth()/2) + (int) ((getStart().x - cameraX + getVelocityX() + getSinX().get()) + cameraX * (1 - getDeltaX()));
    const int currentY = (int) ((getStart().y - cameraY + getVelocityY() + getSinY().get()) + cameraY * (1 - getDeltaY()));
    const int windowAddX = (int) (getWindowDeltaX() * cameraX);
    const int windowAddY = (int) (getWindowDeltaY() * cameraY);

    // Set the clipping window
    bmp.setClipRect( getWindow().x + windowAddX, getWindow().y + windowAddY, getWindow().getX2() + windowAddX, getWindow().getY2() + windowAddY );
    // Render initial animation
    animations[animation]->render(currentX, currentY, bmp);
    // Do tiling
    if (getTile().x > 1){
	int next = currentX+addw;
	// Tile set amount of times but only forward
	for (int t = 0; t < getTile().x; ++t){
	    animations[animation]->render(next, currentY, bmp);
	    next+=addw;
	}
    } else if (getTile().x == 1){
	// infinite tiling goes in both directions
	int next = currentX+addw;
	int prev = currentX-addw;
	bool prevDone = false;
	bool nextDone = false;
	while (!prevDone && !nextDone){
	    if (!nextDone){
		animations[animation]->render(next, currentY, bmp);
	    }
	    if (!prevDone){
		animations[animation]->render(prev, currentY, bmp);
	    }
	    next+=addw;
	    prev-=addw;
	    // If we have gone passed the viewport, no need to draw
	    // *FIXME Not so correct, need to figure this one out
	    if (next >= bmp.getWidth()){
		nextDone = true;
	    }
	    if (prev <= 0){
		prevDone = true;
	    }
	}
    }
    if (getTile().y > 1){
	int next = currentY+addh;
	// Tile set amount of times but only forward
	for (int t = 0; t < getTile().y; ++t){
	    animations[animation]->render(currentX, next, bmp);
	    next+=addh;
	}
    } else if (getTile().y == 1){
	// infinite tiling goes in both directions
	int next = currentY+addh;
	int prev = currentY-addh;
	bool prevDone = false;
	bool nextDone = false;
	while (!prevDone && !nextDone){
	    if (!nextDone){
		animations[animation]->render(currentX, next, bmp);
	    }
	    if (!prevDone){
		animations[animation]->render(currentX, prev, bmp);
	    }
	    next+=addh;
	    prev-=addh;
	    // If we have gone passed the viewport, no need to draw
	    // *FIXME Not so correct, need to figure this one out
	    if (next >= bmp.getHeight()){
		nextDone = true;
	    }
	    if (prev <= 0){
		prevDone = true;
	    }
	}
    }
    // Reset clip state
    bmp.setClipRect(0, 0,bmp.getWidth(),bmp.getHeight());
}

ParallaxElement::ParallaxElement():
BackgroundElement(),
sprite(0),
xscaleX(0),
xscaleY(0),
yscale(100),
yscaleDelta(0){
}

ParallaxElement::~ParallaxElement(){
}

void ParallaxElement::act(){
}

static void doParallax(const Bitmap & bmp, const Bitmap & work, int cameraX, int cameraY, int offsetX, int offsetY, double xscale_top, double xscale_bottom, int centerX, int centerY, double deltaX, double deltaY){
    const int height = bmp.getHeight();
    const int width = bmp.getWidth();

    // bmp.draw(offsetX + cameraX - width / 2, offsetY + cameraY - height / 2, work);
    // bmp.draw(centerX - width / 2 + offsetX, centerY + offsetY, work);
    
    // double x = centerX - width / 2 + offsetX - cameraX;
    double x = centerX + offsetX - cameraX;
    int y = centerY + offsetY - cameraY;

    // Global::debug(0) << "Camera x is " << cameraX << " x is " << x << " delta top " << delta_top << " bottom " << delta_bottom << endl;

    for (int liney = 0; liney < height; liney++){
        int movex = 0;
	//int width = bmp.getWidth()*z;
        const double range = (double)liney / (double)height;
	const double scale = interpolate(xscale_top, xscale_bottom, range);
	// movex = (int)(x - cameraX * (scale - xscale_top) * 0.78);
	movex = (int)(x - cameraX * (scale - xscale_top) + cameraX * (1 - deltaX) * scale);
        int movey = (int)(y + liney + cameraY * (1 - deltaY));
	// movex = (int)(x - cameraX * (scale * delta));

	// bmp.Stretch(work, 0, liney, width, 1, movex, y + liney, width * scale, 1);
        // bmp.BlitMasked(0, liney, width, 1, movex, y + liney, work);
        bmp.BlitMasked(0, liney, width, 1, movex, movey, work);

        // bmp.Blit(0, localy, width, 1, movex, lefty + localy, work);
	//z +=  z_add;
	//Global::debug(1) << "Height: " << height << " | yscalestart: " << yscalestart << " | yscaledelta: " << yscaledelta << " | yoffset: " << yoffset << " | New Height: " << newHeight << " | yscale: " << yscale << endl;	
    }

}

void ParallaxElement::render(int cameraX, int cameraY, const Bitmap & work){

    const Bitmap & show = *sprite->getBitmap();
    const int windowAddX = (int) (getWindowDeltaX() * cameraX);
    const int windowAddY = (int) (getWindowDeltaY() * cameraY);

    // Set the clipping window
    work.setClipRect( getWindow().x + windowAddX, getWindow().y + windowAddY, getWindow().getX2() + windowAddX, getWindow().getY2() + windowAddY );

    // Global::debug(0) << "Clip rect " << (getWindow().x + windowAddX) << ", " << (getWindow().y + windowAddY) << " " << (getWindow().getX2() + windowAddX) << ", " << (getWindow().getY2() + windowAddY) << endl;
   
    /* Remember only do either or if xscale is set then ignore width
     * otherwise do width.
     */
    if (xscaleX || xscaleY){
	doParallax(show, work, cameraX, cameraY, getStart().x - sprite->getX(), getStart().y - sprite->getY(), xscaleX, xscaleY, work.getWidth()/2, 0, getDeltaX(), getDeltaY());
    } else {
	doParallax(show, work, cameraX, cameraY, getStart().x, getStart().y, width.x, width.y, work.getWidth()/2, 0, getDeltaX(), getDeltaY());
    }
    // Reset clip state
    work.setClipRect(0, 0,work.getWidth(),work.getHeight());
#if 0
    /* FIXME */
    const int addw = sprite->getWidth() + getTileSpacing().x;
    const int addh = sprite->getHeight() + getTileSpacing().y;
    int x = 0;
    int y = 0;
    Tile tilev = getTileData(x, 1, addh, getTile().y);
    for (int v = 0; v < tilev.total; ++v){
        Tile tileh = getTileData(y, 1, addw, getTile().x);
        for (int h = 0; h < tileh.total; ++h){
                if (xscaleX && xscaleY){
                    doParallax(show, work, cameraX, cameraY, getStart().x, getStart().y, xscaleX, xscaleY, MAP_X, MAP_Y, getDeltaX(), getDeltaY());
                } else {
                    // doParallax2( *sprite->getBitmap(), bmp, tileh.start, tilev.start, MAP_X, width.x, width.y, yscale, yscaleDelta, currentX, getMask());
                }
                tileh.start += addw;
        }
        tilev.start += addh;
    }
#endif
    /*
    const int addw = sprite->getWidth() + getTileSpacing().x;
    const int addh = sprite->getHeight() + getTileSpacing().y;
    const int currentX = MAP_X + int((getStart().x + cameraX + getVelocityX()) * getDeltaX());
    const int currentY = MAP_Y + int((getStart().y + cameraY + getVelocityY()) * getDeltaY());
    Tile tilev = getTileData(currentY, 1, addh, getTile().y);
    for (int v = 0; v < tilev.total; ++v){
        Tile tileh = getTileData(currentX, 1, addw, getTile().x);
        for (int h = 0; h < tileh.total; ++h){
                if (xscaleX && xscaleY){
                    doParallax2( *sprite->getBitmap(), bmp, tileh.start, tilev.start,MAP_X, xscaleX, xscaleY, yscale, yscaleDelta, currentX, getMask());
                } else {
                    doParallax2( *sprite->getBitmap(), bmp, tileh.start, tilev.start, MAP_X, width.x, width.y, yscale, yscaleDelta, currentX, getMask());
                }
                tileh.start+=addw;
        }
        tilev.start+=addh;
    }
    */
}

DummyElement::DummyElement(){
}

DummyElement::~DummyElement(){
}

void DummyElement::act(){
}

void DummyElement::render(int x, int y, const Bitmap &bmp){
}

//! Type of element
enum ElementType{
    Normal,
    Parallax,
    Anim,
    Dummy,
};

/* TODO: search for the type and create the element first. Then pass the section
 * to the element to do the rest of the parsing.
 */
static BackgroundElement *getElement( Ast::Section *section, Mugen::SpriteMap &sprites, std::map< int, MugenAnimation * > &animations ){
    BackgroundElement *element = 0;
    ElementType elementType = Normal;
    std::string head = section->getName();
    head = PaintownUtil::captureRegex(head, ".*[bB][gG] (.*)", 0);
    std::string name = head;
    Global::debug(1) << "Found background element: " << name << endl;
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "type"){
                std::string type;
                *simple >> type;
                type = Mugen::Util::removeSpaces(type);
                if (type == "normal" ){
		    elementType = Normal;
		    element = new NormalElement();
		} else if( type == "anim" ){
		    elementType = Anim;
		    element = new AnimationElement(animations);
		} else if( type == "parallax" ){
		    elementType = Parallax;
		    element = new ParallaxElement();
		} else if( type == "dummy" ){ 
		    elementType = Dummy;
		    element = new DummyElement();
		}
		element->setName(name);
            } else if (*simple == "spriteno"){
                int group, sprite;
                *simple >> group >> sprite;
		if (elementType == Normal){
		    ((NormalElement *)element)->setSprite(sprites[group][sprite]);
                } else if (elementType == Parallax){
		    ((ParallaxElement *)element)->setSprite(sprites[group][sprite]);
		}
            } else if (*simple == "actionno"){
		if (elementType == Anim){
		    int action;
		    *simple >> action;
		    ((AnimationElement *)element)->setAnimation(action);
		}
            } else if (*simple == "id"){
		int id;
                *simple >> id;
		element->setID(id);
            } else if (*simple == "layerno"){
		int layerno;
                *simple >> layerno;
		if (layerno == 0){
		    element->setLayer(Element::Background);
		} else if (layerno == 1){
		    element->setLayer(Element::Foreground);
		}
            } else if (*simple == "start"){
		Mugen::Point point;
		try {
		    *simple >> point.x >> point.y;
		} catch (const Ast::Exception & e){
		}
		element->setStart(point);
            } else if (*simple == "delta"){
		double x=0,y=0;
		try {
		    *simple >> x >> y;
                    /* the y part is not always given */
                } catch (const Ast::Exception & e){
                }
		element->setDelta(x,y);
            } else if (*simple == "trans"){
                std::string type;
                *simple >> type;
                type = Mugen::Util::removeSpaces(type);
		TransType trans;
                if( type == "none" ){
		    trans = NONE;
		} else if( type == "add" ){
		    trans =  ADD;
		} else if( type == "add1" ){
		    trans = ADD1;
		} else if( type == "sub" ){
		    trans = SUB;
		} else if( type == "addalpha" ){
		    trans = ADDALPHA;
		}
		element->setTrans(trans);
            } else if (*simple == "alpha"){
		int l = 0,h=0;
                try{
                    *simple >> l >> h;
                } catch (const Ast::Exception & e){
                }
		element->setAlpha(l,h);
            } else if (*simple == "mask"){
		bool mask;
                *simple >> mask;
		element->setMask(mask);
            } else if (*simple == "tile"){
		Mugen::Point point;
                try{
                    *simple >> point.x >> point.y;
                } catch (const Ast::Exception & e){
                }
		element->setTile(point);
            } else if (*simple == "tilespacing"){
		Mugen::Point point;
                try{
                    *simple >> point.x >> point.y;
                } catch (const Ast::Exception & e){
                }
		element->setTileSpacing(point);
            } else if (*simple == "window"){
                int x1,y1,x2,y2;
                *simple >> x1 >> y1 >> x2 >> y2;
		element->setWindow(x1,y1,x2,y2);
            } else if (*simple == "windowdelta"){
		double x,y;
                *simple >> x >> y;
		element->setWindowDelta(x,y);
            } else if (*simple == "xscale"){
                if (elementType == Parallax){
		    double x, y;
		    *simple >> x >> y;
		    ((ParallaxElement *)element)->setXScale(x,y);
		}
            } else if (*simple == "width"){
                if (elementType == Parallax){
		    Mugen::Point point;
		    *simple >> point.x >> point.y;
		    ((ParallaxElement *)element)->setWidth(point);
		}
            } else if (*simple == "yscalestart"){
                if (elementType == Parallax){
		    double x;
		    *simple >> x;
		    ((ParallaxElement *)element)->setYScale(x);
		}
            } else if (*simple == "yscaledelta"){
                if (elementType == Parallax){
		    double x;
		    *simple >> x;
		    ((ParallaxElement *)element)->setYScaleDelta(x);
		}
            } else if (*simple == "positionlink"){
		bool link;
                *simple >> link;
		element->setPositionLink(link);
            } else if (*simple == "velocity"){
                double x=0,y=0;
		try{
		    *simple >> x >> y;
                } catch (const Ast::Exception & e){
                }
		element->setVelocity(x,y);
            } else if (*simple == "sin.x"){
		Sin x;
                try{
                    *simple >> x.amp >> x.period >> x.offset;
                } catch (const Ast::Exception & e){
                }
		element->setSinX(x);
            } else if (*simple == "sin.y"){
                Sin y;
                try{
                    *simple >> y.amp >> y.period >> y.offset;
                } catch (const Ast::Exception & e){
                }
		element->setSinY(y);
            } else throw MugenException( "Unhandled option in BG " + head + " Section: " + simple->toString());
        }
    }

    // Global::debug(0) << "Parsed element window as " << element->getWindow().getX1() << ", " << element->getWindow().getY1() << " " << element->getWindow().getX2() << ", " << element->getWindow().getY2() << endl;
    
    return element;
}

/* Background Controller */
Controller::Controller(Ast::Section *section):
name(""),
timeStart(0),
endTime(0),
loopTime(0),
ticker(0){
}

Controller::~Controller(){
}

BackgroundController::BackgroundController(Ast::Section *section):
globalLooptime(-1),
ticker(0){
    std::string head = section->getName();
    head = PaintownUtil::captureRegex(head, ".*bgctrldef", 0);
    std::string name = head;
    Global::debug(1) << "Found background controller definition: " << name << endl;
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
	 Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "type"){
	    }
	}
    }
}

BackgroundController::~BackgroundController(){
}

void BackgroundController::act(){
}




Background::Background(const std::string &file, const std::string &header):
file(file),
header(header),
debug(false),
clearColor(-1){
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed(Mugen::Util::parseDef(file));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + file + " in" + diff.printTime("") << endl;
    
    // for linked position in backgrounds
    BackgroundElement *priorElement = 0;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;;
	std::string head = section->getName();
	head = Mugen::Util::fixCase(head);
	std::string tempHeader = Mugen::Util::fixCase(header);
	Global::debug(1) << "Checking head: " << head << " for Header: " << tempHeader << endl;
        // Lets check if this is a storyboard so we can get our sprite file, otherwise treat it as a normal background */
        if ( head == "scenedef") {
            class SceneDefWalker: public Ast::Walker{
                public:
                    SceneDefWalker(Background & self, Mugen::SpriteMap & sprites):
                        self(self),
                        sprites(sprites){
                        }
                    Background & self;
                    Mugen::SpriteMap & sprites;
                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
			if (simple == "spr"){
			    simple >> self.spriteFile;
			    Global::debug(1) << "Reading Sff (sprite) Data..." << endl;
			    // Strip it of any directory it might have
			    std::string baseDir = Util::getFileDir(self.file);
			    self.spriteFile = Mugen::Util::stripDir(self.spriteFile);
			    Global::debug(1) << "Sprite File: " << self.spriteFile << endl;
			    Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(baseDir, self.spriteFile), "", sprites);
			} 
		    }
            };
            SceneDefWalker walker(*this,sprites);
            section->walk(walker);
        } else if (PaintownUtil::matchRegex(head, ".*" + tempHeader + "def.*")){
	    class BackgroundDefWalker: public Ast::Walker{
		public:
		    BackgroundDefWalker(Background & self, Mugen::SpriteMap & sprites):
			self(self),
			sprites(sprites){
			}
		    Background & self;
		    Mugen::SpriteMap & sprites;

		    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
			if (simple == "spr"){
			    simple >> self.spriteFile;
			    Global::debug(1) << "Reading Sff (sprite) Data..." << endl;
			    // Strip it of any directory it might have
			    std::string baseDir = Util::getFileDir(self.file);
			    self.spriteFile = Mugen::Util::stripDir(self.spriteFile);
			    Global::debug(1) << "Sprite File: " << self.spriteFile << endl;
			    Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(baseDir, self.spriteFile), "", sprites);
			} else if (simple == "debugbg"){
			    simple >> self.debug;
			} else if (simple == "bgclearcolor"){
			    int r, g, b;
			    simple >> r >> g >> b;
			    self.clearColor = Bitmap::makeColor(r,g,b);
			} else {
			    throw MugenException("Unhandled option in Background Definition Section: " + simple.toString());
			}
		    }
            };
            BackgroundDefWalker walker(*this,sprites);
            section->walk(walker);
	// This our background data definitions
        /* probably need a better regex here */
	} else if (PaintownUtil::matchRegex(head, ".*" + tempHeader + " ")){
	    BackgroundElement *element;
	    element = getElement(*section_it, sprites, animations);
	    
	    // Background or Forgeground?
	    if (element->getLayer() == Element::Background){
                backgrounds.push_back(element);
            } else if (element->getLayer() == Element::Foreground){
                foregrounds.push_back(element);
            }
            // Lets check if this element is linked to the prior one
            if (element->getPositionLink()){
                // Ensure prior exists, usually won't happen though as you can't link to any objects if there are non preceeding this one
                if (priorElement){
                    // use prior element to set the current elements properties
	            priorElement->setLink(element);
                }
            }
            priorElement = element;
	} else if(PaintownUtil::matchRegex(head, "begin *action")){
            /* This creates the animations it differs from character animation since these are included in the stage.def file with the other defaults */
	    head.replace(0,13,"");
	    int h;
            istringstream out(head);
	    out >> h;
	    animations[h] = Mugen::Util::getAnimation(section, sprites);
	} else if (PaintownUtil::matchRegex(head, ".*bgctrldef")){
#if 0
	    head.replace(0,10,"");
	    MugenBackgroundController *temp = new MugenBackgroundController(head);
	    Global::debug(1) << "Found background controller definition: " << temp->name << endl;
            class BackgroundControllerWalker: public Ast::Walker {
            public:
                BackgroundControllerWalker(MugenBackgroundManager & manager, MugenBackgroundController * controller, vector<MugenBackground*> & backgrounds, vector<MugenBackground*> & foregrounds):
                    hasId(false),
                    manager(manager),
                    controller(controller),
                    backgrounds(backgrounds),
                    foregrounds(foregrounds){
                }

                bool hasId;
                MugenBackgroundManager & manager;
                MugenBackgroundController * controller;
                vector<MugenBackground*> & backgrounds;
                vector<MugenBackground*> & foregrounds;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "eventid"){
		        simple >> controller->id;
                    } else if (simple == "looptime"){
                        simple >> controller->looptime;
                        if (controller->looptime == 0){
                            controller->looptime = -1;
                        }
                    } else if (simple == "ctrlid"){
                        hasId = true;
                        // Max 10
                        try{
                            while (true){
                                int id;
                                simple >> id;
                                manager.getBackgrounds(controller->backgrounds, id);
                            }
                        } catch (const Ast::Exception & e){
                        }
                    } else {
                        string name; // = controller->getName()
                        throw MugenException("Unhandled option in BGCtrlDef " + name + " Section: " + simple.toString());
                    }
                }

                virtual ~BackgroundControllerWalker(){
                    if (!hasId && controller->backgrounds.empty()){
                        controller->backgrounds.insert(controller->backgrounds.end(), backgrounds.begin(), backgrounds.end());
                        controller->backgrounds.insert(controller->backgrounds.end(), foregrounds.begin(), foregrounds.end());
                    }
                }
            };

            {
                BackgroundControllerWalker walker(*this, temp, backgrounds, foregrounds);
                Ast::Section * section = *section_it;
                section->walk(walker);
            }
	    Global::debug(1) << "Controlling total backgrounds: " << temp->backgrounds.size() << endl;
	    controllers.push_back(temp);
#endif
	} else if (PaintownUtil::matchRegex(head, ".*bgctrl")){
#if 0
	    if (controllers.empty()){
		/* This is a hack to get mugen to do some fancy controlling in a regular
                 * game to accomplish stage fatalities and other tricks
                 */
		Global::debug(1) << "Found a BgCtrl without a parent definition... must be hackery!" << endl;
		continue;
	    }

	    // else we got ourselves some controls... under the last controller added
	    MugenBackgroundController *control = controllers.back();
	    head.replace(0,7,"");
	    BackgroundController *temp = new BackgroundController();
	    temp->name = head;

            class CtrlWalker: public Ast::Walker {
            public:
                CtrlWalker(BackgroundController * controller, MugenBackgroundController * control, MugenBackgroundManager & manager):
                    controller(controller),
                    control(control),
                    manager(manager){
                }

                BackgroundController * controller;
                MugenBackgroundController * control;
                MugenBackgroundManager & manager;
                
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "type"){
                        string type;
                        simple >> type;
                        type = Mugen::Util::fixCase(type);
                        if (type == "Anim") controller->type = Ctrl_Animation;
                        else if( type == "enabled" ) controller->type = Ctrl_Enabled;
                        else if( type == "null" ) controller->type = Ctrl_Null;
                        else if( type == "posadd" ) controller->type = Ctrl_PosAdd;
                        else if( type == "posset" ) controller->type = Ctrl_PosSet;
                        else if( type == "sinx" ) controller->type = Ctrl_Sinx;
                        else if( type == "siny" ) controller->type = Ctrl_Siny;
                        else if( type == "veladd" ) controller->type = Ctrl_VelAdd;
                        else if( type == "velset" ) controller->type = Ctrl_VelSet;
                        else if( type == "visible" ) controller->type = Ctrl_Visible;
                    } else if (simple == "time"){
                        int start = 0, end =0, loop = 0;
                        try{
                            simple >> start;
                            simple >> end;
                            simple >> loop;
                        } catch (const Ast::Exception & e){
                        }

                        controller->timestart = start;
                        if (end == 0){
                            controller->endtime = start;
                        } else {
                            controller->endtime = end;
                        }

                        if (loop == 0){
                            controller->looptime = -1;
                        } else {
                            controller->looptime = loop;
                        }

                        Global::debug(1) << "start: " << controller->timestart << " | end: " << controller->endtime << " | loop: " << controller->looptime << endl;
                    } else if (simple == "value"){
                        simple >> controller->value1;
                        simple >> controller->value2;		    
                        simple >> controller->value3;
                    } else if (simple == "x"){
                        simple >> controller->value1;
                    } else if (simple == "y"){
                        simple >> controller->value2;
                    } else if (simple == "ctrlid"){
                        try{
                            // Max 10
                            while (true){
                                int id;
                                simple >> id;
                                manager.getBackgrounds(controller->backgrounds, id);
                            }
                        } catch (const Ast::Exception & e){
                        }
                    } else {
                        string name;
                        throw MugenException( "Unhandled option in BGCtrl " + name + " Section: " + simple.toString());
                    }
                }

                virtual ~CtrlWalker(){
                    if (controller->backgrounds.empty()){
                        controller->backgrounds.insert(controller->backgrounds.end(), control->backgrounds.begin(), control->backgrounds.end());
                    }
                }
            };

            {
                CtrlWalker walker(temp, control, *this);
                Ast::Section * section = *section_it;
                section->walk(walker);
            }

	    Global::debug(1) << "Controlling total backgrounds: " << temp->backgrounds.size() << endl;
	    control->addControl(temp);
#endif
	}
    }
    
}
Background::~Background(){
    // Get rid of sprites
    for( Mugen::SpriteMap::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
	for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	    if( j->second )delete j->second;
	}
    }
     // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    // Backgrounds
    for( vector< BackgroundElement *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	BackgroundElement *element = *i;
	if (element){
            delete element;
        }
    }
    
     // Foregrounds
    for( vector< BackgroundElement *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	BackgroundElement *element = *i;
	if (element){
            delete element;
        }
    }
}
void Background::act(){
    // Backgrounds
    for( vector< BackgroundElement *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	BackgroundElement *element = *i;
	element->act();
    }
    
     // Foregrounds
    for( vector< BackgroundElement *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	BackgroundElement *element = *i;
	element->act();
    }
}
void Background::renderBackground(int x, int y, const Bitmap &bmp){
    if ( clearColor != -1){
	bmp.fill(clearColor);
    }
	// debug overrides it
    if ( debug ){
	bmp.fill( Bitmap::makeColor(255,0,255) );
    }
    for( vector< BackgroundElement *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	BackgroundElement *element = *i;
	element->render(x, y, bmp);
    }
}
void Background::renderForeground(int x, int y, const Bitmap &bmp){
    for( vector< BackgroundElement *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	BackgroundElement *element = *i;
	element->render(x, y, bmp);
    }
}

//! Returns a vector of Elements by given ID usefull for when assigning elements to a background controller
std::vector< BackgroundElement * > Background::getIDList(int ID){
    std::vector< BackgroundElement *> ourElements;
    // Do backgrounds
    for( vector< BackgroundElement *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	BackgroundElement *element = *i;
	if (element->getID() == ID){
            ourElements.push_back(element);
        }
    }
    
     // do foregrounds
    for( vector< BackgroundElement *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	BackgroundElement *element = *i;
	if (element->getID() == ID){
            ourElements.push_back(element);
        }
    }

    return ourElements;
}


