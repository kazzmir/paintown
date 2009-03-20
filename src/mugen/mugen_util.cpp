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
#include "mugen_background.h"
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


std::string MugenUtil::stripDir( const std::string &str ){ 
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


std::string MugenUtil::getFileDir( const std::string &str ){
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
MugenItemContent *MugenUtil::parseOpt( const std::string &opt ){
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

// Check for Clsn
static bool checkClsn( const std::string &clsn ){
  return ( clsn.find("Clsn") != std::string::npos );
}

enum SearchState{
    Section = 0,
    ContentGet
};

std::vector< MugenSection * > MugenUtil::configReader(  const std::vector<std::string> &configdata ){
   
    std::vector< MugenSection * > collection;
   std::string line;
  
  const char openbracket = '[';
  const char closebracket = ']';
  const char comment = ';';
  const char seperator = ',';
  const char colon = ':';
  const char equal = '=';
  const char quote = '"';
  const char * ignored = "\r\n";
  
  SearchState state = Section;
  // Our section and items
  MugenSection * sectionHolder = 0;
  // Marker for areas
  bool beginSection = false;
      
  for( std::vector<std::string>::const_iterator i = configdata.begin(); i != configdata.end(); ++i ){
      line = *i;
      // Use to hold section or content in the options
      std::string contentHolder = "";
      // Place holder to put back all the grabbed content
      MugenItemContent *itemHolder = new MugenItemContent();
      // Needed to kill loop
      bool breakLoop = false;
      bool inQuote = false;
      bool getExpression = false;
      
      for( unsigned int i = 0; i < line.size(); ++i ){
	    // Go to work
	    switch( state ){
	      case Section:{
		// Done with this
		if( line[i] == comment ){
		  breakLoop = true;
		  break;
		}
		else if ( line[i] == ' ' && !beginSection )continue;
		//Start grabbing our section
		else if( line[i] == openbracket){
		  beginSection = true;
		}
		//End of our section store and go to ContentGet
		else if( line[i] == closebracket){
		  sectionHolder = new MugenSection();
		  sectionHolder->setHeader( contentHolder );
		  state = ContentGet;
		  beginSection = false;
		  breakLoop = true;
		  break;
		}
		else if( beginSection )contentHolder += line[i];
	      }
	      break;
	      case ContentGet:
	      default:{
		// Done with this
		if( line[i] == comment ){
		  if( itemHolder->hasItems() ){
		    if( !contentHolder.empty() ){
		      *itemHolder << contentHolder;
		    }
		    *sectionHolder << itemHolder;
		  }
		  breakLoop = true;
		  break;
		}
		// Check if we are near the end to kill it
		if( i+1 == line.size() && !contentHolder.empty() ){
		    if (line[i] != ' ' && !strchr(ignored, line[i])){
                        contentHolder += line[i];
                    }
		    *itemHolder << contentHolder;
		    *sectionHolder << itemHolder;
		    breakLoop = true;
		    break;
		}
		// Buh bye spaces
		if( line[i] == ' ' && !inQuote ){
		  continue;
		}
		// Start getting expression
		if( line[i] != ' ' && getExpression ){
		  inQuote = true;
		}
		// Check if this section is done, push it on the stack and reset everything
		if( line[i] == openbracket && !checkClsn( contentHolder ) && !itemHolder->hasItems() ){
		  if( sectionHolder->hasItems() )collection.push_back( sectionHolder );
		  beginSection = true;
		  state = Section;
		  contentHolder = "";
		  break;
		}
		// We got one push back the other and reset the holder to get the next
		else if( line[i] == colon || line[i] == seperator ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  contentHolder = "";
		}
		// Equal
		else if(line[i] == equal && !inQuote ){
		  if( !contentHolder.empty() ) *itemHolder << contentHolder;
		  getExpression = true;
		  contentHolder = "";
		}
		// We has a quote begin or end it
		else if( line[i] == quote ){
		  inQuote = !inQuote;
		}
		//Start grabbing our item
		else if (! strchr(ignored, line[i])){
                    contentHolder += line[i];
                }
	      }
	      break;
	    }
	    if( breakLoop )break;
      }
  }
  // Add in last section
  if( sectionHolder->hasItems() )collection.push_back( sectionHolder );
  
  return collection;
    
}

MugenBackground *MugenUtil::getBackground( const unsigned long int &ticker, MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites ){
    MugenBackground *temp = new MugenBackground(ticker);
    std::string head = section->getHeader();
    head.replace(0,3,"");
    temp->name = head;
    Global::debug(1) << "Found background: " << temp->name << endl;
    while( section->hasItems() ){
	MugenItemContent *content = section->getNext();
	const MugenItem *item = content->getNext();
	std::string itemhead = item->query();
	MugenUtil::removeSpaces(itemhead);
	MugenUtil::fixCase(itemhead);
	Global::debug(1) << "Getting next item: " << itemhead << endl;
	if ( itemhead.find("type")!=std::string::npos ){
	    std::string type;
	    *content->getNext() >> type;
	    MugenUtil::removeSpaces( type );
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
	    MugenUtil::removeSpaces( type );
	    if( type == "none" )temp->trans = None;
	    else if( type == "add" )temp->trans = Add;
	    else if( type == "add1" )temp->trans = Add1;
	    else if( type == "sub" )temp->trans = Sub;
	    else if( type == "addalpha" )temp->trans = Addalpha;
	} else if (itemhead == "alpha"){
	    *content->getNext() >> temp->alphalow;
	    *content->getNext() >> temp->alphahigh;
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

MugenAnimation *MugenUtil::getAnimation( MugenSection *section, std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites ){
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
	    Global::debug(1) << "Item Head: " << itemhead << endl;
	    // Attack boxes
	    if( itemhead.find("Clsn1Default") != std::string::npos ){
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
	    else if( itemhead.find("Clsn2Default") != std::string::npos ){
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
	    else if( itemhead.find("Clsn2") != std::string::npos ){
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
	    else if( itemhead.find("Clsn1") != std::string::npos ){
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
	    else if( itemhead.find("Clsn") == std::string::npos ){
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
		    MugenUtil::fixCase(temp);
		    if( temp.find("h") != std::string::npos )frame->flipHorizontal = true;
		    if( temp.find("v") != std::string::npos )frame->flipVertical = true;
		    if (temp[0] == 'a'){
			frame->colorAdd = ADD;
			// Check if we have specified additions
			if (temp.size() > 2){
			    // Source
			    frame->colorSource = atoi(temp.substr(2,4).c_str());
			    // Dest
			    frame->colorDestination = atoi(temp.substr(6,8).c_str());
			}
		    } else if (temp[0] == 's'){
			frame->colorAdd = SUB;
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

std::string MugenUtil::getCorrectFileLocation( const std::string &dir, const std::string &file ){
    // First check initial location else it should be in the base dir
    std::string ourFile = file;
    MugenUtil::removeSpaces(ourFile);
    if (Util::exists(dir + ourFile) == true){
	Global::debug(1) << "No correction needed found File: " << dir + ourFile << endl;
	return dir + ourFile;
    } else {
	// Descend two levels.. if not good enough screw it it doesn't exist
	std::string tempDir = removeLastDir(dir);
	Global::debug(1) << "Going down one dir: " << tempDir + ourFile << endl;
	if (Util::exists(tempDir + ourFile) == true){
	    Global::debug(1) << "Found File: " << tempDir + ourFile << endl;
	    return tempDir + ourFile;
	} 
	tempDir = removeLastDir(tempDir);
	Global::debug(1) << "Going down one more dir: " << tempDir + ourFile << endl;
	if (Util::exists(tempDir + ourFile) == true){
	    Global::debug(1) << "Found File: " << tempDir + ourFile << endl;
	    return tempDir + ourFile;
	} 
    }
    Global::debug(1) << "No correction needed File: " << dir + ourFile << endl;
    return dir + ourFile;
}

