#include <string.h>
#include "mugen_sprite.h"

MugenSprite::MugenSprite():
next(0),
length(0),
x(0),
y(0),
groupNumber(0),
imageNumber(0),
prev(0),
samePalette(0),
pcx(NULL){
    //Nothing
    comments[13] = '\0';
}

MugenSprite::MugenSprite( const MugenSprite &copy ){
    this->next = copy.next;
    this->length = copy.length;
    this->x = copy.x;
    this->y = copy.y;
    this->groupNumber = copy.groupNumber;
    this->imageNumber = copy.imageNumber;
    this->prev = copy.prev;
    this->samePalette = copy.samePalette;
    strncpy( this->comments, copy.comments, 14 );
    this->pcx = new char[this->length];
    memcpy(this->pcx, copy.pcx, this->length);
}

MugenSprite & MugenSprite::operator=( const MugenSprite &copy ){
    this->next = copy.next;
    this->length = copy.length;
    this->x = copy.x;
    this->y = copy.y;
    this->groupNumber = copy.groupNumber;
    this->imageNumber = copy.imageNumber;
    this->prev = copy.prev;
    this->samePalette = copy.samePalette;
    strncpy( this->comments, copy.comments, 14 );
    this->pcx = new char[this->length];
    memcpy(this->pcx, copy.pcx, this->length);
    
    return *this;
}

MugenSprite::~MugenSprite(){
    if (pcx){
        delete[] pcx;
        pcx = NULL;
    }
}
