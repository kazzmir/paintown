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

#include "util/funcs.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_character.h"
#include "mugen_menu.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "globals.h"

#include "mugen_util.h"

using namespace std;

static int lowerCase( int c ){ return tolower( c );}

void Mugen::Util::fixCase( std::string &str ){
    transform( str.begin(), str.end(), str.begin(), lowerCase );
}

void Mugen::Util::removeSpaces( std::string &str ){
    if( str.find(' ') != std::string::npos ){
	Global::debug(1) << "Removing spaces from: " << str << endl;
	for( int i = str.size()-1; i>-1; --i){
	    if( str[i] == ' ' )str.erase( str.begin()+i );
	    else if( str[i] == '\t' )str.erase( str.begin()+i );
	}
    }
}

void Mugen::Util::invertSlashes( std::string &str ){
    if( str.find('\\') != std::string::npos ){
	for( int i = str.size()-1; i>-1; --i){
	    if( str[i] == '\\' )str[i] = '/';
	}
    }
}
/*
std:string getHeadDir( const std::string & dir ){
    return dir.substr( ( dir.find_lastof( '/' ) != std::string::npos ? dir.find_lastof( '/' ) : 0 ), lastslash,dir.size() );
}*/

std::string Mugen::Util::fixFileName( const std::string &dir, std::string str ){
    Global::debug(1) << "Current File: " << str << endl;
    // Temp fix until the lexer fixes this crap
    Mugen::Util::removeSpaces( str );
    // Fixes stupid windows users shit
    Mugen::Util::invertSlashes( str );
    // Lets check if we need to fix anything first
    if( ::Util::exists( dir + str ) == false ){
	Global::debug(1) << "Couldn't find file: " << str << endl;
	std::string returnString = "";
	std::vector< string > files = ::Util::getFiles( dir, "*" );
	Global::debug(1) << "Correcting file: " << str << ", in directory: "<< dir <<".\nGot " << files.size() << " files." << endl;
	for( unsigned int i = 0; i < files.size(); ++i ){
	    std::string temp = files[i].c_str();
	    Mugen::Util::fixCase( temp );
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


std::string Mugen::Util::stripDir( const std::string &str ){ 
    std::string temp = str;
    if( str.find( "/") != std::string::npos || str.find( "\\") != std::string::npos ){
	size_t rem = temp.find_last_of( "/" );
	if( rem != std::string::npos ){
	    return str.substr(rem+1,str.size());
	}
	rem = temp.find_last_of( "\\" );
	if( rem != std::string::npos ){
	    return str.substr(rem+1,str.size());
	}
    }
    return str; 
}


std::string Mugen::Util::getFileDir( const std::string &str ){
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
    Global::debug(1) << "Parsing string to ItemContent: " << opt << endl;
    for( unsigned int i = 0; i < opt.size(); ++i ){
	if( opt[i] == ';' )break;
	if( opt[i] == ' ' ){
	    if( !contentHolder.empty() ) *temp << contentHolder;
	    Global::debug(1) << "Got content: " << contentHolder << endl;
	    contentHolder = "";
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
    if( !contentHolder.empty() ){
	*temp << contentHolder;
	Global::debug(1) << "Got content: " << contentHolder << endl;
    }
    return temp;
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


bool Mugen::Util::readPalette(const string &filename, unsigned char *pal){
    unsigned char colorsave[3]; // rgb pal save
    FILE *act_file = fopen( filename.c_str(), "rb" );
    if( !act_file ){
	Global::debug(1) << "Unable to open ACT file: " << filename << endl;
	if( act_file )fclose( act_file );
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
	    Global::debug(1) << "Applying palette from ACT file: " << filename << endl;
	    if( act_file )fclose( act_file );
	    return true;
	}
	// 128-byte header + 768-byte palette + 0x0C byte = minimum 8-bit PCX file size.
	else if( ftell(act_file) < 897 ){ 
            Global::debug(1) << "File " << filename << " is not a valid palette file." << endl;
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
                    Global::debug(1) << "Applying palette from PCX file: " << filename << endl;
		    return true;
                }
                else{
                    Global::debug(1) << "File " << filename << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
		    if( act_file )fclose( act_file );
		    pal =0;
		    return false;
                }
            }
            // Add support for JASC and RIFF palette files later... 
            // Minimum JASC PAL size = 1,813 bytes (carriage returns are necessary). 
            else{
		Global::debug(1) << "File " << filename << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
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

void Mugen::Util::readSprites(const string & filename, const string & palette, map<unsigned int,map<unsigned int, MugenSprite *> > & sprites) throw (MugenException){
    /* 16 skips the header stuff */
    int location = 16;
    ifstream ifile;
    ifile.open(filename.c_str(), ios::binary);
    if (!ifile){
	throw MugenException("Could not open SFF file: " + filename);
    }
    
    // Lets get the filesize
    FILE *tempstream = fopen( filename.c_str(), "r" );
    fseek( tempstream, 0, SEEK_END );
    int filesize = ftell( tempstream );
    fclose( tempstream );
    

    /* Lets go ahead and skip the crap -> (Elecbyte signature and version)
     * start at the 16th byte
     */
    ifile.seekg(location,ios::beg);
    unsigned long totalGroups;
    unsigned long totalImages;
    unsigned long suboffset;
    unsigned long subhead;
    unsigned int sharedPal;
    
    /* this probably isn't endian safe.. */
    ifile.read((char *)&totalGroups, sizeof(unsigned long));
    ifile.read((char *)&totalImages, sizeof(unsigned long));
    ifile.read((char *)&suboffset, sizeof(unsigned long));
    ifile.read((char *)&subhead, sizeof(unsigned long));
    ifile.read((char *)&sharedPal, sizeof(bool));
    if( sharedPal && sharedPal != 1 )sharedPal = 0;
    location = suboffset;
    if( location < 512 || location > 2147482717 )location = 512;
    
    Global::debug(1) << "Got Total Groups: " << totalGroups << ", Total Images: " << totalImages << ", Next Location in file: " << location << endl;

    MugenSprite *spriteIndex[totalImages + 1];
    
    // Palette related
    int islinked = 0;
    bool useact = false;
    
    //unsigned char colorsave[3]; // rgb pal save
    unsigned char palsave1[768]; // First image palette
    
    // Load in first palette
    if (Mugen::Util::readPalette(palette,palsave1)){
	useact = true;
    }
    
    if( location < 512 || location > 2147482717 )location = 512;
    else location = suboffset;
    
    
    for (unsigned int i = 0; i < totalImages; ++i){
	if( location > filesize ){
	    throw MugenException("Error in SFF file: " + filename + ". Offset of image beyond the end of the file.");
	}
	
        MugenSprite * sprite = new MugenSprite();//readSprite(ifile, location);    
	sprite->read(ifile,location);
	
	if( sprite->getLength() == 0 ){ // Lets get the linked sprite
	    // This is linked
	    islinked = 1;
	    /* Lets check if this is a duplicate sprite if so copy it
	    * if prev is larger than index then this file is corrupt */
	    if( sprite->getPrevious() >= i ) throw MugenException("Error in SFF file: " + filename + ". Incorrect reference to sprite.");
	    if( sprite->getPrevious() > 32767 ) throw MugenException("Error in SFF file: " + filename + ". Reference too large in image.");
	    while( sprite->getLength() == 0 ){
		const MugenSprite *temp = spriteIndex[sprite->getPrevious()];
		if( !temp ) throw MugenException("Error in SFF file: " + filename + ". Referenced sprite is NULL.");
		
		// Seek to the location of the pcx data
		ifile.seekg(temp->getLocation() + 32, ios::beg);
		sprite->setLocation(temp->getLocation());
		sprite->setLength(temp->getNext() - temp->getLocation() - 32);
		
		if( (sprite->getPrevious() <= temp->getPrevious()) && ((sprite->getPrevious() != 0) || (i == 0)) && temp->getLength()==0 ) {
		    std::ostringstream st;
		    st << "Image " << i << "(" << sprite->getGroupNumber() << "," << sprite->getImageNumber() << ") : circular definition or forward linking. Aborting.\n"; 
		    throw MugenException( st.str() );
		}
		else{
		    if(sprite->getLength() == 0){
			sprite->setPrevious(temp->getPrevious());
		    } else {
			sprite->setNewLength(temp->getNext() - temp->getLocation() -32);
			sprite->setRealLength(sprite->getNewLength());
		    }
		}
		
		Global::debug(1) << "Referenced Sprite Location: " << temp->getLocation() << " | Group: " << temp->getGroupNumber() << " | Sprite: " << temp->getGroupNumber() << " | at index: " << sprite->getPrevious() << endl;
	    }
	}
	else islinked = 0;
	/*
	// Read in pcx header
	pcx_header pcxhead;
	ifile.read( (char *)&pcxhead.manufacturer, sizeof(unsigned char) );
	ifile.read( (char *)&pcxhead.version, sizeof(unsigned char) );
	ifile.read( (char *)&pcxhead.encoding, sizeof(unsigned char) );
	ifile.read( (char *)&pcxhead.BitsPerPixel, sizeof(unsigned char) );
	ifile.read( (char *)&pcxhead.Xmin, sizeof(short) );
	ifile.read( (char *)&pcxhead.Ymin, sizeof(short) );
	ifile.read( (char *)&pcxhead.Xmax, sizeof(short));
	ifile.read( (char *)&pcxhead.Ymax, sizeof(short) );
	ifile.read( (char *)&pcxhead.HDpi, sizeof(unsigned short));
	ifile.read( (char *)&pcxhead.VDpi, sizeof(unsigned short) );
	ifile.read( (char *)&pcxhead.colormap, sizeof(unsigned char) * 48);
	ifile.read( (char *)&pcxhead.reserved, sizeof(unsigned char) );
	ifile.read( (char *)&pcxhead.NPlanes, sizeof(unsigned char) );
	ifile.read( (char *)&pcxhead.BytesPerLine, sizeof(unsigned short));
	ifile.read( (char *)&pcxhead.PaletteInfo, sizeof(unsigned short) );
	ifile.read( (char *)&pcxhead.HscreenSize, sizeof(unsigned short));
	ifile.read( (char *)&pcxhead.VscreenSize, sizeof(unsigned short) );
	ifile.read( (char *)&pcxhead.filler, sizeof(char) * 54 );
	
	// Now read in the pcx
	// Seek to the location of the pcx data
	ifile.seekg(sprite->location + 32, ios::beg);
	if( sprite->samePalette ){
	    // Lets give it space for the palette
	    Global::debug(1) << "This sprite is less that 768 or has a shared palette - Group: " << sprite->groupNumber << " | Image: " << sprite->imageNumber << endl;
	    sprite->newlength += 768;
	    sprite->pcx = new char[sprite->newlength];
	}
	else sprite->pcx = new char[sprite->reallength];
	ifile.read((char *)sprite->pcx, sprite->reallength);
	
	if ( !islinked ){
	    if ( !useact){
		if ( sprite->samePalette ){
		    memcpy( sprite->pcx + (sprite->reallength), palsave1, 768);
		    Global::debug(1) << "Applying 1st palette to Sprite: " << sprite->imageNumber << " in Group: " << sprite->groupNumber << endl;
		} else {
		    memcpy( palsave1, sprite->pcx+(sprite->reallength)-768, 768);
		}
	    } else {
		// Replace all palettes with the one supplied in act
		if ( sprite->samePalette){
		    memcpy( sprite->pcx + (sprite->reallength), palsave1, 768);
		} else {
		    if (!(sprite->groupNumber == 9000 && sprite->imageNumber == 1)){
			memcpy( sprite->pcx + (sprite->reallength)-768, palsave1, 768);
		    } 
		}
	    }
	}
	*/
	
	sprite->loadPCX(ifile,islinked,useact,palsave1);
	
	// Add to our lists
	spriteIndex[i] = sprite;
	sprites[sprite->getGroupNumber()][sprite->getImageNumber()] = sprite;
	/*
	// Dump to file just so we can test the pcx in something else
	if( Global::getDebug() == 3 ){
	    std::ostringstream st;
	    st << "pcxdump/g" << sprite->getGroupNumber() << "i" << sprite->getImageNumber() << ".pcx";
	    FILE *pcx;
	    if( (pcx = fopen( st.str().c_str(), "wb" )) != NULL ){
		size_t bleh = fwrite( sprite->pcx, sprite->newlength, 1, pcx );
		bleh = bleh;
		fclose( pcx );
	    }
	}
	*/
	// Set the next file location
	location = sprite->getNext();
	
	if( !location ){
	    Global::debug(1) << "End of Sprites or File. Continuing...." << endl;
	    break;
	}
	
	Global::debug(1) << "Index: " << i << ", Location: " << sprite->getLocation()  << ", Next Sprite: "  << sprite->getNext() << ", Length: " << sprite->getRealLength() << ", x|y: " << sprite->getX() << "|" << sprite->getY() << ", Group|Image Number: " << sprite->getGroupNumber() << "|" << sprite->getImageNumber() << ", Prev: " << sprite->getPrevious() << ", Same Pal: " << sprite->getSamePalette() << ", Comments: " << sprite->getComments() << endl;
	
    }

    ifile.close();
    
    Global::debug(1) << "Got Total Sprites: " << totalImages << endl;
    
    //return sprites;
}

void Mugen::Util::readSounds(const string & filename, std::map<unsigned int,std::map<unsigned int, MugenSound *> > & sounds) throw (MugenException){
    /* 16 skips the header stuff */
    int location = 16;
    ifstream ifile;
    ifile.open(filename.c_str(), ios::binary);
    if (!ifile){
	throw MugenException("Could not open SND file: " + filename);
    }
    
    // Lets go ahead and skip the crap -> (Elecbyte signature and version) start at the 16th byte
    ifile.seekg(location,ios::beg);
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
	    
	    ifile.read( (char *)&temp->next, 4 );
	    ifile.read( (char *)&temp->length, 4 );
	    ifile.read( (char *)&temp->groupNumber, 4 );
	    ifile.read( (char *)&temp->sampleNumber, 4 );
	    temp->sample = new char[temp->length];
	    ifile.read( (char *)temp->sample, temp->length );
	    
	    // Set the next file location
	    location = temp->next;
	    
	    sounds[ temp->groupNumber ][ temp->sampleNumber ] = temp;
	}
    }
    
    ifile.close();
}

MugenBackground *Mugen::Util::getBackground( const unsigned long int &ticker, MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites ){
    MugenBackground *temp = new MugenBackground(ticker);
    std::string head = section->getHeader();
    head.replace(0,3,"");
    temp->name = head;
    Global::debug(1) << "Found background: " << temp->name << endl;
    while( section->hasItems() ){
	MugenItemContent *content = section->getNext();
	const MugenItem *item = content->getNext();
	std::string itemhead = item->query();
	Mugen::Util::removeSpaces(itemhead);
	Mugen::Util::fixCase(itemhead);
	Global::debug(1) << "Getting next item: " << itemhead << endl;
	if ( itemhead.find("type")!=std::string::npos ){
	    std::string type;
	    *content->getNext() >> type;
	    Mugen::Util::removeSpaces( type );
	    if( type == "normal" )temp->type = Normal;
	    else if( type == "anim" )temp->type = Anim;
	    else if( type == "parallax" )temp->type = Parallax;
	    else if( type == "dummy" )temp->type = Dummy;
	} else if (itemhead == "spriteno"){
	    if (temp->type != Anim){
		*content->getNext() >> temp->groupNumber;
		*content->getNext() >> temp->imageNumber;
	    }
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
	    Mugen::Util::removeSpaces( type );
	    if( type == "none" )temp->effects->trans = NONE;
	    else if( type == "add" )temp->effects->trans =  ADD;
	    else if( type == "add1" )temp->effects->trans = ADD1;
	    else if( type == "sub" )temp->effects->trans = SUB;
	    else if( type == "addalpha" )temp->effects->trans = ADDALPHA;
	} else if (itemhead == "alpha"){
	    *content->getNext() >> temp->effects->alphalow;
	    *content->getNext() >> temp->effects->alphahigh;
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
	    // You should only use either xscale or width but not both  (According to kfm.def not sure what to do with width)
	    *content->getNext() >> temp->xscaletop;
	    *content->getNext() >> temp->xscalebot;
	} else if (itemhead == "width"){
	    // You should only use either xscale or width but not both  (According to kfm.def not sure what to do with width)
	   /* 
	   Not sure
		*content->getNext() >> temp->xscaletop;
		*content->getNext() >> temp->xscalebot;
	    */
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
    if ( temp->groupNumber != -1 && temp->imageNumber != -1){
	temp->sprite = sprites[(unsigned int)temp->groupNumber][(unsigned int)temp->imageNumber];
    }
    
    return temp;
}

MugenAnimation *Mugen::Util::getAnimation( MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites ){
    MugenAnimation *animation = new MugenAnimation();
    std::string head = section->getHeader();
    head.replace(0,13,"");
    std::vector<MugenArea> clsn1Holder;
    std::vector<MugenArea> clsn2Holder;
    bool clsn1Reset = false;
    bool clsn2Reset = false;
    bool setloop = false;
    while( section->hasItems() ){
	    MugenItemContent *content = section->getNext();
	    MugenItem *item = content->getNext();
	    std::string itemhead = item->query();
	    Mugen::Util::fixCase(itemhead);
	    Global::debug(1) << "Item Head: " << itemhead << endl;
	    // Attack boxes
	    if( itemhead.find("clsn1default") != std::string::npos ){
		// Get number
		int num;
		*content->getNext() >> num;
		clsn1Holder.clear();
		for( int n = 0; n < num; ++n ){
		    content = section->getNext();
		    Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
		    // Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
		    MugenArea area;
		    *content->getNext() >> area.x1;
		    *content->getNext() >> area.y1;
		    *content->getNext() >> area.x2;
		    *content->getNext() >> area.y2;
		    Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
		    clsn1Holder.push_back(area);
		}
	    }
	    // defend boxes
	    else if( itemhead.find("clsn2default") != std::string::npos ){
		// Get number
		int num;
		*content->getNext() >> num;
		clsn2Holder.clear();
		for( int n = 0; n < num; ++n ){
		    content = section->getNext();
		    Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
		    // Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
		    MugenArea area;
		    *content->getNext() >> area.x1;
		    *content->getNext() >> area.y1;
		    *content->getNext() >> area.x2;
		    *content->getNext() >> area.y2;
		    Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
		    clsn2Holder.push_back(area);
		}
	    }
	    // defend boxes
	    else if( itemhead.find("clsn2") != std::string::npos ){
		clsn2Reset = true;
		// Get number
		int num;
		*content->getNext() >> num;
		clsn2Holder.clear();
		for( int n = 0; n < num; ++n ){
		    content = section->getNext();
		    Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
		    // Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
		    MugenArea area;
		    *content->getNext() >> area.x1;
		    *content->getNext() >> area.y1;
		    *content->getNext() >> area.x2;
		    *content->getNext() >> area.y2;
		    Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
		    clsn2Holder.push_back(area);
		}
	    }
	    // Attack boxes
	    else if( itemhead.find("clsn1") != std::string::npos ){
		clsn1Reset = true;
		// Get number
		int num;
		*content->getNext() >> num;
		clsn1Holder.clear();
		for( int n = 0; n < num; ++n ){
		    content = section->getNext();
		    Global::debug(1) << "Beginning to get frames for: " << content->getNext()->query() << endl;
		    // Need to set the coordinates in MugenArea and add it to list x1,y1,x2,y2
		    MugenArea area;
		    *content->getNext() >> area.x1;
		    *content->getNext() >> area.y1;
		    *content->getNext() >> area.x2;
		    *content->getNext() >> area.y2;
		    Global::debug(1) << "Got: x1: " << area.x1 << ", y1: "<< area.y1 << ", x2: "<< area.x2 << ", y2: "<< area.y2 << endl;
		    clsn1Holder.push_back(area);
		}
	    }
	    // defend boxes for this frame
	    else if( itemhead.find("loopstart") != std::string::npos ){
		setloop = true;
	    }
	    // This is where we get our frame
	    else if( itemhead.find("clsn") == std::string::npos ){
		// This is the new frame
		MugenFrame *frame = new MugenFrame();
		frame->defenseCollision = clsn2Holder;
		frame->attackCollision = clsn1Holder;
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
		while( content->hasItems() ){
		    std::string temp;
		    *content->getNext() >> temp;
		    Mugen::Util::fixCase(temp);
		    if( temp.find("h") != std::string::npos ){
			//frame->flipHorizontal = true;
			frame->effects->facing = -1;
		    }
		    if( temp.find("v") != std::string::npos ){
			//frame->flipVertical = true;
			frame->effects->vfacing = -1;
		    }
		    if (temp[0] == 'a'){
			frame->effects->trans = ADD;
			// Check if we have specified additions
			if (temp.size() > 2){
			    // Source
			    //frame->colorSource = atoi(temp.substr(2,4).c_str());
			    frame->effects->alphalow = atoi(temp.substr(2,4).c_str());
			    // Dest
			    //frame->colorDestination = atoi(temp.substr(6,8).c_str());
			    frame->effects->alphahigh = atoi(temp.substr(6,8).c_str());
			}
		    } else if (temp[0] == 's'){
			frame->effects->trans = SUB;
		    }
		}
		// Add sprite
		frame->sprite = sprites[(unsigned short)group][(unsigned short)spriteNumber];
		if (frame->sprite == 0){
		    Global::debug(0) << "No sprite for group " << group << " number " << spriteNumber << endl;
		}
		// Add frame
		animation->addFrame(frame);
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
    }
    int h;
    MugenItem(head) >> h;
    animation->setType(MugenAnimationType(h));
    Global::debug(1) << "Adding Animation 'Begin Action " << h << "' : '" << animation->getName(animation->getType()) << "'" << endl;
    return animation;
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

std::string Mugen::Util::getCorrectFileLocation( const std::string &dir, const std::string &file ){
    // First check initial location else it should be in the base dir
    std::string ourFile = file;
    Mugen::Util::removeSpaces(ourFile);
    if (::Util::exists(dir + ourFile) == true){
	Global::debug(1) << "No correction needed found File: " << dir + ourFile << endl;
	return dir + ourFile;
    } else {
	// Descend two levels.. if not good enough screw it it doesn't exist
	std::string tempDir = removeLastDir(dir);
	Global::debug(1) << "Going down one dir: " << tempDir + ourFile << endl;
	if (::Util::exists(tempDir + ourFile) == true){
	    Global::debug(1) << "Found File: " << tempDir + ourFile << endl;
	    return tempDir + ourFile;
	} 
	tempDir = removeLastDir(tempDir);
	Global::debug(1) << "Going down one more dir: " << tempDir + ourFile << endl;
	if (::Util::exists(tempDir + ourFile) == true){
	    Global::debug(1) << "Found File: " << tempDir + ourFile << endl;
	    return tempDir + ourFile;
	} 
    }
    Global::debug(1) << "No correction needed File: " << dir + ourFile << endl;
    return dir + ourFile;
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

