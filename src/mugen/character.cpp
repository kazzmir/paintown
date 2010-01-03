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
#include "state.h"

#include "input/input-map.h"
#include "input/input-manager.h"

#include "parser/all.h"
#include "ast/all.h"

using namespace std;

namespace Mugen{

namespace PaintownUtil = ::Util;
    
Command::Command(std::string name, Ast::KeyList * keys, int maxTime, int bufferTime):
name(name),
keys(keys),
maxTime(maxTime),
bufferTime(bufferTime),
ticks(0),
holdKey(-1),
current(keys->getKeys().begin()),
holder(0),
successTime(0),
needRelease(0){
}

void Command::handle(InputMap<Keys>::Output keys){
    class KeyWalker: public Ast::Walker{
    public:
        KeyWalker(InputMap<Keys>::Output & keys, const InputMap<Keys>::Output & oldKeys, int & holdKey, const Ast::Key *& holder, const Ast::Key *& needRelease):
        ok(false),
        fail(false),
        holdKey(holdKey),
        keys(keys),
        oldKeys(keys),
        holder(holder),
        needRelease(needRelease){
        }
        
        bool ok;
        bool fail;
        int & holdKey;
        InputMap<Keys>::Output & keys;
        const InputMap<Keys>::Output & oldKeys;
        const Ast::Key *& holder;
        const Ast::Key *& needRelease;

        virtual void onKeySingle(const Ast::KeySingle & key){
            if (key == "a"){
                ok = keys[A];
            } else if (key == "b"){
                ok = keys[B];
            } else if (key == "c"){
                ok = keys[C];
            } else if (key == "x"){
                ok = keys[X];
            } else if (key == "y"){
                ok = keys[Y];
            } else if (key == "z"){
                ok = keys[Z];
            } else if (key == "B"){
                ok = keys[Back];
            } else if (key == "DB"){
                ok = keys[Back] && keys[Down];
            } else if (key == "D"){
                ok = keys[Down];
            } else if (key == "DF"){
                ok = keys[Forward] && keys[Down];
            } else if (key == "F"){
                ok = keys[Forward];
            } else if (key == "UF"){
                ok = keys[Forward] && keys[Up];
            } else if (key == "U"){
                ok = keys[Up];
            } else if (key == "UB"){
                ok = keys[Back] && keys[Up];
            }
            if (ok){
                needRelease = &key;
            }
        }

        bool sameKeys(const InputMap<Keys>::Output & map1, const InputMap<Keys>::Output & map2 ){
            return map1 == map2;
        }

        virtual void onKeyModifier(const Ast::KeyModifier & key){
            needRelease = NULL;
            switch (key.getModifierType()){
                case Ast::KeyModifier::MustBeHeldDown : {
                    key.getKey()->walk(*this);
                    if (ok){
                        holder = key.getKey();
                    }
                    break;
                }
                case Ast::KeyModifier::Release : {
                    if (key.getExtra() > 0){
                        if (holdKey > 0){
                            int fake = -1;
                            const Ast::Key * fakeKey;
                            KeyWalker walker(keys, oldKeys, fake, holder, fakeKey);
                            key.getKey()->walk(walker);
                            if (walker.ok){
                                holdKey -= 1;
                            } else {
                                fail = true;
                            }
                        } else if (holdKey == 0){
                            int fake = -1;
                            const Ast::Key * fakeKey;
                            KeyWalker walker(keys, oldKeys, fake, holder, fakeKey);
                            key.getKey()->walk(walker);
                            /* if ok is true then the key is still being pressed */
                            if (!walker.ok){
                                ok = true;
                            }
                        } else if (holdKey == -1){
                            holdKey = key.getExtra();
                        }
                    } else {
                        int fake = -1;
                        KeyWalker walker(keys, oldKeys, fake, holder, needRelease);
                        key.getKey()->walk(walker);
                        if (!walker.ok){
                            ok = true;
                        }
                    }
                    break;
                }
                case Ast::KeyModifier::Direction : {
                    key.getKey()->walk(*this);
                    break;
                }
                case Ast::KeyModifier::Only : {
                    key.getKey()->walk(*this);
                    if (!ok){
                        if (!sameKeys(keys, oldKeys)){
                            fail = true;
                        }
                    }
                    break;
                }
            }
        }

        virtual void onKeyCombined(const Ast::KeyCombined & key){
            int fake = -1;
            KeyWalker left(keys, oldKeys, fake, holder, needRelease);
            KeyWalker right(keys, oldKeys, fake, holder, needRelease);
            key.getKey1()->walk(left);
            key.getKey2()->walk(right);
            ok = left.ok && right.ok;
            if (ok){
                needRelease = &key;
            }
        }
    };
    
    if (successTime > 0){
        successTime -= 1;
        Global::debug(0) << "Pressed " << name << endl;
        return;
    }

    bool use = true;
    if (needRelease != NULL){
        const Ast::Key * fake;
        KeyWalker walker(keys, oldKeys, holdKey, holder, fake);
        needRelease->walk(walker);
        Global::debug(1) << "Waiting for key " << needRelease->toString() << " to be released: " << walker.ok << endl;
        if (walker.ok){
            use = false;
        } else {
            needRelease = NULL;
        }
    }

    bool fail = false;
    bool ok = false;

    if (use){
        KeyWalker walker(keys, oldKeys, holdKey, holder, needRelease);
        (*current)->walk(walker);

        ok = walker.ok;
        fail = walker.fail;
        if (holder != 0){
            holder->walk(walker);
            ok &= walker.ok;
            fail |= walker.fail;
        }
    }

    oldKeys = keys;
    ticks += 1;
    if (ticks > maxTime){
        fail = true;
        Global::debug(2) << name << " ran out of time" << endl;
    }

    if (fail){
        current = this->keys->getKeys().begin();
        ticks = 0;
        needRelease = NULL;
        holdKey = -1;
        holder = 0;
    } else if (ok){
        current++;
        holdKey = -1;
        if (current == this->keys->getKeys().end()){
            /* success! */
            current = this->keys->getKeys().begin();
            ticks = 0;
            needRelease = NULL;
            holder = 0;
            successTime = bufferTime - 1;
            Global::debug(0) << "Pressed " << name << endl;
        }
    }
}

Command::~Command(){
    delete keys;
}

Character::Character( const string & s ):
ObjectAttack(0){
    this->location = s;
    initialize();
}

Character::Character( const char * location ):
ObjectAttack(0){
    this->location = std::string(location);
    initialize();
}

Character::Character( const string & s, int alliance ):
ObjectAttack(alliance){
    this->location = s;
    initialize();
}

Character::Character( const string & s, const int x, const int y, int alliance ):
ObjectAttack(x,y,alliance){
    this->location = s;
    initialize();
}

Character::Character( const Character & copy ):
ObjectAttack(copy){
}

Character::~Character(){
     // Get rid of sprites
    for (std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
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

    for (vector<Command*>::iterator it = commands.begin(); it != commands.end(); it++){
        delete (*it);
    }
}

void Character::initialize(){
    currentState = Standing;

    input.set(Keyboard::Key_UP, 0, false, Command::Up);
    input.set(Keyboard::Key_DOWN, 0, false, Command::Down);
    input.set(Keyboard::Key_RIGHT, 0, false, Command::Forward);
    input.set(Keyboard::Key_LEFT, 0, false, Command::Back);

    input.set(Keyboard::Key_A, 0, false, Command::A);
    input.set(Keyboard::Key_S, 0, false, Command::B);
    input.set(Keyboard::Key_D, 0, false, Command::C);
    input.set(Keyboard::Key_Z, 0, false, Command::X);
    input.set(Keyboard::Key_X, 0, false, Command::Y);
    input.set(Keyboard::Key_C, 0, false, Command::Z);
}
    
void Character::addCommand(Command * command){
    commands.push_back(command);
    /* todo */
}

void Character::loadCmdFile(const string & path){
    string full = Filesystem::find("mugen/chars/" + location + "/" + PaintownUtil::trim(path));
    try{
        int defaultTime = 15;
        int defaultBufferTime = 1;

        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
        for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            Util::fixCase(head);

            if (head == "command"){
                class CommandWalker: public Ast::Walker {
                public:
                    CommandWalker(Character & self, const int defaultTime, const int defaultBufferTime):
                        self(self),
                        time(defaultTime),
                        bufferTime(defaultBufferTime),
                        key(0){
                        }

                    Character & self;
                    int time;
                    int bufferTime;
                    string name;
                    Ast::Key * key;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "name"){
                            simple >> name;
                        } else if (simple == "command"){
                            key = (Ast::Key*) simple.getValue()->copy();
                        } else if (simple == "time"){
                            simple >> time;
                        } else if (simple == "buffer.time"){
                            simple >> bufferTime;
                            /* Time that the command will be buffered for. If the command is done
                             * successfully, then it will be valid for this time. The simplest
                             * case is to set this to 1. That means that the command is valid
                             * only in the same tick it is performed. With a higher value, such
                             * as 3 or 4, you can get a "looser" feel to the command. The result
                             * is that combos can become easier to do because you can perform
                             * the command early.
                             */
                        }
                    }

                    virtual ~CommandWalker(){
                        if (name == ""){
                            throw MugenException("No name given for command");
                        }

                        if (key == 0){
                            throw MugenException("No key sequence given for command");
                        }

                        /* parser guarantees the key will be a KeyList */
                        self.addCommand(new Command(name, (Ast::KeyList*) key, time, bufferTime));
                    }
                };

                CommandWalker walker(*this, defaultTime, defaultBufferTime);
                section->walk(walker);
            } else if (head == "defaults"){
                class DefaultWalker: public Ast::Walker {
                public:
                    DefaultWalker(int & time, int & buffer):
                        time(time),
                        buffer(buffer){
                        }

                    int & time;
                    int & buffer;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "command.time"){
                            simple >> time;
                        } else if (simple == "command.buffer.time"){
                            simple >> buffer;
                        }
                    }
                };

                DefaultWalker walker(defaultTime, defaultBufferTime);
                section->walk(walker);
            }

            /* [Defaults]
             * ; Default value for the "time" parameter of a Command. Minimum 1.
             * command.time = 15
             *
             * ; Default value for the "buffer.time" parameter of a Command. Minimum 1,
             * ; maximum 30.
             * command.buffer.time = 1
             */
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

void Character::loadStateFile(const std::string & base, const string & path){
    string full = Filesystem::find(base + "/" + PaintownUtil::trim(path));
    try{
        /* st can use the Cmd parser */
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    }
}

/* a container for a directory and a file */
struct Location{
    Location(string base, string file):
        base(base), file(file){
        }

    string base;
    string file;
};

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
                FilesWalker(Character & self, const string & location):
                location(location),
                self(self){
                }

                vector<Location> stateFiles;
                const string & location;

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
                            string path;
                            simple >> path;
                            stateFiles.push_back(Location("mugen/chars/" + location, path));
                            // simple >> self.stFile[num];
                        }
                    } else if (simple == "stcommon"){
                        string path;
                        simple >> path;
                        stateFiles.insert(stateFiles.begin(), Location("mugen/data/", path));
                        /* TODO: load from the common directory */
                        // self.loadStateFile(self.commonStateFile);
                    } else if (simple == "st"){
                        string path;
                        simple >> path;
                        stateFiles.push_back(Location("mugen/chars/" + location, path));
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

            FilesWalker walker(*this, location);
            Ast::Section * section = *section_it;
            section->walk(walker);

            for (vector<Location>::iterator it = walker.stateFiles.begin(); it != walker.stateFiles.end(); it++){
                Location & where = *it;
                loadStateFile(where.base, where.file);
            }

            /*
            if (commonStateFile != ""){
                loadStateFile("mugen/data/", commonStateFile);
            }
            if (stateFile != ""){
                loadStateFile("mugen/chars/" + location, stateFile);
            }
            if (
            */

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
void Character::renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Bitmap *bmp , const int flip, const double scalex, const double scaley ){
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

MugenAnimation * Character::getCurrentAnimation() const {
    typedef std::map< int, MugenAnimation * > Animations;
    Animations::const_iterator it = getAnimations().find(currentState);
    if (it != getAnimations().end()){
        MugenAnimation * animation = (*it).second;
        return animation;
    }
    return NULL;
}

void Character::doInput(InputMap<Command::Keys>::Output output){
    for (vector<Command*>::iterator it = commands.begin(); it != commands.end(); it++){
        Command * command = *it;
        command->handle(output);
    }
}

/* Inherited members */
void Character::act(std::vector<Object*, std::allocator<Object*> >*, World*, std::vector<Object*, std::allocator<Object*> >*){
    MugenAnimation * animation = getCurrentAnimation();
    if (animation != 0){
        animation->logic();
    }

    doInput(InputManager::getMap(input));
}

void Character::draw(Bitmap * work, int x_position){
    MugenAnimation * animation = getCurrentAnimation();
    if (animation != 0){
        /* FIXME: change these numbers */
        animation->render(260, 230, *work, 0, 0);
    }
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
