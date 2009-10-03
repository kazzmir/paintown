#include "util/bitmap.h"

#include "mugen/characterhud.h"

#include "mugen/mugen_sprite.h"
#include "mugen/mugen_animation.h"
#include "mugen/mugen_font.h"

using namespace std;
using namespace Mugen;

Element::Element():
type(IS_NOTSET),
action(0),
sprite(0),
font(0),
offset(0,0),
range(0,0),
displaytime(0),
layerno(0),
text(""){
}
Element::~Element(){
}

void Element::act(){
    switch (type){
	case IS_ACTION:
	    action->logic();
	    break;
	case IS_SPRITE:
	case IS_FONT:
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void Element::render(const int xaxis, const int yaxis, Bitmap &bmp){
    switch (type){
	case IS_ACTION:
	    action->render(effects.facing,effects.vfacing,xaxis,yaxis,bmp,effects.scalex,effects.scaley);
	    break;
	case IS_SPRITE:
	    sprite->render(xaxis,yaxis,bmp,effects);
	    //sprite->render(facing,vfacing,xaxis,yaxis,bmp,scalex,scaley);
	    break;
	case IS_FONT:
	    break;
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void Element::setAction(MugenAnimation *anim){
    if (anim){
	setType(IS_ACTION);
	action = anim;
    }
}
void Element::setSprite(MugenSprite *spr){
    if (spr){
	setType(IS_SPRITE);
	sprite = spr;
    }
}
void Element::setFont(MugenFont *fnt){
    if (fnt){
	setType(IS_FONT);
	font = fnt;
    }
}

Bar::Bar(const int x, const int y){
}
Bar::~Bar(){
}
void Bar::setBack0(Element *){
}
void Bar::setBack1(Element *){
}
void Bar::setBack2(Element *){
}
void Bar::setMiddle(Element *){
}
void Bar::setFront(Element *){
}
void Bar::setCounter(Element *){
}
void Bar::setSound(Element *){
}
void Bar::act(){
}
void Bar::render(const int xaxis, const int yaxis, Bitmap &){
}


CharacterHUD::CharacterHUD(Token *token){
    // notin
}

CharacterHUD::~CharacterHUD(){
}


