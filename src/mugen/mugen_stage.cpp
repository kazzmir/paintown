#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>
#include <sstream>
#include <iostream>

#include "mugen_stage.h"

#include "util/funcs.h"
#include "util/bitmap.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "globals.h"
#include "factory/font_render.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"

// Some static variables
static const int DEFAULT_BACKGROUND_ID = -9999;
static const int DEFAULT_OBJECT_OFFSET = 160;
static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const double DEFAULT_JUMP_VELOCITY = 7.2;

void reverseNumber(int &number){
    // These bastards use it backwards 0 lightest 256 darkest.. lets reverse it
    number = number > 128 ? (128 - (number - 128)) : (number < 128 ? (128 + (128 - number)) : 128);
}

static void correctStageName( std::string &str ){
    if( str.find( "stages/") != std::string::npos || str.find( "stages\\") != std::string::npos ){
	size_t rem = str.find_first_of( "stages/" );
	if( rem != std::string::npos ){
	    str.replace( rem, std::string("stages/").size(), "" );
	    return;
	}
	rem = str.find_first_of( "stages\\" );
	if( rem != std::string::npos ){
	    str.replace( rem, std::string("stages\\").size(), "" );
	    return;
	}
    }
}

static bool centerCollision( Character *p1, Character *p2 ){
    //p1->getCurrentMovement()->getCurrentFrame();
    const int p1width = p1->getCurrentMovement()->getCurrentFrame()->getWidth(), p1height = p1->getCurrentMovement()->getCurrentFrame()->getHeight();
    const int p2width = p2->getCurrentMovement()->getCurrentFrame()->getWidth(), p2height = p2->getCurrentMovement()->getCurrentFrame()->getHeight();
    
    const int x1 = p1->getX() + ((p1width/2)/2), y1 = 0, x2 = p1->getX() + p1width - ((p1width/2)/2), y2 = p1height, 
	      x3 = p2->getX() + ((p2width/2)/2), y3 = 0, x4 = p2->getX() + p2width - ((p2width/2)/2), y4 = p2height;
    
    if ( x1 < x3 && x1 < x4 &&
	    x2 < x3 && x2 < x4 ) return false;
    if ( x1 > x3 && x1 > x4 &&
	    x2 > x3 && x2 > x4 ) return false;
    if ( y1 < y3 && y1 < y4 &&
	    y2 < y3 && y2 < y4 ) return false;
    if ( y1 > y3 && y1 > y4 &&
	    y2 > y3 && y2 > y4 ) return false;
    
    return true;
}

MugenStage::MugenStage( const string & s ):
World(),
location( s ),
baseDir(""),
name(""),
startx(0),
starty(0),
boundleft(-150),
boundright(150),
boundhigh(-25),
boundlow(0),
verticalfollow(.2),
floortension(0),
tension(60),
p1startx(-70),
p1starty(0),
p1startz(0),
p1facing(1),
p2startx(70),
p2starty(0),
p2startz(0),
p2facing(-1),
leftbound(-1000),
rightbound(1000),
topbound(-25),
botbound(0),
/*topz(0),
botz(50),
topscale(1),
botscale(1.2),*/
screenleft(15),
screenright(15),
zoffset(200),
zoffsetlink(DEFAULT_BACKGROUND_ID),
autoturn(true),
resetBG(true),
shadowIntensity(128),
reflect(false),
shadowColor(Bitmap::makeColor(0,0,0)),
shadowYscale(0.4),
shadowFadeRangeHigh(0),
shadowFadeRangeMid(0),
reflectionIntensity(0),
sffFile(""),
debugbg(false),
board(0),
xaxis(0),
yaxis(0),
camerax(0),
cameray(0),
stageStart(false),
ticker(0),
totalRounds(3),
round(1),
totalTime(99),
time(99),
p1points(0),
p2points(0),
inleft(0),
inright(0),
inabove(0){
}

MugenStage::MugenStage( const char * location ): World(),
location( std::string(location) ),
baseDir(""),
name(""),
startx(0),
starty(0),
boundleft(-150),
boundright(150),
boundhigh(-25),
boundlow(0),
verticalfollow(.2),
floortension(0),
tension(60),
p1startx(-70),
p1starty(0),
p1startz(0),
p1facing(1),
p2startx(70),
p2starty(0),
p2startz(0),
p2facing(-1),
leftbound(-1000),
rightbound(1000),
topbound(-25),
botbound(0),
/*topz(0),
botz(50),
topscale(1),
botscale(1.2),*/
screenleft(15),
screenright(15),
zoffset(200),
autoturn(true),
resetBG(true),
shadowIntensity(128),
reflect(false),
shadowColor(Bitmap::makeColor(0,0,0)),
shadowYscale(0.4),
shadowFadeRangeHigh(0),
shadowFadeRangeMid(0),
reflectionIntensity(0),
sffFile(""),
debugbg(false),
board(0),
xaxis(0),
yaxis(0),
camerax(0),
cameray(0),
stageStart(false),
ticker(0),
totalRounds(3),
round(1),
totalTime(99),
time(99),
p1points(0),
p2points(0),
inleft(0),
inright(0),
inabove(0){
}

MugenStage::~MugenStage(){
    cleanup();
}
	
void MugenStage::load() throw( MugenException ){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    baseDir = Util::getDataPath() + "mugen/stages/";
    Global::debug(1) << baseDir << endl;
    location+=".def";
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, std::string(location) );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate stage definition file for: " + location );
    
    std::string filesdir = "";
    
    size_t strloc = location.find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = location.substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();

    struct cymk_holder{
        cymk_holder():c(0),m(0),y(0),k(0){}
        int c, m, y, k;
    } shadow;
    
    // for linked position in backgrounds
    MugenBackground *prior = 0;
    
    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	MugenUtil::fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("name")!=std::string::npos ){
		    *content->getNext() >> name;
                    Global::debug(1) << "Read name '" << name << "'" << endl;
		} else if ( itemhead.find("author")!=std::string::npos ){
		    // Seems to be that some people think that author belongs in background defs
		    std::string temp;
		    *content->getNext() >> temp;
                    Global::debug(1) << "Made by this guy: '" << temp << "'" << endl;
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "camera" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("startx")!=std::string::npos ){
		    *content->getNext() >> startx;
		} else if ( itemhead.find("starty")!=std::string::npos ){
		    *content->getNext() >> starty;
		} else if ( itemhead.find("boundleft")!=std::string::npos ){
		    *content->getNext() >> boundleft;
		} else if ( itemhead.find("boundright")!=std::string::npos ){
		    *content->getNext() >> boundright;
		} else if ( itemhead.find("boundhigh")!=std::string::npos ){
		    *content->getNext() >> boundhigh;
		} else if ( itemhead.find("boundlow")!=std::string::npos ){
		    // This is always 0 so don't grab it
		    //*content->getNext() >> boundlow;
		} else if ( itemhead.find("verticalfollow")!=std::string::npos ){
		    *content->getNext() >> verticalfollow;
		    if (verticalfollow > 1) verticalfollow = 1;
		    else if(verticalfollow < 0) verticalfollow = 0;
		} else if ( itemhead.find("floortension")!=std::string::npos ){
		    *content->getNext() >> floortension;
		} else if ( itemhead.find("tension")!=std::string::npos ){
		    *content->getNext() >> tension;
		} else throw MugenException( "Unhandled option in Camera Section: " + itemhead );
	    }
	}
	else if( head == "playerinfo" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("p1startx")!=std::string::npos ){
		    *content->getNext() >> p1startx;
		} else if ( itemhead.find("p1starty")!=std::string::npos ){
		    *content->getNext() >> p1starty;
		} else if ( itemhead.find("p1startz")!=std::string::npos ){
		    *content->getNext() >> p1startz;
		} else if ( itemhead.find("p1facing")!=std::string::npos ){
		    *content->getNext() >> p1facing;
		} else if ( itemhead.find("p2startx")!=std::string::npos ){
		    *content->getNext() >> p2startx;
		} else if ( itemhead.find("p2starty")!=std::string::npos ){
		    *content->getNext() >> p2starty;
		} else if ( itemhead.find("p2startz")!=std::string::npos ){
		    *content->getNext() >> p2startz;
		} else if ( itemhead.find("p2facing")!=std::string::npos ){
		    *content->getNext() >> p2facing;
		} else if ( itemhead.find("leftbound")!=std::string::npos ){
		    *content->getNext() >> leftbound;
		} else if ( itemhead.find("rightbound")!=std::string::npos ){
		    *content->getNext() >> rightbound;
		} else if ( itemhead.find("topbound")!=std::string::npos ){
		    *content->getNext() >> topbound;
		} else if ( itemhead.find("botbound")!=std::string::npos ){
		    *content->getNext() >> botbound;
		} else throw MugenException( "Unhandled option in PlayerInfo Section: " + itemhead );
	    }
	}
	else if( head == "scaling" ){ /* do Nothing read below */ }
	/*
	Aparently no longer in use, fuck it ignore it
	else if( head == "scaling" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("topz")!=std::string::npos ){
		    *content->getNext() >> topz;
		} else if ( itemhead.find("botz")!=std::string::npos ){
		    *content->getNext() >> botz;
		} else if ( itemhead.find("topscale")!=std::string::npos ){
		    *content->getNext() >> topscale;
		} else if ( itemhead.find("botscale")!=std::string::npos ){
		    *content->getNext() >> botscale;
		} else throw MugenException( "Unhandled option in Scaling Section: " + itemhead );
	    }
	}
	*/
	else if( head == "bound" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("screenleft")!=std::string::npos ){
		    *content->getNext() >> screenleft;
		} else if ( itemhead.find("screenright")!=std::string::npos ){
		    *content->getNext() >> screenright;
		} else throw MugenException( "Unhandled option in Bound Section: " + itemhead );
	    }
	}
	else if( head == "stageinfo" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead == "zoffset" ){
		    *content->getNext() >> zoffset;
		} else if ( itemhead == "zoffsetlink" ){
		    *content->getNext() >> zoffsetlink;
		} else if ( itemhead.find("autoturn")!=std::string::npos ){
		    *content->getNext() >> autoturn;
		} else if ( itemhead.find("resetbg")!=std::string::npos ){
		    *content->getNext() >> resetBG;
		} else throw MugenException( "Unhandled option in StageInfo Section: " + itemhead );
	    }
	} 
	else if( head == "shadow" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("intensity")!=std::string::npos ){
		    *content->getNext() >> shadow.k;
		    // *content->getNext() >> shadowIntensity;
                    // shadow.k = shadowIntensity;
		    //reverseNumber(shadowIntensity);
		} else if ( itemhead == "reflect" ){
		    *content->getNext() >> reflect;
		} else if ( itemhead.find("color")!=std::string::npos ){
		    // int c,m,k;
		    *content->getNext() >> shadow.c;
		    *content->getNext() >> shadow.y;
		    *content->getNext() >> shadow.m;
		    //reverseNumber(r);
		    //reverseNumber(g);
		    //reverseNumber(b);
		    // shadowColor = Bitmap::makeColor(r,g,b);
		} else if ( itemhead.find("yscale")!=std::string::npos ){
		    *content->getNext() >> shadowYscale;
		} else if ( itemhead.find("fade.range")!=std::string::npos ){
		    *content->getNext() >> shadowFadeRangeHigh;
		    *content->getNext() >> shadowFadeRangeMid;
		} else throw MugenException( "Unhandled option in Shadow Section: " + itemhead );
	    }
	}
	else if( head == "reflection" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("intensity")!=std::string::npos ){
		    *content->getNext() >> reflectionIntensity;
		} else throw MugenException( "Unhandled option in Reflection Section: " + itemhead );
	    }
	}
	else if( head == "bgdef" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("spr")!=std::string::npos ){
		    *content->getNext() >> sffFile;
		    Global::debug(1) << "Reading Sff (sprite) Data..." << endl; 
		    correctStageName( sffFile );
		    MugenUtil::readSprites( MugenUtil::fixFileName(baseDir + filesdir, sffFile), "", sprites );
		} else if ( itemhead.find("debugbg")!=std::string::npos ){
		    *content->getNext() >> debugbg;
		} else throw MugenException( "Unhandled option in Reflection Section: " + itemhead );
	    }
	}
	else if( head == "music" ){ /* Ignore for now */ }
	// This our background data definitions
	else if( head.find("bg ") !=std::string::npos ){
	    MugenBackground *temp = new MugenBackground(ticker);
	    head.replace(0,3,"");
	    temp->name = head;
	    Global::debug(1) << "Found background: " << temp->name << endl;
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		Global::debug(1) << "Getting next item: " << itemhead << endl;
		if ( itemhead.find("type")!=std::string::npos ){
		    std::string type;
		    *content->getNext() >> type;
		    MugenUtil::removeSpaces( type );
		    if( type == "normal" )temp->type = Normal;
		    else if( type == "anim" )temp->type = Anim;
		    else if( type == "parallax" )temp->type = Parallax;
		    else if( type == "dummy" )temp->type = Dummy;
		} else if (itemhead == "spriteno"){
		    *content->getNext() >> temp->groupNumber;
		    *content->getNext() >> temp->imageNumber;
		} else if (itemhead == "actionno"){
		    *content->getNext() >> temp->actionno;
		} else if (itemhead == "id"){
		    *content->getNext() >> temp->id;
		} else if (itemhead == "layerno"){
		    *content->getNext() >> temp->layerno;
		} else if (itemhead == "start"){
		    *content->getNext() >> temp->startx;
		    *content->getNext() >> temp->starty;
		} else if (itemhead == "delta"){
		    *content->getNext() >> temp->deltax;
		    *content->getNext() >> temp->deltay;
		} else if (itemhead == "trans"){
		    std::string type;
		    *content->getNext() >> type;
		    MugenUtil::removeSpaces( type );
		    if( type == "none" )temp->trans = None;
		    else if( type == "add" )temp->trans = Add;
		    else if( type == "add1" )temp->trans = Add1;
		    else if( type == "sub" )temp->trans = Sub;
		    else if( type == "addalpha" )temp->trans = Addalpha;
		} else if (itemhead == "alpha"){
		    *content->getNext() >> temp->alphalow;
		    *content->getNext() >> temp->alphahigh;
		} else if (itemhead == "mask"){
		    *content->getNext() >> temp->mask;
		} else if (itemhead == "tile"){
		    *content->getNext() >> temp->tilex;
                    Global::debug(2) << "Tile x is " << temp->tilex << endl;
		    *content->getNext() >> temp->tiley;
                    Global::debug(2) << "Tile y is " << temp->tiley << endl;
		} else if (itemhead == "tilespacing"){
		    *content->getNext() >> temp->tilespacingx;
		    *content->getNext() >> temp->tilespacingy;
		} else if (itemhead == "window"){
		    MugenArea area;
		    *content->getNext() >> area.x1;
		    *content->getNext() >> area.y1;
		    *content->getNext() >> area.x2;
		    *content->getNext() >> area.y2;
		    temp->window = area;
		} else if (itemhead == "windowdelta"){
		    *content->getNext() >> temp->windowdeltax;
		    *content->getNext() >> temp->windowdeltay;
		} else if (itemhead == "xscale"){
		    // You should only use either xscale or width but not both if width is used ignore xscale
		    if (!temp->xscaletop && !temp->xscalebot){
			*content->getNext() >> temp->xscaletop;
			*content->getNext() >> temp->xscalebot;
		    }
		} else if (itemhead == "width"){
		    // You should only use either xscale or width but not both if xscale is used ignore width
		    if (!temp->xscaletop && !temp->xscalebot){
			*content->getNext() >> temp->xscaletop;
			*content->getNext() >> temp->xscalebot;
		    }
		} else if (itemhead == "yscalestart"){
		    *content->getNext() >> temp->yscalestart;
		} else if (itemhead == "yscaledelta"){
		    *content->getNext() >> temp->yscaledelta;
		} else if (itemhead == "positionlink"){
		    *content->getNext() >> temp->positionlink;
		} else if (itemhead == "velocity"){
		    *content->getNext() >> temp->velocityx;
		    *content->getNext() >> temp->velocityy;
		} else if (itemhead == "sin.x"){
		    *content->getNext() >> temp->sinx_amp;
		    *content->getNext() >> temp->sinx_period;
		    *content->getNext() >> temp->sinx_offset;
		} else if (itemhead == "sin.y"){
		    *content->getNext() >> temp->siny_amp;
		    *content->getNext() >> temp->siny_period;
		    *content->getNext() >> temp->siny_offset;
		} else throw MugenException( "Unhandled option in BG " + head + " Section: " + itemhead );
	    }
            Global::debug(2) << "Background " << temp->id << " has tilex " << temp->tilex << endl;
	    // Do some fixups and necessary things
	    // lets see where we lay
	    temp->sprite = sprites[(unsigned int)temp->groupNumber][(unsigned int)temp->imageNumber];
	    if( temp->layerno == 0 )backgrounds.push_back(temp);
	    else if( temp->layerno == 1 )foregrounds.push_back(temp);
	    
	    // If position link lets set to previous item
	    if( temp->positionlink ){
		temp->linked = prior;
		Global::debug(1) << "Set positionlink to id: '" << prior->id << "' Position at x(" << prior->startx << ")y(" << prior->starty << ")" << endl;
	    } 
	    
	    // This is so we can have our positionlink info for the next item if true
	    prior = temp;
	}
	/* This creates the animations it differs from character animation since these are included in the stage.def file with the other defaults */
	else if( head.find("begin action") !=std::string::npos ){
	    MugenAnimation *animation = new MugenAnimation();
	    head.replace(0,13,"");
	    bool setloop = false;
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Global::debug(1) << "Item Head: " << itemhead << endl;
		if( itemhead.find("loopstart") != std::string::npos ){
		    setloop = true;
		}
		// This is where we get our frame
		else{
		    // This is the new frame
		    MugenFrame *frame = new MugenFrame();
		    frame->loopstart = setloop;
		    /* Get sprite details */
		    int group, spriteNumber;
		    // Need to get the parsed data and populate these above items
		    *item >> group;
		    *content->getNext() >> spriteNumber;
		    *content->getNext() >> frame->xoffset;
		    *content->getNext() >> frame->yoffset;
		    *content->getNext() >> frame->time;
		    Global::debug(1) << "Group: " << group << " | Sprite: " << spriteNumber << " | x: " << frame->xoffset << " | y: " << frame->yoffset << " | time: " << frame->time << endl;
		    // Check for flips
		    if( content->hasItems() ){
			std::string flip;
			*content->getNext() >> flip;
			MugenUtil::fixCase( flip );
			if( flip.find("h") != std::string::npos )frame->flipHorizontal = true;
			if( flip.find("v") != std::string::npos )frame->flipVertical = true;
		    }
		    if( content->hasItems() )*content->getNext() >> frame->colorAdd;
		    // Add sprite
		    frame->sprite = sprites[(unsigned short)group][(unsigned short)spriteNumber];
                    if (frame->sprite == 0){
                        Global::debug(0) << "No sprite for group " << group << " number " << spriteNumber << endl;
                    }
		    // Add frame
		    animation->addFrame(frame);
		    
		    if( setloop )setloop = false;
		}
	    }
	    int h;
	    MugenItem(head) >> h;
	    animation->setType(MugenAnimationType(h));
	    Global::debug(1) << "Adding Animation 'Begin Action " << h << "' : '" << animation->getName(animation->getType()) << "'" << endl;
	    animations[h] = animation;
	    
	}
	else if( head.find("bgctrldef") != std::string::npos ){ /* Ignore for now */ }
	else if( head.find("bgctrl") != std::string::npos ){ /* Ignore for now */ }
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
	
    }
    
    Global::debug(1) << "Got total backgrounds: " << backgrounds.size() << " total foregrounds: " << foregrounds.size() << endl;
    // Setup board our worksurface to the proper size of the entire stage 320x240 :P
    Global::debug(1) << "Creating level size of Width: " << abs(boundleft) + boundright << " and Height: " << abs(boundhigh) + boundlow << endl;
    board = new Bitmap( DEFAULT_WIDTH, DEFAULT_HEIGHT );
    camerax = xaxis = startx;
    cameray = yaxis = starty;
    
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	if( (*i)->actionno != -1 ){
	    (*i)->action = animations[ (*i)->actionno ];
	}
	// now load
	(*i)->preload( xaxis, yaxis );
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	if( (*i)->actionno != -1 ){
	    (*i)->action = animations[ (*i)->actionno ];
	}
	// now load
	(*i)->preload( xaxis, yaxis );
    }
    
    // zoffsetlink
    if( zoffsetlink != DEFAULT_BACKGROUND_ID ){
	// Link zoffset to id
	zoffset = getBackground(zoffsetlink)->y;
    }

    int r, g, b;
    Bitmap::cymkToRGB(shadow.c, shadow.y, shadow.m, shadow.k, &r, &g, &b);
    Global::debug(1) << "Shadow c/y/m/k " << shadow.c << " " << shadow.y << " " << shadow.m << " " << shadow.k << " r/g/b " << r << " " << g << " " << b << endl;
    shadowColor = Bitmap::makeColor(r, g, b);

    /* shadowIntensity is used as the alpha value. its some combination of the
     * cymk components but I'm not sure what it is. This is relatively close
     * but its definately not 100% accurate.
     */
    shadowIntensity = Util::min((shadow.c + shadow.y + shadow.m + shadow.k * 2) / 3, 255);
    Global::debug(1) << "Shadow intensity " << shadowIntensity << endl;
}

void MugenStage::setCamera( const double &x, const double &y ){ 
    camerax = x; cameray = y; 
    // Camera boundaries
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
}
void MugenStage::moveCamera( const double &x, const double &y ){ 
    camerax += x; cameray += y; 
    // Camera boundaries
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
}

MugenBackground *MugenStage::getBackground( int ID ){
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	if( (*i)->id == ID )return (*i);
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	if( (*i)->id == ID )return (*i);
    }
    
    return 0;
}

void MugenStage::logic( ){
    // camera crap
    if (quake_time > 0){
	quake_time--;
    }
    
    // implement some stuff before we actually begin the round then start the round
    if(!stageStart)stageStart = true;
    
    // Run our ticker on and on like energizer bunnies (tm)
    ticker++;
    
    const int diffx = startx - camerax;
    const int diffy = starty - cameray;
    
    //zoffsetlink
    if( zoffsetlink != DEFAULT_BACKGROUND_ID )zoffset = getBackground(zoffsetlink)->y;
    Global::debug(1) << "zoffsetlink ID: " <<zoffsetlink << " | zoffset: " << zoffset << endl;
    
    // Backgrounds
    for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	(*i)->logic( diffx, diffy );
    }
    
    // Players go in here
    std::vector<Object *> add;
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it){
        /* use local variables more often, iterators can be easily confused */
        Object * player = *it;
        player->act( &objects, (World *)this, &add);
	if(zoffsetlink == DEFAULT_BACKGROUND_ID )player->setZ( zoffset - cameray );
	else player->setZ( zoffset );
	
	
	// Check collisions
	for (vector<Object*>::iterator enem = objects.begin(); enem != objects.end(); ++enem){
	    Object *enemy = *enem;
	    if (player->getAlliance() != enemy->getAlliance()){
		// Do stuff for players
		if (isaPlayer( enemy )){
		    // He collides with another push him away
		    if ( player->collision( (ObjectAttack*)enemy ) && centerCollision( ((Character *)player), ((Character *)enemy) ) ){
			if ( enemy->getX() < player->getX() ){
			    enemy->moveLeft( ((Character *)player)->getSpeed() );
			}
			else if ( enemy->getX() > player->getX() ){
			    enemy->moveRight( ((Character *)player)->getSpeed() );
			}
		    }
		    // autoturn need to do turning actions
		    if (autoturn){
			if (player->getX() < enemy->getX() && player->getFacing() != Object::FACING_RIGHT ){
			    player->setFacing(Object::FACING_RIGHT);
			}

			if (player->getX() > enemy->getX() && player->getFacing() != Object::FACING_LEFT ){
			    player->setFacing(Object::FACING_LEFT);
			}
		    }
		}
		// Attacking
		if ( player->isAttacking() ){
		    ObjectAttack * playerAttack = (ObjectAttack *)player;
		    if ( enemy != player && enemy->isCollidable( player ) && player->isCollidable( enemy ) ){
			if ( player->ZDistance( enemy ) <= playerAttack->minZDistance() && enemy->collision( playerAttack ) ){ 
			    double x = 0, y = 0;
			    
			    x = enemy->getX();
			    y = enemy->getRY() - enemy->getHeight() + enemy->getHeight() / 3;
/*
			    if ( bang != NULL ){
				    Object * addx = bang->copy();
				    addx->setX( x );
				    addx->setY( 0 );
				    addx->setZ( y+addx->getHeight()/2 );
				    addx->setHealth( 1 );
				    added_effects.push_back( addx );
			    }
*/
			    playerAttack->attacked(this, enemy, add );
			    enemy->collided( playerAttack, add );
			    enemy->takeDamage( this, playerAttack, playerAttack->getDamage() );
			}    
		    }
		}
	    }
	}
	
	if ( isaPlayer(player)){
	    // Lets check their boundaries and camera whateva
	    updatePlayer( player );
	    
	    // Lets put their health back on the radar until we get the stages ready for real games
	    if ( player->getHealth() < player->getMaxHealth() ){
		player->setHealth( player->getHealth() + 1 );
	    }
	    
	    // Update old position
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
		
	// Non players, objects, projectiles misc
	} else if ( !isaPlayer(player) && player->getHealth() <= 0 ){
	    delete player;
	    it = objects.erase( it );
	    if( it == objects.end() ) break;
	}
    }
    objects.insert(objects.end(),add.begin(),add.end());
    
    for( vector< MugenBackground *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	(*i)->logic( diffx, diffy );
    }
}
	
void MugenStage::render( Bitmap *work ){
    // Clear to magenta like big daddy mugen does
    if( debugbg )board->floodfill( 0, 0, Bitmap::makeColor(255,0,255) );
    for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	(*i)->render( (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    }
    
    // Players go in here
    int distance = 10;
    bool change = false;
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	Object *obj = *it;
	// Reflection
	if (reflectionIntensity)obj->drawReflection( board, 0, reflectionIntensity );
	// Shadow
	obj->drawShade( board, 0, shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeMid, shadowFadeRangeHigh);
        obj->draw( board, 0);
	// Debug crap
	int color = change ? Bitmap::makeColor( 255, 255, 255 ) : Bitmap::makeColor( 200, 200, 200 );
	Font::getDefaultFont().printf( 15, distance, color, *board, "Object: %i",0, obj);
	Font::getDefaultFont().printf( 15, distance+10, color, *board, "x: %f, y: %f",0, obj->getX(),obj->getY());
	distance+=20;
	change=!change;
    }
    
    for( vector< MugenBackground *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	(*i)->render( (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    }
    
    // Life bars?
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	int p1Side = 5;
	int p2Side = 5;
	if (isaPlayer(*it)){
	    Character *character = (Character*)*it;
	    if ( character->getAlliance() == Player1Side ){
		character->drawLifeBar( 5, p1Side, board );
		p1Side +=10;
	    } else if ( character->getAlliance() == Player2Side ){
		character->drawLifeBar( 215, p2Side, board );
		p2Side +=10;
	    }
	}
    }
    
    
    board->Blit( xaxis + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), yaxis + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT, 0, 0, *work );
}

void MugenStage::reset( ){
    camerax = startx; cameray = starty;
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	// reset just reloads it to default
	(*i)->preload( xaxis, yaxis );
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	// reset
	(*i)->preload( xaxis, yaxis );
    }
    
    // Reset player positions
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	Object *player = *it;
	if( player->getAlliance() == Player1Side ){
	    player->setX( DEFAULT_OBJECT_OFFSET + p1startx );
	    player->setY( p1starty );
	    player->setZ( zoffset );
	    player->setFacing( Object::FACING_RIGHT );
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
	    playerInfo[player].leftTension = false;
	    playerInfo[player].rightTension = false;
	    ((Character *)player)->setJumpingYVelocity(DEFAULT_JUMP_VELOCITY);
	} else if( player->getAlliance() == Player2Side ){
	    player->setX( DEFAULT_OBJECT_OFFSET + p2startx );
	    player->setY( p2starty );
	    player->setZ( zoffset );
	    player->setFacing( Object::FACING_LEFT );
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
	    playerInfo[player].leftTension = false;
	    playerInfo[player].rightTension = false;
	    ((Character *)player)->setJumpingYVelocity(DEFAULT_JUMP_VELOCITY);
	}
    }
    
    inleft = inright = 0;
}

// Add player1 people
void MugenStage::addp1( Object * o ){
    o->setAlliance(Player1Side);
    o->setX( DEFAULT_OBJECT_OFFSET + p1startx );
    o->setY( p1starty );
    o->setZ( zoffset );
    o->setFacing( Object::FACING_RIGHT );
    objects.push_back(o);
    players.push_back(o);
    ((Character *)o)->setJumpingYVelocity(DEFAULT_JUMP_VELOCITY);
    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
}

// Add player2 people
void MugenStage::addp2( Object * o ){
    o->setAlliance(Player2Side);
    o->setX( DEFAULT_OBJECT_OFFSET + p2startx );
    o->setY( p2starty );
    o->setZ( zoffset );
    o->setFacing( Object::FACING_LEFT );
    objects.push_back(o);
    players.push_back(o);
    ((Character *)o)->setJumpingYVelocity(DEFAULT_JUMP_VELOCITY);
    playerInfo[o].oldx = o->getX();
    playerInfo[o].oldy = o->getY();
    playerInfo[o].leftTension = false;
    playerInfo[o].rightTension = false;
}

void MugenStage::act(){
    logic();
}
void MugenStage::draw( Bitmap * work ){
    render( work );
}
void MugenStage::addObject( Object * o ){ /* Does nothing */ }
const bool MugenStage::finished() const { return false; }
void MugenStage::reloadLevel() throw( LoadException ){ 
    cleanup();
    load(); 
}
Script::Engine * const MugenStage::getEngine() const { return NULL; }
/* upper left hand corner of the screen */
int MugenStage::getX(){
    return getCameraX();
}

int MugenStage::getY(){
    return getCameraY();
}
/* this shouldn't be here */
// I guess ignore this one
const deque<Bitmap*> & MugenStage::getScreenshots(){
    return garbage;
}
const int MugenStage::levelLength() const { return 0; }
// Since this isn't a paintown level, I guess block wouldn't apply
const Block * MugenStage::currentBlock() const { return NULL; }
/* bleh.. */
void MugenStage::addEnemy(Enemy * obj){ /* does nothing */ }
Object * MugenStage::findObject(int id){ 
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
        Object * object = *it;
        if (object->getObjectId() == id){
            return object;
        }
    }
    return NULL;
}
// These should be the same, but we'll see, mugen has some funny parameters
int MugenStage::getMaximumZ(){ return zoffset; }
int MugenStage::getMinimumZ(){ return zoffset; }
void MugenStage::drawMiniMaps( bool b ){ /* Not likely */ }
bool MugenStage::shouldDrawMiniMaps(){ return false; }
void MugenStage::killAllHumans( Object * player ){ 
    for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		o->takeDamage( this, NULL, 999999 );
    }
}
void MugenStage::addMessage(Network::Message m, Network::Socket from){ }
Network::Message MugenStage::createBangMessage( int x, int y, int z ){
    Network::Message m;
    return m;
}

void MugenStage::cleanup(){
    // Get rid of sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    
    // Get rid of background lists;
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin() ; i != backgrounds.end() ; ++i ){
	if( (*i) )delete (*i);
    }
    
    // Get rid of foreground lists;
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin() ; i != foregrounds.end() ; ++i ){
	if( (*i) )delete (*i);
    }
    
    if( board ) delete board;
}

bool MugenStage::isaPlayer( Object * o ){
    for ( vector< Object * >::iterator it = players.begin(); it != players.end(); it++ ){
        if ( (*it) == o ){
            return true;
        }
    }
    return false;
}


void MugenStage::updatePlayer( Object *o ){
    
    if (o->getX() <= screenleft){ 
	o->setX( screenleft );
    } else if (o->getX() >= DEFAULT_WIDTH - screenright){
	o->setX( DEFAULT_WIDTH - screenright );
    }
    // Move camera
    const double px = o->getX();
    const double py = o->getY();
    Global::debug(1) << "Are we in left: " << inleft << " | Are we in right: " << inright << " | pdiffx: " << px - playerInfo[o].oldx << endl;
    // Horizontal movement of camera
    if (playerInfo[o].oldx != px){
	const double pdiffx = px - playerInfo[o].oldx;
	// 0 no move, 1 move left, 2 move right for other players so they don't float along
	int movex = 0;
	// This is to move extra in case a boundary was hit
	double cameramovex = 0;
	// Left side x
	if (px <= tension && pdiffx < 0){
	    cameramovex -= (tension - px);
	    movex = 1;
	    if ( !playerInfo[o].leftTension ){
		playerInfo[o].leftTension = true;
		inleft++;
	    }
	} else if (px >= (DEFAULT_WIDTH - tension) && pdiffx > 0){
	    cameramovex += (px - (DEFAULT_WIDTH - tension));
	    movex = 2;
	    if ( !playerInfo[o].rightTension ){
		playerInfo[o].rightTension = true;
		inright++;
	    }
	} else {
	    if ( playerInfo[o].leftTension ){
		playerInfo[o].leftTension = false;
		inleft--;
	    }
	    if ( playerInfo[o].rightTension ){
		playerInfo[o].rightTension = false;
		inright--;
	    }
	}
	if (playerInfo[o].leftTension && pdiffx > 0){
	    cameramovex += (tension - px);
	    movex = 2;
	} else if (playerInfo[o].rightTension && pdiffx < 0){
	    cameramovex -= (px - (DEFAULT_WIDTH - tension));
	    movex = 1;
	}  
	// If we got camera moves lets do them
	if( !inleft || !inright ){
	    moveCamera(cameramovex,0);
	    if (movex){
		for (vector<Object*>::iterator move = objects.begin(); move != objects.end(); ++move){
		    Object *moveplayer = *move;
		    if (movex == 1){
			if (camerax != boundleft){
			    moveplayer->moveRight(abs((int)cameramovex));
			}
		    } else if (movex == 2){
			if (camerax != boundright){
			    moveplayer->moveLeft(abs((int)cameramovex));
			}
		    }
		}
	    }
	}
    }
    // Vertical movement of camera
    if (playerInfo[o].oldy != py){
	if (verticalfollow > 0){
	    const double pdiffy = playerInfo[o].oldy - py;
	    if (py > floortension){
		if (!playerInfo[o].above){
		    playerInfo[o].above = true;
		    inabove++;
		}
	    } else if ( playerInfo[o].above){
		playerInfo[o].above = false;
		inabove--;
	    }
	    if (playerInfo[o].above && pdiffy < 0){
		moveCamera( 0, verticalfollow * -3.2 );
	    } else if (playerInfo[o].above && pdiffy > 0){
		moveCamera( 0, verticalfollow * 3.2 );
	    }
	}
    }
    Global::debug(0) << "Our players Y: " << py << " | Above: "<< playerInfo[o].above << " | total inabove: " << inabove << endl;
}
