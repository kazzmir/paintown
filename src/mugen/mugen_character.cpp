#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>

#include "util/funcs.h"

#include "mugen_animation.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_character.h"
#include "mugen_sff_reader.h"
#include "mugen_snd_reader.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "globals.h"

static int lowerCase( int c ){ return tolower( c );}

static void fixCase( std::string &str ){
    transform( str.begin(), str.end(), str.begin(), lowerCase );
}

static std::string fixFileName( const std::string &dir, std::string str ){
    Global::debug(1) << "Current File: " << str << endl;
    // Temp fix until the lexer fixes this crap
    if( str.find(' ') != std::string::npos ){
	Global::debug(1) << "Removing spaces from: " << str << endl;
	for( int i = str.size()-1; i>-1; --i){
	    if( str[i] == ' ' )str.erase( str.begin()+i );
	    else break;
	}
    }
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

MugenCharacter::MugenCharacter( const string & s ){
    this->location = s;
}

MugenCharacter::MugenCharacter( const char * location ){
    this->location = std::string(location);
}

MugenCharacter::~MugenCharacter(){
     // Get rid of sprites
    for( std::map< int, std::map< int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    
}

namespace Sff{
    
static MugenSprite *getCorrectIndex( MugenSprite *spr[], int index ){
    if( !spr[index]->length ){
	    return getCorrectIndex( spr, spr[index]->prev );
    }
    else return spr[index];
}

// Get next sprite
static MugenSprite * readSprite(ifstream & ifile, int & location){
    // if ( !location )return;
    // Go to next sprite
    ifile.seekg(location, ios::beg);
    // next sprite
    MugenSprite *temp = new MugenSprite();
    
    ifile.read((char *)&temp->next, 4);
    ifile.read((char *)&temp->length, 4);
    ifile.read((char *)&temp->x, 2);
    ifile.read((char *)&temp->y, 2);
    ifile.read((char *)&temp->groupNumber, 2);
    ifile.read((char *)&temp->imageNumber, 2);
    ifile.read((char *)&temp->prev, 2);
    ifile.read((char *)&temp->samePalette, 1);
    ifile.read((char *)&temp->comments, 13);
    if( temp->length ){
	// Lets get the real length, in case we've been duped
	//temp->length = temp->next - location - 32;
	temp->pcx = new char[temp->length];
	ifile.read((char *)temp->pcx, temp->length);
    }
    // Set the next file location
    location = temp->next;
    
    return temp;
}

static const map<int,map<int, MugenSprite *> > readSprites(const string & filename) throw (MugenException){
    /* 16 skips the header stuff */
    int location = 16;
    ifstream ifile;
    ifile.open(filename.c_str(), ios::binary);
    if (!ifile){
	throw MugenException("Could not open SFF file: " + filename);
    }

    /* Lets go ahead and skip the crap -> (Elecbyte signature and version)
     * start at the 16th byte
     */
    ifile.seekg(location,ios::beg);
    int totalGroups;
    int totalImages;
    
    /* this probably isn't endian safe.. */
    ifile.read((char *)&totalGroups, 4);
    ifile.read((char *)&totalImages, 4);
    ifile.read((char *)&location, 4);
    
    Global::debug(1) << "Got Total Groups: " << totalGroups << ", Total Images: " << totalImages << ", Next Location in file: " << location << endl;

    map<int, map<int, MugenSprite*> > sprites;
    
    MugenSprite *spriteIndex[totalImages];
    
    for (int i = 0; i < totalImages; ++i){
        MugenSprite * sprite = readSprite(ifile, location);
	/* Lets check if this is a duplicate sprite if so copy it
	* if prev is larger than index then this file is corrupt */
	if( sprite->prev > i && !sprite->length ) throw MugenException("Error in SFF file: " + filename + ". Incorrect reference to sprite.");
	else if( !sprite->length ){
	    const MugenSprite *temp = getCorrectIndex( spriteIndex, sprite->prev );//spriteIndex[sprite->prev];
	    sprite->pcx = new char[temp->length];
	    strncpy( sprite->pcx, temp->pcx, temp->length );
	}
	spriteIndex[i] = sprite;
        sprites[sprite->groupNumber][sprite->imageNumber] = sprite;
	
	Global::debug(1) << "Next Sprite: " << sprite->next << ", Length: " << sprite->length << ", x|y: " << sprite->x << "|" << sprite->y << ", Group|Image Number: " << sprite->groupNumber << "|" << sprite->imageNumber << ", Prev: " << sprite->prev << ", Same Pal: " << sprite->samePalette << ", Comments: " << sprite->comments << endl;
    }

    ifile.close();
    
    return sprites;
}

}

void MugenCharacter::load() throw( MugenException ){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    baseDir = Util::getDataPath() + "mugen/chars/" + location + "/";
    Global::debug(1) << baseDir << endl;
    const std::string ourDefFile = fixFileName( baseDir, std::string(location + ".def") );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate player definition file for: " + location );
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	const std::string &head = collection[i]->getHeader();
	if( head == "Info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		fixCase( itemhead );
		if (itemhead == "name"){
		    *content->getNext() >> name;
                    Global::debug(1) << "Read name '" << name << "'" << endl;
		} else if (itemhead == "displayname" ){
		    *content->getNext() >> displayName;
		} else if (itemhead == "versiondate" ){
		    *content->getNext() >> versionDate;
		} else if (itemhead == "mugenversion" ){
		    *content->getNext() >> mugenVersion;
		} else if (itemhead == "author" ){
		    *content->getNext() >> author;
		} else if (itemhead == "pal.defaults" ){
		    // Add parse info
		    MugenItemContent *temp = parseOpt( content->getNext()->query() );
		    for( int pal = 0; pal < 12; ++pal ){
			if( temp->hasItems() )*temp->getNext() >> palDefaults[pal];
			else break;
			Global::debug(1) << "Pal" << pal << ": " << palDefaults[pal] << endl;
		    }
		    delete temp;
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "Files" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		fixCase( itemhead );
		if( itemhead == "cmd" ){
		    *content->getNext() >> cmdFile;
		}
		else if (itemhead == "cns" ){
		    *content->getNext() >> constantsFile;
		}
		else if (itemhead == "st" ){
		    *content->getNext() >> stateFile;
		}
		else if (itemhead == "stcommon" ){
		    *content->getNext() >> commonStateFile;
		}
		else if (itemhead == "sprite" ){
		    *content->getNext() >> sffFile;
		}
		else if (itemhead == "anim" ){
		    *content->getNext() >> airFile;
		}
		else if (itemhead == "sound" ){
		    *content->getNext() >> sndFile;
		}
		else if (itemhead == "pal1" ){
		    *content->getNext() >> palFile[0];
		}
		else if (itemhead == "pal2" ){
		    *content->getNext() >> palFile[1];
		}
		else if (itemhead == "pal3" ){
		    *content->getNext() >> palFile[2];
		}
		else if (itemhead == "pal4" ){
		    *content->getNext() >> palFile[3];
		}
		else if (itemhead == "pal5" ){
		    *content->getNext() >> palFile[4];
		}
		else if (itemhead == "pal6" ){
		    *content->getNext() >> palFile[5];
		}
		else if (itemhead == "pal7" ){
		    *content->getNext() >> palFile[6];
		}
		else if (itemhead == "pal8" ){
		    *content->getNext() >> palFile[7];
		}
		else if (itemhead == "pal9" ){
		    *content->getNext() >> palFile[8];
		}
		else if (itemhead == "pal10" ){
		    *content->getNext() >> palFile[9];
		}
		else if (itemhead == "pal11" ){
		    *content->getNext() >> palFile[10];
		}
		else if (itemhead == "pal12" ){
		    *content->getNext() >> palFile[11];
		}
		else if (itemhead == "st1" ){
		    *content->getNext() >> stFile[0];
		}
		else if (itemhead == "st2" ){
		    *content->getNext() >> stFile[1];
		}
		else if (itemhead == "st3" ){
		    *content->getNext() >> stFile[2];
		}
		else if (itemhead == "st4" ){
		    *content->getNext() >> stFile[3];
		}
		else if (itemhead == "st5" ){
		    *content->getNext() >> stFile[4];
		}
		else if (itemhead == "st6" ){
		    *content->getNext() >> stFile[5];
		}
		else if (itemhead == "st7" ){
		    *content->getNext() >> stFile[6];
		}
		else if (itemhead == "st8" ){
		    *content->getNext() >> stFile[7];
		}
		else if (itemhead == "st9" ){
		    *content->getNext() >> stFile[8];
		}
		else if (itemhead == "st10" ){
		    *content->getNext() >> stFile[9];
		}
		else if (itemhead == "st11" ){
		    *content->getNext() >> stFile[10];
		}
		else if (itemhead == "st12" ){
		    *content->getNext() >> stFile[11];
		}
		else throw MugenException( "Unhandled option in Files Section: " + itemhead );
	    }
	}
	else if( head == "Arcade" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		fixCase( itemhead );
		if( itemhead == "intro.storyboard" ){
		    *content->getNext() >> introFile;
		}
		else if (itemhead == "ending.storyboard" ){
		    *content->getNext() >> endingFile;
		}
		else throw MugenException( "Unhandled option in Arcade Section: " + itemhead );
	    }    
	}
    }
    
    /* Sprites */
    // MugenSffReader spriteReader( fixFileName( baseDir, sffFile ) );
    // sprites = spriteReader.getCollection();
    sprites = Sff::readSprites(fixFileName(baseDir, sffFile));
    
    /* Animations */
    bundleAnimations();
    
    /* Sounds */
    MugenSndReader soundReader( fixFileName( baseDir, sndFile ) );
    sounds = soundReader.getCollection();
}

// animations
void MugenCharacter::bundleAnimations() throw( MugenException){
    MugenReader reader( Util::trim(baseDir + airFile) );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	MugenAnimation *animation = new MugenAnimation();
	std::string head = collection[i]->getHeader();
	head.replace(0,13,"");
	std::vector<MugenArea> clsn1Holder;
	std::vector<MugenArea> clsn2Holder;
	bool clsn1Reset = false;
	bool clsn2Reset = false;
	bool setloop = false;
	while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		fixCase( itemhead );
		// Attack boxes
		if( itemhead.find("clsn1default:") != std::string::npos ){
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn1Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << content->getNext() << " start" << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenItemContent *temp = parseOpt( content->getNext()->query() );
			MugenArea area;
			*temp->getNext() >> area.x1;
			*temp->getNext() >> area.y1;
			*temp->getNext() >> area.x2;
			*temp->getNext() >> area.y2;
			delete temp;
			clsn1Holder.push_back(area);
		    }
		}
		// defend boxes
		else if( itemhead.find("clsn2default:") != std::string::npos ){
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn2Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << content->getNext() << " start" << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenItemContent *temp = parseOpt( content->getNext()->query() );
			MugenArea area;
			*temp->getNext() >> area.x1;
			*temp->getNext() >> area.y1;
			*temp->getNext() >> area.x2;
			*temp->getNext() >> area.y2;
			delete temp;
			clsn2Holder.push_back(area);
		    }
		}
		// defend boxes
		else if( itemhead.find("clsn2:") != std::string::npos ){
		    clsn2Reset = true;
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn2Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << content->getNext() << " start" << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenItemContent *temp = parseOpt( content->getNext()->query() );
			MugenArea area;
			*temp->getNext() >> area.x1;
			*temp->getNext() >> area.y1;
			*temp->getNext() >> area.x2;
			*temp->getNext() >> area.y2;
			delete temp;
			clsn2Holder.push_back(area);
		    }
		}
		// Attack boxes
		else if( itemhead.find("clsn1:") != std::string::npos ){
		    clsn1Reset = true;
		    // Get number
		    int num;
		    *content->getNext() >> num;
		    clsn1Holder.clear();
		    for( int n = 0; n < num; ++n ){
			content = collection[i]->getNext();
			Global::debug(1) << content->getNext() << " start" << endl;
			// Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
			MugenItemContent *temp = parseOpt( content->getNext()->query() );
			MugenArea area;
			*temp->getNext() >> area.x1;
			*temp->getNext() >> area.y1;
			*temp->getNext() >> area.x2;
			*temp->getNext() >> area.y2;
			delete temp;
			clsn1Holder.push_back(area);
		    }
		}
		// defend boxes for this frame
		else if( itemhead.find("loopstart") != std::string::npos ){
		    setloop = true;
		}
		// This is where we get our frame
		else{
		    // This is the new frame
		    MugenFrame *frame = new MugenFrame();
		    frame->defenseCollision = clsn2Holder;
		    frame->attackCollision = clsn1Holder;
		    frame->loopstart = setloop;
		    /* Get sprite details */
		    int group, spriteNumber;
		    // Need to get the parsed data and populate these above items
		    *content->getNext() >> group;
		    *content->getNext() >> spriteNumber;
		    *content->getNext() >> frame->xoffset;
		    *content->getNext() >> frame->yoffset;
		    *content->getNext() >> frame->time;
		    // Check for flips
		    if( content->hasItems() ){
			std::string flip;
			*content->getNext() >> flip;
			fixCase( flip );
			if( flip.find("h") != std::string::npos )frame->flipHorizontal = true;
			if( flip.find("v") != std::string::npos )frame->flipVertical = true;
		    }
		    if( content->hasItems() )*content->getNext() >> frame->colorAdd;
		    // Add sprite
		    frame->sprite = sprites[group][spriteNumber];
                    if (frame->sprite == 0){
                        Global::debug(0) << "No sprite for group " << group << " number " << spriteNumber << endl;
                    }
		    // Add frame
		    animation->addFrame(frame);
		}
		// Clear if it was not a default set, since it's valid for only one frame
		if( clsn1Reset ){
		    clsn1Holder.clear();
		    clsn1Reset = false;
		}
		if( clsn2Reset ){
		    clsn2Holder.clear();
		    clsn2Reset = false;
		}
		if( setloop )setloop = false;
	}
	int h;
	MugenItem(head) >> h;
	Global::debug(1) << "Adding Animation 'Begin Action " << h << "'" << endl;
	animations[h] = animation;
    }
}


