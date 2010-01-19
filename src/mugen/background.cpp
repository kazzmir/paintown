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

// Default to center of screen
const unsigned int MAP_X = 120;
const unsigned int MAP_Y = 0;

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

static double interpolate(double f1, double f2, double p){
    return (f1 * (1.0 - p)) + (f2 * p);
}
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


BackgroundElement::BackgroundElement():
deltaX(1),
deltaY(1),
windowDeltaX(0),
windowDeltaY(0),
positionLink(false),
velocityX(0),
velocityY(0),
linkedElement(0){
}
BackgroundElement::~BackgroundElement(){
}

void BackgroundElement::setLink(BackgroundElement *element){
    if (getPositionLink()){
        if (element){
            linkedElement = element;
	    if (element->getPositionLink()){
	        element->setPositionLink(this);
	        return;
	    }
            Mugen::Point newStart(start.x + element->getStart().x,getStart().y + element->getStart().y);
            setStart(newStart);
            setDelta(element->getDeltaX(), element->getDeltaY());
            setVelocity(element->getVelocityX(), element->getVelocityY());
            setSinX(element->getSinX());
            setSinY(element->getSinY());
        }
    }
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
void NormalElement::render(int x, int y, const Bitmap &bmp){
    const int addw = sprite->getWidth() + getTileSpacing().x;
    const int addh = sprite->getHeight() + getTileSpacing().y;
    const int currentX = MAP_X + int((getStart().x + x + getVelocityX() + getSinX().get()) * getDeltaX());
    const int currentY = MAP_Y + int((getStart().y + y + getVelocityY() + getSinY().get()) * getDeltaY());
    Tile tilev = getTileData(currentY, 1, addh, getTile().y);
    for (int v = 0; v < tilev.total; ++v){
        Tile tileh = getTileData(currentX, 1, addw, getTile().x);
        for (int h = 0; h < tileh.total; ++h){
                sprite->render(tileh.start, tilev.start, bmp, getEffects());
                tileh.start+=addw;
        }
        tilev.start+=addh;
    }		
}

AnimationElement::AnimationElement(std::map< int, MugenAnimation * >  & animations):
animation(0),
animations(animations){
    getSinX().act();
    getSinY().act();
}
AnimationElement::~AnimationElement(){
}
void AnimationElement::act(){
    animations[animation]->logic();
}
void AnimationElement::render(int x, int y, const Bitmap &bmp){
    const int addw = getTileSpacing().x;
    const int addh = getTileSpacing().y;
    const int currentX = MAP_X + int((getStart().x + x + getVelocityX() + getSinX().get()) * getDeltaX());
    const int currentY = MAP_Y + int((getStart().y + y + getVelocityY() + getSinY().get()) * getDeltaY());
    Tile tilev = getTileData(currentY, 1, addh, getTile().y);
    for (int v = 0; v < tilev.total; ++v){
        Tile tileh = getTileData(currentX, 1, addw, getTile().x);
        for (int h = 0; h < tileh.total; ++h){
                animations[animation]->render(tileh.start, tilev.start, bmp);
                tileh.start+=addw;
        }
        tilev.start+=addh;
    }
}

ParallaxElement::ParallaxElement():
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
void ParallaxElement::render(int x, int y, const Bitmap &bmp){
    const int addw = sprite->getWidth() + getTileSpacing().x;
    const int addh = sprite->getHeight() + getTileSpacing().y;
    const int currentX = MAP_X + int((getStart().x + x + getVelocityX()) * getDeltaX());
    const int currentY = MAP_Y + int((getStart().y + y + getVelocityY()) * getDeltaY());
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

//! Get Element
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
                *simple >> point.x >> point.y;
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
    
    return element;
}

Background::Background(const std::string &file, const std::string &header):
file(file),
header(header),
debug(false),
clearColor(-1){
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(file));
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
            // Do linked elements
	    element->setLink(priorElement);
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



