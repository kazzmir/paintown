#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include <string.h>
#include "sprite.h"
#include "util/funcs.h"
#include "util/pointer.h"
#include "util/debug.h"
#include <math.h>

namespace PaintownUtil = ::Util;

using namespace std;
using namespace Mugen;

MugenSprite::MugenSprite(bool mask):
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
width(0),
height(0),
loaded(false),
defaultMask(mask){
}

MugenSprite::MugenSprite(const MugenSprite &copy){
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
    this->width = copy.width;
    this->height = copy.height;
    this->loaded = copy.loaded;
    this->defaultMask = copy.defaultMask;

    memcpy(this->originalPalette, copy.originalPalette, sizeof(originalPalette));

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

    this->unmaskedBitmap = copy.unmaskedBitmap;
    this->maskedBitmap = copy.maskedBitmap;
    this->unmaskedBitmapPalette = copy.unmaskedBitmapPalette;
    this->maskedBitmapPalette = copy.maskedBitmapPalette;
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
    this->width = copy.width;
    this->height = copy.height;
    this->loaded = copy.loaded;
    this->defaultMask = copy.defaultMask;
    memcpy(this->originalPalette, copy.originalPalette, sizeof(originalPalette));
    if (copy.comments){
        memcpy( this->comments, copy.comments, sizeof(MugenSprite::comments) );
    }

    if (copy.pcx){
        if (this->pcx != NULL){
            delete[] this->pcx;
            this->pcx = NULL;
        }
        this->pcx = new char[this->reallength];
        memcpy(this->pcx, copy.pcx, this->reallength);
    }

    this->unmaskedBitmap = copy.unmaskedBitmap;
    this->maskedBitmap = copy.maskedBitmap;
    this->unmaskedBitmapPalette = copy.unmaskedBitmapPalette;
    this->maskedBitmapPalette = copy.maskedBitmapPalette;
    
    return *this;
}

void MugenSprite::copyImage(const MugenSprite * copy){
    this->reallength = copy->reallength;
    this->newlength = copy->newlength;

    if (this->pcx != NULL){
        delete[] this->pcx;
        this->pcx = NULL;
    }

    if (copy->pcx != NULL){
        this->pcx = new char[this->newlength];
        memcpy(this->pcx, copy->pcx, this->newlength);
    }

    memcpy(this->originalPalette, copy->originalPalette, sizeof(originalPalette));
    this->width = copy->width;
    this->height = copy->height;
    this->unmaskedBitmap = copy->unmaskedBitmap;
    this->maskedBitmap = copy->maskedBitmap;
    this->unmaskedBitmapPalette = copy->unmaskedBitmapPalette;
    this->maskedBitmapPalette = copy->maskedBitmapPalette;
    this->loaded = copy->loaded;
    this->defaultMask = copy->defaultMask;
}

bool MugenSprite::isLoaded() const {
    return loaded;
}

bool MugenSprite::operator<(const MugenSprite &copy){
    return (this->groupNumber < copy.groupNumber) &&
           (this->imageNumber < copy.imageNumber);
}

void MugenSprite::cleanup(){
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }

    loaded = false;

    unmaskedBitmap = NULL;
    maskedBitmap = NULL;
    unmaskedBitmapPalette = NULL;
    maskedBitmapPalette = NULL;
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

    Storage::LittleEndianReader reader(ifile);
    
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

PaintownUtil::ReferenceCount<Graphics::Bitmap> MugenSprite::getFinalBitmap(const Mugen::Effects & effects){
    PaintownUtil::ReferenceCount<Graphics::Bitmap> use = getBitmap(effects.mask, effects.ownPalette);
    if (use == NULL){
        return use;
    }

    PaintownUtil::ReferenceCount<Graphics::Bitmap> modImage = use;
    if (isScaled(effects)){
        modImage = PaintownUtil::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Graphics::Bitmap::temporaryBitmap((int) (use->getWidth() * effects.scalex), (int) (use->getHeight() * effects.scaley))));
        use->Stretch(*(modImage.raw()));
    }

    return modImage;
}

void MugenSprite::render(const int xaxis, const int yaxis, const Graphics::Bitmap &where, const Mugen::Effects &effects){
    draw(getFinalBitmap(effects), xaxis, yaxis, where, effects);
}

PaintownUtil::ReferenceCount<Graphics::Bitmap> MugenSprite::load(bool mask, bool ownPalette){
    if (pcx){
        /* Handle the palette depending on ownPalette */

        char oldPalette[768];
        if (ownPalette){
            memcpy(oldPalette, pcx + newlength - 768, 768);
            memcpy(pcx + newlength - 768, originalPalette, 768);
        }

        PaintownUtil::ReferenceCount<Graphics::Bitmap> bitmap = PaintownUtil::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(Graphics::memoryPCX((unsigned char*) pcx, newlength), mask));
        if (mask){
            bitmap->replaceColor(bitmap->get8BitMaskColor(), Graphics::MaskColor());
        }

        if (ownPalette){
            memcpy(pcx + newlength - 768, oldPalette, 768);
        }

        return bitmap;
    }

    return PaintownUtil::ReferenceCount<Graphics::Bitmap>(NULL);
}

void MugenSprite::reload(bool mask){
    maskedBitmap = NULL;
    unmaskedBitmap = NULL;

    if (mask){
        maskedBitmap = load(mask, false);
    } else {
        unmaskedBitmap = load(mask, false);
    }
}

PaintownUtil::ReferenceCount<Graphics::Bitmap> MugenSprite::getBitmap(bool mask, bool ownPalette){
    if (ownPalette){
        if (mask){
            if (maskedBitmapPalette != NULL){
                return maskedBitmapPalette;
            }
            if (unmaskedBitmapPalette != NULL){
                maskedBitmapPalette = PaintownUtil::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(*unmaskedBitmapPalette, true));
                maskedBitmapPalette->replaceColor(maskedBitmapPalette->get8BitMaskColor(), Graphics::MaskColor());
                return maskedBitmapPalette;
            }

            maskedBitmapPalette = load(true, true);
            return maskedBitmapPalette;
        } else {
            if (unmaskedBitmapPalette != NULL){
                return unmaskedBitmapPalette;
            }
            unmaskedBitmapPalette = load(false, true);
            return unmaskedBitmapPalette;
        }
    } else if (mask){
        if (maskedBitmap != NULL){
            return maskedBitmap;
        }
        if (unmaskedBitmap != NULL){
            maskedBitmap = PaintownUtil::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(*unmaskedBitmap, true));
            maskedBitmap->replaceColor(maskedBitmap->get8BitMaskColor(), Graphics::MaskColor());
            return maskedBitmap;
        }

        maskedBitmap = load(true, false);
        return maskedBitmap;
    } else {
        if (unmaskedBitmap != NULL){
            return unmaskedBitmap;
        }
        unmaskedBitmap = load(defaultMask, false);
        return unmaskedBitmap;
    }
    return PaintownUtil::ReferenceCount<Graphics::Bitmap>(NULL);
}

/* deletes raw data */
void MugenSprite::unloadRaw(){
    /*
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }
    */
}

int MugenSprite::getWidth() const {
    return width;
}

int MugenSprite::getHeight() const {
    return height;
}

static int littleEndian16(const char * input){
    int byte1 = input[0];
    int byte2 = input[1];
    return (((unsigned char) byte2) << 8) | (unsigned char) byte1;
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
    memcpy(originalPalette, pcx + newlength - 768, 768);
    if (!islinked){
        if (!useact){
            if (samePalette){
                memcpy(pcx + (reallength), palsave1, 768);
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

    /* read values directly from the pcx header */
    int xmin = littleEndian16(&pcx[4]);
    int ymin = littleEndian16(&pcx[6]);
    int xmax = littleEndian16(&pcx[8]);
    int ymax = littleEndian16(&pcx[10]);

    width = (xmax - xmin) + 1;
    height = (ymax - ymin) + 1;

    loaded = true;

    /*
    PaintownUtil::ReferenceCount<Graphics::Bitmap> sprite = load(mask, false);
    if (mask){
        maskedBitmap = sprite;
    } else {
        unmaskedBitmap = sprite;
    }
    */
}

void MugenSprite::drawPartStretched(int sourceX1, int sourceY, int sourceWidth, int sourceHeight, int destX, int destY, int destWidth, int destHeight, const Mugen::Effects & effects, const Graphics::Bitmap & work){
    PaintownUtil::ReferenceCount<Graphics::Bitmap> final = getFinalBitmap(effects);
    Graphics::Bitmap single(*final, sourceX1, sourceY, sourceWidth, sourceHeight);
    single.drawStretched(destX, destY, destWidth, destHeight, work);
}

static void drawReal(const PaintownUtil::ReferenceCount<Graphics::Bitmap> & bmp, const int xaxis, const int yaxis, const int x, const int y, const Graphics::Bitmap &where, const Mugen::Effects &effects){
    if (bmp == NULL){
        return;
    }

    int startWidth = 0;
    int startHeight = 0;
    int width = bmp->getWidth();
    int height = bmp->getHeight();

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
    const int flippedX = (xaxis - bmp->getWidth() + x);
    const int flippedY = (yaxis - bmp->getHeight() + y);

    const int FLIPPED = -1;
    const int NOT_FLIPPED = 1;

    switch (effects.trans){
	case AddAlpha : {
            Graphics::Bitmap::alphaBlender(effects.alphaSource, effects.alphaDest);
	    break;
	}
	case Add : {
	    // this additive 100% I assume... not totally sure
	    // Bitmap::addBlender( 255, 255, 255, 255 );
            Graphics::Bitmap::addBlender(0, 0, 0, 255);
	    break;
	}
        case Translucent: {
            Graphics::Bitmap::transBlender(0, 0, 0, effects.alphaSource);
            break;
        }
	case Add1 : {
	    // 50%
            Graphics::Bitmap::addBlender(128, 128, 128, 255);
	    break;
	}
	case Sub : {
	    // Shadow effect
	    // Bitmap::multiplyBlender( 0, 0, 0, 128 );
            Graphics::Bitmap::differenceBlender(0, 0, 0, 255);
	    // Bitmap::burnBlender(0, 0, 0, 0);
	    break;
	}
	case None:
	default:{
	    break;
	}
    }

    if ((effects.facing == FLIPPED) && (effects.vfacing == NOT_FLIPPED)){
        if (effects.trans != None){
            bmp->translucent().drawHFlip(flippedX, normalY, effects.filter, where);
        } else {
            // bmp.drawHFlip(placex + bmp.getWidth() / 2, placey, where);
            bmp->drawHFlip(flippedX, normalY, startWidth, startHeight, width, height, effects.filter, where);
        }
    } else if ( (effects.vfacing == FLIPPED) && (effects.facing == NOT_FLIPPED)){
        if (effects.trans != None){
            bmp->translucent().drawVFlip(normalX, flippedY, effects.filter, where);
        } else {
            bmp->drawVFlip(normalX, flippedY, effects.filter, where);
        }
    } else if ((effects.vfacing == FLIPPED) && (effects.facing == FLIPPED)){
        if (effects.trans != None){
            bmp->translucent().drawHVFlip(flippedX, flippedY, effects.filter, where);
        } else {
            bmp->drawHVFlip(flippedX, flippedY, effects.filter, where);
        }
    } else {
        //if( effects.mask ){
        if (effects.trans != None){
            bmp->translucent().draw(normalX, normalY, effects.filter, where);
        } else {
            bmp->draw(normalX, normalY, startWidth, startHeight, width, height, effects.filter, where);
        }
        //} else {
        //    bmp.Blit( placex, placey, where );
        //	}
    }
}

void MugenSprite::draw(const PaintownUtil::ReferenceCount<Graphics::Bitmap> & bmp, const int xaxis, const int yaxis, const Graphics::Bitmap &where, const Mugen::Effects &effects){
    drawReal(bmp, xaxis, yaxis, this->x * effects.scalex, this->y * effects.scaley, where, effects);
}
