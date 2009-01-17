#include <string.h>
#include "mugen_sprite.h"

MugenSprite::MugenSprite():
next(0),
pcxSize(0),
x(0),
y(0),
groupNumber(0),
imageNumber(0),
prev(0),
samePalette(0){
    //Nothing
}

MugenSprite::MugenSprite( const MugenSprite &copy ){
    this->next = copy.next;
    this->pcxSize = copy.pcxSize;
    this->x = copy.x;
    this->y = copy.y;
    this->groupNumber = copy.groupNumber;
    this->imageNumber = copy.imageNumber;
    this->prev = copy.prev;
    this->samePalette = copy.samePalette;
    strncpy(this->comments, copy.comments, 14);
    this->pcxData = copy.pcxData;
}

MugenSprite::~MugenSprite(){
    if( pcxData ) delete pcxData;
}

