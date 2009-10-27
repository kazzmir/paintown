#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <ostream>
#include <sstream>
#include <iostream>

#include "mugen_stage.h"

#include "util/funcs.h"
#include "util/file-system.h"
#include "util/bitmap.h"
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "globals.h"
#include "factory/font_render.h"
#include "ast/all.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"

/* this is bad.. peg.py should just produce a .h file we can include */
namespace Mugen{
    namespace Def{
        extern const void * main(const std::string & filename);
    }
}

using namespace std;

// Some static variables
static const int CONTROLLER_VALUE_NOT_SET = -999999;
static const int DEFAULT_BACKGROUND_ID = -9999;
static const int DEFAULT_OBJECT_OFFSET = 160;
static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const double DEFAULT_JUMP_VELOCITY = 7.2;
static const int CONSOLE_SIZE = 95;
static const double DEFAULT_X_JUMP_VELOCITY = 2.2;

static bool centerCollision( Character *p1, Character *p2 ){
    //p1->getCurrentMovement()->getCurrentFrame();
    const int p1width = p1->getCurrentMovement()->getCurrentFrame()->getWidth(), p1height = p1->getCurrentMovement()->getCurrentFrame()->getHeight();
    const int p2width = p2->getCurrentMovement()->getCurrentFrame()->getWidth(), p2height = p2->getCurrentMovement()->getCurrentFrame()->getHeight();
    
    const double x1 = p1->getX() + ((p1width/2)/2), y1 = 0, x2 = p1->getX() + p1width - ((p1width/2)/2), y2 = p1height, 
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
//sffFile(""),
//debugbg(false),
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
console(new Console::Console(CONSOLE_SIZE)),
debugMode(false),
inleft(0),
inright(0),
onLeftSide(0),
onRightSide(0),
inabove(0),
loaded(false){
    initializeName();
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
//sffFile(""),
//debugbg(false),
background(0),
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
console(new Console::Console(CONSOLE_SIZE)),
debugMode(false),
inleft(0),
inright(0),
onLeftSide(0),
onRightSide(0),
inabove(0),
loaded(false){
    initializeName();
}

MugenStage::~MugenStage(){
    cleanup();
}

/* fix */
void MugenStage::loadSectionCamera(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "startx"){
                (*simple) >> startx;
            } else if (*simple == "starty"){
                *simple >> starty;
            } else if (*simple == "boundleft"){
                *simple >> boundleft;
            } else if (*simple == "boundright"){
                *simple >> boundright;
            } else if (*simple == "boundhigh"){
                *simple >> boundhigh;
            } else if (*simple == "boundlow"){
                // This is always 0 so don't grab it
                // *content->getNext() >> boundlow;
            } else if (*simple == "verticalfollow"){
                *simple >> verticalfollow;
                if (verticalfollow > 1){
                    verticalfollow = 1;
                } else if (verticalfollow < 0){
                    verticalfollow = 0;
                }
            } else if (*simple == "floortension"){
                *simple >> floortension;
            } else if (*simple == "tension"){
                *simple >> tension;
            } else {
                throw MugenException( "Unhandled option in Camera Section: " + simple->toString());
            }
        }
    }
}

void MugenStage::loadSectionInfo(Ast::Section * section){
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "name"){
                /* use setName() here */
                *simple >> name;
                Global::debug(1) << "Read name '" << name << "'" << endl;
            } else if (*simple == "author"){
                // Seems to be that some people think that author belongs in background defs
                std::string temp;
                *simple >> temp;
                Global::debug(1) << "Made by this guy: '" << temp << "'" << endl;
            } else {
                throw MugenException( "Unhandled option in Info Section: " + simple->toString());
            }
        }
    }
}

/* fix */
void MugenStage::loadSectionPlayerInfo(Ast::Section * section){
    /*
    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Mugen::Util::removeSpaces(itemhead);
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
            */
}

/* fix */
void MugenStage::loadSectionBound(Ast::Section * section){
    /*
    while( collection[i]->hasItems() ){
        MugenItemContent *content = collection[i]->getNext();
        const MugenItem *item = content->getNext();
        std::string itemhead = item->query();
        Mugen::Util::removeSpaces(itemhead);
        if ( itemhead.find("screenleft")!=std::string::npos ){
            *content->getNext() >> screenleft;
        } else if ( itemhead.find("screenright")!=std::string::npos ){
            *content->getNext() >> screenright;
        } else throw MugenException( "Unhandled option in Bound Section: " + itemhead );
    }
    */
}

/* fix */
void MugenStage::loadSectionStageInfo(Ast::Section * section){
    /*
    while( collection[i]->hasItems() ){
        MugenItemContent *content = collection[i]->getNext();
        const MugenItem *item = content->getNext();
        std::string itemhead = item->query();
        Mugen::Util::removeSpaces(itemhead);
        Mugen::Util::fixCase(itemhead);
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
    */
}

/* fix */
void MugenStage::loadSectionShadow(Ast::Section * section){
    /*
    while( collection[i]->hasItems() ){
        MugenItemContent *content = collection[i]->getNext();
        const MugenItem *item = content->getNext();
        std::string itemhead = item->query();
        Mugen::Util::removeSpaces(itemhead);
        Mugen::Util::fixCase(itemhead);
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
    */
}

/* fix */
void MugenStage::loadSectionReflection(Ast::Section * section){
    /*
    while( collection[i]->hasItems() ){
        MugenItemContent *content = collection[i]->getNext();
        const MugenItem *item = content->getNext();
        std::string itemhead = item->query();
        Mugen::Util::removeSpaces(itemhead);
        Mugen::Util::fixCase(itemhead);
        if ( itemhead.find("intensity")!=std::string::npos ){
            *content->getNext() >> reflectionIntensity;
        } else throw MugenException( "Unhandled option in Reflection Section: " + itemhead );
    }
    */
}
	
void MugenStage::load(){
    if (loaded)return;
    // Lets look for our def since some assholes think that all file systems are case insensitive
    baseDir = Filesystem::find("mugen/stages/");
    Global::debug(1) << baseDir << endl;
    if (location.find(".def")==std::string::npos){
	location+=".def";
    }
    // Get correct directory
    baseDir = Mugen::Util::getFileDir(baseDir + location);
    location = Mugen::Util::stripDir(location);
    const std::string ourDefFile = Mugen::Util::getCorrectFileLocation(baseDir, location);//Mugen::Util::fixFileName( baseDir, std::string(location) );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate stage definition file for: " + location );
    
    std::string filesdir = "";
    
    size_t strloc = location.find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = location.substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;

    list<Ast::Section*> * sections = (list<Ast::Section*>*) Mugen::Def::main(ourDefFile);

    struct cymk_holder{
        cymk_holder():c(0),m(0),y(0),k(0){}
        int c, m, y, k;
    } shadow;
    
    /* Extract info for our first section of our stage */
    for (list<Ast::Section*>::iterator section_it = sections->begin(); section_it != sections->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	Mugen::Util::fixCase(head);
	if (head == "info"){
            loadSectionInfo(section);
        } else if (head == "camera"){
            loadSectionCamera(section);
	} else if (head == "playerinfo"){
            loadSectionPlayerInfo(section);
	} else if (head == "scaling"){
            /* not used anymore.. print a warning? */
        } else if (head == "bound"){
            loadSectionBound(section);
	} else if (head == "stageinfo"){
            loadSectionStageInfo(section);
	} else if (head == "shadow"){
            loadSectionShadow(section);
	} else if (head == "reflection"){
            loadSectionReflection(section);
	} else if (head == "bgdef"){
	    // Background management
            /* fix */
            /*
	    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir,collection, i,ticker,0);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
            */
	}
	else if (head == "music" ){
            /* Ignore for now */
        } else {
            throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
        }
    }
    
    for (list<Ast::Section*>::iterator section_it = sections->begin(); section_it != sections->end(); section_it++){
        delete (*section_it);
    }
    delete sections;

     
#if 0
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();

    struct cymk_holder{
        cymk_holder():c(0),m(0),y(0),k(0){}
        int c, m, y, k;
    } shadow;
    
    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	Mugen::Util::fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Mugen::Util::removeSpaces(itemhead);
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
		Mugen::Util::removeSpaces(itemhead);
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
		Mugen::Util::removeSpaces(itemhead);
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
		Mugen::Util::removeSpaces(itemhead);
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
		Mugen::Util::removeSpaces(itemhead);
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
		Mugen::Util::removeSpaces(itemhead);
		Mugen::Util::fixCase(itemhead);
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
		Mugen::Util::removeSpaces(itemhead);
		Mugen::Util::fixCase(itemhead);
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
		Mugen::Util::removeSpaces(itemhead);
		Mugen::Util::fixCase(itemhead);
		if ( itemhead.find("intensity")!=std::string::npos ){
		    *content->getNext() >> reflectionIntensity;
		} else throw MugenException( "Unhandled option in Reflection Section: " + itemhead );
	    }
	}
	else if( head == "bgdef" ){
	    // Background management
	    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir,collection, i,ticker,0);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	}
	else if( head == "music" ){ /* Ignore for now */ }
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
	
    }
#endif
    
    // Setup board our worksurface to the proper size of the entire stage 320x240 :P
    Global::debug(1) << "Creating level size of Width: " << abs(boundleft) + boundright << " and Height: " << abs(boundhigh) + boundlow << endl;
    //board = new Bitmap( DEFAULT_WIDTH, DEFAULT_HEIGHT );
    // Nope we need it to be the size of the entire board... we then pan the blit so our characters will stay put without fiddling with their x coordinates
    board = new Bitmap( abs(boundleft) + boundright + DEFAULT_WIDTH, abs(boundhigh) + boundlow + DEFAULT_HEIGHT);
    camerax = startx;
    cameray = starty;
    xaxis = (abs(boundleft) + boundright + DEFAULT_WIDTH)/2;//abs(boundleft);
    yaxis = abs(boundhigh);
    
    // Preload background
    background->preload( startx, starty );
    
    // zoffsetlink
    if( zoffsetlink != DEFAULT_BACKGROUND_ID ){
	// Link zoffset to id
	zoffset = background->getBackground(zoffsetlink)->y;
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
    
    // Stage is loaded
    loaded = true;
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

void MugenStage::logic( ){
    Console::ConsoleEnd & cend = Console::Console::endl;
    // camera crap
    if (quake_time > 0){
	quake_time--;
    }
    
    // implement some stuff before we actually begin the round then start the round
    if(!stageStart)stageStart = true;
    
    // Run our ticker on and on like energizer bunnies (tm)
    ticker++;
    
    const double diffx = startx - camerax;
    const double diffy = starty - cameray;
    
    // Clear console so we can see our debug
    console->clear();
    
    //zoffsetlink
    const MugenBackground *zlinkbackground = 0;
    if( zoffsetlink != DEFAULT_BACKGROUND_ID ){
	zlinkbackground = background->getBackground(zoffsetlink);
	zoffset = zlinkbackground->y;
    }
    *console << "zoffsetlink ID: " << zoffsetlink << " | zoffset: " << zoffset << " | floortension: " << floortension << cend;
    
    // Backgrounds
    background->logic( diffx, diffy, xaxis + camerax, yaxis + cameray );
    
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
			    if (enemy->getX() > player->getX() && enemy->getFacing() != Object::FACING_LEFT && ((Character *)enemy)->getStatus() == Status_Ground){
				enemy->setFacing(Object::FACING_LEFT);
			    }

			    if (enemy->getX() < player->getX() && enemy->getFacing() != Object::FACING_RIGHT && ((Character *)enemy)->getStatus() == Status_Ground){
				enemy->setFacing(Object::FACING_RIGHT);
			    }
			}
		    }
		}
		// Attacking
		/*if ( player->isAttacking() ){
		    ObjectAttack * playerAttack = (ObjectAttack *)player;
		    if ( enemy != player && enemy->isCollidable( player ) && player->isCollidable( enemy ) ){
			if ( player->ZDistance( enemy ) <= playerAttack->minZDistance() && enemy->collision( playerAttack ) ){ 
			    double x = 0, y = 0;
			    
			    x = enemy->getX();
			    y = enemy->getRY() - enemy->getHeight() + enemy->getHeight() / 3;*/
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
/*
			    playerAttack->attacked(this, enemy, add );
			    enemy->collided( playerAttack, add );
			    enemy->takeDamage( this, playerAttack, playerAttack->getDamage() );
			}    
		    }
		}*/
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
	*console << "Object: " << player << " x: " << player->getX() << " y: " << player->getY() << cend;
    }
    objects.insert(objects.end(),add.begin(),add.end());
    
    // Correct camera
    if ((verticalfollow > 0) && !inabove && (getCameraY() < 0)){
	moveCamera( 0, verticalfollow * 3.2 );
    }
    
    // Console
    *console << "Camera X: " << getCameraX() << " Camera Y: " << getCameraY() << cend;
    *console << "Frames: " << getTicks() << cend;
    console->act();
}
	
void MugenStage::render( Bitmap *work ){
    
    // Background
    background->renderBack( (xaxis + camerax) - DEFAULT_OBJECT_OFFSET, yaxis + cameray, (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    
    // Players go in here
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	Object *obj = *it;
	// Reflection
	if (reflectionIntensity)obj->drawReflection( board, 0, reflectionIntensity );
	// Shadow
	obj->drawShade( board, 0, shadowIntensity, shadowColor, shadowYscale, shadowFadeRangeMid, shadowFadeRangeHigh);
        obj->draw( board, 0);
    }
    // Foreground
    background->renderFront( (xaxis + camerax) - DEFAULT_OBJECT_OFFSET, yaxis + cameray, (DEFAULT_WIDTH + (abs(boundleft) + boundright)), DEFAULT_HEIGHT + abs(boundhigh) + boundlow, board );
    
    // Player debug
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
	if (isaPlayer(*it)){
	    Character *character = (Character*)*it;
	    // Player debug crap
	    if (debugMode){
		// Players x positioning
		board->vLine( 150, (int)character->getX(), (int)character->getZ(), Bitmap::makeColor( 255, 0, 0));
	    }
	}
    }
    
    // Debug crap for board coordinates
    if (debugMode){
	board->hLine( 0, abs(boundhigh) + zoffset, board->getWidth(), Bitmap::makeColor( 0,255,0 ));
	board->vLine( 0, xaxis, board->getHeight(), Bitmap::makeColor(255,0,0));
    }
    
    board->Blit( (int)(abs(boundleft) + camerax) + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), (int)(yaxis + cameray) + ( quake_time > 0 ? Util::rnd( 9 ) - 4 : 0 ), DEFAULT_WIDTH, DEFAULT_HEIGHT, 0,0, *work);
    
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
    /*for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	// reset just reloads it to default
	MugenBackground *background = *i;
	if (resetBG){
	    background->preload( startx, starty );
	}
    }
    for( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	// reset
	MugenBackground *background = *i;
	if (resetBG){
	    background->preload( startx, starty );
	}
    }*/
    background->reset(startx,starty,resetBG);
    
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
bool MugenStage::finished() const { return false; }
void MugenStage::reloadLevel() throw( LoadException ){ 
    cleanup();
    loaded = false;
    load(); 
}
Script::Engine * MugenStage::getEngine() const { return NULL; }
/* upper left hand corner of the screen */
int MugenStage::getX(){
    return (int)getCameraX();
}

int MugenStage::getY(){
    return (int)getCameraY();
}
/* this shouldn't be here */
// I guess ignore this one
const deque<Bitmap*> & MugenStage::getScreenshots(){
    return garbage;
}
int MugenStage::levelLength() const { return 0; }
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
		o->takeDamage(*this, NULL, 999999 );
    }
}
void MugenStage::addMessage(Network::Message m, Network::Socket from){ }
Network::Message MugenStage::createBangMessage( int x, int y, int z ){
    Network::Message m;
    return m;
}
        
bool MugenStage::isPaused(){
    return false;
}
        
void MugenStage::pause(){
}

void MugenStage::unpause(){
}

void MugenStage::changePause(){
}

const std::string MugenStage::getStageName(const std::string &filename) throw (MugenException){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string dir = Filesystem::find("mugen/stages/");
    Global::debug(1) << dir << endl;
    string fullname = filename;
    if ( fullname.find(".def") == std::string::npos){
	fullname += ".def";
    }
    const std::string defFile = Mugen::Util::fixFileName( dir, std::string(fullname) );
    
    if (defFile.empty()){
        throw MugenException( "Cannot locate stage definition file for: " + fullname );
    }
    
    std::string filesdir = "";
    
    size_t strloc = fullname.find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = fullname.substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;
     
    MugenReader reader( defFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();

    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	Mugen::Util::fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Mugen::Util::removeSpaces(itemhead);
		if ( itemhead.find("name")!=std::string::npos ){
		    std::string stageName;
		    *content->getNext() >> stageName;
                    Global::debug(1) << "Read name '" << stageName << "'" << endl;
		    return stageName;
		} 
	    }
	}
    }
    
    throw MugenException( "Cannot locate stage definition file for: " + fullname );
    return "";
}

void MugenStage::cleanup(){
    if (loaded){
	if (background){
	    delete background;
	    background = 0;
	}
	
	if (board){
	    delete board;
	    board = 0;
	}
	
	if (console){
	    delete console;
	    console = 0;
	}
    }
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
    //Global::debug(1) << "Our players Y: " << py << " | Above: "<< playerInfo[o].above << " | total inabove: " << inabove << endl;
}

void MugenStage::initializeName(){
    try{
	std::string str = this->location;
	// Lets look for our def since some assholes think that all file systems are case insensitive
	baseDir = Filesystem::find("mugen/stages/");
	Global::debug(1) << baseDir << endl;
	if (str.find(".def")==std::string::npos){
	    str+=".def";
	}
	// Get correct directory
	baseDir = Mugen::Util::getFileDir(baseDir + str);
	str = Mugen::Util::stripDir(str);
	const std::string ourDefFile = Mugen::Util::getCorrectFileLocation(baseDir, str);
	// Set name of map
	name = Mugen::Util::probeDef(ourDefFile, "info", "name");
    } catch (MugenException &ex){
	Global::debug(1) << "Couldn't find the name of the map!" << endl;
	Global::debug(1) << "Error was: " << ex.getReason() << endl;
    }
}


