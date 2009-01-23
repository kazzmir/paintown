#include <string.h>
#include "mugen_sprite.h"

MugenSprite::MugenSprite():
next(0),
location(0),
length(0),
reallength(0),
x(0),
y(0),
groupNumber(0),
imageNumber(0),
prev(0),
samePalette(0),
comments(NULL),
pcx(NULL){
    //Nothing
}

MugenSprite::MugenSprite( const MugenSprite &copy ){
    this->next = copy.next;
    this->location = copy.location;
    this->length = copy.length;
    this->reallength = copy.reallength;
    this->x = copy.x;
    this->y = copy.y;
    this->groupNumber = copy.groupNumber;
    this->imageNumber = copy.imageNumber;
    this->prev = copy.prev;
    this->samePalette = copy.samePalette;
    this->comments = new char[13];
    memcpy( this->comments, copy.comments, 13 );
    this->pcx = new char[this->reallength];
    memcpy(this->pcx, copy.pcx, this->reallength);
}

MugenSprite & MugenSprite::operator=( const MugenSprite &copy ){
    this->next = copy.next;
    this->location = copy.location;
    this->length = copy.length;
    this->reallength = copy.reallength;
    this->x = copy.x;
    this->y = copy.y;
    this->groupNumber = copy.groupNumber;
    this->imageNumber = copy.imageNumber;
    this->prev = copy.prev;
    this->samePalette = copy.samePalette;
    this->comments = new char[13];
    memcpy( this->comments, copy.comments, 13 );
    this->pcx = new char[this->reallength];
    memcpy(this->pcx, copy.pcx, this->reallength);
    
    return *this;
}

MugenSprite::~MugenSprite(){
    if (comments){
	delete[] comments;
	comments = NULL;
    }
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }
}
