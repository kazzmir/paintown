#include "util/bitmap.h"

#include "mugen/characterhud.h"

#include "mugen/mugen_sprite.h"
#include "mugen/mugen_animation.h"
#include "mugen/mugen_font.h"

using namespace std;

Element::Element():
type(IS_NOTSET),
action(0),
sprite(0),
font(0),
offset(0,0),
displaytime(0),
facing(1),
vfacing(1),
layerno(0),
scalex(1),
scaley(1),
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
	    action->render(facing,vfacing,xaxis,yaxis,bmp,scalex,scaley);
	    break;
	case IS_SPRITE:
	    
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

MugenCharacterHUD::MugenCharacterHUD(){
    // notin
}

MugenCharacterHUD::~MugenCharacterHUD(){
}


