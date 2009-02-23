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

#include "mugen_util.h"


static int lowerCase( int c ){ return tolower( c );}

void MugenUtil::fixCase( std::string &str ){
    transform( str.begin(), str.end(), str.begin(), lowerCase );
}

void MugenUtil::removeSpaces( std::string &str ){
    if( str.find(' ') != std::string::npos ){
	Global::debug(1) << "Removing spaces from: " << str << endl;
	for( int i = str.size()-1; i>-1; --i){
	    if( str[i] == ' ' )str.erase( str.begin()+i );
	    else if( str[i] == '\t' )str.erase( str.begin()+i );
	}
    }
}

void MugenUtil::invertSlashes( std::string &str ){
    if( str.find('\\') != std::string::npos ){
	for( int i = str.size()-1; i>-1; --i){
	    if( str[i] == '\\' )str[i] = '/';
	}
    }
}
/*
std:string MugenUtil::getHeadDir( const std::string & dir ){
    return dir.substr( ( dir.find_lastof( '/' ) != std::string::npos ? dir.find_lastof( '/' ) : 0 ), lastslash,dir.size() );
}*/

std::string MugenUtil::fixFileName( const std::string &dir, std::string str ){
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
MugenItemContent *MugenUtil::parseOpt( const std::string &opt ){
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

// Get next sprite
static MugenSprite * readSprite(ifstream & ifile, int & location){
    // Go to next sprite
    ifile.seekg(location, ios::beg);
    // next sprite
    MugenSprite *temp = new MugenSprite();
    // set the sprite location
    temp->location = location;
    
    ifile.read((char *)&temp->next, sizeof(unsigned long));
    ifile.read((char *)&temp->length, sizeof(unsigned long));
    ifile.read((char *)&temp->x, sizeof(short));
    ifile.read((char *)&temp->y, sizeof(short));
    ifile.read((char *)&temp->groupNumber, sizeof(unsigned short));
    ifile.read((char *)&temp->imageNumber, sizeof(unsigned short));
    ifile.read((char *)&temp->prev, sizeof(unsigned short));
    ifile.read((char *)&temp->samePalette, sizeof(bool));
    ifile.read((char *)temp->comments, sizeof(temp->comments));
    temp->newlength = temp->reallength = temp->next - temp->location - 32;
     
    // Last sprite
    if( temp->next == 0 ) {
	if( temp->samePalette ) temp->newlength = temp->reallength = temp->length-768;
	else temp->newlength = temp->reallength = temp->length;
    }
    
    return temp;
}

void MugenUtil::readSprites(const string & filename, const string & palette, map<unsigned int,map<unsigned int, MugenSprite *> > & sprites) throw (MugenException){
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
    bool useact = true;
    
    unsigned char colorsave[3]; // rgb pal save
    unsigned char palsave1[768]; // First image palette
    
    // Load in first palette

    FILE *act_file = fopen( palette.c_str(), "rb" );
    if( !act_file ){
	Global::debug(1) << "Unable to open ACT file: " << palette << endl;
	useact = false;
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
		palsave1[3*i]=colorsave[0];
		palsave1[3*i+1]=colorsave[1];
		palsave1[3*i+2]=colorsave[2];
	    }
	    Global::debug(1) << "Applying palette from ACT file: " << palette << endl;
	}
	// 128-byte header + 768-byte palette + 0x0C byte = minimum 8-bit PCX file size.
	else if( ftell(act_file) < 897 ){ 
            useact = false;
            Global::debug(1) << "File " << palette << " is not a valid palette file." << endl;
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
                    bleh = fread( palsave1, 768, 1, act_file );
                    Global::debug(1) << "Applying palette from PCX file: " << palette << endl;
                }
                else{
                    useact = false;
                    Global::debug(1) << "File " << palette << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
                }
            }
            // Add support for JASC and RIFF palette files later... 
            // Minimum JASC PAL size = 1,813 bytes (carriage returns are necessary). 
            else{
                useact = false;
		Global::debug(1) << "File " << palette << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
            }
        }
    }
    if( act_file )fclose( act_file );
    
    if( location < 512 || location > 2147482717 )location = 512;
    else location = suboffset;
    
    
    for (unsigned int i = 0; i < totalImages; ++i){
	if( location > filesize ){
	    throw MugenException("Error in SFF file: " + filename + ". Offset of image beyond the end of the file.");
	}
	
        MugenSprite * sprite = readSprite(ifile, location);    
	
	if( sprite->length == 0 ){ // Lets get the linked sprite
	    // This is linked
	    islinked = 1;
	    /* Lets check if this is a duplicate sprite if so copy it
	    * if prev is larger than index then this file is corrupt */
	    if( sprite->prev >= i ) throw MugenException("Error in SFF file: " + filename + ". Incorrect reference to sprite.");
	    if( sprite->prev > 32767 ) throw MugenException("Error in SFF file: " + filename + ". Reference too large in image.");
	    while( sprite->length == 0 ){
		const MugenSprite *temp = spriteIndex[sprite->prev];
		if( !temp ) throw MugenException("Error in SFF file: " + filename + ". Referenced sprite is NULL.");
		
		// Seek to the location of the pcx data
		ifile.seekg(temp->location + 32, ios::beg);
		sprite->location = temp->location;
		sprite->length = temp->next - temp->location - 32;
		
		if( (sprite->prev <= temp->prev) && ((sprite->prev != 0) || (i == 0)) && temp->length==0 ) {
		    std::ostringstream st;
		    st << "Image " << i << "(" << sprite->groupNumber << "," << sprite->imageNumber << ") : circular definition or forward linking. Aborting.\n"; 
		    throw MugenException( st.str() );
		}
		else{
		    if(sprite->length == 0) sprite->prev = temp->prev;
		    else sprite->newlength = sprite->reallength = temp->next - temp->location -32;
		}
		
		Global::debug(1) << "Referenced Sprite Location: " << temp->location << " | Group: " << temp->groupNumber << " | Sprite: " << temp->groupNumber << " | at index: " << sprite->prev << endl;
	    }
	}
	else islinked = 0;
	
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
		    memcpy( sprite->pcx + (sprite->reallength)-768, palsave1, 768);
		}
	    }
	}
	    
	
	// Add to our lists
	spriteIndex[i] = sprite;
	sprites[sprite->groupNumber][sprite->imageNumber] = sprite;
	
	// Dump to file just so we can test the pcx in something else
	if( Global::getDebug() == 3 ){
	    std::ostringstream st;
	    st << "pcxdump/g" << sprite->groupNumber << "i" << sprite->imageNumber << ".pcx";
	    FILE *pcx;
	    if( (pcx = fopen( st.str().c_str(), "wb" )) != NULL ){
		size_t bleh = fwrite( sprite->pcx, sprite->newlength, 1, pcx );
		bleh = bleh;
		fclose( pcx );
	    }
	}
	
	// Set the next file location
	location = sprite->next;
	
	if( !location ){
	    Global::debug(1) << "End of Sprites or File. Continuing...." << endl;
	    break;
	}
	
	Global::debug(1) << "Index: " << i << ", Location: " << sprite->location  << ", Next Sprite: "  << sprite->next << ", Length: " << sprite->reallength << ", x|y: " << sprite->x << "|" << sprite->y << ", Group|Image Number: " << sprite->groupNumber << "|" << sprite->imageNumber << ", Prev: " << sprite->prev << ", Same Pal: " << sprite->samePalette << ", Comments: " << sprite->comments << endl;
	
    }

    ifile.close();
    
    Global::debug(1) << "Got Total Sprites: " << totalImages << endl;
    
    //return sprites;
}

void MugenUtil::readSounds(const string & filename, std::map<unsigned int,std::map<unsigned int, MugenSound *> > & sounds) throw (MugenException){
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



