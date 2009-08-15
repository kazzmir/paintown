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
type(IS_NOTSET),
action(0),
sprite(0),
spriteBmp(0),
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
    if (spriteBmp){
	delete spriteBmp;
    }
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
	    renderSprite(xaxis,yaxis,bmp);
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
	spriteBmp = new Bitmap(Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength));
    }
}
void Element::setFont(MugenFont *fnt){
    if (fnt){
	setType(IS_FONT);
	font = fnt;
    }
}

void Element::renderSprite(const int xaxis, const int yaxis, Bitmap &where){
    const int placex = (xaxis - sprite->x );
    const int placey = (yaxis - sprite->y );
    // temp for scaling
    Bitmap modImage = Bitmap::temporaryBitmap(spriteBmp->getWidth() * scalex, spriteBmp->getHeight() * scaley);
    spriteBmp->Stretch(modImage);
    
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

MugenCharacterHUD::MugenCharacterHUD(){
    // notin
}

MugenCharacterHUD::~MugenCharacterHUD(){
}


