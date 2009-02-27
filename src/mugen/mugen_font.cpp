#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

#include "mugen_font.h"

#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_reader.h"
#include "mugen_util.h"
#include "globals.h"


MugenFont::MugenFont( const string & file ):
type(Fixed),
width(0),
height(0),
spacingx(0),
spacingy(0),
colors(0),
offsetx(0),
offsety(0),
bmp(0){
    Global::debug(1) << "[mugen font] Opening file '" << file << "'" << endl;
    ifile.open( file.c_str() );
    if (!ifile){
        perror("cant open file");
    }
    myfile = file;
    
    load();
}

MugenFont::MugenFont( const char * file ):
type(Fixed),
width(0),
height(0),
spacingx(0),
spacingy(0),
colors(0),
offsetx(0),
offsety(0),
bmp(0){
    Global::debug(1) << "[mugen font] Opening file '" << file << "'" << endl;
    ifile.open( file );
    if (!ifile){
        perror("cant open file");
    }
    myfile = string( file );
    
    load();
}

MugenFont::MugenFont( const MugenFont &copy ){
    this->type = copy.type;
    this->width = copy.width;
    this->height = copy.height;
    this->spacingx = copy.spacingx;
    this->spacingy = copy.spacingy;
    this->colors = copy.colors;
    this->offsetx = copy.offsetx;
    this->offsety = copy.offsety;
    this->bmp = copy.bmp;
}

MugenFont::~MugenFont(){
    if(bmp){
	delete bmp;
    }
}
    
MugenFont & MugenFont::operator=( const MugenFont &copy ){
    this->type = copy.type;
    this->width = copy.width;
    this->height = copy.height;
    this->spacingx = copy.spacingx;
    this->spacingy = copy.spacingy;
    this->colors = copy.colors;
    this->offsetx = copy.offsetx;
    this->offsety = copy.offsety;
    this->bmp = copy.bmp;
    
    return *this;
}

// Implement Font stuff
void MugenFont::setSize( const int x, const int y ){
    // We don't change sizes
    if  ( (x < 0 || y < 0) && type!=Fixed ){
	return;
    }
}
const int MugenFont::getSizeX() const{
    return width;
}
const int MugenFont::getSizeY() const{
    return height;
}

const int MugenFont::textLength( const char * text ) const{
    std::string str(text);
    int size =0;
    for (unsigned int i = 0; i < str.size(); ++i){
	std::map<char, FontLocation>::const_iterator loc = positions.find(str[i]);
	if (loc!=positions.end()){
	   size+=loc->second.width + spacingx;
	} else{
	    // Couldn't find a position for this character assume regular width and skip to the next character
	    size+=width + spacingx;
	}
    }
    return size;
}

const int MugenFont::getHeight( const string & str ) const{
    // What? I guess this is for freetype?
    return getHeight();
}

const int MugenFont::getHeight() const{
    return height;
}

void MugenFont::printf( int x, int y, int color, const Bitmap & work, const string & str, int marker, ... ) const{
    // need to get VA List later and append it to str or whatever
    int offsetx = 0;
    for (unsigned int i = 0; i < str.size(); ++i){
	std::map<char, FontLocation>::const_iterator loc = positions.find(str[i]);
	if (loc!=positions.end()){
	    Bitmap character = Bitmap::temporaryBitmap( loc->second.width, height);
	    bmp->Stretch( character, loc->second.startx + offsetx, offsety, loc->second.width, height, x + offsetx, y, width, height );
	    character.draw( x + offsetx, y, work);
	    offsetx+=loc->second.width + spacingx;
	} else{
	    // Couldn't find a position for this character draw nothing, assume width, and skip to the next character
	    offsetx+=width + spacingx;
	}
    }
}

void MugenFont::load(){
    
    /* 16 skips the header stuff */
    int location = 16;
    // Lets go ahead and skip the crap -> (Elecbyte signature and version) start at the 16th byte
    ifile.seekg(location,ios::beg);
    unsigned long pcxlocation;
    unsigned long pcxsize;
    unsigned long txtlocation;
    unsigned long txtsize;
    unsigned char *pcx;
    
    ifile.read( (char *)&pcxlocation, sizeof(unsigned long) );
    ifile.read( (char *)&pcxsize, sizeof(unsigned long) );
    ifile.read( (char *)&txtlocation, sizeof(unsigned long) );
    ifile.read( (char *)&txtsize, sizeof(unsigned long) );
    Global::debug(1) << "PCX Location: " << pcxlocation << " | PCX Size: " << pcxsize << endl;
    Global::debug(1) << "TXT Location: " << txtlocation << " | TXT Actual location: " << pcxlocation + pcxsize << " | TXT Size: " << txtsize << endl;
    
    // Get the pcx load our bitmap
    ifile.seekg(pcxlocation,ios::beg);
    pcx = new unsigned char[pcxsize];
    ifile.read((char *)&pcx, pcxsize);
    
    bmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, pcxsize));
    
    delete [] pcx;
    
    // Get the text
    ifile.seekg(pcxlocation+pcxsize, ios::beg);
    std::vector<std::string> ourText;
    while( !ifile.eof() ){
	std::string line;
	getline( ifile, line );
	ourText.push_back(line);
    }
    
    std::vector< MugenSection * > collection = MugenUtil::configReader( ourText );
    
    for( unsigned int i = 0; i < collection.size(); ++i ){
	const std::string &head = collection[i]->getHeader();
	if( head == "Def" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		// This is so we don't have any problems with crap like Name, NaMe, naMe or whatever
		MugenUtil::fixCase( itemhead );
		if ( itemhead.find("size")!=std::string::npos ){
		    *content->getNext() >> width;
		    *content->getNext() >> height;
		} else if ( itemhead.find("spacing")!=std::string::npos ){
		    *content->getNext() >> spacingx;
		    *content->getNext() >> spacingy;
		} else if ( itemhead.find("colors")!=std::string::npos ){
		    *content->getNext() >> colors;
		} else if ( itemhead.find("offset")!=std::string::npos ){
		    *content->getNext() >> offsetx;
		    *content->getNext() >> offsety;
		} else if ( itemhead.find("type")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
		    MugenUtil::removeSpaces(temp);
		    MugenUtil::fixCase(temp);
		    if (temp == "variable")type = Variable;
		    else if (temp == "fixed")type = Fixed;
		} //else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	if( head == "Map" ){
	    int locationx = 0;
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		// We go ahead and map everything else
		int character;
		int startx = locationx;
		int chrwidth = width;
		*content->getNext() >> character;
		if (type != Fixed){
		    *content->getNext() >> startx;
		    *content->getNext() >> width;
		}
		FontLocation loc;
		loc.startx = startx;
		loc.width = chrwidth;
		positions[character] = loc;
	    }
	}
    }
    
    // Time to get rid of collection
    for( std::vector< MugenSection * >::iterator i = collection.begin() ; i != collection.end() ; ++i ){
        if(*i)delete (*i);
    }
    
    ifile.close();
    
}
    
  
  