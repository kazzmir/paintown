#include "util/bitmap.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <ostream>
#include <sstream>
#include <iostream>

// To aid in filesize
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "util/funcs.h"
#include "util/file-system.h"
#include "util/debug.h"
#include "util/timedifference.h"

#include "util.h"
#include "animation.h"
#include "item.h"
#include "item-content.h"
#include "section.h"
#include "menu.h"
#include "sound.h"
#include "reader.h"
#include "config.h"
#include "sprite.h"
#include "globals.h"
#include "util/init.h"
#include "state.h"
#include "parse-cache.h"

#include "ast/all.h"
#include "parser/all.h"

using namespace std;

namespace PaintownUtil = ::Util;

static int lowerCase( int c ){ return tolower( c );}

const std::string Mugen::Util::fixCase( const std::string &str ){
    std::string tempStr = str;
    transform( tempStr.begin(), tempStr.end(), tempStr.begin(), lowerCase );
    return tempStr;
}

const std::string Mugen::Util::removeSpaces( const std::string &str ){
    std::string tempStr = str;
    if (tempStr.find(' ') != std::string::npos){
	Global::debug(2) << "Removing spaces from: " << tempStr << endl;
	for (int i = tempStr.size()-1; i>-1; --i){
	    if( tempStr[i] == ' ' )tempStr.erase( tempStr.begin()+i );
	    else if( tempStr[i] == '\t' )tempStr.erase( tempStr.begin()+i );
	}
    }
    return tempStr;
}

const std::string Mugen::Util::invertSlashes(const std::string &str){
    std::string tempStr = str;
    if (tempStr.find('\\') != std::string::npos){
	for (int i = tempStr.size()-1; i>-1; --i){
	    if (tempStr[i] == '\\'){
                tempStr[i] = '/';
            }
	}
    }
    return tempStr;
}
/*
std:string getHeadDir( const std::string & dir ){
    return dir.substr( ( dir.find_lastof( '/' ) != std::string::npos ? dir.find_lastof( '/' ) : 0 ), lastslash,dir.size() );
}*/

static const Filesystem::AbsolutePath findCharacterDefMotif(const string & name){
    /* maybe pass in the motif directory instead of just magically getting it here? */
    Filesystem::AbsolutePath chars = Storage::instance().findInsensitive(Mugen::Data::getInstance().getMotifDirectory().getDirectory().join(Filesystem::RelativePath("chars")));
    Filesystem::AbsolutePath base = Storage::instance().lookupInsensitive(chars, Filesystem::RelativePath(name));
    return Storage::instance().lookupInsensitive(base, Filesystem::RelativePath(name + ".def"));
}

static const Filesystem::AbsolutePath findCharacterDefNormal(const string & name){
    Filesystem::AbsolutePath chars = Storage::instance().findInsensitive(Mugen::Data::getInstance().getCharDirectory());
    Filesystem::AbsolutePath base = Storage::instance().lookupInsensitive(chars, Filesystem::RelativePath(name));
    return Storage::instance().lookupInsensitive(base, Filesystem::RelativePath(name + ".def"));
}

const Filesystem::AbsolutePath Mugen::Util::findCharacterDef(const string & name){
    try{
        return findCharacterDefMotif(name);
    } catch (const Filesystem::NotFound & fail){
        Global::debug(1) << "Failed to find character " << name << " in motif " << Data::getInstance().getMotifDirectory().path() << " because " << fail.getTrace() << endl;
        return findCharacterDefNormal(name);
    }
}

/*
const Filesystem::AbsolutePath findStageMotif(const Filesystem::RelativePath & path){
    Filesystem::AbsolutePath top = Filesystem::findInsensitive(Mugen::Data::getInstance().getMotifDirectory().getDirectory());
    return Filesystem::lookupInsensitive(top, path);
}

const Filesystem::AbsolutePath findStageNormal(const Filesystem::RelativePath & path){
    return Filesystem::findInsensitive(Mugen::Data::getInstance().getDirectory().join(path));
}

const Filesystem::AbsolutePath Mugen::Util::findStageDef(const Filesystem::RelativePath & path){
    try{
        return findStageMotif(path);
    } catch (const Filesystem::NotFound & fail){
        return findStageNormal(path);
    }
}
*/

/* search in <motif> and then <motif>/.. */
static const Filesystem::AbsolutePath findFileMotif(const Filesystem::RelativePath & path){
    try{
        return Storage::instance().lookupInsensitive(Storage::instance().findInsensitive(Mugen::Data::getInstance().getMotifDirectory()), path);
    } catch (const Filesystem::NotFound & fail){
        Filesystem::AbsolutePath top = Storage::instance().findInsensitive(Mugen::Data::getInstance().getMotifDirectory().getDirectory());
        return Storage::instance().lookupInsensitive(top, path);
    }
}

/* search in mugen/data and mugen/ */
static const Filesystem::AbsolutePath findFileNormal(const Filesystem::RelativePath & path){
    try{
        return Storage::instance().findInsensitive(Mugen::Data::getInstance().getDataDirectory().join(path));
    } catch (const Filesystem::NotFound & fail){
        return Storage::instance().findInsensitive(Mugen::Data::getInstance().getDirectory().join(path));
    }
}

const Filesystem::AbsolutePath Mugen::Util::findFile(const Filesystem::RelativePath & path){
    try{
        return findFileMotif(path);
    } catch (const Filesystem::NotFound & fail){
        return findFileNormal(path);
    }
}

/* search for a file starting in the absolute path and then search the rest of the
 * mugen directories
 */
const Filesystem::AbsolutePath Mugen::Util::findFile(const Filesystem::AbsolutePath & base, const Filesystem::RelativePath & path){
    try{
        return Storage::instance().lookupInsensitive(base, path);
    } catch (const Filesystem::NotFound & fail){
        return findFile(path);
    }
}

const Filesystem::AbsolutePath Mugen::Util::fixFileName(const Filesystem::AbsolutePath &dir, std::string str){
    Global::debug(2) << "Current File: " << str << endl;
    // Temp fix until the lexer fixes this crap
    str = Mugen::Util::removeSpaces(str);
    // Fixes windows paths
    str = Mugen::Util::invertSlashes(str);
    // Lets check if we need to fix anything first
    Global::debug(2) << "Checking for file in " << (dir.path()+str) << endl;
    if (::Util::exists(dir.path() + str) == false){
	Global::debug(2) << "Couldn't find file: " << str << endl;
	std::string returnString = "";
	std::vector<Filesystem::AbsolutePath> files = Storage::instance().getFiles(dir, "*");
	Global::debug(2) << "Correcting file: " << str << ", in directory: "<< dir.path() <<".\nGot " << files.size() << " files." << endl;
        for (vector<Filesystem::AbsolutePath>::iterator it = files.begin(); it != files.end(); it++){
            Filesystem::AbsolutePath & path = *it;
	    // temp = Mugen::Util::fixCase( temp );
	    if (Filesystem::InsensitivePath(dir.join(Filesystem::RelativePath(str))) == path){
		// We got number one chinese retaurant
                return path;
	    }
	}
        throw Filesystem::NotFound(__FILE__, __LINE__, str);
	// Global::debug(2) << "Corrected file: " << returnString << endl;
	// return returnString;
    }
    return Filesystem::AbsolutePath(dir.path() + str);
}

/* this is basename() */
const std::string Mugen::Util::stripDir( const std::string &str ){ 
    return Path::stripDir(str);
}

/* this is dirname() */
const std::string Mugen::Util::getFileDir( const std::string &str ){
    std::string temp = str;
    if( str.find( "/") != std::string::npos || str.find( "\\") != std::string::npos ){
	size_t rem = temp.find_last_of( "/" );
	if( rem != std::string::npos ){
	    return str.substr(0,rem+1);
	}
	rem = temp.find_last_of( "\\" );
	if( rem != std::string::npos ){
	    return str.substr(0,rem+1);
	}
    }
    return "";
}

// If you use this, please delete the item after you use it, this isn't java ok
MugenItemContent *Mugen::Util::parseOpt( const std::string &opt ){
    std::string contentHolder = "";
    MugenItemContent *temp = new MugenItemContent();
    const char * ignored = " \r\n";
    Global::debug(2) << "Parsing string to ItemContent: " << opt << endl;
    for( unsigned int i = 0; i < opt.size(); ++i ){
	if( opt[i] == ';' )break;
	if( opt[i] == ' ' ){
	    if( !contentHolder.empty() ) *temp << contentHolder;
	    Global::debug(3) << "Got content: " << contentHolder << endl;
	    contentHolder = "";
	}
	// We got one push back the other and reset the holder to get the next
	else if( opt[i] == ',' ){
	    if( !contentHolder.empty() ) *temp << contentHolder;
	    Global::debug(3) << "Got content: " << contentHolder << endl;
	    contentHolder = "";
	}
	//Start grabbing our item
	else if (! strchr(ignored, opt[i])){
		contentHolder += opt[i];
	}
    }
    if( !contentHolder.empty() ){
	*temp << contentHolder;
	Global::debug(3) << "Got content: " << contentHolder << endl;
    }
    return temp;
}

double Mugen::Util::gameTicks(double ticks, double gameSpeed){
    int mugenSpeed = 60;
    return ticks * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
}

/* PCX HEADER */
typedef struct {
    unsigned char manufacturer; /* Constant Flag, 10 = ZSoft .pcx */
    unsigned char version;      /* Version information */
    unsigned char encoding;     /* 1 = .PCX run length encoding */
    unsigned char BitsPerPixel; /* Number of bits to represent a pixel */
    short Xmin;                 /* Image Dimensions */
    short Ymin;
    short Xmax;
    short Ymax;
    unsigned short HDpi;         /* Horizontal Resolution of image in DPI */
    unsigned short VDpi;         /* Vertical Resolution of image in DPI */
    unsigned char colormap[48];  /* Color palette setting */
    unsigned char reserved;      /* Should be set to 0. */
    unsigned char NPlanes;       /* Number of color planes */
    unsigned short BytesPerLine; /* Number of bytes to allocate for a scanline plane. */
    unsigned short PaletteInfo;  /* How to interpret palette: 1 = Color/BW,
				    2 = Grayscale (ignored in PB IV/ IV +) */
    unsigned short HscreenSize;  /* Horizontal screen size in pixels. */
    unsigned short VscreenSize;  /* Vertical screen size in pixels. */
    char filler[54];             /* Blank to fill out 128 byte header. */
} pcx_header;
/* Source: ZSoft Corporation's PCX File Format Technical Reference Manual, Revision 5. */

bool Mugen::Util::readPalette(const Filesystem::AbsolutePath & filename, unsigned char * out){
    const int PALETTE_SIZE = 768;
    ifstream file(filename.path().c_str());
    if (!file){
        return false;
    }
    try{
        file.seekg(0, std::ios::end);
        int size = file.tellg();
        file.seekg(0, std::ios::beg);
        if (size == PALETTE_SIZE){
            file.read((char*) out, PALETTE_SIZE);
            if (file.gcount() != PALETTE_SIZE){
                throw MugenException("Didn't read 768 bytes");
            }
            unsigned char save[3];
            /* in-memory palette reverse */
            for (int i = 0; i < 256 / 2; i++){
                memcpy(save, out + i * 3, 3);
                memcpy(out + i * 3, out + (256 - 1) * 3 - i * 3, 3);
                memcpy(out + (256 - 1) * 3 - i * 3, save, 3);
            }
        } else if (size < PALETTE_SIZE){
            throw MugenException("Not an ACT file or PCX file");
        } else {
            pcx_header pcxhead;
            file.read((char*) &pcxhead, sizeof(pcx_header));

            bool valid_pcx = pcxhead.manufacturer == 10 && 
                             pcxhead.version >= 5 &&
                             pcxhead.BitsPerPixel == 8 &&
                             pcxhead.NPlanes == 1;

            if (valid_pcx){
                /* Palette is at the end. Check to see if the PCX file uses an 8-bit palette. */
                file.seekg(-769, std::ios::end);
                unsigned char save;
                file.read((char*) &save, 1);
                if (save == 12){
                    file.read((char*) out, PALETTE_SIZE);
                } else {
                    Global::debug(0) << "File " << filename.path() << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
                    throw MugenException("Failed");
                }
            }
        }

    } catch (const MugenException & e){
        file.close();
        return false;
    }

    file.close();
    return true;
}

#if 0
bool Mugen::Util::readPalette(const Filesystem::AbsolutePath & filename, unsigned char *pal){
    unsigned char colorsave[3]; // rgb pal save
    FILE *act_file = fopen(filename.path().c_str(), "rb");
    if (!act_file){
	Global::debug(1) << "Unable to open ACT file: " << filename.path() << endl;
	if (act_file){
            fclose(act_file);
        }
	pal = 0;
	return false;
    } else {
	fseek( act_file, 0, SEEK_END );
	// Then it must be an act file
	if (ftell(act_file) == 768){ 
	    for (int i = 0; i < 256; i++){
		fseek(act_file, -3*(i+1), SEEK_END);
		size_t bleh = fread(colorsave, 3, 1, act_file);
		bleh = bleh;
		pal[3*i]=colorsave[0];
		pal[3*i+1]=colorsave[1];
		pal[3*i+2]=colorsave[2];
	    }
	    Global::debug(1) << "Applying palette from ACT file: " << filename.path() << endl;
	    if (act_file){
                fclose(act_file);
            }

	    return true;
	} else if (ftell(act_file) < 897){ 
            // 128-byte header + 768-byte palette + 0x0C byte = minimum 8-bit PCX file size.
            Global::debug(1) << "File " << filename.path() << " is not a valid palette file." << endl;
	    if (act_file){
                fclose(act_file);
            }
	    pal = 0;
	    return false;
        } else {
            // we'll assume it's a PCX file, for now.
            fseek(act_file, 0, SEEK_SET);
	    pcx_header pcxhead;
            size_t bleh = fread(&pcxhead, sizeof(pcx_header), 1, act_file);
            // Check to see if the PCX file uses an 8-bit palette.
            if ((pcxhead.manufacturer == 10) && (pcxhead.version >= 5) && (pcxhead.BitsPerPixel == 8) && (pcxhead.NPlanes == 1)){
                fseek( act_file, -769, SEEK_END);
		// No need to define another variable; colorsave will do just fine.
                bleh = fread( colorsave, 1, 1, act_file ); 
                if (colorsave[0] == 12){
                    fseek(act_file, -768, SEEK_END);
                    bleh = fread(pal, 768, 1, act_file);
                    Global::debug(1) << "Applying palette from PCX file: " << filename.path() << endl;
		    return true;
                } else {
                    Global::debug(1) << "File " << filename.path() << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
		    if (act_file){
                        fclose(act_file);
                    }
		    pal = 0;
		    return false;
                }
            } else {
            // Add support for JASC and RIFF palette files later... 
            // Minimum JASC PAL size = 1,813 bytes (carriage returns are necessary). 
		Global::debug(1) << "File " << filename.path() << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
		if (act_file){
                    fclose( act_file );
                }
		pal = 0;
		return false;
            }
        }
    }

    if (act_file){
        fclose(act_file);
    }

    pal = 0;
    return false;
}
#endif

/* maybe move this to Filesystem */
static int computeFileSize(const string & path){
    // Lets get the filesize
    FILE * stream = fopen(path.c_str(), "r" );
    fseek(stream, 0, SEEK_END);
    int filesize = ftell(stream);
    fclose(stream);
    return filesize;
}

namespace Mugen{
    namespace Util{

class SffReader{
public:
    SffReader(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette):
    filename(filename),
    currentSprite(0){
        /* 16 skips the header stuff */
        sffStream.open(filename.path().c_str(), ios::binary);
        if (!sffStream){
            throw MugenException("Could not open SFF file: '" + filename.path() + "'");
        }

        filesize = computeFileSize(filename.path());

        /* TODO: read the first 16 bytes to get the version info.
         * Data starts at the 16th byte.
         */
        location = 16;
        sffStream.seekg(location, ios::beg);
        /* FIXME: change these to uint32 or whatever */
        uint32_t totalGroups = 0;
        totalImages = 0;
        uint32_t suboffset = 0;
        uint32_t subhead = 0;
        uint32_t sharedPal = 0;

        Storage::LittleEndianReader reader(sffStream);

        totalGroups = reader.readByte4();
        totalImages = reader.readByte4();
        suboffset = reader.readByte4();
        subhead = reader.readByte4();
        sharedPal = reader.readByte1();

        if (sharedPal && sharedPal != 1){
            sharedPal = 0;
        }

        location = suboffset;

        if (location < 512 || location > 2147482717 ){
            location = 512;
        }

        Global::debug(2) << "Got Total Groups: " << totalGroups << ", Total Images: " << totalImages << ", Next Location in file: " << location << endl;

        // spriteIndex = new MugenSprite*[totalImages + 1];

        // Palette related
        useact = false;

        //unsigned char colorsave[3]; // rgb pal save

        memset(palsave1, 0, sizeof(palsave1));

        // Load in first palette
        if (readPalette(palette, palsave1)){
            useact = true;
        }
    }

    virtual ~SffReader(){
        sffStream.close();
        // delete[] spriteIndex;
    }

    /* gets all the sprite headers without loading the pcx information */
    void quickReadSprites(){
        for (unsigned int index = 0; index < totalImages; index++){
            MugenSprite * sprite = new MugenSprite();
            sprite->read(sffStream, location);
            spriteIndex[index] = sprite;
            location = sprite->getNext();
        }
    }

    /* Actually loads the pcx data */
    MugenSprite * loadSprite(MugenSprite * sprite, bool mask){
        if (sprite->getBitmap(mask) == NULL){
            if (sprite->getLength() == 0){
                const MugenSprite * temp = loadSprite(spriteIndex[sprite->getPrevious()], mask);
                if (!temp){
                    ostringstream out;
                    out << "Unknown linked sprite " << sprite->getPrevious() << endl;
                    throw MugenException(out.str());
                }
                sprite->copyImage(temp);
            } else {
                bool islinked = false;
                sprite->loadPCX(sffStream, islinked, useact, palsave1, mask);
            }
        }
        return sprite;
    }

    MugenSprite * findSprite(int group, int item, bool mask){
        if (spriteIndex.size() == 0){
            quickReadSprites();
        }
        for (map<int, MugenSprite *>::iterator it = spriteIndex.begin(); it != spriteIndex.end(); it++){
            MugenSprite * sprite = it->second;
            if (sprite->getGroupNumber() == group &&
                sprite->getImageNumber() == item){
                /* make a deep copy */
                return new MugenSprite(*loadSprite(sprite, mask));
            }
        }
        return NULL;
    }

    /* deletes all sprites, only call this if you don't want them! */
    void cleanup(){
        for (map<int, MugenSprite *>::iterator it = spriteIndex.begin(); it != spriteIndex.end(); it++){
            MugenSprite * sprite = it->second;
            delete sprite;
        }
    }

    MugenSprite * readSprite(bool mask){
        bool islinked = false;
        if (location > filesize){
            throw MugenException("Error in SFF file: " + filename.path() + ". Offset of image beyond the end of the file.");
        }

        MugenSprite * sprite = new MugenSprite();
        sprite->read(sffStream, location);
        location = sprite->getNext();

        if (sprite->getLength() == 0){
            const MugenSprite * temp = spriteIndex[sprite->getPrevious()];
            if (!temp){
                ostringstream out;
                out << "Unknown linked sprite " << sprite->getPrevious() << endl;
                throw MugenException(out.str());
            }
            sprite->copyImage(temp);
        } else {
            sprite->loadPCX(sffStream, islinked, useact, palsave1, mask);
        }
            
        spriteIndex[currentSprite] = sprite;
        currentSprite += 1;

        return sprite;
    }

    bool moreSprites(){
        return currentSprite < totalImages;
    }

protected:
    const Filesystem::AbsolutePath filename;
    ifstream sffStream;
    unsigned long currentSprite;
    int totalSprites;
    map<int, MugenSprite*> spriteIndex;
    bool useact;
    int filesize;
    int location;
    uint32_t totalImages;
    unsigned char palsave1[768]; // First image palette
};

    }
}

void Mugen::Util::readSprites(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette, Mugen::SpriteMap & sprites, bool mask){
    SffReader reader(filename, palette);
    /* where replaced sprites go */
    vector<MugenSprite*> unused;
    while (reader.moreSprites()){
        try{
            MugenSprite * sprite = reader.readSprite(mask);

            Mugen::SpriteMap::iterator first_it = sprites.find(sprite->getGroupNumber());
            if (first_it != sprites.end()){
                std::map< unsigned int, MugenSprite * >::iterator it = first_it->second.find(sprite->getImageNumber());
                if (it != first_it->second.end()){
                    Global::debug(0) << "Warning: replacing sprite in " << Storage::instance().cleanse(filename).path() << " group " << sprite->getGroupNumber() << " item " << sprite->getImageNumber() << endl;
                    unused.push_back(it->second);
                }
            }
            sprites[sprite->getGroupNumber()][sprite->getImageNumber()] = sprite;
        } catch (const MugenException & e){
            Global::debug(0) << e.getReason() << endl;
        }
    }

    /* delete all replaced sprites */
    for (vector<MugenSprite*>::iterator it = unused.begin(); it != unused.end(); it++){
        delete (*it);
    }
}

/* TODO: turn this code into a class like SffReader */
void Mugen::Util::readSounds(const Filesystem::AbsolutePath & filename, Mugen::SoundMap & sounds){
    /* 16 skips the header stuff */
    int location = 16;
    ifstream ifile;
    ifile.open(filename.path().c_str(), ios::binary);
    if (!ifile){
        throw MugenException("Could not open SND file: " + filename.path());
    }
    
    // Lets go ahead and skip the crap -> (Elecbyte signature and version) start at the 16th byte
    ifile.seekg(location, ios::beg);
    int totalSounds;
    
    Storage::LittleEndianReader reader(ifile);
    totalSounds = reader.readByte4();
    location = reader.readByte4();
    
     Global::debug(2) << "Got Total Sounds: " << totalSounds << ", Next Location in file: " << location << endl;
    
    // We got some stuff
     if( totalSounds > 0){
         for( int i = 0; i < totalSounds; ++i ){
             // Go to next sound
             ifile.seekg(location, ios::beg);
             // next sprite
             MugenSound *temp = new MugenSound();

             /* FIXME: change 4 to sizeof(...) */
             temp->next = reader.readByte4();
             temp->length = reader.readByte4();
             temp->groupNumber = reader.readByte4();
             temp->sampleNumber = reader.readByte4();
             temp->sample = new char[temp->length];
             ifile.read((char *)temp->sample, temp->length);

             // Set the next file location
             location = temp->next;
             temp->load();

             sounds[temp->groupNumber][temp->sampleNumber] = temp;
         }
     }
    
    ifile.close();
}

vector<Ast::Section*> Mugen::Util::collectBackgroundStuff(list<Ast::Section*>::iterator & section_it, const list<Ast::Section*>::iterator & end, const std::string & name){
    list<Ast::Section*>::iterator last = section_it;
    vector<Ast::Section*> stuff;

    Ast::Section * section = *section_it;
    std::string head = section->getName();
    /* better to do case insensitive regex matching rather than
     * screw up the original string
     */
    head = Mugen::Util::fixCase(head);
    string prefix = PaintownUtil::captureRegex(head, "(.*)" + name + "def", 0);
    stuff.push_back(section);
    section_it++;

    while (true){
        if (section_it == end){
            break;
        }

        section = *section_it;
        string sectionName = section->getName();
        sectionName = Mugen::Util::fixCase(sectionName);
        Global::debug(2, __FILE__) << "Match '" << (prefix + name + ".*") << "' against '" << sectionName << "'" << endl;
        if (PaintownUtil::matchRegex(sectionName, prefix + name + ".*") || PaintownUtil::matchRegex(sectionName, ".*begin *action.*")){
            stuff.push_back(section);
        } else {
            break;
        }

        last = section_it;
        section_it++;
    }
    section_it = last;
    return stuff;
}

MugenSprite * Mugen::Util::getSprite(const Mugen::SpriteMap & sprites, int group, int item){
    Mugen::SpriteMap::const_iterator map = sprites.find(group);
    if (map == sprites.end()){
        return NULL;
    }

    const Mugen::GroupMap & groupMap = (*map).second;
    Mugen::GroupMap::const_iterator it = groupMap.find(item);
    if (it != groupMap.end()){
        return (*it).second;
    }
    return NULL;
}

MugenAnimation *Mugen::Util::getAnimation(Ast::Section * section, const Mugen::SpriteMap &sprites, bool mask){
    MugenAnimation *animation = new MugenAnimation();

    /* see parser/air.peg */
    class Walker: public Ast::Walker{
    public:
        Walker(MugenAnimation * animation, const Mugen::SpriteMap & sprites, bool mask):
        Ast::Walker(),
        animation(animation),
        sprites(sprites),
        clsn1Reset(false),
        clsn2Reset(false),
        setloop(false),
        mask(mask){
        }

        /* data */
        MugenAnimation * animation;
        const Mugen::SpriteMap & sprites;
        std::vector<MugenArea> clsn1Holder;
        std::vector<MugenArea> clsn2Holder;
        bool clsn1Reset;
        bool clsn2Reset;
        bool setloop;
        bool mask;

        enum Expect{
            clsn1, clsn2
        };

        Expect expected;

        /* callbacks */
        virtual void onValueList(const Ast::ValueList & values){
            // This is the new frame
            MugenFrame *frame = new MugenFrame(mask);
            frame->defenseCollision = clsn2Holder;
            frame->attackCollision = clsn1Holder;
            frame->loopstart = setloop;
            /* Get sprite details */
            Ast::View view = values.view();
            int group = 0, spriteNumber = 0;
            // Need to get the parsed data and populate these above items
            try{
                view >> group >> spriteNumber >> frame->xoffset >> frame->yoffset >> frame->time;
            } catch (const Ast::Exception & fail){
                std::ostringstream out;
                out << "Could not parse animation because " << fail.getReason();
                throw MugenException(out.str());
            }
            string flip;
            string blend;
            try{
                view >> flip >> blend;
            } catch (const Ast::Exception & e){
            }

            flip = Mugen::Util::fixCase(flip);
            blend = Mugen::Util::fixCase(blend);
            
            Global::debug(2) << "Group: " << group << " | Sprite: " << spriteNumber << " | x: " << frame->xoffset << " | y: " << frame->yoffset << " | time: " << frame->time << " | flip " << flip << " blend '" << blend << "'" << endl;

            if (flip == "h"){
                //frame->flipHorizontal = true;
                frame->effects.facing = -1;
            } else if (flip == "v"){
                //frame->flipVertical = true;
                frame->effects.vfacing = -1;
            }

            if (PaintownUtil::matchRegex(blend, "^a")){
                frame->effects.trans = Add;

                /*
                 * If you wish to specify alpha values for color addition, use the
                 * parameter format "AS???D???", where ??? represents the values of the
                 * source and destination alpha respectively. Values range from 0 (low)
                 * to 256 (high). For example, "AS64D192" stands for "Add Source_64 to
                 * Dest_192". Also, "AS256D256" is equivalent to just "A". A shorthand
                 * for "AS256D128" is "A1".
                 */
                int source = frame->effects.alphalow;
                int dest = frame->effects.alphahigh;
                // string regex = "as\\([[:digit:]]\\+\\)d\\([[:digit:]]\\+\\)";
                string regex = "as([[:digit:]]+)d([[:digit:]]+)";
                // string regex = "as[0-9]+d[0-9]+";
                if (PaintownUtil::matchRegex(blend, regex)){
                    source = atoi(PaintownUtil::captureRegex(blend, regex, 0).c_str());
                    dest = atoi(PaintownUtil::captureRegex(blend, regex, 1).c_str());
                } else if (blend == "a1"){
                    source = 256;
                    dest = 128;
                }
                Global::debug(2) << "Alpha source " << source << " destination " << dest << endl;
                frame->effects.alphalow = source;
                frame->effects.alphahigh = dest;
            } else if (blend == "s"){
                frame->effects.trans = Sub;
            }

            // Add sprite
            frame->sprite = getSprite(sprites, group, spriteNumber);
            if (frame->sprite == 0){
                Global::debug(1) << "No sprite for group " << group << " number " << spriteNumber << endl;
            }
            // Add frame
            this->animation->addFrame(frame);

            // Clear if it was not a default set, since it's valid for only one frame
            if (clsn1Reset){
                clsn1Holder.clear();
                clsn1Reset = false;
            }

            if (clsn2Reset){
                clsn2Holder.clear();
                clsn2Reset = false;
            }

            if (setloop){
                setloop = false;
            }

        }

        virtual void onAttributeArray(const Ast::AttributeArray & array){
            // Global::debug(1, __FILE__) << "Array item: " << array.toString() << endl;
            MugenArea area;
            array.view() >> area.x1 >> area.y1 >> area.x2 >> area.y2;
            Global::debug(2) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
            string source = "some file";
            if (array == "clsn1"){
                if (expected == clsn2){
                    Global::debug(0) << "Warning: Expected clsn2 but read clsn1: " << array.toString() << " at " << source << endl;
                }
                clsn1Holder.push_back(area);
            } else if (array == "clsn2"){
                if (expected == clsn1){
                    Global::debug(0) << "Warning: Expected clsn1 but read clsn2: " << array.toString() << " at " << source << endl;
                }
                clsn2Holder.push_back(area);
            }
        }

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
        }
    
        virtual void onAttributeKeyword(const Ast::AttributeKeyword & simple){
            if (simple == "loopstart"){
                setloop = true;
                return;
            }

            if (simple == "clsn1default"){
                expected = clsn1;
                clsn1Holder.clear();
                clsn1Reset = false;
                /* do we really care how many boxes are specified here? it seems good
                 * enough to just read them in as they are defined in
                 *  clsn1[0] = ...
                 *  clsn1[1] = ...
                 * I mean so what if the file has
                 *   clsn1default: 1
                 *   clsn1[0] = ...
                 *   clsn1[1] = ...
                 *   clsn1[2] = ...
                 * Just read 3 boxes because thats whats given.
                 */
                int boxes;
                simple.view() >> boxes;
            } else if (simple == "clsn2default"){
                expected = clsn2;
                clsn2Holder.clear();
                clsn2Reset = false;
                int boxes;
                simple.view() >> boxes;
            } else if (simple == "clsn1"){
                expected = clsn1;
                clsn1Holder.clear();
                clsn1Reset = true;
                int boxes;
                simple.view() >> boxes;
            } else if (simple == "clsn2"){
                expected = clsn2;
                clsn2Holder.clear();
                clsn2Reset = true;
                int boxes;
                simple.view() >> boxes;
            }
        }
    };

    /* copy the section because it might be parsed again later */
    /* FIXME: this is sort of ugly, instead use some sort of view on the ast */
    PaintownUtil::ReferenceCount<Ast::Section> copy((Ast::Section*) section->copy());
    Walker walker(animation, sprites, mask);
    copy->walk(walker);

    /* FIXME!! use regex to get the number */
    std::string head = section->getName();
    head.replace(0,13,"");
    int h;
    MugenItem(head) >> h;
    animation->setType(Mugen::AnimationType(h));
    Global::debug(2) << "Adding Animation 'Begin Action " << h << "' : '" << animation->getName(animation->getType()) << "'" << endl;
    return animation;
}

static void parseException(const string & file, const string & error, int line, int column){
    ifstream safe(file.c_str());
    char buffer[1024];
    int last = 0;
    for (int i = 0; i < line && safe.good(); i++){
        safe.getline(buffer, sizeof(buffer));
        last = safe.gcount();
    }
    /* force a null byte */
    if (last == sizeof(buffer) - 1){
        buffer[sizeof(buffer) - 1] = '\0';
    }
    safe.close();
    ostringstream out;
    out << "Could not parse " << file << ". Line " << line << " has an error around column " << column << "\n\n" << buffer << "\n\nParser error: " << error;
    throw MugenException(out.str());
}

PaintownUtil::ReferenceCount<Ast::AstParse> Mugen::Util::parseAir(const string & filename){
    try{
        return ParseCache::parseAir(filename);
    } catch (const Ast::Exception & e){
        throw MugenException(e.getReason());
    } catch (const Mugen::Air::ParseException & e){
        parseException(filename, e.getReason(), e.getLine(), e.getColumn());
        throw MugenException("won't get here");
    }
}

PaintownUtil::ReferenceCount<Ast::AstParse> Mugen::Util::parseDef(const string & filename){
    try{
        return ParseCache::parseDef(filename);
    } catch (const Ast::Exception & e){
        throw MugenException(e.getReason());
    } catch (const Mugen::Def::ParseException & e){
        parseException(filename, e.getReason(), e.getLine(), e.getColumn());
        throw MugenException("won't get here");
    }
}

PaintownUtil::ReferenceCount<Ast::AstParse> Mugen::Util::parseCmd(const string & filename){
    try{
        return ParseCache::parseCmd(filename);
    } catch (const Ast::Exception & e){
        throw MugenException(e.getReason());
    } catch (const Mugen::Cmd::ParseException & e){
        parseException(filename, e.getReason(), e.getLine(), e.getColumn());
        throw MugenException("won't get here");
    }
}

std::map<int, MugenAnimation *> Mugen::Util::loadAnimations(const Filesystem::AbsolutePath & filename, const SpriteMap sprites, bool mask){
    AstRef parsed(parseAir(filename.path()));
    // Global::debug(2, __FILE__) << "Parsing animations. Number of sections is " << parsed->getSections()->size() << endl;
    
    map<int, MugenAnimation*> animations;
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
        std::string head = section->getName();
        Global::debug(2, __FILE__) << "Animation section '" << head << "'" << endl;
	head = Mugen::Util::fixCase(head);
        int number;
        if (PaintownUtil::matchRegex(head, "begin action [0-9]+")){
            number = atoi(PaintownUtil::captureRegex(head, "begin action ([0-9]+)", 0).c_str());
            Global::debug(2, __FILE__) << "Parse animation " << number << endl;
            animations[number] = Mugen::Util::getAnimation(section, sprites, mask);
        }
    }
    return animations;
}

static std::string removeLastDir( const std::string &dir ){
    if (dir.find( "/") != std::string::npos){
	std::string temp = dir;
	size_t rem = temp.find_last_of( "/" );
	temp.erase(rem);
	rem = temp.find_last_of("/");
	temp.erase(rem + 1);
	return temp;
    }
    return dir;
}

/*
const Filesystem::AbsolutePath Mugen::Util::getCorrectFileLocation(const Filesystem::AbsolutePath &dir, const std::string &file ){
    // First check initial location else it should be in the base dir
    std::string ourFile = file;
    ourFile = Mugen::Util::removeSpaces(ourFile);
    if (::Util::exists(dir.path() + ourFile) == true){
	Global::debug(1) << "No correction needed found File: " << (dir.path() + ourFile) << endl;
	return dir.join(Filesystem::RelativePath(ourFile));
    } else {
	// Descend two levels.. if not good enough screw it it doesn't exist
	std::string tempDir = removeLastDir(dir.path());
	Global::debug(1) << "Going down one dir: " << tempDir + ourFile << endl;
	if (::Util::exists(tempDir + ourFile) == true){
	    Global::debug(1) << "Found File: " << tempDir + ourFile << endl;
	    return Filesystem::AbsolutePath(tempDir + ourFile);
	} 
	tempDir = removeLastDir(tempDir);
	Global::debug(1) << "Going down one more dir: " << tempDir + ourFile << endl;
	if (::Util::exists(tempDir + ourFile) == true){
	    Global::debug(1) << "Found File: " << tempDir + ourFile << endl;
	    return Filesystem::AbsolutePath(tempDir + ourFile);
	} 
    }
    Global::debug(1) << "No correction needed File: " << dir.path() + ourFile << endl;
    return dir.join(Filesystem::RelativePath(ourFile));
}
*/

const std::string Mugen::Util::probeDef(const AstRef & parsed, const std::string & section, const std::string & search){
    std::string ourSection = fixCase(section);
    std::string ourSearch = fixCase(search);
   
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
	Ast::Section * astSection = *section_it;
	std::string head = astSection->getName();
        
	head = fixCase(head);

        if (head.compare(ourSection) == 0){
	    class SearchWalker: public Ast::Walker{
            public:
                SearchWalker(const std::string &search, std::string &result):
                search(search),
                result(result){}
                virtual ~SearchWalker(){}
                const std::string &search;
                std::string &result;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == search){
                        simple.view() >> result;
                        Global::debug(2) << "Found result: " << result << endl;
                    } 
                }
	    };
	    std::string result;
	    SearchWalker walk(ourSearch, result);
	    astSection->walk(walk);
	    
	    if (!result.empty()){
		// Global::debug(1) << "Found '" << search << "' in Section '" << section << "' in Definition file '" << file.path() << "'" << endl;
		return result;
	    }
	}
    }
    // Couldn't find search item throw exception
    // throw MugenException("Couldn't find '" + search + "' in Section '" + section + "' in Definition file '" + file.path() + "'");
    throw MugenException("Couldn't find '" + search + "' in section '" + section + "'");
}

void Mugen::Util::destroyRaw(const map< unsigned int, std::map< unsigned int, MugenSprite * > > & sprites){
    for (map< unsigned int, std::map< unsigned int, MugenSprite * > >::const_iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
        for(map< unsigned int, MugenSprite * >::const_iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
            MugenSprite * sprite = j->second;
            sprite->unloadRaw();
        }
    }
}

const std::string Mugen::Util::probeDef(const Filesystem::AbsolutePath &file, const std::string &section, const std::string &search){
    return probeDef(parseDef(file.path()), section, search);
}

MugenSprite *Mugen::Util::probeSff(const Filesystem::AbsolutePath &file, int groupNumber, int spriteNumber, bool mask, const Filesystem::AbsolutePath & actFile){
    SffReader reader(file, actFile);
    MugenSprite * found = reader.findSprite(groupNumber, spriteNumber, mask);
    reader.cleanup();
    if (found != NULL){
        return found;
    }
    ostringstream out;
    out << "Could not find sprite " << groupNumber << ", " << spriteNumber << " in " << file.path();
    throw MugenException(out.str());
}
        
void Mugen::Util::getIconAndPortrait(const Filesystem::AbsolutePath & sffPath, const Filesystem::AbsolutePath & actPath, MugenSprite ** icon, MugenSprite ** portrait){
    SffReader reader(sffPath, actPath);
    *icon = reader.findSprite(9000, 0, true);
    *portrait = reader.findSprite(9000, 1, true);
    reader.cleanup();
    if (*icon == NULL || *portrait == NULL){
        bool failed_icon = *icon == NULL;
        bool failed_portrait = *portrait == NULL;
        delete *icon;
        *icon = NULL;
        delete *portrait;
        *portrait = NULL;
        ostringstream out;
        if (failed_icon){
            out << "Could not find sprite " << 9000 << ", " << 0 << " in " << sffPath.path();
        } else {
            out << "Could not find sprite " << 9000 << ", " << 1 << " in " << sffPath.path();
        }
        throw MugenException(out.str());
    }
}

Mugen::Point::Point():
x(0),
y(0){
}

Mugen::Point::Point(int x, int y):
x(x),
y(y){
}

Mugen::Point &Mugen::Point::operator=(const Mugen::Point &p){
    this->x = p.x;
    this->y = p.y;
    return *this;
}

Mugen::Point::~Point(){
}

Mugen::Effects::Effects():
trans(None),
alphalow(255),
alphahigh(255),
mask(false),
facing(1),
vfacing(1),
scalex(1),
scaley(1),
filter(NULL){
}

Mugen::Effects::Effects(const Mugen::Effects & copy){
    this->trans = copy.trans;
    this->alphalow = copy.alphalow;
    this->alphahigh = copy.alphahigh;
    this->mask = copy.mask;
    this->facing = copy.facing;
    this->vfacing = copy.vfacing;
    this->scalex = copy.scalex;
    this->scaley = copy.scaley;
    this->dimension = copy.dimension;
    this->filter = copy.filter;
}

const Mugen::Effects &Mugen::Effects::operator=(const Mugen::Effects &e){
    this->trans = e.trans;
    this->alphalow = e.alphalow;
    this->alphahigh = e.alphahigh;
    this->mask = e.mask;
    this->facing = e.facing;
    this->vfacing = e.vfacing;
    this->scalex = e.scalex;
    this->scaley = e.scaley;
    this->dimension = e.dimension;
    this->filter = e.filter;
    return *this;
}
        
Mugen::Effects Mugen::Effects::operator+(const Mugen::Effects & e2) const {
    Mugen::Effects result(*this);
    result.mask |= e2.mask;
    return result;
}
            
Mugen::Effects Mugen::Effects::operator+(const TransType & translucent) const {
    Mugen::Effects result(*this);
    result.trans = translucent;
    return result;
}

Mugen::Effects::~Effects(){
}

Mugen::Element::Element():
ID(0),
layer(Background){
}

Mugen::Element::~Element(){
}
