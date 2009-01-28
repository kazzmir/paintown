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

#include "mugen_animation.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_character.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "globals.h"


static int lowerCase( int c ){ return tolower( c );}

static void fixCase( std::string &str ){
    transform( str.begin(), str.end(), str.begin(), lowerCase );
}

static void removeSpaces( std::string &str ){
    if( str.find(' ') != std::string::npos ){
	Global::debug(1) << "Removing spaces from: " << str << endl;
	for( int i = str.size()-1; i>-1; --i){
	    if( str[i] == ' ' )str.erase( str.begin()+i );
	    else if( str[i] == '\t' )str.erase( str.begin()+i );
	}
    }
}

static void invertSlashes( std::string &str ){
    if( str.find('\\') != std::string::npos ){
	for( int i = str.size()-1; i>-1; --i){
	    if( str[i] == '\\' )str[i] = '/';
	}
    }
}

static std::string fixFileName( const std::string &dir, std::string str ){
    Global::debug(1) << "Current File: " << str << endl;
    // Temp fix until the lexer fixes this crap
    removeSpaces( str );
    // Fixes stupid windows users shit
    invertSlashes( str );
    // Lets check if we need to fix anything first
    if( Util::exists( dir + str ) == false ){
	Global::debug(1) << "Couldn't find file: " << str << endl;
	std::string returnString = "";
	std::vector< string > files = Util::getFiles( dir, "*" );
	Global::debug(1) << "Correcting file: " << str << ", in directory: "<< dir <<".\nGot " << files.size() << " files." << endl;
	for( unsigned int i = 0; i < files.size(); ++i ){
	    std::string temp = files[i].c_str();
	    fixCase( temp );
	    if( std::string( dir + str ) == temp ){
		// We got number one chinese retaurant
		returnString = files[i];
		break;
	    }
	}
	Global::debug(1) << "Corrected file: " << returnString << endl;
	return returnString;
    }
    return std::string(dir + str);
}

// If you use this, please delete the item after you use it, this isn't java ok
static MugenItemContent *parseOpt( const std::string &opt ){
    std::string contentHolder = "";
    MugenItemContent *temp = new MugenItemContent();
    const char * ignored = "\r\n";
    Global::debug(1) << "Parsing string to ItemContent: " << opt << endl;
    for( unsigned int i = 0; i < opt.size(); ++i ){
	if( opt[i] == ' ' ){
	    continue;
	}
	// We got one push back the other and reset the holder to get the next
	else if( opt[i] == ',' ){
	    if( !contentHolder.empty() ) *temp << contentHolder;
	    Global::debug(1) << "Got content: " << contentHolder << endl;
	    contentHolder = "";
	}
	//Start grabbing our item
	else if (! strchr(ignored, opt[i])){
		contentHolder += opt[i];
	}
    }
    return temp;
}

MugenStage::MugenStage( const string & s ):
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
topbound(0),
botbound(0),
topz(0),
botz(50),
topscale(1),
botscale(1.2),
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
debugbg(0){
}

MugenStage::MugenStage( const char * location ):
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
topbound(0),
botbound(0),
topz(0),
botz(50),
topscale(1),
botscale(1.2),
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
debugbg(0){
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
    
}
	
void MugenStage::load() throw( MugenException ){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    baseDir = Util::getDataPath() + "mugen/stages/" + location + "/";
    Global::debug(1) << baseDir << endl;
    const std::string ourDefFile = fixFileName( baseDir, std::string(location + ".def") );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate stage definition file for: " + location );
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
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
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
		if ( itemhead.find("startx")!=std::string::npos ){
		    *content->getNext() >> startx;
		} else if ( itemhead.find("starty")!=std::string::npos ){
		    *content->getNext() >> starty;
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
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
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
	else if( head == "scaling" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
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
	else if( head == "bound" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
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
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
		if ( itemhead.find("zoffset")!=std::string::npos ){
		    *content->getNext() >> zoffset;
		} else if ( itemhead.find("autoturn")!=std::string::npos ){
		    *content->getNext() >> autoturn;
		} else if ( itemhead.find("resetBG")!=std::string::npos ){
		    *content->getNext() >> resetBG;
		} else throw MugenException( "Unhandled option in StageInfo Section: " + itemhead );
	    }
	} 
	else if( head == "shadow" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
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
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
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
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
		if ( itemhead.find("spr")!=std::string::npos ){
		    *content->getNext() >> sffFile;
		} else if ( itemhead.find("debugbg")!=std::string::npos ){
		    *content->getNext() >> debugbg;
		} else throw MugenException( "Unhandled option in Reflection Section: " + itemhead );
	    }
	}
	else if( head == "music" ){ /* Ignore for now */ }
	// This our background data definitions
	else if( head.find("bg") !=std::string::npos ){
	    /*
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
		if ( itemhead.find("spr")!=std::string::npos ){
		    *content->getNext() >> sffFile;
		} else if ( itemhead.find("debugbg")!=std::string::npos ){
		    *content->getNext() >> debugbg;
		} else throw MugenException( "Unhandled option in Reflection Section: " + itemhead );
	    }
	    */
	}
	// Animation Info
	else if( head.find("Begin Action") !=std::string::npos ){
	}
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
	
    }
}
	

/* This creates the animations it differs from character animation since these are included in the stage.def file with the other defaults 
This will be called when Begin Action is found in the file */
void bundleAnimation( const MugenItemContent *content ) throw( MugenException){
    
}

	