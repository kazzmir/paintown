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
#include "mugen/mugen_sound.h"
#include "mugen/character.h"

#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;

using namespace std;
using namespace Mugen;

FightElement::FightElement():
type(IS_NOTSET),
action(0),
sprite(0),
font(0),
sound(0),
offset(0,0),
displaytime(0),
text(""){
}
FightElement::~FightElement(){
}

void FightElement::act(){
    switch (type){
	case IS_ACTION:
	    action->logic();
	    break;
	case IS_SPRITE:
            break;
	case IS_FONT:
            break;
        case IS_SOUND:
            if (sound){
                sound->play();
            }
            break;
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void FightElement::render(int x, int y, const Bitmap & bmp){
    switch (type){
	case IS_ACTION:
	    action->render(effects.facing, effects.vfacing, x + offset.x, y+ offset.y , bmp, effects.scalex, effects.scaley);
	    break;
	case IS_SPRITE:
	    sprite->render(x + offset.x, y + offset.y, bmp,effects);
	    break;
	case IS_FONT:
	    break;
        case IS_SOUND:
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void FightElement::setAction(MugenAnimation *anim){
    if (anim){
	setType(IS_ACTION);
	action = anim;
    }
}
void FightElement::setSprite(MugenSprite *spr){
    if (spr){
	setType(IS_SPRITE);
	sprite = spr;
    }
}
void FightElement::setFont(MugenFont *fnt){
    if (fnt){
	setType(IS_FONT);
	font = fnt;
    }
}

void FightElement::setSound(MugenSound * sound){
    if (sound){
        setType(IS_SOUND);
        this->sound = sound;
    }
}

static void interpolateRange(FightElement & element, int max, int current, const Mugen::Point & position, const Mugen::Point & range, const Bitmap & bmp){
    if (range.y > 0){
        for (int i = range.x; i < range.y; ++i){
            element.render(position.x + i, position.y, bmp);
        }
    } else {
        for (int i = range.x; i > range.y; --i){
            element.render(position.x + i, position.y, bmp);
        }
    }
} 

Bar::Bar():
maxHealth(0),
currentHealth(0),
damage(0){
}
Bar::~Bar(){
}
void Bar::act(const Mugen::Character & character){
    maxHealth = character.getMaxHealth();
    currentHealth = character.getHealth();
    // Update damage counter if char has been damaged
    if (character.hasControl()){
        if (damage > currentHealth){
            damage--;
        } else {
            damage = currentHealth;
        }
    }
}
void Bar::render(Element::Layer layer, const Bitmap & bmp){
    switch (layer){
	default:
	case Element::Background:
            // Background is full range
            if (back0.getLayer() == Element::Background){
                interpolateRange( back0, maxHealth, maxHealth, position, range, bmp);
            }
            // This is a container just render it normally 
            if (back1.getLayer() == Element::Background){
                back1.render(position.x, position.y, bmp);
            }
            // Middle is the damage indicator
            if (middle.getLayer() == Element::Background){
                interpolateRange( middle, maxHealth, damage, position, range, bmp);
            }
            // Front is the actual current health
            if (front.getLayer() == Element::Background){
                interpolateRange( front, maxHealth, currentHealth, position, range, bmp);
            }
	    break;
	case Element::Foreground:
            if (back0.getLayer() == Element::Foreground){
                interpolateRange( back0, maxHealth, maxHealth, position, range, bmp);
            }
            if (back1.getLayer() == Element::Foreground){
                back1.render(position.x, position.y, bmp);
            }
            if (middle.getLayer() == Element::Foreground){
                interpolateRange( middle, maxHealth, damage, position, range, bmp);
            }
            if (front.getLayer() == Element::Foreground){
                interpolateRange( front, maxHealth, currentHealth, position, range, bmp);
            }
	    break;
	case Element::Top:
            if (back0.getLayer() == Element::Top){
                interpolateRange( back0, maxHealth, maxHealth, position, range, bmp);
            }
            if (back1.getLayer() == Element::Top){
                back1.render(position.x, position.y, bmp);
            }
            if (middle.getLayer() == Element::Top){
                interpolateRange( middle, maxHealth, damage, position, range, bmp);
            }
            if (front.getLayer() == Element::Top){
                interpolateRange( front, maxHealth, currentHealth, position, range, bmp);
            }
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

PlayerInfo::PlayerInfo(const std::string & fightFile){
    
    std::string baseDir = Mugen::Util::getFileDir(fightFile);
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(fightFile) );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate fight definition file for: " + fightFile );
    
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;
    
    std::string spriteFile; 

    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        
        if (head == "files"){
            class FileWalk: public Ast::Walker{
            public:
                FileWalk(std::string & spriteFile):
                spriteFile(spriteFile){
                }
                std::string & spriteFile;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "sff"){
                        simple >> spriteFile;
                    } 
                }
            };

            FileWalk walk(spriteFile);
            section->walk(walk);
        } else if (head == "lifebar"){
            class BarWalk: public Ast::Walker{
            public:
                BarWalk(PlayerInfo & self):
                self(self){
                }
                PlayerInfo & self;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		    if (simple == "p1.pos"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			self.player1LifeBar.setPosition(x,y);
                    } else if (simple == "p1.bg0.spr"){
			int g=0, s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
		    } else if (simple == "p1.bg0.anim"){
		    } else if (simple == "p1.bg0.facing"){
		    } else if (simple == "p1.bg1.spr"){
		    } else if (simple == "p1.bg1.anim"){
		    } else if (simple == "p1.bg1.facing"){
		    } else if (simple == "p1.mid.spr"){
		    } else if (simple == "p1.mid.anim"){
		    } else if (simple == "p1.mid.facing"){
		    } else if (simple == "p1.front.spr"){
		    } else if (simple == "p1.front.anim"){
		    } else if (simple == "p1.front.facing"){
		    } else if (simple == "p1.range.x"){
		    }
                }
            };

            BarWalk walk(*this);
            section->walk(walk);
        }
    }
}

PlayerInfo::~PlayerInfo(){
}

void PlayerInfo::act(const Mugen::Character & player1, const Mugen::Character & player2){
    player1LifeBar.act(player1);
    player2LifeBar.act(player2);
}

void PlayerInfo::render(){
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

