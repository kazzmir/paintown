#include "mugen/characterhud.h"

#include "mugen/mugen_sprite.h"
#include "mugen/mugen_animation.h"
#include "mugen/mugen_font.h"

#include "util/bitmap.h"

using namespace std;

DisplayPoint::DisplayPoint():
x(0),
y(0){
}

DisplayPoint::DisplayPoint(int x, int y):
x(x),
y(y){
}

DisplayPoint &DisplayPoint::operator=(const DisplayPoint &p){
    this->x = p.x;
    this->y = p.y;
    return *this;
}

DisplayPoint::~DisplayPoint(){
}

Element::Element():
action(0),
sprite(0){
}
Element::~Element(){
}
void Element::act(){
}
void Element::render(const int xaxis, const int yaxis, Bitmap *bmp){
    
}

void Element::setAction(MugenAnimation *anim){
    setType(IS_ACTION);
    action = anim;
}
void Element::setSprite(MugenSprite *spr){
    setType(IS_SPRITE);
    sprite = spr;
}
void Element::setFont(MugenFont *fnt){
    setType(IS_FONT);
    font = fnt;
}

MugenCharacterHUD::MugenCharacterHUD(){
    // notin
}

MugenCharacterHUD::~MugenCharacterHUD(){
}


