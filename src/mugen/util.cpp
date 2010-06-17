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
#include "util/timedifference.h"

#include "util.h"
#include "animation.h"
#include "item.h"
#include "item-content.h"
#include "section.h"
#include "menu.h"
#include "sound.h"
#include "reader.h"
#include "sprite.h"
#include "globals.h"
#include "init.h"
#include "state.h"

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

const std::string Mugen::Util::invertSlashes( const std::string &str ){
    std::string tempStr = str;
    if( tempStr.find('\\') != std::string::npos ){
	for( int i = tempStr.size()-1; i>-1; --i){
	    if( tempStr[i] == '\\' )tempStr[i] = '/';
	}
    }
    return tempStr;
}
/*
std:string getHeadDir( const std::string & dir ){
    return dir.substr( ( dir.find_lastof( '/' ) != std::string::npos ? dir.find_lastof( '/' ) : 0 ), lastslash,dir.size() );
}*/

const Filesystem::AbsolutePath Mugen::Util::fixFileName( const Filesystem::AbsolutePath &dir, std::string str ){
    Global::debug(2) << "Current File: " << str << endl;
    // Temp fix until the lexer fixes this crap
    str = Mugen::Util::removeSpaces(str);
    // Fixes windows paths
    str = Mugen::Util::invertSlashes(str);
    // Lets check if we need to fix anything first
    Global::debug(2) << "Checking for file in " << (dir.path()+str) << endl;
    if (::Util::exists( dir.path() + str ) == false){
	Global::debug(2) << "Couldn't find file: " << str << endl;
	std::string returnString = "";
	std::vector< string > files = ::Util::getFiles( dir, "*" );
	Global::debug(2) << "Correcting file: " << str << ", in directory: "<< dir.path() <<".\nGot " << files.size() << " files." << endl;
	for( unsigned int i = 0; i < files.size(); ++i ){
	    std::string temp = files[i].c_str();
	    temp = Mugen::Util::fixCase( temp );
	    if (std::string( dir.path() + str ) == temp ){
		// We got number one chinese retaurant
		// returnString = Filesystem::AbsolutePath(files[i]);
                return Filesystem::AbsolutePath(files[i]);
		// break;
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
    return Filesystem::stripDir(str);
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

double Mugen::Util::gameTicks(double gameSpeed){
    int mugenSpeed = 60;
    return Global::speed_counter * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
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


bool Mugen::Util::readPalette(const Filesystem::AbsolutePath & filename, unsigned char *pal){
    unsigned char colorsave[3]; // rgb pal save
    FILE *act_file = fopen(filename.path().c_str(), "rb");
    if( !act_file ){
	Global::debug(1) << "Unable to open ACT file: " << filename.path() << endl;
	if (act_file){
            fclose( act_file );
        }
	pal = 0;
	return false;
    }
    else{
	fseek( act_file, 0, SEEK_END );
	// Then it must be an act file
	if( ftell(act_file) == 768 ){ 
	    for( int i=0; i<256; i++ ){
		fseek( act_file, -3*(i+1), SEEK_END );
		// stupid fread
		size_t bleh = fread( colorsave, 3, 1, act_file );
		bleh = bleh;
		pal[3*i]=colorsave[0];
		pal[3*i+1]=colorsave[1];
		pal[3*i+2]=colorsave[2];
	    }
	    Global::debug(1) << "Applying palette from ACT file: " << filename.path() << endl;
	    if( act_file )fclose( act_file );
	    return true;
	}
	// 128-byte header + 768-byte palette + 0x0C byte = minimum 8-bit PCX file size.
	else if( ftell(act_file) < 897 ){ 
            Global::debug(1) << "File " << filename.path() << " is not a valid palette file." << endl;
	    if( act_file )fclose( act_file );
	    pal = 0;
	    return false;
        }
	// we'll assume it's a PCX file, for now.
        else{ 
            fseek( act_file, 0, SEEK_SET );
	    pcx_header pcxhead;
            size_t bleh = fread( &pcxhead, sizeof(pcx_header), 1, act_file );
            // Check to see if the PCX file uses an 8-bit palette.
            if( (pcxhead.manufacturer == 10) && (pcxhead.version >= 5) && (pcxhead.BitsPerPixel == 8) && (pcxhead.NPlanes == 1) ){
                fseek( act_file, -769, SEEK_END);
		// No need to define another variable; colorsave will do just fine.
                bleh = fread( colorsave, 1, 1, act_file ); 
                if( colorsave[0] == 12 ){
                    fseek( act_file, -768, SEEK_END );
                    bleh = fread( pal, 768, 1, act_file );
                    Global::debug(1) << "Applying palette from PCX file: " << filename.path() << endl;
		    return true;
                }
                else{
                    Global::debug(1) << "File " << filename.path() << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
		    if( act_file )fclose( act_file );
		    pal =0;
		    return false;
                }
            }
            // Add support for JASC and RIFF palette files later... 
            // Minimum JASC PAL size = 1,813 bytes (carriage returns are necessary). 
            else{
		Global::debug(1) << "File " << filename.path() << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
		if( act_file )fclose( act_file );
		pal = 0;
		return false;;
            }
        }
    }
    if( act_file )fclose( act_file );
    pal = 0;
    return false;
}

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

        /* this probably isn't endian safe.. */
        sffStream.read((char *)&totalGroups, sizeof(totalGroups));
        sffStream.read((char *)&totalImages, sizeof(totalImages));
        sffStream.read((char *)&suboffset, sizeof(suboffset));
        sffStream.read((char *)&subhead, sizeof(subhead));
        sffStream.read((char *)&sharedPal, sizeof(bool));
        if (sharedPal && sharedPal != 1){
            sharedPal = 0;
        }

        location = suboffset;

        if (location < 512 || location > 2147482717 ){
            location = 512;
        }

        Global::debug(2) << "Got Total Groups: " << totalGroups << ", Total Images: " << totalImages << ", Next Location in file: " << location << endl;

        spriteIndex = new MugenSprite*[totalImages + 1];

        // Palette related
        useact = false;

        //unsigned char colorsave[3]; // rgb pal save

        // Load in first palette
        if (readPalette(palette, palsave1)){
            useact = true;
        }
    }

    virtual ~SffReader(){
        sffStream.close();
        delete[] spriteIndex;
    }

    MugenSprite * readSprite(){
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
            sprite->loadPCX(sffStream, islinked, useact, palsave1);
        }
            
        spriteIndex[currentSprite] = sprite;
        currentSprite += 1;

        return sprite;

#if 0
        if (sprite->getLength() == 0){ // Lets get the linked sprite
            // This is linked
            islinked = true;
            /* Lets check if this is a duplicate sprite if so copy it
             * if prev is larger than index then this file is corrupt
             */
            if (sprite->getPrevious() >= currentSprite){
                throw MugenException("Error in SFF file: " + filename + ". Incorrect reference to sprite.");
            }

            if (sprite->getPrevious() > 32767){
                throw MugenException("Error in SFF file: " + filename + ". Reference too large in image.");
            }

            while (sprite->getLength() == 0){
                const MugenSprite * temp = spriteIndex[sprite->getPrevious()];
                if (!temp){
                    throw MugenException("Error in SFF file: " + filename + ". Referenced sprite is NULL.");
                }

                // Seek to the location of the pcx data
                sffStream.seekg(temp->getLocation() + 32, ios::beg);
                sprite->setLocation(temp->getLocation());
                sprite->setLength(temp->getNext() - temp->getLocation() - 32);

                if ((sprite->getPrevious() <= temp->getPrevious()) &&
                    ((sprite->getPrevious() != 0) || (currentSprite == 0)) &&
                    temp->getLength()==0){

                    std::ostringstream st;
                    st << "Image " << currentSprite << "(" << sprite->getGroupNumber() << "," << sprite->getImageNumber() << ") : circular definition or forward linking. Aborting.\n"; 
                    throw MugenException(st.str());
                } else {
                    if(sprite->getLength() == 0){
                        sprite->setPrevious(temp->getPrevious());
                    } else {
                        sprite->setNewLength(temp->getNext() - temp->getLocation() -32);
                        sprite->setRealLength(sprite->getNewLength());
                    }
                }

                Global::debug(2) << "Referenced Sprite Location: " << temp->getLocation() << " | Group: " << temp->getGroupNumber() << " | Sprite: " << temp->getGroupNumber() << " | at index: " << sprite->getPrevious() << endl;
            }
        }

        try{
            sprite->loadPCX(sffStream, islinked, useact, palsave1);
            spriteIndex[currentSprite] = sprite;
            location = sprite->getNext();

            if (!location){
                Global::debug(1) << "End of Sprites or File. Continuing...." << endl;
                throw MugenException("No sprites left");
            }

            Global::debug(2) << "Index: " << currentSprite << ", Location: " << sprite->getLocation()  << ", Next Sprite: "  << sprite->getNext() << ", Length: " << sprite->getRealLength() << ", x|y: " << sprite->getX() << "|" << sprite->getY() << ", Group|Image Number: " << sprite->getGroupNumber() << "|" << sprite->getImageNumber() << ", Prev: " << sprite->getPrevious() << ", Same Pal: " << sprite->getSamePalette() << ", Comments: " << sprite->getComments() << endl;

            currentSprite += 1;
            return sprite;
        } catch (const LoadException & le){
            /* ignore this error?? */
            location = sprite->getNext();
            spriteIndex[currentSprite] = 0;
            currentSprite += 1;
            ostringstream out;
            out << "Could not load sprite " << sprite->getGroupNumber() << ", " << sprite->getImageNumber() << ": " << le.getReason() << endl;
            delete sprite;
            throw MugenException(out.str());
        }
#endif
    }

    bool moreSprites(){
        return currentSprite < totalImages;
    }

protected:
    const Filesystem::AbsolutePath filename;
    ifstream sffStream;
    unsigned long currentSprite;
    int totalSprites;
    MugenSprite ** spriteIndex;
    bool useact;
    int filesize;
    int location;
    uint32_t totalImages;
    unsigned char palsave1[768]; // First image palette
};

    }
}

void Mugen::Util::readSprites(const Filesystem::AbsolutePath & filename, const Filesystem::AbsolutePath & palette, Mugen::SpriteMap & sprites){
    SffReader reader(filename, palette);
    while (reader.moreSprites()){
        try{
            MugenSprite * sprite = reader.readSprite();

            Mugen::SpriteMap::iterator first_it = sprites.find(sprite->getGroupNumber());
            if (first_it != sprites.end()){
                std::map< unsigned int, MugenSprite * >::iterator it = first_it->second.find(sprite->getImageNumber());
                if (it != first_it->second.end()){
                    Global::debug(0) << "Warning: replacing sprite " << sprite->getGroupNumber() << ", " << sprite->getImageNumber() << endl;
                    delete it->second;
                }
            }
            sprites[sprite->getGroupNumber()][sprite->getImageNumber()] = sprite;
        } catch (const MugenException & e){
            Global::debug(0) << e.getReason() << endl;
        }
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
    
    ifile.read( (char *)&totalSounds, 4 );
    ifile.read( (char *)&location, 4 );
    
     Global::debug(1) << "Got Total Sounds: " << totalSounds << ", Next Location in file: " << location << endl;
    
    // We got some stuff
    if( totalSounds > 0){
	for( int i = 0; i < totalSounds; ++i ){
	   // Go to next sound
	    ifile.seekg(location, ios::beg);
	    // next sprite
	    MugenSound *temp = new MugenSound();
	    
            /* FIXME: change 4 to sizeof(...) */
	    ifile.read( (char *)&temp->next, 4 );
	    ifile.read( (char *)&temp->length, 4 );
	    ifile.read( (char *)&temp->groupNumber, 4 );
	    ifile.read( (char *)&temp->sampleNumber, 4 );
	    temp->sample = new char[temp->length];
	    ifile.read( (char *)temp->sample, temp->length );
	    
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

/* replaced by background.cpp */
#if 0
MugenBackground *Mugen::Util::getBackground( const unsigned long int &ticker, Ast::Section *section, Mugen::SpriteMap &sprites ){
    MugenBackground *temp = new MugenBackground(ticker);
    std::string head = section->getName();
    head = PaintownUtil::captureRegex(head, ".*[bB][gG] (.*)", 0);
    temp->name = head;
    Global::debug(1) << "Found background: " << temp->name << endl;
    for (list<Ast::Attribute*>::const_iterator attribute_it = section->getAttributes().begin(); attribute_it != section->getAttributes().end(); attribute_it++){
        Ast::Attribute * attribute = *attribute_it;
        if (attribute->getKind() == Ast::Attribute::Simple){
            Ast::AttributeSimple * simple = (Ast::AttributeSimple*) attribute;
            if (*simple == "type"){
                std::string type;
                *simple >> type;
                type = Mugen::Util::removeSpaces(type);
                if (type == "normal" )temp->type = Normal;
                else if( type == "anim" )temp->type = Anim;
                else if( type == "parallax" )temp->type = Parallax;
                else if( type == "dummy" )temp->type = Dummy;
            } else if (*simple == "spriteno"){
                if (temp->type != Anim){
                    *simple >> temp->groupNumber;
                    *simple >> temp->imageNumber;
                }
            } else if (*simple == "actionno"){
                *simple >> temp->actionno;
            } else if (*simple == "id"){
                *simple >> temp->id;
            } else if (*simple == "layerno"){
                *simple >> temp->layerno;
            } else if (*simple == "start"){
                *simple >> temp->startx;
                *simple >> temp->starty;
            } else if (*simple == "delta"){
                *simple >> temp->deltax;
                try{
                    /* the y part is not always given */
                    *simple >> temp->deltay;
                } catch (const Ast::Exception & e){
                }
            } else if (*simple == "trans"){
                std::string type;
                *simple >> type;
                type = Mugen::Util::removeSpaces(type);
                if( type == "none" )temp->effects.trans = NONE;
                else if( type == "add" )temp->effects.trans =  ADD;
                else if( type == "add1" )temp->effects.trans = ADD1;
                else if( type == "sub" )temp->effects.trans = SUB;
                else if( type == "addalpha" )temp->effects.trans = ADDALPHA;
            } else if (*simple == "alpha"){
                *simple >> temp->effects.alphalow;
                *simple >> temp->effects.alphahigh;
            } else if (*simple == "mask"){
                *simple >> temp->mask;
            } else if (*simple == "tile"){
                try{
                    *simple >> temp->tilex;
                    Global::debug(2) << "Tile x is " << temp->tilex << endl;
                    *simple >> temp->tiley;
                    Global::debug(2) << "Tile y is " << temp->tiley << endl;
                } catch (const Ast::Exception & e){
                }
            } else if (*simple == "tilespacing"){
                try{
                    *simple >> temp->tilespacingx;
                    *simple >> temp->tilespacingy;
                } catch (const Ast::Exception & e){
                }
            } else if (*simple == "window"){
                MugenArea area;
                *simple >> area.x1;
                *simple >> area.y1;
                *simple >> area.x2;
                *simple >> area.y2;
                temp->window = area;
            } else if (*simple == "windowdelta"){
                *simple >> temp->windowdeltax;
                *simple >> temp->windowdeltay;
            } else if (*simple == "xscale"){
                // You should only use either xscale or width but not both  (According to kfm.def not sure what to do with width)
                *simple >> temp->xscaletop;
                *simple >> temp->xscalebot;
            } else if (*simple == "width"){
                // You should only use either xscale or width but not both  (According to kfm.def not sure what to do with width)
                /* 
                   Not sure
                 *content->getNext() >> temp->xscaletop;
                 *content->getNext() >> temp->xscalebot;
                 */
            } else if (*simple == "yscalestart"){
                *simple >> temp->yscalestart;
            } else if (*simple == "yscaledelta"){
                *simple >> temp->yscaledelta;
            } else if (*simple == "positionlink"){
                *simple >> temp->positionlink;
            } else if (*simple == "velocity"){
                try{
                    *simple >> temp->velocityx;
                    *simple >> temp->velocityy;
                } catch (const Ast::Exception & e){
                }
            } else if (*simple == "sin.x"){
                *simple >> temp->sinx_amp;
                try{
                    *simple >> temp->sinx_period;
                    *simple >> temp->sinx_offset;
                } catch (const Ast::Exception & e){
                }
            } else if (*simple == "sin.y"){
                *simple >> temp->siny_amp;
                try{
                    *simple >> temp->siny_period;
                    *simple >> temp->siny_offset;
                } catch (const Ast::Exception & e){
                }
            } else throw MugenException( "Unhandled option in BG " + head + " Section: " + simple->toString());
        }
    }
    Global::debug(2) << "Background " << temp->id << " has tilex " << temp->tilex << endl;
    // Do some fixups and necessary things
    if ( temp->groupNumber != -1 && temp->imageNumber != -1){
	temp->sprite = sprites[(unsigned int)temp->groupNumber][(unsigned int)temp->imageNumber];
    }
    
    return temp;
}
#endif

MugenAnimation *Mugen::Util::getAnimation(Ast::Section * section, const Mugen::SpriteMap &sprites ){
    MugenAnimation *animation = new MugenAnimation();

    /* see parser/air.peg */
    class Walker: public Ast::Walker{
    public:
        Walker(MugenAnimation * animation, const Mugen::SpriteMap & sprites):
        Ast::Walker(),
        animation(animation),
        sprites(sprites),
        clsn1Reset(false),
        clsn2Reset(false),
        setloop(false){
        }

        /* data */
        MugenAnimation * animation;
        const Mugen::SpriteMap & sprites;
        std::vector<MugenArea> clsn1Holder;
        std::vector<MugenArea> clsn2Holder;
        bool clsn1Reset;
        bool clsn2Reset;
        bool setloop;

        enum Expect{
            clsn1, clsn2
        };

        Expect expected;

        MugenSprite * getSprite(int group, int item){
            Mugen::SpriteMap::const_iterator map = sprites.find(group);
            if (map == sprites.end()){
                return 0;
            }
            
            const Mugen::GroupMap & groupMap = (*map).second;
            Mugen::GroupMap::const_iterator it = groupMap.find(item);
            if (it != groupMap.end()){
                return (*it).second;
            }
            return 0;
        }

        /* callbacks */
        virtual void onValueList(const Ast::ValueList & values){
            // This is the new frame
            MugenFrame *frame = new MugenFrame();
            frame->defenseCollision = clsn2Holder;
            frame->attackCollision = clsn1Holder;
            frame->loopstart = setloop;
            /* Get sprite details */
            int group, spriteNumber;
            // Need to get the parsed data and populate these above items
            values >> group >> spriteNumber >> frame->xoffset >> frame->yoffset >> frame->time;
            string flip;
            string blend;
            try{
                values >> flip >> blend;
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
                frame->effects.trans = ADD;

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
                frame->effects.trans = SUB;
            }

            // Add sprite
            frame->sprite = getSprite(group, spriteNumber);
            if (frame->sprite == 0){
                Global::debug(0) << "No sprite for group " << group << " number " << spriteNumber << endl;
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
            array >> area.x1 >> area.y1 >> area.x2 >> area.y2;
            Global::debug(2) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
            if (array == "clsn1"){
                if (expected == clsn2){
                    Global::debug(0) << "Expected clsn2 but read clsn1: " << array.toString() << endl;
                }
                clsn1Holder.push_back(area);
            } else if (array == "clsn2"){
                if (expected == clsn1){
                    Global::debug(0) << "Expected clsn1 but read clsn2: " << array.toString() << endl;
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
                simple >> boxes;
            } else if (simple == "clsn2default"){
                expected = clsn2;
                clsn2Holder.clear();
                clsn2Reset = false;
                int boxes;
                simple >> boxes;
            } else if (simple == "clsn1"){
                expected = clsn1;
                clsn1Holder.clear();
                clsn1Reset = true;
                int boxes;
                simple >> boxes;
            } else if (simple == "clsn2"){
                expected = clsn2;
                clsn2Holder.clear();
                clsn2Reset = true;
                int boxes;
                simple >> boxes;
            }
        }
    };

    Walker walker(animation, sprites);
    section->walk(walker);

    /* FIXME!! use regex to get the number */
    std::string head = section->getName();
    head.replace(0,13,"");
    int h;
    MugenItem(head) >> h;
    animation->setType(Mugen::AnimationType(h));
    Global::debug(1) << "Adding Animation 'Begin Action " << h << "' : '" << animation->getName(animation->getType()) << "'" << endl;
    return animation;
}

list<Ast::Section*>* Mugen::Util::parseAir(const string & filename){
    try{
        return (list<Ast::Section*>*) Mugen::Air::parse(filename);
    } catch (const Ast::Exception & e){
        throw MugenException(e.getReason());
    } catch (const Mugen::Air::ParseException & e){
        ostringstream out;
        out << "Could not parse " << filename << " because " << e.getReason();
        throw MugenException(out.str());
    }
}

list<Ast::Section*>* Mugen::Util::parseDef(const string & filename){
    try{
        return (list<Ast::Section*>*) Mugen::Def::parse(filename);
    } catch (const Ast::Exception & e){
        throw MugenException(e.getReason());
    } catch (const Mugen::Def::ParseException & e){
        ostringstream out;
        out << "Could not parse " << filename << " because " << e.getReason();
        throw MugenException(out.str());
    }
}

std::map<int, MugenAnimation *> Mugen::Util::loadAnimations(const Filesystem::AbsolutePath & filename, const SpriteMap sprites){
    Ast::AstParse parsed(parseAir(filename.path()));
    Global::debug(2, __FILE__) << "Parsing animations. Number of sections is " << parsed.getSections()->size() << endl;
    
    map<int, MugenAnimation*> animations;
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
        std::string head = section->getName();
        Global::debug(1, __FILE__) << "Animation section '" << head << "'" << endl;
	head = Mugen::Util::fixCase(head);
        int number;
        if (PaintownUtil::matchRegex(head, "begin action [0-9]+")){
            number = atoi(PaintownUtil::captureRegex(head, "begin action ([0-9]+)", 0).c_str());
            Global::debug(1, __FILE__) << "Parse animation " << number << endl;
            animations[number] = Mugen::Util::getAnimation(section, sprites);
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

const std::string Mugen::Util::probeDef(const Filesystem::AbsolutePath &file, const std::string &section, const std::string &search){
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed(parseDef(file.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + file.path() + " in" + diff.printTime("") << endl;
    
    std::string ourSection = fixCase(section);;
    std::string ourSearch = fixCase(search);;
   
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
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
                        simple >> result;
                        Global::debug(1) << "Found result: " << result << endl;
                    } 
                }
	    };
	    std::string result;
	    SearchWalker walk(ourSearch, result);
	    astSection->walk(walk);
	    
	    if (!result.empty()){
		Global::debug(1) << "Found '" << search << "' in Section '" << section << "' in Definition file '" << file.path() << "'" << endl;
		return result;
	    }
	}
    }
    // Couldn't find search item throw exception
    throw MugenException("Couldn't find '" + search + "' in Section '" + section + "' in Definition file '" + file.path() + "'");
}

/* clean this function up */
MugenSprite *Mugen::Util::probeSff(const Filesystem::AbsolutePath &file, int groupNumber, int spriteNumber, const Filesystem::AbsolutePath &actFile){
    SffReader reader(file, actFile);
    vector<MugenSprite*> sprites;
    while (reader.moreSprites()){
        try{
            MugenSprite * sprite = reader.readSprite();            
            // Check if this is the sprite we're after
	    Global::debug(1) << "Matching group: (" << groupNumber << ") with (" << sprite->getGroupNumber() << ") | sprite: (" << spriteNumber << ") with (" << sprite->getImageNumber() << ")" << endl;
	    if ((sprite->getGroupNumber() == groupNumber) && (sprite->getImageNumber() == spriteNumber)){
	        // we got it return it
                for (vector<MugenSprite*>::iterator it = sprites.begin(); it != sprites.end(); it++){
                    delete *it;
                }
                return sprite;
	    } else {
                sprites.push_back(sprite);
            }
        } catch (const MugenException & e){
            Global::debug(0) << e.getReason() << endl;
        }
    }
    for (vector<MugenSprite*>::iterator it = sprites.begin(); it != sprites.end(); it++){
        delete *it;
    }
    /* Not found */
    ostringstream out;
    out << "Could not find sprite " << groupNumber << ", " << spriteNumber << " in " << file.path();
    throw MugenException(out.str());
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
trans(NONE),
alphalow(255),
alphahigh(255),
mask(false),
facing(1),
vfacing(1),
scalex(1),
scaley(1){
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
    return *this;
}

Mugen::Effects::~Effects(){
}

Mugen::Element::Element():
ID(0),
layer(Background){
}

Mugen::Element::~Element(){
}
