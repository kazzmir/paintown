#include "util/bitmap.h"
#include <string.h>
#include "mugen_sprite.h"
#include "util/funcs.h"
#include "globals.h"

using namespace std;

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
sprite(0){
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
    memcpy( this->comments, copy.comments, sizeof(MugenSprite::comments) );
    this->pcx = new char[this->reallength];
    memcpy(this->pcx, copy.pcx, this->reallength);
    this->sprite = new Bitmap(*copy.sprite);
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
    memcpy( this->comments, copy.comments, sizeof(MugenSprite::comments) );
    this->pcx = new char[this->reallength];
    memcpy(this->pcx, copy.pcx, this->reallength);
    this->sprite = new Bitmap(*copy.sprite);
    
    return *this;
}

bool MugenSprite::operator<( const MugenSprite &copy ){
    return ( (this->groupNumber < copy.groupNumber) && (this->imageNumber < copy.imageNumber) );
}

MugenSprite::~MugenSprite(){
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }
    if (sprite){
	delete sprite;
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

void MugenSprite::render(const int xaxis, const int yaxis, Bitmap &where, const double scalex, const double scaley){
    if (!sprite){
	load();
    }
    const int placex = (xaxis - this->x );
    const int placey = (yaxis - this->y );
    
    // temp for scaling
    Bitmap modImage = Bitmap::temporaryBitmap(sprite->getWidth() * scalex, sprite->getHeight() * scaley);
    sprite->Stretch(modImage);
    
    modImage.draw(placex, placey, where);
}
void MugenSprite::render(int facing, int vfacing, const int xaxis, const int yaxis, Bitmap &where, const double scalex, const double scaley){
    if (!sprite){
	load();
    }
    const int placex = (xaxis - this->x );
    const int placey = (yaxis - this->y );
    // temp for scaling
    Bitmap modImage = Bitmap::temporaryBitmap(sprite->getWidth() * scalex, sprite->getHeight() * scaley);
    sprite->Stretch(modImage);
    
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

void MugenSprite::load(){
    if (!sprite){
	sprite = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength));
    }
}

void MugenSprite::reload(){
    if (sprite){
	delete sprite;
    }
    sprite = new Bitmap(Bitmap::memoryPCX((unsigned char*) pcx, newlength));
}

// get sprite
Bitmap *MugenSprite::getBitmap(){
    if (!sprite){
	load();
    }
    return sprite;
}
const int MugenSprite::getWidth(){
    if (!sprite){
	load();
    }
    return sprite->getWidth();
}
const int MugenSprite::getHeight(){
    if (!sprite){
	load();
    }
    return sprite->getHeight();
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

