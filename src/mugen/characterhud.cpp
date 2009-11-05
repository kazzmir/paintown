#include "util/bitmap.h"

#include "mugen/characterhud.h"

#include "util/bitmap.h"
#include "init.h"
#include "resource.h"
#include "util/funcs.h"
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "globals.h"
#include "factory/font_render.h"

#include "mugen/mugen_sprite.h"
#include "mugen/mugen_animation.h"
#include "mugen/mugen_font.h"
#include "mugen/mugen_reader.h"
#include "mugen/mugen_section.h"

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


Face::Face(){}
Face::Face(const int x, const int y){}
Face::~Face(){}
void Face::act(){}
void Face::render(){}

Name::Name(){}
Name::~Name(){}

PlayerInfo::PlayerInfo(){}
PlayerInfo::~PlayerInfo(){}
    
    
CharacterHUD::CharacterHUD( const std::string & s ):
location(s){
}

CharacterHUD::~CharacterHUD(){
}

void CharacterHUD::load() throw (MugenException){
     // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Mugen::Util::getFileDir(location);
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(location) );
    // get real basedir
    //baseDir = Mugen::Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate fight definition file for: " + location );
    
    std::string filesdir = "";
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	Mugen::Util::fixCase(head);
	Global::debug(1) << "Name: " << head << endl;
    }
}

void CharacterHUD::act(){
}
void CharacterHUD::render(const int xaxis, const int yaxis, Bitmap &){
}

