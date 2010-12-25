#include "util/bitmap.h"
#include <string.h>
#include "mugen/sprite.h"
#include "util/funcs.h"
#include "globals.h"
#include <math.h>

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
samePalette(false),
pcx(NULL),
unmaskedBitmap(NULL),
maskedBitmap(NULL){
    //Nothing
}

MugenSprite::MugenSprite( const MugenSprite &copy ):
unmaskedBitmap(NULL),
maskedBitmap(NULL){
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
    if (copy.pcx != NULL){
        this->pcx = new char[this->reallength];
        /* this line is right */
        memcpy(this->pcx, copy.pcx, this->reallength);
    } else {
        this->pcx = NULL;
    }

    if (copy.unmaskedBitmap){
	this->unmaskedBitmap = new Bitmap(*copy.unmaskedBitmap);
    }

    if (copy.maskedBitmap){
	this->maskedBitmap = new Bitmap(*copy.maskedBitmap);
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

    if (copy.unmaskedBitmap){
	this->unmaskedBitmap = new Bitmap(*copy.unmaskedBitmap);
    }

    if (copy.maskedBitmap){
	this->maskedBitmap = new Bitmap(*copy.maskedBitmap);
    }
    
    return *this;
}
        
void MugenSprite::copyImage(const MugenSprite * copy){
    cleanup();

    if (copy->unmaskedBitmap){
	this->unmaskedBitmap = new Bitmap(*copy->unmaskedBitmap);
    }

    if (copy->maskedBitmap){
	this->maskedBitmap = new Bitmap(*copy->maskedBitmap);
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

    if (unmaskedBitmap){
	delete unmaskedBitmap;
        unmaskedBitmap = NULL;
    }

    if (maskedBitmap){
	delete maskedBitmap;
        maskedBitmap = NULL;
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

    Filesystem::LittleEndianReader reader(ifile);
    
    next = reader.readByte4();
    length = reader.readByte4();
    x = reader.readByte2();
    y = reader.readByte2();
    groupNumber = reader.readByte2();
    imageNumber = reader.readByte2();
    prev = reader.readByte2();
    samePalette = reader.readByte1();
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

static bool isScaled(const Mugen::Effects & effects){
    double epsilon = 0.00001;
    return fabs(effects.scalex - 1) > epsilon ||
           fabs(effects.scaley - 1) > epsilon;
}

Bitmap MugenSprite::getFinalBitmap(const Mugen::Effects & effects){
    Bitmap * use = getBitmap(effects.mask);

    Bitmap modImage = *use;
    if (isScaled(effects)){
        modImage = Bitmap::temporaryBitmap((int) (use->getWidth() * effects.scalex), (int) (use->getHeight() * effects.scaley));
        use->Stretch(modImage);
    }

    return modImage;
}

void MugenSprite::render(const int xaxis, const int yaxis, const Bitmap &where, const Mugen::Effects &effects){
    draw(getFinalBitmap(effects), xaxis, yaxis, where, effects);
}

void MugenSprite::load(bool mask){
    if (pcx){
        if (mask){
            maskedBitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength), mask);
            maskedBitmap->replaceColor(maskedBitmap->get8BitMaskColor(), Bitmap::MaskColor());
        } else {
            unmaskedBitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength), mask);
        }
    }
}

void MugenSprite::reload(bool mask){
    if (maskedBitmap){
        delete maskedBitmap;
        maskedBitmap = NULL;
    }

    if (unmaskedBitmap){
        delete unmaskedBitmap;
        unmaskedBitmap = NULL;
    }

    load(mask);
}

Bitmap * MugenSprite::getBitmap(bool mask){
    if (mask){
        if (maskedBitmap){
            return maskedBitmap;
        }
        if (unmaskedBitmap){
            maskedBitmap = new Bitmap(*unmaskedBitmap, true);
            maskedBitmap->replaceColor(maskedBitmap->get8BitMaskColor(), Bitmap::MaskColor());
            return maskedBitmap;
        }
    } else {
        if (unmaskedBitmap){
            return unmaskedBitmap;
        }
        return maskedBitmap;
    }
    return NULL;
}

/* deletes raw data */
void MugenSprite::unloadRaw(){
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }
}

int MugenSprite::getWidth(){
    if (maskedBitmap){
        return maskedBitmap->getWidth();
    }
    return unmaskedBitmap->getWidth();
}

int MugenSprite::getHeight(){
    if (maskedBitmap){
        return maskedBitmap->getHeight();
    }
    return unmaskedBitmap->getHeight();
}

void MugenSprite::loadPCX(std::ifstream & ifile, bool islinked, bool useact, unsigned char palsave1[], bool mask){
    /* TODO: 768 is littered everywhere, replace with a constant */
    ifile.seekg(location + 32, ios::beg);
    ifile.clear();
    if (samePalette){
        // Lets give it space for the palette
        Global::debug(2) << "This sprite is less that 768 or has a shared palette - Group: " << getGroupNumber() << " | Image: " << getImageNumber() << endl;
        newlength += 768;
        pcx = new char[newlength];
        memset(pcx, 0, newlength);
    } else {
        pcx = new char[reallength];
        if (!pcx){
            printf("out of memory!!\n");
        }
        memset(pcx, 0, reallength);
    }
    ifile.read((char *)pcx, reallength); 
    if (ifile.eofbit || ifile.failbit){
        int read = (int) ifile.tellg() - (location + 32);
        if (read != (int) reallength){
            Global::debug(0) << "Warning: could not read " << reallength << " bytes from pcx file, only read " << read << endl;
        }
    }
    if (!islinked){
	if (!useact){
	    if (samePalette){
		memcpy( pcx + (reallength), palsave1, 768);
		Global::debug(2) << "Applying 1st palette to Sprite: " << imageNumber << " in Group: " << groupNumber << endl;
	    } else {
		memcpy(palsave1, pcx+(reallength)-768, 768);
	    }
	} else {
	    // Replace all palettes with the one supplied in act
	    if (samePalette){
		memcpy(pcx + (reallength), palsave1, 768);
	    } else {
                /* TODO: add an explanation of what group 9000 means here */
		if (!(groupNumber == 9000 && imageNumber == 1)){
		    memcpy(pcx + (reallength)-768, palsave1, 768);
		} 
	    }
	}
    }

    load(mask);
}

void MugenSprite::draw(const Bitmap &bmp, const int xaxis, const int yaxis, const int x, const int y, const Bitmap &where, const Mugen::Effects &effects){
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

    const int normalX = (xaxis - x);
    const int normalY = (yaxis - y);
    const int flippedX = (xaxis - bmp.getWidth() + x);
    const int flippedY = (yaxis - bmp.getHeight() + y);

    const int FLIPPED = -1;
    const int NOT_FLIPPED = 1;

    switch (effects.trans){
	case AddAlpha : {
	    // Need to figure out blend correctly addalpha is given to two locations low and high ?
	    Bitmap::transBlender(0, 0, 0, effects.alphalow);
	    break;
	}
	case Add : {
	    // this additive 100% I assume... not totally sure
	    // Bitmap::addBlender( 255, 255, 255, 255 );
	    Bitmap::addBlender(0, 0, 0, 255);
	    break;
	}
	case Add1 : {
	    // 50%
	    Bitmap::addBlender(128, 128, 128, 255);
	    break;
	}
	case Sub : {
	    // Shadow effect
	    // Bitmap::multiplyBlender( 0, 0, 0, 128 );
	    Bitmap::differenceBlender(0, 0, 0, 128);
	    break;
	}
	case None:
	default:{
	    break;
	}
    }

    if ( (effects.facing == FLIPPED) && (effects.vfacing == NOT_FLIPPED)){
	if (effects.trans != None){
	    bmp.drawTransHFlip(flippedX, normalY, where);
	} else {
	    // bmp.drawHFlip(placex + bmp.getWidth() / 2, placey, where);
	    bmp.drawHFlip(flippedX, normalY, startWidth, startHeight, width, height, where);
	}
	
    } else if ( (effects.vfacing == FLIPPED) && (effects.facing == NOT_FLIPPED)){
	if (effects.trans != None){
	    bmp.drawTransVFlip(normalX, flippedY, where);
	} else {
	    bmp.drawVFlip(normalX, flippedY, where);
	}
    } else if ((effects.vfacing == FLIPPED) && (effects.facing == FLIPPED)){
	if (effects.trans != None){
	    bmp.drawTransHVFlip(flippedX, flippedY, where);
	} else {
	    bmp.drawHVFlip(flippedX, flippedY, where);
	}
    } else {
	//if( effects.mask ){
	if (effects.trans != None){
	    bmp.drawTrans(normalX, normalY, where);
	} else {
	    bmp.draw(normalX, normalY, startWidth, startHeight, width, height, where);
	}
	//} else {
	//    bmp.Blit( placex, placey, where );
//	}
    }
}

void MugenSprite::draw(const Bitmap &bmp, const int xaxis, const int yaxis, const Bitmap &where, const Mugen::Effects &effects){
    draw(bmp, xaxis, yaxis, this->x, this->y, where, effects);
}
