#include "util/bitmap.h"
#include <string.h>
#include "mugen/sprite.h"
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

    if (copy.comments != 0){
        /* this line is right */
        memcpy(this->comments, copy.comments, sizeof(MugenSprite::comments));
    }

    /* why do we need to copy the pcx data if we already have the bitmap? */
    if (copy.pcx != 0){
        this->pcx = new char[this->reallength];
        /* this line is right */
        memcpy(this->pcx, copy.pcx, this->reallength);
    } else {
        this->pcx = 0;
    }

    if (copy.bitmap){
	this->bitmap = new Bitmap(*copy.bitmap);
    } else {
        this->bitmap = 0;
    }
}

MugenSprite & MugenSprite::operator=( const MugenSprite &copy ){
    cleanup();

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
    if (copy.comments){
        memcpy( this->comments, copy.comments, sizeof(MugenSprite::comments) );
    }

    if (copy.pcx){
        this->pcx = new char[this->reallength];
        memcpy(this->pcx, copy.pcx, this->reallength);
    }

    if (copy.bitmap){
	this->bitmap = new Bitmap(*copy.bitmap);
    }
    
    return *this;
}
        
void MugenSprite::copyImage(const MugenSprite * copy){
    if (bitmap){
        delete bitmap;
        bitmap = NULL;
    }

    if (copy->getBitmap() != NULL){
        bitmap = new Bitmap(*copy->getBitmap());
    }
}

bool MugenSprite::operator<( const MugenSprite &copy ){
    return ( (this->groupNumber < copy.groupNumber) && (this->imageNumber < copy.imageNumber) );
}

void MugenSprite::cleanup(){
    /* **FIXME** this needs fixing on determining pcx is empty or not */
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }

    if (bitmap){
	delete bitmap;
        bitmap = NULL;
    }
}

MugenSprite::~MugenSprite(){
    cleanup();
}

// Set sprite info
void MugenSprite::read(std::ifstream & ifile, const int loc){
    // Go to next sprite
    ifile.seekg(loc, std::ios::beg);
    
    // set the sprite location
    this->location = loc;
    
    ifile.read((char *)&next, sizeof(next));
    ifile.read((char *)&length, sizeof(length));
    ifile.read((char *)&x, sizeof(x));
    ifile.read((char *)&y, sizeof(y));
    ifile.read((char *)&groupNumber, sizeof(groupNumber));
    ifile.read((char *)&imageNumber, sizeof(imageNumber));
    ifile.read((char *)&prev, sizeof(prev));
    ifile.read((char *)&samePalette, sizeof(samePalette));
    ifile.read((char *)comments, sizeof(comments));
    newlength = reallength = next - loc - 32;
     
    // Last sprite
    if (next == 0){
	if (samePalette){
            newlength = reallength = length - 768;
        } else {
            newlength = reallength = length;
        }
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
void MugenSprite::render(const int xaxis, const int yaxis, const Bitmap &where, const Mugen::Effects &effects){
    // temp for scaling
    Bitmap modImage = Bitmap::temporaryBitmap((int) (bitmap->getWidth() * effects.scalex), (int) (bitmap->getHeight() * effects.scaley));
    bitmap->Stretch(modImage);
    
    // This needs to be a switch trans = None, Add, Add1, Sub1, Addalpha
    switch (effects.trans){
	case ADDALPHA:{
	    // Need to figure out blend correctly addalpha is given to two locations low and high ?
	    Bitmap::transBlender(0, 0, 0, effects.alphalow);
	    draw(modImage, xaxis, yaxis, where, effects);
	    break;
	}
	case ADD:{
	    // this additive 100% I assume... not totally sure
	    // Bitmap::addBlender( 255, 255, 255, 255 );
	    Bitmap::addBlender(0, 0, 0, 255);
	    draw(modImage,xaxis,yaxis,where,effects);
	    break;
	}
	case ADD1:{
	    // 50%
	    Bitmap::addBlender(128, 128, 128, 255);
	    draw(modImage,xaxis,yaxis,where,effects);
	    break;
	}
	case SUB:{
	    // Shadow effect
	    // Bitmap::multiplyBlender( 0, 0, 0, 128 );
	    Bitmap::differenceBlender(0, 0, 0, 128);
	    draw(modImage, xaxis, yaxis, where, effects);
	    break;
	}
	case NONE:
	default:{
	    draw(modImage, xaxis, yaxis, where, effects);
	    break;
	}
    }
}

void MugenSprite::load(bool mask){
    if (!bitmap){
	bitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength), mask);
    }
}

void MugenSprite::reload(bool mask){
    if (bitmap){
	delete bitmap;
    }
    bitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength), mask);
}

Bitmap *MugenSprite::getBitmap() const {
    return bitmap;
}

const int MugenSprite::getWidth(){
    return bitmap->getWidth();
}

const int MugenSprite::getHeight(){
    return bitmap->getHeight();
}

void MugenSprite::loadPCX(std::ifstream & ifile, bool islinked, bool useact, unsigned char palsave1[]){
    /* TODO: 768 is littered everywhere, replace with a constant */
    ifile.seekg(location + 32, ios::beg);
    if (samePalette){
	// Lets give it space for the palette
	Global::debug(1) << "This sprite is less that 768 or has a shared palette - Group: " << getGroupNumber() << " | Image: " << getImageNumber() << endl;
	newlength += 768;
	pcx = new char[newlength];
        memset(pcx, 0, newlength);
    } else {
	pcx = new char[reallength];
        memset(pcx, 0, reallength);
    }
    ifile.read((char *)pcx, reallength); 
    if (!islinked){
	if (!useact){
	    if (samePalette){
		memcpy( pcx + (reallength), palsave1, 768);
		Global::debug(1) << "Applying 1st palette to Sprite: " << imageNumber << " in Group: " << groupNumber << endl;
	    } else {
		memcpy(palsave1, pcx+(reallength)-768, 768);
	    }
	} else {
	    // Replace all palettes with the one supplied in act
	    if ( samePalette){
		memcpy(pcx + (reallength), palsave1, 768);
	    } else {
                /* TODO: add an explanation of what group 9000 means here */
		if (!(groupNumber == 9000 && imageNumber == 1)){
		    memcpy(pcx + (reallength)-768, palsave1, 768);
		} 
	    }
	}
    }
    
    load();
}

void MugenSprite::draw(const Bitmap &bmp, const int xaxis, const int yaxis, const Bitmap &where, const Mugen::Effects &effects){
    int startWidth = 0;
    int startHeight = 0;
    int width = bmp.getWidth();
    int height = bmp.getHeight();

    if (effects.dimension.x1 != -1){
        startWidth = effects.dimension.x1;
    }

    if (effects.dimension.x2 != -1){
        width = effects.dimension.x2;
    }
    
    if (effects.dimension.y1 != -1){
        startHeight = effects.dimension.y1;
    }

    if (effects.dimension.y2 != -1){
        height = effects.dimension.y2;
    }

    const int normalX = (xaxis - this->x);
    const int normalY = (yaxis - this->y);
    const int flippedX = (xaxis - bmp.getWidth() + this->x);
    const int flippedY = (yaxis - bmp.getHeight() + this->y);

    const int FLIPPED = -1;
    const int NOT_FLIPPED = 1;

    if ( (effects.facing == FLIPPED) && (effects.vfacing == NOT_FLIPPED)){
	if (effects.trans != NONE){
	    bmp.drawTransHFlip(flippedX, normalY, where);
	} else {
	    // bmp.drawHFlip(placex + bmp.getWidth() / 2, placey, where);
	    bmp.drawHFlip(flippedX, normalY, startWidth, startHeight, width, height, where);
	}
	
    } else if ( (effects.vfacing == FLIPPED) && (effects.facing == NOT_FLIPPED)){
	if (effects.trans != NONE){
	    bmp.drawTransVFlip(normalX, flippedY, where);
	} else {
	    bmp.drawVFlip(normalX, flippedY, where);
	}
    } else if ((effects.vfacing == FLIPPED) && (effects.facing == FLIPPED)){
	if (effects.trans != NONE){
	    bmp.drawTransHVFlip(flippedX, flippedY, where);
	} else {
	    bmp.drawHVFlip(flippedX, flippedY, where);
	}
    } else {
	//if( effects.mask ){
	if (effects.trans != NONE){
	    bmp.drawTrans(normalX, normalY, where);
	} else {
	    bmp.draw(normalX, normalY, startWidth, startHeight, width, height, where);
	}
	//} else {
	//    bmp.Blit( placex, placey, where );
//	}
    }
}
