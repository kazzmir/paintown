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

// To aid in filesize
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
#include "mugen_util.h"
#include "globals.h"

using namespace std;

MugenCharacter::MugenCharacter( const string & s ):
ObjectAttack(0){
    this->location = s;
}

MugenCharacter::MugenCharacter( const char * location ):
ObjectAttack(0){
    this->location = std::string(location);
}

MugenCharacter::MugenCharacter( const string & s, int alliance ):
ObjectAttack(alliance){
    this->location = s;
}

MugenCharacter::MugenCharacter( const string & s, const int x, const int y, int alliance ):
ObjectAttack(x,y,alliance){
    this->location = s;
}

MugenCharacter::MugenCharacter( const MugenCharacter & copy ):
ObjectAttack(copy){
}

MugenCharacter::~MugenCharacter(){
     // Get rid of sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
     // Get rid of bitmaps
    for( std::map< unsigned int, std::map< unsigned int, Bitmap * > >::iterator i = bitmaps.begin() ; i != bitmaps.end() ; ++i ){
      for( std::map< unsigned int, Bitmap * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    
    // Get rid of sounds
    for( std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
      for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
}

void MugenCharacter::load() throw( MugenException ){
    // Lets look for our def since some assholes think that all file systems are case insensitive
    baseDir = Util::getDataPath() + "mugen/chars/" + location + "/";
    Global::debug(1) << baseDir << endl;
    std::string realstr = Mugen::Util::stripDir( location );
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, std::string(realstr + ".def") );
    
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
		Mugen::Util::removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		Mugen::Util::fixCase( itemhead );
		if ( itemhead.find("name")!=std::string::npos ){
		    *content->getNext() >> name;
                    Global::debug(1) << "Read name '" << name << "'" << endl;
		} else if ( itemhead.find("displayname")!=std::string::npos ){
		    *content->getNext() >> displayName;
		} else if ( itemhead.find("versiondate")!=std::string::npos ){
		    *content->getNext() >> versionDate;
		} else if ( itemhead.find("mugenversion")!=std::string::npos ){
		    *content->getNext() >> mugenVersion;
		} else if ( itemhead.find("author")!=std::string::npos ){
		    *content->getNext() >> author;
		} else if ( itemhead.find("pal.defaults")!=std::string::npos ){
		    while (content->hasItems()){
			int num;
			*content->getNext() >> num;
			palDefaults.push_back(num-1);
			Global::debug(1) << "Pal" << palDefaults.size() << ": " << num << endl;
		    }
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "Files" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Mugen::Util::removeSpaces( itemhead);
		if( itemhead.find("cmd")!=std::string::npos ){
		    *content->getNext() >> cmdFile;
		}
		else if (itemhead.find("cns")!=std::string::npos ){
		    *content->getNext() >> constantsFile;
		}
		else if ( itemhead.find("st")!=std::string::npos ){
		    Mugen::Util::removeSpaces(itemhead);
		    if (itemhead.find("st0") !=std::string::npos )*content->getNext() >> stFile[0];
		    else if (itemhead.find("st1") !=std::string::npos )*content->getNext() >> stFile[1];
		    else if (itemhead.find("st2")!=std::string::npos )*content->getNext() >> stFile[2];
		    else if (itemhead.find("st3")!=std::string::npos )*content->getNext() >> stFile[3];
		    else if (itemhead.find("st4")!=std::string::npos )*content->getNext() >> stFile[4];
		    else if (itemhead.find("st5")!=std::string::npos )*content->getNext() >> stFile[5];
		    else if (itemhead.find("st6")!=std::string::npos )*content->getNext() >> stFile[6];
		    else if (itemhead.find("st7")!=std::string::npos )*content->getNext() >> stFile[7];
		    else if (itemhead.find("st8")!=std::string::npos )*content->getNext() >> stFile[8];
		    else if (itemhead.find("st9")!=std::string::npos )*content->getNext() >> stFile[9];
		    else if (itemhead.find("st10")!=std::string::npos )*content->getNext() >> stFile[10];
		    else if (itemhead.find("st11")!=std::string::npos )*content->getNext() >> stFile[11];
		    else if (itemhead.find("st12")!=std::string::npos )*content->getNext() >> stFile[12];
		    else if( itemhead.find("stcommon")!=std::string::npos )*content->getNext() >> commonStateFile;
		    else if( itemhead.find("st")!=std::string::npos )*content->getNext() >> stateFile;
		    else throw MugenException( "Unhandled state file in Files Section: " + itemhead );
		}
		else if (itemhead.find("sprite") != std::string::npos ){
		    *content->getNext() >> sffFile;
		}
		else if ( itemhead.find("anim") != std::string::npos ){
		    *content->getNext() >> airFile;
		}
		else if ( itemhead.find("sound") != std::string::npos ){
		    *content->getNext() >> sndFile;
		}
		else if( itemhead.find("pal") != std::string::npos ){
		    if (itemhead == "pal1"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal2"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal3"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal4"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal5"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal6"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal7"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal8"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal9"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal10"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal11"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else if (itemhead == "pal12"){
			std::string temp;
			*content->getNext() >> temp;
			palFile.push_back(temp);
		    }
		    else throw MugenException( "Unhandled Palette File in Files Section: " + itemhead );
		}
		else throw MugenException( "Unhandled option in Files Section: " + itemhead );
	    }
	}
	else if( head == "Arcade" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Mugen::Util::removeSpaces( itemhead);
		if( itemhead.find("intro.storyboard") != std::string::npos ){
		    *content->getNext() >> introFile;
		}
		else if (itemhead.find("ending.storyboard") != std::string::npos ){
		    *content->getNext() >> endingFile;
		}
		else throw MugenException( "Unhandled option in Arcade Section: " + itemhead );
	    }    
	}
    }
    // Current palette
    if (palDefaults.empty()){
	// Correct the palette defaults
	for (unsigned int i = 0; i < palFile.size(); ++i){
	    palDefaults.push_back(i);
	}
    }
    if (palDefaults.size() < palFile.size()){
	bool setPals[palFile.size()];
	for( unsigned int i =0;i<palFile.size();++i){
	    setPals[i] = false;
	}
	// Set the ones already set
	for (unsigned int i = 0; i < palDefaults.size(); ++i){
	    setPals[palDefaults[i]] = true;
	}
	// now add the others
	for( unsigned int i =0;i<palFile.size();++i){
	    if(!setPals[i]){
		palDefaults.push_back(i);
	    }
	}
    }
    currentPalette = 0;
    Global::debug(1) << "Current pal: " << currentPalette << " | Palette File: " << palFile[palDefaults[currentPalette]] << endl;
    Global::debug(1) << "Reading Sff (sprite) Data..." << endl; 
    /* Sprites */
    Mugen::Util::readSprites( Mugen::Util::fixFileName(baseDir, sffFile), Mugen::Util::fixFileName(baseDir, palFile[palDefaults[currentPalette]]), sprites );
    Global::debug(1) << "Reading Air (animation) Data..." << endl;
    /* Animations */
    bundleAnimations();
    Global::debug(1) << "Reading Snd (sound) Data..." << endl; 
    /* Sounds */
    Mugen::Util::readSounds( Mugen::Util::fixFileName( baseDir, sndFile ), sounds );
}

// Render sprite
void MugenCharacter::renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Bitmap *bmp , 
				   const int flip, const double scalex, const double scaley ){
    MugenSprite *sprite = sprites[group][image];
    if (sprite){
	Bitmap *bitmap = sprite->getBitmap();//bitmaps[group][image];
	/*if (!bitmap){
	    bitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength));
	    bitmaps[group][image] = bitmap;
	}*/
	const int width = (int)(bitmap->getWidth() * scalex);
	const int height =(int)(bitmap->getHeight() * scaley);
	if (flip == 1){
	    bitmap->drawStretched(x,y, width, height, *bmp);
	} else if (flip == -1){
	    // temp bitmap to flip and crap
	    Bitmap temp = Bitmap::temporaryBitmap(bitmap->getWidth(), bitmap->getHeight());
	    temp.fill(Bitmap::MaskColor);
	    bitmap->drawHFlip(0,0,temp);
	    temp.drawStretched(x-width,y, width, height, *bmp);
	}
    }
}

void MugenCharacter::nextPalette(){
    if (currentPalette < palDefaults.size()-1){
	currentPalette++;
    } else currentPalette = 0;
    Global::debug(1) << "Current pal: " << currentPalette << " | Location: " << palDefaults[currentPalette] << " | Palette File: " << palFile[palDefaults[currentPalette]] << endl;
   /*
    // Now replace the palettes
    unsigned char pal[768];
    if (Mugen::Util::readPalette(Mugen::Util::fixFileName(baseDir, palFile[palDefaults[currentPalette]]),pal)){
	for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
	    for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
		if( j->second ){
		    MugenSprite *sprite = j->second;
		    if ( sprite->samePalette){
			memcpy( sprite->pcx + (sprite->reallength), pal, 768);
		    } else {
			if (!(sprite->groupNumber == 9000 && sprite->imageNumber == 1)){
			    memcpy( sprite->pcx + (sprite->reallength)-768, pal, 768);
			} 
		    }
		}
	    }
	}
	// reload with new palette
	for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	    if( i->second )i->second->reloadBitmaps();
	}
    }
    */
}

void MugenCharacter::priorPalette(){
    if (currentPalette > 0){
	currentPalette--;
    } else currentPalette = palDefaults.size() -1;
    Global::debug(1) << "Current pal: " << currentPalette << " | Palette File: " << palFile[palDefaults[currentPalette]] << endl;
    // Now replace the palettes
    /*unsigned char pal[768];
    if (Mugen::Util::readPalette(Mugen::Util::fixFileName(baseDir, palFile[palDefaults[currentPalette]]),pal)){
	for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
	    for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
		if( j->second ){
		    MugenSprite *sprite = j->second;
		    if ( sprite->samePalette){
			memcpy( sprite->pcx + (sprite->reallength), pal, 768);
		    } else {
			if (!(sprite->groupNumber == 9000 && sprite->imageNumber == 1)){
			    memcpy( sprite->pcx + (sprite->reallength)-768, pal, 768);
			} 
		    }
		}
	    }
	}
	// Get rid of animation lists;
	for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	    if( i->second )i->second->reloadBitmaps();
	}
    }*/
}

// animations
void MugenCharacter::bundleAnimations() throw( MugenException){
    MugenReader reader( Mugen::Util::fixFileName(baseDir, airFile) );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    if( collection.empty() ) throw MugenException( "Problem loading Animations from file: " + airFile );
    
    /* Extract info for our first section of our character */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	head.replace(0,13,"");
	int h;
	MugenItem(head) >> h;
	animations[h] = Mugen::Util::getAnimation(collection[i], sprites);
	Global::debug(1) << "Added Animation 'Begin Action " << h << "' : '" << animations[h]->getName(animations[h]->getType()) << "'" << endl;
    }
}

/* Inherited members */
void MugenCharacter::act(std::vector<Object*, std::allocator<Object*> >*, World*, std::vector<Object*, std::allocator<Object*> >*){
}
void MugenCharacter::draw(Bitmap*, int){
}                      
void MugenCharacter::grabbed(Object*){
}
void MugenCharacter::unGrab(){
}
bool MugenCharacter::isGrabbed(){
    return false;
}
Object* MugenCharacter::copy(){
    return this;
}
const std::string& MugenCharacter::getAttackName(){
    return getName();
}
bool MugenCharacter::collision(ObjectAttack*){
    return false;
}
int MugenCharacter::getDamage() const{
    return 0;
}
bool MugenCharacter::isCollidable(Object*){
    return true;
}
bool MugenCharacter::isGettable(){
    return false;
}
bool MugenCharacter::isGrabbable(Object*){
    return true;
}
bool MugenCharacter::isAttacking(){
    return false;
}
const int MugenCharacter::getWidth() const{
    return groundfront;
}
const int MugenCharacter::getHeight() const{
    return height;
}
Message MugenCharacter::getCreateMessage(){
    return Message();
}
void MugenCharacter::getAttackCoords(int&, int&){
}
const double MugenCharacter::minZDistance() const{
    return 0;
}
void MugenCharacter::attacked(World*, Object*, std::vector<Object*, std::allocator<Object*> >&){
}




