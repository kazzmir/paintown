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

namespace Mugen{

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


BackgroundElement::BackgroundElement(const string & name, Ast::Section * data):
visible(true),
enabled(true),
x(0),
y(0),
deltaX(1),
deltaY(1),
window(0,0,319,239),
windowDeltaX(0),
windowDeltaY(0),
positionLink(false),
velocityX(0),
velocityY(0),
linkedElement(0){
    setName(name);

    class Walker: public Ast::Walker{
    public:
        Walker(BackgroundElement & self):
            self(self){
            }

        BackgroundElement & self;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "type"){
                /* ignore.. */
            } else if (simple == "id"){
		int id;
                simple >> id;
		self.setID(id);
            } else if (simple == "layerno"){
		int layerno;
                simple >> layerno;
		if (layerno == 0){
		    self.setLayer(Element::Background);
		} else if (layerno == 1){
		    self.setLayer(Element::Foreground);
		}
            } else if (simple == "start"){
		Mugen::Point point;
		try {
		    simple >> point.x >> point.y;
		} catch (const Ast::Exception & e){
		}
		self.setStart(point);
            } else if (simple == "delta"){
		double x = 0, y = 0;
		try {
		    simple >> x >> y;
                    /* the y part is not always given */
                } catch (const Ast::Exception & e){
                }
		self.setDelta(x,y);
            } else if (simple == "trans"){
                std::string type;
                simple >> type;
                type = Mugen::Util::fixCase(Mugen::Util::removeSpaces(type));
		TransType trans = NONE;
                if (type == "none" ){
		    trans = NONE;
		} else if (type == "add"){
		    trans = ADD;
		} else if (type == "add1"){
		    trans = ADD1;
		} else if (type == "sub"){
		    trans = SUB;
		} else if (type == "addalpha"){
		    trans = ADDALPHA;
		}
		self.setTrans(trans);
            } else if (simple == "alpha"){
		int l = 0,h=0;
                try{
                    simple >> l >> h;
                } catch (const Ast::Exception & e){
                }
		self.setAlpha(l,h);
            } else if (simple == "mask"){
		bool mask;
                simple >> mask;
		self.setMask(mask);
            } else if (simple == "tile"){
		Mugen::Point point;
                try{
                    simple >> point.x >> point.y;
                } catch (const Ast::Exception & e){
                }
		self.setTile(point);
            } else if (simple == "tilespacing"){
		Mugen::Point point;
                try{
                    simple >> point.x >> point.y;
                } catch (const Ast::Exception & e){
                }
		self.setTileSpacing(point);
            } else if (simple == "window"){
                int x1=0,y1=0,x2=319,y2=239;
                try {
                    simple >> x1 >> y1 >> x2 >> y2;
		} catch (const Ast::Exception & e){
		}
		self.setWindow(x1,y1,x2,y2);
            } else if (simple == "windowdelta"){
		double x=0,y=0;
                try{
                    simple >> x >> y;
                } catch (const Ast::Exception & e){
                }
		self.setWindowDelta(x,y);
            } else if (simple == "positionlink"){
		bool link;
                simple >> link;
		self.setPositionLink(link);
            } else if (simple == "velocity"){
                double x=0,y=0;
		try{
		    simple >> x >> y;
                } catch (const Ast::Exception & e){
                }
		self.setVelocity(x,y);
            } else if (simple == "sin.x"){
		Sin x;
                try{
                    simple >> x.amp >> x.period >> x.offset;
                } catch (const Ast::Exception & e){
                }
		self.setSinX(x);
            } else if (simple == "sin.y"){
                Sin y;
                try{
                    simple >> y.amp >> y.period >> y.offset;
                } catch (const Ast::Exception & e){
                }
		self.setSinY(y);
            } else {
                // throw MugenException("Unhandled option in BG '" + self.getName() + "' Section: " + simple.toString());
            }
        }
    };

    Walker walker(*this);
    data->walk(walker);
}

BackgroundElement::BackgroundElement(const BackgroundElement &copy){
    this->setID(copy.getID());
    this->setLayer(copy.getLayer());
    this->setName(copy.getName());
    this->visible = copy.visible;
    this->enabled = copy.enabled;
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

double BackgroundElement::getCurrentX() const {
    return getStart().x + getX() + getSinX().get();
}

double BackgroundElement::getCurrentY() const {
    return getStart().y + getY() + getSinY().get();
}
        
void BackgroundElement::reset(){
    x = 0;
    y = 0;
    /* TODO: set velocity to its original velocity and whatever else */
}

void BackgroundElement::act(){
    x += getVelocityX();
    y += getVelocityY();
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
    this->visible = copy.visible;
    this->enabled = copy.enabled;
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


NormalElement::NormalElement(const string & name, Ast::Section * data, Mugen::SpriteMap & sprites):
BackgroundElement(name, data),
sprite(0){

    class Walker: public Ast::Walker{
    public:
        Walker(NormalElement & self, Mugen::SpriteMap & sprites):
            self(self),
            sprites(sprites){
            }

        NormalElement & self;
        Mugen::SpriteMap & sprites;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "spriteno"){
                int group=0, sprite=0;
                try{
                    simple >> group >> sprite;
                } catch (const Ast::Exception & e){
                }
                self.setSprite(sprites[group][sprite]);
            }
        }
    };

    Walker walker(*this, sprites);
    data->walk(walker);
}

NormalElement::~NormalElement(){
}

void NormalElement::act(){
    if (!getEnabled()){
        return;
    }
    BackgroundElement::act();
    getSinX().act();
    getSinY().act();
}

/* this would be a generator if c++ had coroutines */
class Tiler{
public:
    Tiler(const Point & tile, int startX, int startY, int nextX, int nextY, int spriteOffsetX, int spriteOffsetY, int spriteWidth, int spriteHeight, int maxWidth, int maxHeight){
        if (tile.x == 0 && tile.y == 0){
            /* just a single position */
            points.push_back(Point(startX, startY));
        } else if (tile.x == 1 && tile.y != 1){
            /* x is infinite, y is a single tile or some repetition */
            int xPosition = startX;

            int x1 = 0;
            int x2 = maxWidth;

            while (xPosition - spriteOffsetX > x1){
                xPosition -= nextX;
            }

            while (xPosition - spriteOffsetX + spriteWidth < x1){
                xPosition += nextX;
            }

            int yTiles = tile.y == 0 ? 1 : tile.y;
            int yPosition = startY;
            for (int y = 0; y < yTiles; y++){
                while (xPosition - spriteOffsetX < x2){
                    points.push_back(Point(xPosition, yPosition));
                    xPosition += nextX;
                }
                yPosition += nextY;
            }
        } else if (tile.x != 1 && tile.y == 1){
            /* y is infinite, x is a single tile or some repetition */
            int yPosition = startY;

            int y1 = 0;
            int y2 = maxHeight;

            while (yPosition - spriteOffsetY > y1){
                yPosition -= nextY;
            }

            while (yPosition - spriteOffsetY + spriteHeight < y1){
                yPosition += nextY;
            }

            int xTiles = tile.x == 0 ? 1 : tile.x;
            int xPosition = startX;
            for (int x = 0; x < xTiles; x++){
                while (yPosition - spriteOffsetY < y2){
                    points.push_back(Point(xPosition, yPosition));
                    yPosition += nextY;
                }
                xPosition += nextX;
            }
        } else if (tile.x == 1 && tile.y == 1){
            int xPosition = startX;
            int yPosition = startY;

            int y1 = 0;
            int y2 = maxHeight;
            int x1 = 0;
            int x2 = maxWidth;

            while (xPosition - spriteOffsetX > x1){
                xPosition -= nextX;
            }

            while (xPosition - spriteOffsetX + spriteWidth < x1){
                xPosition += nextX;
            }

            while (yPosition - spriteOffsetY > y1){
                yPosition -= nextY;
            }
            
            while (yPosition - spriteOffsetY + spriteHeight < y1){
                yPosition += nextY;
            }

            // Global::debug(0) << "Tile both start " << startX << ", " << startY << " upper left " << xPosition << ", " << yPosition << endl;
            for (int ypos = yPosition; ypos - spriteOffsetY < y2; ypos += nextY){
                for (int xpos = xPosition; xpos - spriteOffsetX < x2; xpos += nextX){
                    // Global::debug(0) << xpos << ", " << ypos << endl;
                    points.push_back(Point(xpos, ypos));
                }
            }
            // Global::debug(0) << "Done tile both" << endl;
        }

        current_point = points.begin();
    }

    bool hasMore(){
        return current_point != points.end();
    }

    Point nextPoint(){
        Point point = *current_point;
        current_point++;
        return point;
    }

    vector<Point> points;
    vector<Point>::iterator current_point;
};

void NormalElement::render(int cameraX, int cameraY, const Bitmap &bmp){
    if (!getVisible() || !getEnabled()){
        return;
    }
    const int addw = sprite->getWidth() + getTileSpacing().x;
    const int addh = sprite->getHeight() + getTileSpacing().y;
    const int currentX = (int)(bmp.getWidth()/2 + getCurrentX() - cameraX + cameraX * (1 - getDeltaX()));
    const int currentY = (int)(getCurrentY() - cameraY + cameraY * (1 - getDeltaY()));
    const int windowAddX = (int) (getWindowDeltaX() * cameraX);
    const int windowAddY = (int) (getWindowDeltaY() * cameraY);

    // const int currentX = (bmp.getWidth()/2) + int((getStart().x + cameraX + getVelocityX() + getSinX().get()) * getDeltaX());
    // const int currentY =  int((getStart().y + y + getVelocityY() + getSinY().get()) * getDeltaY());
    // const int currentY = (int) (getStart().y - cameraY);
    
    // Set the clipping window
    bmp.setClipRect( getWindow().x + windowAddX, getWindow().y + windowAddY, getWindow().getX2() + windowAddX, getWindow().getY2() + windowAddY );

    Tiler tiler(getTile(), currentX, currentY, addw, addh, sprite->getX(), sprite->getY(), sprite->getWidth(), sprite->getHeight(), bmp.getWidth(), bmp.getHeight());

    while (tiler.hasMore()){
        Point where = tiler.nextPoint();
        sprite->render(where.x, where.y, bmp, getEffects());
    }

#if 0
    /* Render initial sprite */
    if (getTile().x == 0 && getTile().y == 0){
        sprite->render(currentX, currentY, bmp, getEffects());
    }

    /* Do tiling */
    if (getTile().x > 1){
        // sprite->render(currentX, currentY, bmp, getEffects());
	int next = currentX;
	// Tile set amount of times but only forward
	for (int t = 0; t < getTile().x; ++t){
	    sprite->render(next, currentY, bmp, getEffects());
	    next += addw;
	}
    } else if (getTile().x == 1){
	// infinite tiling goes in both directions
        int start = currentX;
	int next = currentX+addw;
	int prev = currentX-addw;
	bool prevDone = false;
	bool nextDone = false;

        int x1 = 0;
        int x2 = bmp.getWidth();

        while (start - sprite->getX() > x1){
            start -= addw;
        }

        while (start - sprite->getX() + sprite->getWidth() < x1){
            start += addw;
        }

        while (start - sprite->getX() < x2){
            sprite->render(start, currentY, bmp, getEffects());
            // Global::debug(0) << "Render background at " << start << endl;
            start += addw;
        }

        // Global::debug(0) << "Done tiling" << endl;

        /*
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
        */
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
#endif
    // Reset clip state
    bmp.setClipRect(0, 0,bmp.getWidth(),bmp.getHeight());
}

AnimationElement::AnimationElement(std::map< int, MugenAnimation * > & animations, const string & name, Ast::Section * data):
BackgroundElement(name, data),
animation(0),
animations(animations){

    class Walker: public Ast::Walker{
    public:
        Walker(AnimationElement & self):
            self(self){
            }

        AnimationElement & self;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "actionno"){
                int action;
                simple >> action;
                self.setAnimation(action);
            }
        }
    };

    Walker walker(*this);
    data->walk(walker);
}

AnimationElement::~AnimationElement(){
}

void AnimationElement::act(){
    if (!getEnabled()){
        return;
    }
    animations[animation]->logic();
    BackgroundElement::act();
    getSinX().act();
    getSinY().act();
}

void AnimationElement::render(int cameraX, int cameraY, const Bitmap &bmp){
    if (!getVisible() || !getEnabled()){
        return;
    }
    const int addw = getTileSpacing().x;
    const int addh = getTileSpacing().y;

    // const int currentX = (bmp.getWidth()/2) + int((getStart().x - cameraX + getVelocityX() + getSinX().get()) * getDeltaX());
    // const int currentY =  int((getStart().y - cameraY + getVelocityY() + getSinY().get()) * getDeltaY());

    const int currentX = (bmp.getWidth()/2) + (int) (getCurrentX() - cameraX + cameraX * (1 - getDeltaX()));
    const int currentY = (int) (getCurrentY() - cameraY + cameraY * (1 - getDeltaY()));
    const int windowAddX = (int) (getWindowDeltaX() * cameraX);
    const int windowAddY = (int) (getWindowDeltaY() * cameraY);

    // Set the clipping window
    bmp.setClipRect( getWindow().x + windowAddX, getWindow().y + windowAddY, getWindow().getX2() + windowAddX, getWindow().getY2() + windowAddY );

    Tiler tiler(getTile(), currentX, currentY, addw, addh, 0, 0, 0, 0, bmp.getWidth(), bmp.getHeight());

    MugenAnimation * ani = animations[animation];

    while (tiler.hasMore()){
        Point where = tiler.nextPoint();
        ani->render(where.x, where.y, bmp);
    }

#if 0
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
#endif
    // Reset clip state
    bmp.setClipRect(0, 0,bmp.getWidth(),bmp.getHeight());
}

ParallaxElement::ParallaxElement(const string & name, Ast::Section * data, Mugen::SpriteMap & sprites):
BackgroundElement(name, data),
sprite(0),
xscaleX(0),
xscaleY(0),
yscale(100),
yscaleDelta(0){

    class Walker: public Ast::Walker{
    public:
        Walker(ParallaxElement & self, Mugen::SpriteMap & sprites):
            self(self),
            sprites(sprites){
            }

        ParallaxElement & self;
        Mugen::SpriteMap & sprites;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "spriteno"){
                int group=0, sprite=0;
                try{
                    simple >> group >> sprite;
                } catch (const Ast::Exception & e){
                }
                self.setSprite(sprites[group][sprite]);
            } else if (simple == "xscale"){
                double x=0, y=0;
                try{
                    simple >> x >> y;
                } catch (const Ast::Exception & e){
                }
                self.setXScale(x,y);
            } else if (simple == "width"){
                Mugen::Point point;
                try{
                    simple >> point.x >> point.y;
                } catch (const Ast::Exception & e){
                }
                self.setWidth(point);
            } else if (simple == "yscalestart"){
                double x;
                simple >> x;
                self.setYScale(x);
            } else if (simple == "yscaledelta"){
                double x;
                simple >> x;
                self.setYScaleDelta(x);
            }
        }
    };

    Walker walker(*this, sprites);
    data->walk(walker);
}

ParallaxElement::~ParallaxElement(){
}

void ParallaxElement::act(){
    if (!getEnabled()){
        return;
    }
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
    if (!getVisible()){
        return;
    }
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
	doParallax(show, work, cameraX, cameraY, getStart().x - sprite->getX(), getStart().y - sprite->getY(), width.x, width.y, work.getWidth()/2, 0, getDeltaX(), getDeltaY());
    }

    // Reset clip state
    work.setClipRect(0, 0,work.getWidth(),work.getHeight());
}

DummyElement::DummyElement(const std::string & name, Ast::Section * data):
BackgroundElement(name,data){
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

static BackgroundElement *getElement( Ast::Section *section, Mugen::SpriteMap &sprites, std::map< int, MugenAnimation * > & animations ){
    std::string head = section->getName();
    head = PaintownUtil::captureRegex(head, ".*[bB][gG] (.*)", 0);
    std::string name = head;
    Global::debug(1) << "Found background element: " << name << endl;

    string type;
    *section->findAttribute("type") >> type;

    if (type == "normal"){
        return new NormalElement(name, section, sprites);
    } else if (type == "anim"){
        return new AnimationElement(animations, name, section);
    } else if (type == "parallax"){
        return new ParallaxElement(name, section, sprites);
    } else if (type == "dummy"){
        return new DummyElement(name, section);
    } else {
        ostringstream out;
        out << "Unknown background type '" << type << "' in " << name;
        throw MugenException(out.str());
    }
}

/* Controller */
Controller::Controller(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
name(name),
timeStart(0),
endTime(0),
loopTime(-1),
ticker(0),
dontReset(false){
    class Walker: public Ast::Walker{
    public:
        Walker(Controller & self, BackgroundController & control, Background & background, bool & hasID):
            self(self),
            control(control),
            background(background),
            hasID(hasID){
            }
        Controller & self;
        BackgroundController & control;
        Background & background;
        bool & hasID;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "time"){
                int start=0, end=0, loop=-2;
                try {
                    simple >> start >> end >> loop;
                } catch (const Ast::Exception & e){
                }
                self.timeStart = start;
                self.endTime = end;
                if (loop != -2){
                    self.loopTime = loop;
                    self.dontReset = true;
                }
                /*
                if (loop == -1){
                    self.dontReset = true;
                }
                */
            } else if (simple == "ctrlid"){
                try{
                    while (true){
                        int id;
                        simple >> id;
                        self.addElements(background.getIDList(id));
                        hasID = true;
                    }
                } catch (const Ast::Exception & e){
                }
            }  
        }
    };

    bool hasID = false;
    Walker walker(*this, control, background, hasID);
    data->walk(walker);
    
    if (!hasID){
        // No IDs set, use the global ID's that the BackgroundController has
        addElements(control.getElements());
    } 
}

Controller::~Controller(){
}

void Controller::reset(){
    if (!dontReset){
        ticker = 0;
    }
}

/*! Null Controller doesn't do anything */
class NullController : public Controller{
    public:
        NullController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background){
	}
        virtual ~NullController(){
	}
        virtual void act(){
	}
    private:
};

/*! Add to start by values given */
class PosAddController : public Controller{
    public:
        PosAddController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background),
	x(0),
	y(0){
	    class Walker: public Ast::Walker{
	    public:
		Walker(PosAddController & self):
		    self(self){
		    }
		PosAddController & self;
		
		virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "x"){
			simple >> self.x;
		    } else if (simple == "y"){
			simple >> self.y;
		    } 
		}
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}
        virtual ~PosAddController(){
	}
        virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    element->setX(element->getX() + x);
		    element->setY(element->getY() + y);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
        int x, y;
};

/*! Set start to values given */
class PosSetController : public Controller{
    public:
        PosSetController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background),
	x(0),
	y(0){
	    class Walker: public Ast::Walker{
	    public:
		Walker(PosSetController & self):
		    self(self){
		    }
		PosSetController & self;
		
		virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "x"){
			simple >> self.x;
		    } else if (simple == "y"){
			simple >> self.y;
		    } 
		}
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}

        virtual ~PosSetController(){
	}

        virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    element->setX(x);
		    element->setY(y);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
        int x, y;
};

/*! Set Sin X to values given */
class SinXController : public Controller{
public:
    SinXController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
    Controller(name,data,control,background){
        class Walker: public Ast::Walker {
        public:
            Walker(SinXController & self):
                self(self){
                }

            SinXController & self;
            
            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    if (simple == "value"){
                      try{
                          simple >> self.sin.amp >> self.sin.period >> self.sin.offset;
                      } catch (const Ast::Exception & e){
                      }
                    } 
                } 
            }
        };
        Walker walker(*this);
        data->walk(walker);
    }

    virtual ~SinXController(){
    }

    virtual void act(){
        if (ticker >= timeStart && ticker <= endTime){
            for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
                BackgroundElement *element = *i;
                element->setSinX(sin);
            }
        }

        if (loopTime != -1){
            if (ticker == loopTime){
                reset();
            }
        }

        ticker += 1;
    }

private:
    Sin sin;
};

/*! Set Sin Y to values given */
class SinYController : public Controller{
    public:
        SinYController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background){
	    class Walker: public Ast::Walker{
		public:
		    Walker(SinYController & self):
			self(self){
			}
		    SinYController & self;
		    
		    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
			if (simple == "value"){
			  try{
			      simple >> self.sin.amp >> self.sin.period >> self.sin.offset;
			  } catch (const Ast::Exception & e){
			  }
			} 
		    }
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}
	virtual ~SinYController(){
	}
	virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    element->setSinY(sin);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
	Sin sin;
};

/*! Add to current Velocity setting */
class VelAddController : public Controller{
    public:
        VelAddController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background),
	x(0),
	y(0){
	    class Walker: public Ast::Walker{
	    public:
		Walker(VelAddController & self):
		    self(self){
		    }
		VelAddController & self;
		
		virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "x"){
			simple >> self.x;
		    } else if (simple == "y"){
			simple >> self.y;
		    } 
		}
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}
        virtual ~VelAddController(){
	}
        virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    element->setVelocity(element->getVelocityX() + x,element->getVelocityY() + y);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
	double x, y;
};

/*! Set current velocity to values given */
class VelSetController : public Controller{
    public:
        VelSetController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background),
	x(0),
	y(0){
	    class Walker: public Ast::Walker{
	    public:
		Walker(VelSetController & self):
		    self(self){
		    }
		VelSetController & self;
		
		virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "x"){
			simple >> self.x;
		    } else if (simple == "y"){
			simple >> self.y;
		    } 
		}
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}
        virtual ~VelSetController(){
	}
        virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    element->setVelocity(x, y);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
	double x, y;
};

/*! Enable or Disables Background */
class EnabledController : public Controller{
    public:
        EnabledController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background),
	enabled(true){
	    class Walker: public Ast::Walker{
	    public:
		Walker(EnabledController & self):
		    self(self){
		    }
		EnabledController & self;
		
		virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "value"){
			simple >> self.enabled;
		    }
		}
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}
        virtual ~EnabledController(){
	}
        virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    element->setEnabled(enabled);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
	bool enabled;
};

/*! Make Background Visible, if not visible it will still act */
class VisibleController : public Controller{
    public:
        VisibleController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background),
	visible(true){
	    class Walker: public Ast::Walker{
	    public:
		Walker(VisibleController & self):
		    self(self){
		    }
		VisibleController & self;
		
		virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "value"){
			simple >> self.visible;
		    } 
		}
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}
        virtual ~VisibleController(){
	}
        virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    element->setVisible(visible);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
	bool visible;
};

/*! Change current animation of Background to given value */
class AnimationController : public Controller{
    public:
        AnimationController(const std::string & name, Ast::Section * data, BackgroundController & control, Background & background):
	Controller(name,data,control,background),
	animation(0){
	    class Walker: public Ast::Walker{
	    public:
		Walker(AnimationController & self):
		    self(self){
		    }
		AnimationController & self;
		
		virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "value"){
			simple >> self.animation;
		    } 
		}
	    };
	    Walker walker(*this);
	    data->walk(walker);
	}
        virtual ~AnimationController(){
	}
        virtual void act(){
	    if ( ticker >= timeStart && ticker <= endTime){
		for (std::vector< BackgroundElement *>::iterator i = elements.begin(); i != elements.end(); ++i){
		    BackgroundElement *element = *i;
		    ((AnimationElement *)element)->setAnimation(animation);
		}
	    }
	    if (loopTime != -1){
		if (ticker == loopTime){
		    reset();
		}
	    }
	    ticker++;
	}
    private:
	int animation;
};

/* Background controller which handles all individual controllers under it */
BackgroundController::BackgroundController(const std::string & name, Ast::Section * data, Background & background):
name(name),
ID(0),
globalLooptime(-1),
ticker(0){
    class Walker: public Ast::Walker{
    public:
        Walker(BackgroundController & self, Background & background, bool & hasID):
            self(self),
            background(background),
            hasID(hasID){
            }

        BackgroundController & self;
        Background & background;
        bool & hasID;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "eventid"){
                simple >> self.ID;
            } else if (simple == "looptime"){
                simple >> self.globalLooptime;
                if (self.globalLooptime == 0){
                    self.globalLooptime = -1;
                }
            } else if (simple == "ctrlid"){
                try{
                    while (true){
                        int id;
                        simple >> id;
                        self.addElements(background.getIDList(id));
                        hasID = true;
                    }
                } catch (const Ast::Exception & e){
                }
            } else {
                throw MugenException("Unhandled option in BGCtrlDef " + self.name + " Section: " + simple.toString());
            }
        }
    };

    bool hasID = false;
    Walker walker(*this, background, hasID);
    data->walk(walker);
    
    if (!hasID){
        // Has no ids set. Default to all background elements.
        addElements(background.getElements());
    }
}

BackgroundController::~BackgroundController(){
    for (std::vector< Controller *>::iterator i = controllers.begin(); i != controllers.end(); ++i){
        Controller *controller = *i;
        if (controller){
            delete controller;
        }
    }
}

void BackgroundController::act(){
    for (std::vector< Controller *>::iterator i = controllers.begin(); i != controllers.end(); ++i){
        Controller *controller = *i;
        controller->act();
    }
    if (globalLooptime != -1){
        if (ticker == globalLooptime){
            ticker = 0;
            for (std::vector< Controller *>::iterator i = controllers.begin(); i != controllers.end(); ++i){
               Controller *controller = *i;
               controller->reset();
            }
        }
        ticker++;
    }
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
        // Lets check if this is a normal def so we can get our sprite file, otherwise treat it as a normal background */
        if ( head == "files") {
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
        } else if ( head == "scenedef") {
	    // Lets check if this is a storyboard so we can get our sprite file, otherwise treat it as a normal background */
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
			    int r=0, g=0, b=0;
                            try{
			        simple >> r >> g >> b;
                            } catch (const Ast::Exception & e){
                            }
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
	    BackgroundElement * element = getElement(*section_it, sprites, animations);
	    
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
	} else if (PaintownUtil::matchRegex(head, "begin *action")){
            /* This creates the animations. It differs from character animation since
             * these are included in the stage.def file with the other defaults
             */
	    head.replace(0,13,"");
	    int h;
            istringstream out(head);
	    out >> h;
	    animations[h] = Mugen::Util::getAnimation(section, sprites);
	} else if (PaintownUtil::matchRegex(head, ".*bgctrldef")){
            // Grabs the Background Controller Definition so that we can go through the child controllers
            head.replace(0,10,"");
            BackgroundController *temp = new BackgroundController(head, section, *this);
            controllers.push_back(temp);
	} else if (PaintownUtil::matchRegex(head, ".*bgctrl")){
	    if (controllers.empty()){
		/* This is a hack to get mugen to do some fancy controlling in a regular
                 * game to accomplish stage fatalities and other tricks
                 * Maybe we can support this later, for now just ignore
                 */
		Global::debug(1) << "Found a BgCtrl without a parent Def definition... its not a bowl!" << endl;
		continue;
	    }
	    // else we got ourselves some controls... under the last controller added
	    BackgroundController *control = controllers.back();

            // Get name
            head.replace(0,7,"");
            
            // Check type
            string type;
            *section->findAttribute("type") >> type;
            type = Util::fixCase(type);
            
            // Our Controller
            Controller *controller=0;
            if (type == "anim"){
                controller = new AnimationController(head,section,*control,*this);
            } else if (type == "enabled" || type == "enable"){
                controller = new EnabledController(head,section,*control,*this);
            } else if (type == "null"){
                controller = new NullController(head,section,*control,*this);
            } else if (type == "posadd"){
                controller = new PosAddController(head,section,*control,*this);
            } else if (type == "posset"){
                controller = new PosSetController(head,section,*control,*this);
            } else if (type == "sinx"){
                controller = new SinXController(head,section,*control,*this);
            } else if (type == "siny"){
                controller = new SinYController(head,section,*control,*this);
            } else if (type == "veladd"){
                controller = new VelAddController(head,section,*control,*this);
            } else if (type == "velset"){
                controller = new VelSetController(head,section,*control,*this);
            } else if (type == "visible"){
                controller = new VisibleController(head,section,*control,*this);
            } else {
                ostringstream out;
                out << "Unknown background type '" << type << "' in " << head;
                throw MugenException(out.str());
            }
            // Finally add it to our background controller
            control->addController(controller);
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

    // Controllers
    for( vector< BackgroundController *>::iterator i = controllers.begin(); i != controllers.end(); ++i ){
	BackgroundController *controller = *i;
	if (controller){
            delete controller;
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
    // Controllers
    for( vector< BackgroundController *>::iterator i = controllers.begin(); i != controllers.end(); ++i ){
	BackgroundController *controller = *i;
	controller->act();
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

}
