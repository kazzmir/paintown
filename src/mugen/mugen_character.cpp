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
#include "util/file-system.h"

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

#include "parser/all.h"
#include "ast/all.h"

using namespace std;

namespace PaintownUtil = ::Util;

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

void MugenCharacter::load(){
    // Lets look for our def since some people think that all file systems are case insensitive
    baseDir = Filesystem::find("mugen/chars/" + location + "/");
    Global::debug(1) << baseDir << endl;
    std::string realstr = Mugen::Util::stripDir(location);
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, std::string(realstr + ".def") );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate player definition file for: " + location );
     
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    /* Extract info for our first section of our stage */
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	Mugen::Util::fixCase(head);

        if (head == "info"){
            class InfoWalker: public Ast::Walker {
            public:
                InfoWalker(MugenCharacter & who):
                self(who){
                }

                MugenCharacter & self;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "name"){
                        simple >> self.name;
                    } else if (simple == "displayname"){
                        simple >> self.displayName;
                    } else if (simple == "versiondate"){
                        simple >> self.versionDate;
                    } else if (simple == "mugenversion"){
                        simple >> self.mugenVersion;
                    } else if (simple == "author"){
                        simple >> self.author;
                    } else if (simple == "pal.defaults"){
                        vector<int> numbers;
                        simple >> numbers;
                        for (vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            self.palDefaults.push_back((*it) - 1);
                        }
                        // Global::debug(1) << "Pal" << self.palDefaults.size() << ": " << num << endl;
                    } else throw MugenException("Unhandled option in Info Section: " + simple.toString());
                }
            };

            InfoWalker walker(*this);
            Ast::Section * section = *section_it;
            section->walk(walker);
        } else if (head == "files"){
            class FilesWalker: public Ast::Walker {
            public:
                FilesWalker(MugenCharacter & self):
                self(self){
                }

                MugenCharacter & self;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "cmd"){
                        simple >> self.cmdFile;
                    } else if (simple == "cns"){
                        simple >> self.constantsFile;
                    } else if (PaintownUtil::matchRegex(simple.idString(), "st[0-9]+")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "st([0-9]+)", 0).c_str());
                        if (num >= 0 && num <= 12){
                            simple >> self.stFile[num];
                        }
                    } else if (simple == "stcommon"){
                        simple >> self.commonStateFile;
                    } else if (simple == "st"){
                        simple >> self.stateFile;
                    } else if (simple == "sprite"){
                        simple >> self.sffFile;
                    } else if (simple == "anim"){
                        simple >> self.airFile;
                    } else if (simple == "sound"){
                        simple >> self.sndFile;
                    } else if (PaintownUtil::matchRegex(simple.idString(), "pal[0-9]+")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "pal([0-9]+)", 0).c_str());
                        string what;
                        simple >> what;
                        self.palFile[num] = what;
                    } else {
                        throw MugenException("Unhandled option in Files Section: " + simple.toString());
                    }
                }
            };

            FilesWalker walker(*this);
            Ast::Section * section = *section_it;
            section->walk(walker);
	} else if (head == "arcade"){
            class ArcadeWalker: public Ast::Walker {
            public:
                ArcadeWalker(MugenCharacter & self):
                self(self){
                }

                MugenCharacter & self;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "intro.storyboard"){
                        simple >> self.introFile;
                    } else if (simple == "ending.storyboard"){
                        simple >> self.endingFile;
                    } else {
                        throw MugenException("Unhandled option in Arcade Section: " + simple.toString());
                    }
                }
            };
            
            ArcadeWalker walker(*this);
            Ast::Section * section = *section_it;
            section->walk(walker);
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

/* parse animations.
 * badly named and doesn't return anything.. maybe return an std::map ?
 */
void MugenCharacter::bundleAnimations(){
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Air::main(Mugen::Util::fixFileName(baseDir, airFile)));
    Global::debug(1, __FILE__) << "Parsing animations. Number of sections is " << parsed.getSections()->size() << endl;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
        std::string head = section->getName();
        Global::debug(1, __FILE__) << "Animation section '" << head << "'" << endl;
	Mugen::Util::fixCase(head);
        int number;
        if (PaintownUtil::matchRegex(head, "begin action [0-9]+")){
            number = atoi(PaintownUtil::captureRegex(head, "begin action ([0-9]+)", 0).c_str());
            Global::debug(1, __FILE__) << "Parse animation " << number << endl;
            animations[number] = Mugen::Util::getAnimation(section, sprites);
        }
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
int MugenCharacter::getWidth() const{
    return groundfront;
}
int MugenCharacter::getHeight() const{
    return height;
}
Message MugenCharacter::getCreateMessage(){
    return Message();
}
void MugenCharacter::getAttackCoords(int&, int&){
}
double MugenCharacter::minZDistance() const{
    return 0;
}
void MugenCharacter::attacked(World*, Object*, std::vector<Object*, std::allocator<Object*> >&){
}




