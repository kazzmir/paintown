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

#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;

using namespace std;
using namespace Mugen;

/*
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
*/

Bar::Bar():
hitPoints(0),
currentHitPoints(0),
damage(0){
}
Bar::Bar(const Bar & copy){
}
Bar::~Bar(){
}
const Bar & Bar::operator=(const Bar & copy){


    return *this;
}

void Bar::act(){
    
}
void Bar::render(Element::Layer layer, int x, int y, const Bitmap & bmp){
    switch (layer){
	default:
	case Element::Background:
	    break;
	case Element::Foreground:
	    break;
	case Element::Top:
	    break;
    }
}


Face::Face():
background(0),
face(0){
}
Face::Face(const int x, const int y):
background(0),
face(0){
    setPosition(x,y);
}
Face::~Face(){
    if (background){
	delete background;
    }
    if (face){
	delete face;
    }
}
void Face::act(){
    if (background){
	background->act();
    }
    if (face){
	face->act();
    }
}
void Face::render(const int xaxis, const int yaxis, Bitmap &bmp){
    if (background){
	/*background->render(xaxis + position.x + spacing.x,yaxis + position.y + spacing.y,bmp);*/
    }
    if (face){
	//face->render(xaxis + position.x + spacing.x,yaxis + position.y + spacing.y,bmp);
    }
}
void Face::setPosition(const int x, const int y){
    position.x = x;
    position.y = y;
}
void Face::setSpacing(const int x, const int y){
    spacing.x = x;
    spacing.y = y;
}
void Face::setBackground(Element *e){
    background = e;
}
void Face::setFace(Element *e){
    face = e;
}

Name::Name():
font(0){
}
Name::Name(const int x, const int y):
font(0){
    setPosition(x,y);
}
Name::~Name(){
    if (font){
	delete font;
    }
}
void Name::act(){
    if (font){
	font->act();
    }
}
void Name::render(const int xaxis, const int yaxis, Bitmap &bmp){
    if (font){
	//font->render(xaxis + position.x,yaxis + position.y,bmp);
    }
}
void Name::setPosition(const int x, const int y){
    position.x = x;
    position.y = y;
}
void Name::setFont(Element *e){
    font = e;
}

PlayerInfo::PlayerInfo():
lifeBar(0),
powerBar(0),
face(0),
name(0){}
PlayerInfo::~PlayerInfo(){}
void PlayerInfo::setLifeBar(Bar *b){
    lifeBar = b;
}
void PlayerInfo::setPowerbar(Bar *b){
    powerBar = b;
}
void PlayerInfo::setFace(Face *f){
    face = f;
}
void PlayerInfo::setName(Name *n){
    name = n;
}
    
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
    
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	head = Mugen::Util::fixCase(head);

        // Global::debug(1) << "Name: " << head << endl;
        if (head == "files"){
            class FileWalk: public Ast::Walker{
            public:
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "sff"){
                        // Sprite file
                    } else if (simple == "snd"){
                        // Sound file
                    } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            /*string temp;
                            simple >> temp;
                            fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
			    */
		    } else if (simple == "fightfx.sff"){
		    } else if (simple == "fightfx.air"){
		    } else if (simple == "common.snd"){
		    } else {
                        Global::debug(0) << "Warning: ignored attribute: " << simple.toString() << endl;
                    }
                }
            };

            FileWalk walk;
            section->walk(walk);
	} if (head == "lifebar"){
            class LifeBarWalk: public Ast::Walker{
            public:
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "sff"){
                        // Sprite file
                    } else if (simple == "snd"){
                        // Sound file
                    } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            /*string temp;
                            simple >> temp;
                            fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
			    */
		    } else if (simple == "fightfx.sff"){
		    } else if (simple == "fightfx.air"){
		    } else if (simple == "common.snd"){
		    } else {
                        Global::debug(0) << "Warning: ignored attribute: " << simple.toString() << endl;
                    }
                }
            };

            LifeBarWalk walk;
            section->walk(walk);
	} else if (head == "lifebar"){
	    
	} else if (head == "simul lifebar"){
	} else if (head == "turns lifebar"){
	} else if (head == "powerbar"){
	} else if (head == "face"){
	} else if (head == "simul face"){
	} else if (head == "turns face"){
	} else if (head == "name"){
	} else if (head == "simul name"){
	} else if (head == "turns name"){
	} else if (head == "time"){
	} else if (head == "combo"){
	} else if (head == "round"){
	} else if (head == "winicon"){
	} else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
    }
}

void CharacterHUD::act(){
}
void CharacterHUD::render(const int xaxis, const int yaxis, Bitmap &){
}

