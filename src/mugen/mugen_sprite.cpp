#include "util/bitmap.h"
#include <string.h>
#include "mugen_sprite.h"
#include "util/funcs.h"
#include "globals.h"

using namespace std;
using namespace Mugen;

MugenSprite::MugenSprite():
next(0),
location(0),
length(0),
reallength(0),
newlength(0),
x(0),
y(0),
groupNumber(0),
imageNumber(0),
prev(0),
samePalette(0),
pcx(NULL),
bitmap(0){
    //Nothing
}

MugenSprite::MugenSprite( const MugenSprite &copy ){
    this->next = copy.next;
    this->location = copy.location;
    this->length = copy.length;
    this->reallength = copy.reallength;
    this->newlength = copy.newlength;
    this->x = copy.x;
    this->y = copy.y;
    this->groupNumber = copy.groupNumber;
    this->imageNumber = copy.imageNumber;
    this->prev = copy.prev;
    this->samePalette = copy.samePalette;
    memcpy( this->comments, &copy.comments, sizeof(MugenSprite::comments) );
    this->pcx = new char[this->reallength];
    memcpy(this->pcx, &copy.pcx, this->reallength);
    if (copy.bitmap){
	this->bitmap = new Bitmap(*copy.bitmap);
    }
}

MugenSprite & MugenSprite::operator=( const MugenSprite &copy ){
    this->next = copy.next;
    this->location = copy.location;
    this->length = copy.length;
    this->reallength = copy.reallength;
    this->newlength = copy.newlength;
    this->x = copy.x;
    this->y = copy.y;
    this->groupNumber = copy.groupNumber;
    this->imageNumber = copy.imageNumber;
    this->prev = copy.prev;
    this->samePalette = copy.samePalette;
    memcpy( this->comments, &copy.comments, sizeof(MugenSprite::comments) );
    this->pcx = new char[this->reallength];
    memcpy(this->pcx, &copy.pcx, this->reallength);
    if (copy.bitmap){
	this->bitmap = new Bitmap(*copy.bitmap);
    }
    
    return *this;
}

bool MugenSprite::operator<( const MugenSprite &copy ){
    return ( (this->groupNumber < copy.groupNumber) && (this->imageNumber < copy.imageNumber) );
}

MugenSprite::~MugenSprite(){
    /* **FIXME** this needs fixing on determining pcx is empty or not */
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }
    if (bitmap){
	delete bitmap;
    }
}

// Set sprite info
void MugenSprite::read(std::ifstream & ifile, const int loc){
    // Go to next sprite
    ifile.seekg(loc, std::ios::beg);
    
    // set the sprite location
    this->location = loc;
    
    ifile.read((char *)&next, sizeof(unsigned long));
    ifile.read((char *)&length, sizeof(unsigned long));
    ifile.read((char *)&x, sizeof(short));
    ifile.read((char *)&y, sizeof(short));
    ifile.read((char *)&groupNumber, sizeof(unsigned short));
    ifile.read((char *)&imageNumber, sizeof(unsigned short));
    ifile.read((char *)&prev, sizeof(unsigned short));
    ifile.read((char *)&samePalette, sizeof(bool));
    ifile.read((char *)comments, sizeof(comments));
    newlength = reallength = next - loc - 32;
     
    // Last sprite
    if( next == 0 ) {
	if( samePalette ) newlength = reallength = length-768;
	else newlength = reallength = length;
    }
}
/*
void MugenSprite::render(const int xaxis, const int yaxis, Bitmap &where, const double scalex, const double scaley){
    const int placex = (xaxis - this->x );
    const int placey = (yaxis - this->y );
    
    // temp for scaling
    Bitmap modImage = Bitmap::temporaryBitmap(bitmap->getWidth() * scalex, bitmap->getHeight() * scaley);
    bitmap->Stretch(modImage);
    
    modImage.draw(placex, placey, where);
}

void MugenSprite::render(int facing, int vfacing, const int xaxis, const int yaxis, Bitmap &where, const double scalex, const double scaley){
    const int placex = (xaxis - this->x );
    const int placey = (yaxis - this->y );
    // temp for scaling
    Bitmap modImage = Bitmap::temporaryBitmap(bitmap->getWidth() * scalex, bitmap->getHeight() * scaley);
    bitmap->Stretch(modImage);
    
    if (!facing && vfacing){
	modImage.drawHFlip(placex + modImage.getWidth(), placey, where);
	
    } else if (!vfacing && facing){
	modImage.drawVFlip(placex, placey - modImage.getHeight(), where);
	
    } else if ( !vfacing && !facing ){
	modImage.drawHVFlip(placex + modImage.getWidth(), placey - modImage.getHeight(), where);
	
    } else{
	modImage.draw(placex, placey, where);
    }
}
*/
void MugenSprite::render(const int xaxis, const int yaxis, Bitmap &where, Mugen::Effects &effects){
    if (!bitmap){
	load();
    }
    
    // temp for scaling
    Bitmap modImage = Bitmap::temporaryBitmap(bitmap->getWidth() * effects.scalex, bitmap->getHeight() * effects.scaley);
    bitmap->Stretch(modImage);
    
    // This needs to be a switch trans = None, Add, Add1, Sub1, Addalpha
    switch( effects.trans ){
	case ADDALPHA:{
	    // Need to figure out blend correctly addalpha is given to two locations low and high ?
	    Bitmap::transBlender( 255, 255, 255, effects.alphalow );
	    draw(modImage,xaxis,yaxis,where,effects);
	    break;
	}
	case ADD:{
	    // this additive 100% I assume... not totally sure
	    Bitmap::addBlender( 255, 255, 255, 255 );
	    draw(modImage,xaxis,yaxis,where,effects);
	    break;
	}
	case ADD1:{
	    // 50%
	    Bitmap::addBlender( 128, 128, 128, 255 );
	    draw(modImage,xaxis,yaxis,where,effects);
	    break;
	}
	case SUB:{
	    // Shadow effect
	    Bitmap::multiplyBlender( 0, 0, 0, 128 );
	    draw(modImage,xaxis,yaxis,where,effects);
	    break;
	}
	case NONE:
	default:{
	    draw(modImage,xaxis,yaxis,where,effects);
	    break;
	}
    }
}

void MugenSprite::load(bool mask){
    if (!bitmap){
	bitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength),mask);
    }
}

void MugenSprite::reload(bool mask){
    if (bitmap){
	delete bitmap;
    }
    bitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength),mask);
}

// get bitmap
Bitmap *MugenSprite::getBitmap(){
    if (!bitmap){
	load();
    }
    return bitmap;
}
const int MugenSprite::getWidth(){
    if (!bitmap){
	load();
    }
    return bitmap->getWidth();
}
const int MugenSprite::getHeight(){
    if (!bitmap){
	load();
    }
    return bitmap->getHeight();
}

void MugenSprite::loadPCX(std::ifstream & ifile,bool islinked, bool useact, unsigned char palsave1[]){
    ifile.seekg(location + 32, ios::beg);
    if (samePalette){
	// Lets give it space for the palette
	Global::debug(1) << "This sprite is less that 768 or has a shared palette - Group: " << getGroupNumber() << " | Image: " << getImageNumber() << endl;
	newlength+= 768;
	pcx = new char[newlength];
    }
    else {
	pcx = new char[reallength];
    }
    ifile.read((char *)pcx, reallength); 
    if ( !islinked ){
	if ( !useact){
	    if ( samePalette ){
		memcpy( pcx + (reallength), palsave1, 768);
		Global::debug(1) << "Applying 1st palette to Sprite: " << imageNumber << " in Group: " << groupNumber << endl;
	    } else {
		memcpy( palsave1, pcx+(reallength)-768, 768);
	    }
	} else {
	    // Replace all palettes with the one supplied in act
	    if ( samePalette){
		memcpy( pcx + (reallength), palsave1, 768);
	    } else {
		if (!(groupNumber == 9000 && imageNumber == 1)){
		    memcpy( pcx + (reallength)-768, palsave1, 768);
		} 
	    }
	}
    }
}

void MugenSprite::draw(const Bitmap &bmp, const int xaxis, const int yaxis, const Bitmap &where, const Mugen::Effects &effects){
    const int placex = (xaxis - this->x );
    const int placey = (yaxis - this->y );
    
    if ( (effects.facing == -1) && (effects.vfacing == 1)){
	if (effects.trans != NONE){
	    bmp.drawTransHFlip(placex + bmp.getWidth(), placey, where);
	} else {
	    bmp.drawHFlip(placex + bmp.getWidth(), placey, where);
	}
	
    } else if ( (effects.vfacing == -1) && (effects.facing == 1)){
	if (effects.trans != NONE){
	    bmp.drawTransVFlip(placex, placey - bmp.getHeight(), where);
	} else {
	    bmp.drawVFlip(placex, placey - bmp.getHeight(), where);
	}
    } else if ( (effects.vfacing == -1) && (effects.facing == -1) ){
	if (effects.trans != NONE){
	    bmp.drawTransHVFlip(placex + bmp.getWidth(), placey - bmp.getHeight(), where);
	} else {
	    bmp.drawHVFlip(placex + bmp.getWidth(), placey - bmp.getHeight(), where);
	}
    } else{
	//if( effects.mask ){
	if (effects.trans != NONE){
	    bmp.drawTrans( placex,placey, where );
	} else {
	    bmp.draw( placex,placey, where );
	}
	//} else {
	//    bmp.Blit( placex, placey, where );
//	}
    }
}

