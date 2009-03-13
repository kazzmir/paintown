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
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
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
static const int CONTROLLER_VALUE_NOT_SET = -999999;
static const int DEFAULT_BACKGROUND_ID = -9999;
static const int DEFAULT_OBJECT_OFFSET = 160;
static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const double DEFAULT_JUMP_VELOCITY = 7.2;
static const int CONSOLE_SIZE = 95;
static const double DEFAULT_X_JUMP_VELOCITY = 2.2;

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

BackgroundController::BackgroundController():
name(""),
type(Ctrl_Null),
timestart(0),
endtime(0),
looptime(-1),
ownticker(0),
value1(CONTROLLER_VALUE_NOT_SET),
value2(CONTROLLER_VALUE_NOT_SET),
value3(CONTROLLER_VALUE_NOT_SET){
}
BackgroundController::~BackgroundController(){
}

void BackgroundController::act(const double xaxis, const double yaxis){
    Global::debug(1) << "Control Name: " << name << "Control type: " << type << " is running." << endl;
    Global::debug(1) << "ticker: " << ownticker << " Start time: " << timestart << " End Time: " << endtime << endl;
    // Do we run this?
    if( ownticker >= timestart && ownticker <= endtime ){
	Global::debug(1) << "We have action, total backgrounds: " << backgrounds.size() << endl;
	for (std::vector<MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i){
	    MugenBackground *background = *i;
	    // Lets make sure this background is under control from a controller so it doesn't get reset
	    if (!background->underControl){
		background->underControl = true;
	    }
	    Global::debug(1) << "Acting on background: " << background->getName() << " | Type: " << type << endl;
	    switch (type){
		case Ctrl_Visible:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->setVisible(value1);
		    }
		    break;
		case Ctrl_Enabled:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->setEnabled(value1);
		    }
		    break;
		case Ctrl_VelSet:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->velocityx = value1;
			Global::debug(1) << "	Set X velocity to: " << value1 << endl;
		    }
		    if (value2 != CONTROLLER_VALUE_NOT_SET){
			background->velocityy = value2;
			Global::debug(1) << "	Set Y velocity to: " << value2 << endl;
		    }
		    break;
		case Ctrl_VelAdd:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->velocityx += value1;
		    }
		    if (value2 != CONTROLLER_VALUE_NOT_SET){
			background->velocityy += value2;
		    }
		    break;
		case Ctrl_PosSet:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->controller_offsetx = value1;
			Global::debug(1) << "	Set X position to: " << value1 << endl;
		    }
		    if (value2 != CONTROLLER_VALUE_NOT_SET){
			background->controller_offsety = value2;
			Global::debug(1) << "	Set Y position to: " << value2 << endl;
		    }
		    break;
		case Ctrl_PosAdd:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->controller_offsetx += value1;
			Global::debug(1) << "	Add to Position X: " << value1 << endl;
		    }
		    if (value2 != CONTROLLER_VALUE_NOT_SET){
			background->controller_offsety += value2;
			Global::debug(1) << "	Add to Position Y: " << value2 << endl;
		    }
		    break;
		case Ctrl_Animation:
		    // Lets not do this for now, I can pass in the animation lists later
		    break;
		case Ctrl_Sinx:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->sinx_amp = value1;
		    }
		    if (value2 != CONTROLLER_VALUE_NOT_SET){
			background->sinx_offset = value2;
		    }
		    if (value3 != CONTROLLER_VALUE_NOT_SET){
			background->sinx_period = value3;
		    }
		    break;
		case Ctrl_Siny:
		    if (value1 != CONTROLLER_VALUE_NOT_SET){
			background->siny_amp = value1;
		    }
		    if (value2 != CONTROLLER_VALUE_NOT_SET){
			background->siny_offset = value2;
		    }
		    if (value3 != CONTROLLER_VALUE_NOT_SET){
			background->siny_period = value3;
		    }
		    break;
		case Ctrl_Null:
		default:
		    break;
	    }
	    Global::debug(1) << "Background X: " << background->x << endl;
	}
    }
    // Shall we reset?
    if( (looptime != -1) && (ownticker > endtime) ){
	ownticker=0;
    }
    ownticker++;
}

void BackgroundController::reset(){
    if( looptime == -1){
	// I'm not totally convinced we should be doing this, but it seems to work otherwise....
	//ownticker = 0;
    }
}

/* our controller handler */
MugenBackgroundController::MugenBackgroundController(const std::string &n):
name(n),
id(DEFAULT_BACKGROUND_ID),
looptime(-1),
ticker(0){
}
MugenBackgroundController::~MugenBackgroundController(){
    // Kill all controllers initiated by the load
    for (std::vector<BackgroundController *>::iterator i = controls.begin(); i != controls.end(); ++i){
	    if(*i)delete *i;
    }
}
void MugenBackgroundController::addControl( BackgroundController *ctrl ){
    controls.push_back(ctrl);
}
void MugenBackgroundController::act(const double xaxis, const double yaxis){
    // Lets act out our controllers
    Global::debug(1) << "Controller Def: " << name << " | Total controls: " << controls.size() << endl;
    for (std::vector<BackgroundController *>::iterator i = controls.begin(); i != controls.end(); ++i){
	    BackgroundController *ctrl = *i;
	    Global::debug(1) << "Acting on Controller: " << ctrl->name << " | timestart: " << ctrl->timestart << " | endtime: " << ctrl->endtime << " | looptime" << ctrl->looptime << " | ticker: " << ctrl->ownticker << endl;
	    ctrl->act(xaxis, yaxis);
    }
    if( (looptime != -1) && (ticker > looptime) ){
	// Reset itself and everybody that needs reseting
	ticker = 0;
	for (std::vector<BackgroundController *>::iterator i = controls.begin(); i != controls.end(); ++i){
	    BackgroundController *ctrl = *i;
	    ctrl->reset();
	}
    }
    ticker++;
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
console(new Console(CONSOLE_SIZE)),
debugMode(false),
inleft(0),
inright(0),
onLeftSide(0),
onRightSide(0),
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
console(new Console(CONSOLE_SIZE)),
debugMode(false),
inleft(0),
inright(0),
onLeftSide(0),
onRightSide(0),
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
		MugenUtil::fixCase(itemhead);
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
		MugenUtil::fixCase(itemhead);
		if ( itemhead.find("intensity")!=std::string::npos ){
		    *content->getNext() >> shadow.k;
		    // *content->getNext() >> shadowIntensity;
                    // shadow.k = shadowIntensity;
		} else if ( itemhead == "reflect" ){
		    *content->getNext() >> reflect;
		} else if ( itemhead.find("color")!=std::string::npos ){
		    // int c,m,k;
		    *content->getNext() >> shadow.c;
		    *content->getNext() >> shadow.y;
		    *content->getNext() >> shadow.m;
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
		MugenUtil::fixCase(itemhead);
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
		MugenUtil::fixCase(itemhead);
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
		MugenUtil::fixCase(itemhead);
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
	else if( head.find("bgctrldef") != std::string::npos ){ 
	    head.replace(0,10,"");
	    MugenBackgroundController *temp = new MugenBackgroundController(head);
	    Global::debug(1) << "Found background controller definition: " << temp->name << endl;
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		Global::debug(1) << "Getting next item: " << itemhead << endl;
		if ( itemhead.find("eventid")!=std::string::npos ){
		    *content->getNext() >> temp->id;
		} else if (itemhead == "looptime"){
		    *content->getNext() >> temp->looptime;
		    if (temp->looptime == 0)temp->looptime = -1;
		} else if (itemhead == "ctrlid"){
		    // Max 10
		    while(content->hasItems()){
			int id;
			*content->getNext() >> id;
			getBackgrounds(temp->backgrounds, id);
		    }
		} else throw MugenException( "Unhandled option in BGCtrlDef " + head + " Section: " + itemhead );
	    }
	    // Give it all the backgrounds to make changes to
	    if ( temp->backgrounds.empty() ){
		temp->backgrounds.insert(temp->backgrounds.end(),backgrounds.begin(),backgrounds.end());
		temp->backgrounds.insert(temp->backgrounds.end(),foregrounds.begin(),foregrounds.end());
	    }
	    Global::debug(1) << "Controlling total backgrounds: " << temp->backgrounds.size() << endl;
	    controllers.push_back(temp);
	}
	else if( head.find("bgctrl") != std::string::npos ){ 
	    if (controllers.empty()){
		// This is a hack to get mugen to do some fancy controlling in a regular game
		// to accomplish stage fatalaties and other tricks
		Global::debug(1) << "Found a BgCtrl without a parent definition... must be hackery!" << endl;
		continue;
	    }
	    // else we got ourselves some controls... under the last controller added
	    MugenBackgroundController *control = controllers.back();
	    head.replace(0,7,"");
	    BackgroundController *temp = new BackgroundController();
	    temp->name = head;
	    Global::debug(1) << "Found background controller: " << temp->name << endl;
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		Global::debug(1) << "Getting next item: " << itemhead << endl;
		if ( itemhead.find("type")!=std::string::npos ){
		    std::string type;
		    *content->getNext() >> type;
		    MugenUtil::removeSpaces( type );
		    Global::debug(1) << "Type after lowercase: " << type << endl;
		    if( type == "Anim" )temp->type = Ctrl_Animation;
		    else if( type == "Enabled" )temp->type = Ctrl_Enabled;
		    else if( type == "Null" )temp->type = Ctrl_Null;
		    else if( type == "PosAdd" )temp->type = Ctrl_PosAdd;
		    else if( type == "PosSet" )temp->type = Ctrl_PosSet;
		    else if( type == "SinX" )temp->type = Ctrl_Sinx;
		    else if( type == "SinY" )temp->type = Ctrl_Siny;
		    else if( type == "VelAdd" )temp->type = Ctrl_VelAdd;
		    else if( type == "VelSet" )temp->type = Ctrl_VelSet;
		    else if( type == "Visible" )temp->type = Ctrl_Visible;
		} else if (itemhead == "time"){
		    int start=0,end=0,loop=0;
		    *content->getNext() >> start;
		    *content->getNext() >> end;
		    *content->getNext() >> loop;
		    temp->timestart = start;
		    if (end == 0)temp->endtime = start;
		    else temp->endtime = end;
		    if (loop == 0)temp->looptime = -1;
		    else temp->looptime = loop;
		    Global::debug(1) << "start: " << temp->timestart << " | end: " << temp->endtime << " | loop: " << temp->looptime << endl;
		} else if (itemhead == "value"){
		    *content->getNext() >> temp->value1;
		    *content->getNext() >> temp->value2;		    
		    *content->getNext() >> temp->value3;
		} else if (itemhead == "x"){
		    *content->getNext() >> temp->value1;
		} else if (itemhead == "y"){
		    *content->getNext() >> temp->value2;
		} else if (itemhead == "ctrlid"){
		    // Max 10
		    while(content->hasItems()){
			int id;
			*content->getNext() >> id;
			getBackgrounds(temp->backgrounds, id);
		    }
		} else throw MugenException( "Unhandled option in BGCtrl " + head + " Section: " + itemhead );
	    }
	    // Does it use its own background ids? else give it the main mamas backgrounds
	    if (temp->backgrounds.empty()){
		temp->backgrounds.insert(temp->backgrounds.end(),control->backgrounds.begin(),control->backgrounds.end());
	    }
	    Global::debug(1) << "Controlling total backgrounds: " << temp->backgrounds.size() << endl;
	    control->addControl(temp);
	}
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
	
    }
    
    Global::debug(1) << "Got total backgrounds: " << backgrounds.size() << " total foregrounds: " << foregrounds.size() << endl;
    // Setup board our worksurface to the proper size of the entire stage 320x240 :P
    Global::debug(1) << "Creating level size of Width: " << abs(boundleft) + boundright << " and Height: " << abs(boundhigh) + boundlow << endl;
    //board = new Bitmap( DEFAULT_WIDTH, DEFAULT_HEIGHT );
    // Nope we need it to be the size of the entire board... we then pan the blit so our characters will stay put without fiddling with their x coordinates
    board = new Bitmap( abs(boundleft) + boundright + DEFAULT_WIDTH, abs(boundhigh) + boundlow + DEFAULT_HEIGHT);
    camerax = startx;
    cameray = starty;
    xaxis = startx + abs(boundleft);
    yaxis = starty + abs(boundhigh);
    
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	if( (*i)->actionno != -1 ){
	    (*i)->action = animations[ (*i)->actionno ];
	}
	// now load
	(*i)->preload( startx, starty );
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	if( (*i)->actionno != -1 ){
	    (*i)->action = animations[ (*i)->actionno ];
	}
	// now load
	(*i)->preload( startx, starty );
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
    
    // Console stuff
    console->setTextHeight(10);
    console->setTextWidth(10);
}

void MugenStage::setCamera( const double x, const double y ){ 
    camerax = x; cameray = y; 
    // Camera boundaries
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
}
void MugenStage::moveCamera( const double x, const double y ){ 
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
    
    // Clear console so we can see our debug
    console->clear();
    
    //zoffsetlink
    const MugenBackground *zlinkbackground = 0;
    if( zoffsetlink != DEFAULT_BACKGROUND_ID ){
	zlinkbackground = getBackground(zoffsetlink);
	zoffset = zlinkbackground->y;
    }
    *console << "zoffsetlink ID: " << zoffsetlink << " | zoffset: " << zoffset << " | floortension: " << floortension << Console::endl;
    
    // Backgrounds
    for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	(*i)->logic( diffx, diffy, xaxis + camerax, yaxis + cameray );
    }
    
    // Players go in here
    std::vector<Object *> add;
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it){
        /* use local variables more often, iterators can be easily confused */
        Object * player = *it;
        player->act( &objects, (World *)this, &add);
	
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
			if (isaPlayer( player )){
			    if (enemy->getX() > player->getX() && enemy->getFacing() != Object::FACING_LEFT ){
				enemy->setFacing(Object::FACING_LEFT);
			    }

			    if (enemy->getX() < player->getX() && enemy->getFacing() != Object::FACING_RIGHT ){
				enemy->setFacing(Object::FACING_RIGHT);
			    }
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
	    if ( player->getHealth() < 30){//player->getMaxHealth() ){
		player->setHealth( player->getMaxHealth() );
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

	// Debug crap put it on console
	*console << "Object: " << player << " x: " << player->getX() << " y: " << player->getY() << Console::endl;
    }
    objects.insert(objects.end(),add.begin(),add.end());
    
    // Foregrounds
    for( vector< MugenBackground *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	(*i)->logic( diffx, diffy, xaxis + camerax, yaxis + cameray );
    }
    
    // Controllers
    for( vector< MugenBackgroundController *>::iterator i = controllers.begin(); i != controllers.end(); ++i ){
	(*i)->act(xaxis, yaxis);
    }
    // Console
    *console << "Camera X: " << getCameraX() << " Camera Y: " << getCameraY() << Console::endl;
    *console << "Frames: " << getTicks() << Console::endl;
    console->act();
}
	
void MugenStage::render( Bitmap *work ){
    // Clear to magenta like big daddy mugen does
    if( debugbg )board->floodfill( 0, 0, Bitmap::makeColor(255,0,255) );
    for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	(*i)->render( (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    }
    
    // Players go in here
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	Object *obj = *it;
	// Reflection
	if (reflectionIntensity)obj->drawReflection( board, 0, reflectionIntensity );
	// Shadow
	obj->drawShade( board, 0, shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeMid, shadowFadeRangeHigh);
        obj->draw( board, 0);
    }
    
    for( vector< MugenBackground *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	(*i)->render( (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    }
    
    // Player debug
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	if (isaPlayer(*it)){
	    Character *character = (Character*)*it;
	    // Player debug crap
	    if (debugMode){
		// Players x positioning
		board->vLine( 150, character->getX(), character->getZ(), Bitmap::makeColor( 255, 0, 0));
	    }
	}
    }
    
    // Debug crap for board coordinates
    if (debugMode){
	board->hLine( 0, zoffset - floortension, board->getWidth(), Bitmap::makeColor( 0,255,0 ));
    }
    
    //board->Blit( xaxis + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), yaxis + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT, 0, 0, *work );
    board->Blit( xaxis + camerax, yaxis + cameray, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0,0, *work);
    
    // Debug crap for screen coordinates
    if (debugMode){
	work->vLine( 0, tension, 240, Bitmap::makeColor( 0,255,0 ));
	work->vLine( 0, 320 - tension, 240, Bitmap::makeColor( 0,255,0 ));
    }
    
    // Life bars, will eventually be changed out with mugens interface
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	int p1Side = 5;
	int p2Side = 5;
	if (isaPlayer(*it)){
	    Character *character = (Character*)*it;
	    if ( character->getAlliance() == Player1Side ){
		character->drawLifeBar( 5, p1Side, work );
		p1Side +=10;
	    } else if ( character->getAlliance() == Player2Side ){
		character->drawLifeBar( 215, p2Side, work );
		p2Side +=10;
	    }
	}
    }
    
    // Render console
    console->draw( *work );
}

void MugenStage::reset( ){
    camerax = startx; cameray = starty;
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	// reset just reloads it to default
	MugenBackground *background = *i;
	if (!background->underControl){
	    background->preload( startx, starty );
	}
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	// reset
	MugenBackground *background = *i;
	if (!background->underControl){
	    background->preload( startx, starty );
	}
    }
    
    // Reset player positions
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	Object *player = *it;
	if( player->getAlliance() == Player1Side ){
	    //((Player *)player)->deathReset();
	    player->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p1startx );
	    player->setY( p1starty );
	    player->setZ( zoffset );
	    player->setFacing( Object::FACING_RIGHT );
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
	    playerInfo[player].leftTension = false;
	    playerInfo[player].rightTension = false;
	    playerInfo[player].leftSide = false;
	    playerInfo[player].rightSide = false;
	    playerInfo[player].jumped = false;
	} else if( player->getAlliance() == Player2Side ){
	    //((Player *)player)->deathReset();
	    player->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p2startx );
	    player->setY( p2starty );
	    player->setZ( zoffset );
	    player->setFacing( Object::FACING_LEFT );
	    playerInfo[player].oldx = player->getX();
	    playerInfo[player].oldy = player->getY();
	    playerInfo[player].leftTension = false;
	    playerInfo[player].rightTension = false;
	    playerInfo[player].leftSide = false;
	    playerInfo[player].rightSide = false;
	    playerInfo[player].jumped = false;
	}
    }
    
    inleft = inright = onLeftSide = onRightSide = 0;
}

// Add player1 people
void MugenStage::addp1( Object * o ){
    o->setAlliance(Player1Side);
    o->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p1startx );
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
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;
}

// Add player2 people
void MugenStage::addp2( Object * o ){
    o->setAlliance(Player2Side);
    o->setX( DEFAULT_OBJECT_OFFSET + abs(boundleft) + p2startx );
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
    playerInfo[o].leftSide = false;
    playerInfo[o].rightSide = false;
    playerInfo[o].jumped = false;
}

// Console
void MugenStage::toggleConsole(){ 
    console->toggle(); 
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
    
    // Get rid of control lists;
    for( std::vector< MugenBackgroundController * >::iterator i = controllers.begin() ; i != controllers.end() ; ++i ){
	if( (*i) )delete (*i);
    }
    
    if (board) delete board;
    
    if (console) delete console;
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
    // Z/Y offset
    if (zoffsetlink == DEFAULT_BACKGROUND_ID){
	o->setZ( zoffset + abs(boundhigh) );
    } else {
	o->setZ( zoffset );
    }
    
    // Move X and Camera
    const double px = o->getX();
    const double py = o->getY();
    const double pdiffx = px - playerInfo[o].oldx;
    const double screenLeft = abs(boundleft) + camerax;
    const double screenRight = abs(boundleft) + camerax + DEFAULT_WIDTH;
    const double leftTension = screenLeft + tension;
    const double rightTension = screenRight - tension;
    // Check leftbound rightbound
    if (px <= (abs(boundleft) + boundleft)){ 
	o->setX(abs(boundleft) + boundleft );
	playerInfo[o].oldx = px;
    } else if (px >= (DEFAULT_WIDTH + abs(boundleft) + boundright)){
	o->setX(DEFAULT_WIDTH + abs(boundleft) + boundright );
	playerInfo[o].oldx = px;
    } 
    
    // Check if in tension
    if (px <= leftTension){
	if (!playerInfo[o].leftTension){
	    playerInfo[o].leftTension = true;
	    inleft++;
	}
    } else if (px >= rightTension){
	if (!playerInfo[o].rightTension){
	    playerInfo[o].rightTension = true;
	    inright++;
	}
    } 
    
    // Release tension
    if (px > leftTension){
	if (playerInfo[o].leftTension){
	    playerInfo[o].leftTension = false;
	    inleft--;
	}
    }
    // Release tension
    if (px < rightTension){
	if (playerInfo[o].rightTension){
	    playerInfo[o].rightTension = false;
	    inright--;
	}
    }
    
    // Check Screen sides
    if (px <= screenLeft){
	o->setX(screenLeft);
	playerInfo[o].oldx = px;
	if (!playerInfo[o].leftSide){
	    playerInfo[o].leftSide = true;
	    onLeftSide++;
	}
    } else if (px >= screenRight){
	o->setX(screenRight);
	playerInfo[o].oldx = px;
	if (!playerInfo[o].rightSide){
	    playerInfo[o].rightSide = true;
	    onRightSide++;
	}
    } 
    // Release side
    if (px > screenLeft){
	if (playerInfo[o].leftSide){
	    playerInfo[o].leftSide = false;
	    onLeftSide--;
	}
    }
    // Release side
    if (px < screenRight){
	if (playerInfo[o].rightSide){
	    playerInfo[o].rightSide = false;
	    onRightSide--;
	}
    }
    
    //Global::debug(0) << "Left Tension: " << inleft << " | Right Tension: "<< inright << endl;
    //Global::debug(0) << "Left Screen Edge: " << onLeftSide << " | Right Screen Edge: "<< onRightSide << endl;
    
    if (playerInfo[o].leftTension){
	if (pdiffx < 0){
	    if (!onRightSide){
		moveCamera(pdiffx,0);
	    }
	} else if (pdiffx > 0){
	    if (inright){
		moveCamera(pdiffx,0);
	    }
	}
    } else if (playerInfo[o].rightTension){
	if (pdiffx > 0){
	    if(!onLeftSide){
		moveCamera(pdiffx,0);
	    }
	} else if (pdiffx < 0){
	    if(inleft){
		moveCamera(pdiffx,0);
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
    // Correct camera
    if ((verticalfollow > 0) && (!inabove && getCameraX() < 0)){
	moveCamera( 0, verticalfollow * 3.2 );
    }
    //Global::debug(1) << "Our players Y: " << py << " | Above: "<< playerInfo[o].above << " | total inabove: " << inabove << endl;
}

// This is for controllers as sometimes backgrounds share IDs for this purpose
void MugenStage::getBackgrounds( std::vector<MugenBackground *> &bgs, int ID ){
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	if( (*i)->id == ID ){
	    bgs.push_back( *i );
	}
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	if( (*i)->id == ID ){
	    bgs.push_back( *i );
	}
    }
}


