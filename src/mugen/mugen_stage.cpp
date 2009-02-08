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
#include "globals.h"

#include "game/adventure_world.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"

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

MugenStage::MugenStage( const string & s ):
AdventureWorld(),
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
autoturn(true),
resetBG(true),
shadowIntensity(128),
color(Bitmap::makeColor(0,0,0)),
yscale(0.4),
fadeRangeHigh(0),
fadeRangeMid(0),
reflectionIntensity(0),
sffFile(""),
debugbg(false),
board(0),
xaxis(0),
yaxis(0),
camerax(0),
cameray(0),
totalRounds(3),
round(1),
totalTime(99),
time(99),
p1points(0),
p2points(0){
}

MugenStage::MugenStage( const char * location ): AdventureWorld(),
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
color(Bitmap::makeColor(0,0,0)),
yscale(0.4),
fadeRangeHigh(0),
fadeRangeMid(0),
reflectionIntensity(0),
sffFile(""),
debugbg(false),
board(0),
xaxis(0),
yaxis(0),
camerax(0),
cameray(0),
totalRounds(3),
round(1),
totalTime(99),
time(99),
p1points(0),
p2points(0){
}

MugenStage::~MugenStage(){
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
	
void MugenStage::load() throw( MugenException ){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    baseDir = Util::getDataPath() + "mugen/stages/";
    Global::debug(1) << baseDir << endl;
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, std::string(location) );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate stage definition file for: " + location );
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
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
		    *content->getNext() >> boundlow;
		} else if ( itemhead.find("verticalfollow")!=std::string::npos ){
		    *content->getNext() >> verticalfollow;
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
		if ( itemhead.find("zoffset")!=std::string::npos ){
		    *content->getNext() >> zoffset;
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
		    *content->getNext() >> shadowIntensity;
		} else if ( itemhead.find("color")!=std::string::npos ){
		    int r,b,g;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    color = Bitmap::makeColor(r,g,b);
		} else if ( itemhead.find("yscale")!=std::string::npos ){
		    *content->getNext() >> yscale;
		} else if ( itemhead.find("fade.range")!=std::string::npos ){
		    *content->getNext() >> fadeRangeHigh;
		    *content->getNext() >> fadeRangeMid;
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
		    MugenUtil::readSprites( MugenUtil::fixFileName(baseDir, sffFile), "", sprites );
		} else if ( itemhead.find("debugbg")!=std::string::npos ){
		    *content->getNext() >> debugbg;
		} else throw MugenException( "Unhandled option in Reflection Section: " + itemhead );
	    }
	}
	else if( head == "music" ){ /* Ignore for now */ }
	// This our background data definitions
	else if( head.find("bg") !=std::string::npos ){
	    MugenBackground *temp = new MugenBackground();
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
		    *content->getNext() >> temp->xscaletop;
		    *content->getNext() >> temp->xscalebot;
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
	else if( head.find("BGCtrlDef") != std::string::npos ){ /* Ignore for now */ }
	else if( head.find("BGCtrl") != std::string::npos ){ /* Ignore for now */ }
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
	
    }
    
    Global::debug(1) << "Got total backgrounds: " << backgrounds.size() << " total foregrounds: " << foregrounds.size() << endl;
    // Setup board our worksurface to the proper size of the entire stage 320x240 :P
    Global::debug(1) << "Creating level size of Width: " << abs(boundleft) + boundright << " and Height: " << abs(boundhigh) + boundlow << endl;
    board = new Bitmap( 320, 240 );
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
}

void MugenStage::logic( ){
    
    if( camerax < boundleft ) camerax = boundleft;
    else if( camerax > boundright )camerax = boundright;
    if( cameray < boundhigh ) cameray = boundhigh;
    else if( cameray > boundlow )cameray = boundlow;
    
    const int diffx = startx - camerax;
    const int diffy = starty - cameray;
    
    for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	(*i)->logic( diffx, diffy );
    }
    
    // Players go in here
    
    for( vector< MugenBackground *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	(*i)->logic( diffx, diffy );
    }
    
}
	
void MugenStage::render( Bitmap *work ){
    // Clear to magenta like big daddy mugen does
    if( debugbg )board->floodfill( 0, 0, Bitmap::makeColor(255,0,255) );
    for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	(*i)->render( (320 + (abs(boundleft) + boundright)), 240 + abs(boundhigh) + boundlow, board );
    }
    
    // Players go in here
    
    for( vector< MugenBackground *>::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	(*i)->render( (320 + (abs(boundleft) + boundright)), 240 + abs(boundhigh) + boundlow, board );
    }
    
    board->Blit( xaxis, yaxis, 320, 240, 0, 0, *work );
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
}

void MugenStage::act(){
    logic();
}
void MugenStage::draw( Bitmap * work ){
    render( work );
}

