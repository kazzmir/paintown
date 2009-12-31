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
#include "character.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "globals.h"

#include "parser/all.h"
#include "ast/all.h"

using namespace std;

namespace Mugen{

namespace PaintownUtil = ::Util;

Character::Character( const string & s ):
ObjectAttack(0){
    this->location = s;
}

Character::Character( const char * location ):
ObjectAttack(0){
    this->location = std::string(location);
}

Character::Character( const string & s, int alliance ):
ObjectAttack(alliance){
    this->location = s;
}

Character::Character( const string & s, const int x, const int y, int alliance ):
ObjectAttack(x,y,alliance){
    this->location = s;
}

Character::Character( const Character & copy ):
ObjectAttack(copy){
}

Character::~Character(){
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

void Character::loadCmdFile(const string & path){
    string full = Filesystem::find("mugen/chars/" + location + "/" + PaintownUtil::trim(path));
    try{
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
        for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            Util::fixCase(head);

            if (head == "command"){
                class CommandWalker: public Ast::Walker {
                    public:
                        CommandWalker(Character & self):
                            self(self){
                            }

                        Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "name"){
                        } else if (simple == "command"){
                        } else if (simple == "time"){
                        }
                    }
                };

                CommandWalker walker(*this);
                section->walk(walker);
            }
        }
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    }
}

static bool isStateDefSection(string name){
    Util::fixCase(name);
    return PaintownUtil::matchRegex(name, "state ") ||
           PaintownUtil::matchRegex(name, "statedef ");
}
    
void Character::setConstant(std::string name, const vector<double> & values){
    constants[name] = Constant(values);
}

void Character::setConstant(std::string name, double value){
    constants[name] = Constant(value);
}

void Character::loadCnsFile(const string & path){
    string full = Filesystem::find("mugen/chars/" + location + "/" + PaintownUtil::trim(path));
    try{
        /* cns can use the Cmd parser */
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
        for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            Util::fixCase(head);
            if (!isStateDefSection(head)){
                class AttributeWalker: public Ast::Walker {
                public:
                    AttributeWalker(Character & who):
                    self(who){
                    }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        string name = simple.idString();
                        if (simple.getValue() != 0 && simple.getValue()->hasMultiple()){
                            vector<double> values;
                            simple >> values;
                            self.setConstant(name, values);
                        } else {
                            double value;
                            simple >> value;
                            self.setConstant(name, value);
                        }
                    }
                };

                AttributeWalker walker(*this);
                section->walk(walker);
            }
        }
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    } catch (const Ast::Exception & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    }
}

void Character::loadStateFile(const string & path){
    string full = Filesystem::find("mugen/chars/" + location + "/" + PaintownUtil::trim(path));
    try{
        /* st can use the Cmd parser */
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    }
}

void Character::load(){
    // Lets look for our def since some people think that all file systems are case insensitive
    baseDir = Filesystem::find("mugen/chars/" + location + "/");
    Global::debug(1) << baseDir << endl;
    std::string realstr = Mugen::Util::stripDir(location);
    const std::string ourDefFile = Mugen::Util::fixFileName(baseDir, std::string(realstr + ".def"));
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate player definition file for: " + location );
    }
     
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
                InfoWalker(Character & who):
                self(who){
                }

                Character & self;

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
                FilesWalker(Character & self):
                self(self){
                }

                Character & self;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "cmd"){
                        simple >> self.cmdFile;
                        self.loadCmdFile(self.cmdFile);
                    } else if (simple == "cns"){
                        simple >> self.constantsFile;
                        self.loadCnsFile(self.constantsFile);
                    } else if (PaintownUtil::matchRegex(simple.idString(), "st[0-9]+")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "st([0-9]+)", 0).c_str());
                        if (num >= 0 && num <= 12){
                            simple >> self.stFile[num];
                        }
                    } else if (simple == "stcommon"){
                        simple >> self.commonStateFile;
                        /* TODO: load from the common directory */
                        // self.loadStateFile(self.commonStateFile);
                    } else if (simple == "st"){
                        simple >> self.stateFile;
                        self.loadStateFile(self.stateFile);
                    } else if (simple == "sprite"){
                        simple >> self.sffFile;
                    } else if (simple == "anim"){
                        simple >> self.airFile;
                    } else if (simple == "sound"){
                        simple >> self.sndFile;
                        Mugen::Util::readSounds( Mugen::Util::fixFileName(self.baseDir, self.sndFile ), self.sounds );
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
                ArcadeWalker(Character & self):
                self(self){
                }

                Character & self;

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
}

// Render sprite
void Character::renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Bitmap *bmp , 
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

void Character::nextPalette(){
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

void Character::priorPalette(){
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
void Character::bundleAnimations(){
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
void Character::act(std::vector<Object*, std::allocator<Object*> >*, World*, std::vector<Object*, std::allocator<Object*> >*){
}
void Character::draw(Bitmap*, int){
}                      
void Character::grabbed(Object*){
}
void Character::unGrab(){
}
bool Character::isGrabbed(){
    return false;
}
Object* Character::copy(){
    return this;
}
const std::string& Character::getAttackName(){
    return getName();
}
bool Character::collision(ObjectAttack*){
    return false;
}
int Character::getDamage() const{
    return 0;
}
bool Character::isCollidable(Object*){
    return true;
}
bool Character::isGettable(){
    return false;
}
bool Character::isGrabbable(Object*){
    return true;
}
bool Character::isAttacking(){
    return false;
}
int Character::getWidth() const{
    return groundfront;
}
int Character::getHeight() const{
    return height;
}

Network::Message Character::getCreateMessage(){
    return Network::Message();
}

void Character::getAttackCoords(int&, int&){
}
double Character::minZDistance() const{
    return 0;
}
void Character::attacked(World*, Object*, std::vector<Object*, std::allocator<Object*> >&){
}

}
