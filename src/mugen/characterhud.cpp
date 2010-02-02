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

void FightElement::render(const Element::Layer & layer, int x, int y, const Bitmap & bmp){
    switch (type){
	case IS_ACTION:
            if (layer == getLayer()){
	        action->render(effects.facing, effects.vfacing, x + offset.x, y+ offset.y , bmp, effects.scalex, effects.scaley);
            }
	    break;
	case IS_SPRITE:
            if (layer == getLayer()){
	        sprite->render(x + offset.x, y + offset.y, bmp,effects);
            }
	    break;
	case IS_FONT:
            if (layer == getLayer()){
            }
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

static void interpolateRange(const Element::Layer & layer, FightElement & element, int max, int current, const Mugen::Point & position, const Mugen::Point & range, const Bitmap & bmp){
    if (layer != element.getLayer()){
        return;
    }
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
void Bar::act(Mugen::Character & character){
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
    // Background is full range
    interpolateRange(layer, back0, maxHealth, maxHealth, position, range, bmp);
    // This is a container just render it normally 
    back1.render(layer, position.x, position.y, bmp);
    // Middle is the damage indicator
    interpolateRange(layer, middle, maxHealth, damage, position, range, bmp);
    // Front is the actual current health
    interpolateRange(layer, front, maxHealth, currentHealth, position, range, bmp);
}


Face::Face(){
}
Face::~Face(){
}
void Face::act(Character & character){
    face.setSprite(character.getSprite(face.getSpriteData().x,face.getSpriteData().y));
}
void Face::render(const Element::Layer & layer, const Bitmap & bmp){
    background.render(layer,position.x,position.y,bmp);
    face.render(layer,position.x,position.y,bmp);
}

Name::Name(){
}
Name::~Name(){
}
void Name::act(Mugen::Character & character){
    
}
void Name::render(const Element::Layer & layer, const Bitmap & bmp){
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

    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        if (head == "Files"){
            class FileWalk: public Ast::Walker{
            public:
                FileWalk(std::string & baseDir, PlayerInfo & self):
                baseDir(baseDir),
		self(self){
                }
                std::string & baseDir;
		PlayerInfo & self;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "sff"){
			std::string sff;
			simple >> sff;
                        Global::debug(1) << "Got Sprite File: '" << sff << "'" << endl;
			Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(baseDir, sff), "", self.sprites);
			for( Mugen::SpriteMap::iterator i = self.sprites.begin() ; i != self.sprites.end() ; ++i ){
			    // Load these sprites so they are ready to use
			    for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
				if( j->second )j->second->load();
			    }
			}
                    } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                        string temp;
                        simple >> temp;
                        self.fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                        Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
                    }  
                }
            };

            FileWalk walk(baseDir, *this);
            section->walk(walk);
            
            // Get animations so we can set up the lifebars
            parseAnimations(parsed);

        } else if (head == "Lifebar"){
            class BarWalk: public Ast::Walker{
            public:
                BarWalk(PlayerInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations):
                self(self),
		sprites(sprites),
                animations(animations){
                }
                PlayerInfo & self;
		Mugen::SpriteMap & sprites;
                std::map<int,MugenAnimation *> & animations;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (PaintownUtil::matchRegex(simple.toString(), "p1")){
                        getBar(simple,"p1",self.player1LifeBar);
                    } else if (PaintownUtil::matchRegex(simple.toString(), "p2")){
                        getBar(simple,"p2",self.player2LifeBar);
                    }
                }
                void getBar(const Ast::AttributeSimple & simple, const std::string & component, Bar & bar){
                    if (simple == component + ".pos"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			bar.setPosition(x,y);
                    } else if (simple == component + ".bg0.spr"){
			int g=0, s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
                        bar.getBack0().setSpriteData(g,s);
			bar.getBack0().setSprite(sprites[g][s]);
		    } else if (simple == component + ".bg0.anim"){
                        int anim;
                        simple >> anim;
                        bar.getBack0().setAction(animations[anim]);
		    } else if (simple == component + ".bg0.facing"){
                        int face;
                        simple >> face;
                        bar.getBack0().setFacing(face);
		    } else if (simple == component + ".bg0.layerno"){
                        int layer = 0;
                        simple >> layer;
                        if (layer == 0){
                            bar.getBack0().setLayer(Element::Background);
                        } else if (layer == 1){
                            bar.getBack0().setLayer(Element::Foreground);
                        } else if (layer == 2){
                            bar.getBack0().setLayer(Element::Top);
                        }
		    } else if (simple == component + ".bg1.spr"){
			int g=0, s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
                        bar.getBack1().setSpriteData(g,s);
			bar.getBack1().setSprite(sprites[g][s]);
		    } else if (simple == component + ".bg1.anim"){
                        int anim;
                        simple >> anim;
                        bar.getBack1().setAction(animations[anim]);
		    } else if (simple == component + ".bg1.facing"){
                        int face;
                        simple >> face;
                        bar.getBack1().setFacing(face);
		    } else if (simple == component + ".bg1.layerno"){
                        int layer = 0;
                        simple >> layer;
                        if (layer == 0){
                            bar.getBack1().setLayer(Element::Background);
                        } else if (layer == 1){
                            bar.getBack1().setLayer(Element::Foreground);
                        } else if (layer == 2){
                            bar.getBack1().setLayer(Element::Top);
                        }
		    } else if (simple == component + ".mid.spr"){
                        int g=0, s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
                        bar.getMiddle().setSpriteData(g,s);
			bar.getMiddle().setSprite(sprites[g][s]);
		    } else if (simple == component + ".mid.anim"){
                        int anim;
                        simple >> anim;
                        bar.getMiddle().setAction(animations[anim]);
		    } else if (simple == component + ".mid.facing"){
                        int face;
                        simple >> face;
                        bar.getMiddle().setFacing(face);
		    } else if (simple == component + ".mid.layerno"){
                        int layer = 0;
                        simple >> layer;
                        if (layer == 0){
                            bar.getMiddle().setLayer(Element::Background);
                        } else if (layer == 1){
                            bar.getMiddle().setLayer(Element::Foreground);
                        } else if (layer == 2){
                            bar.getMiddle().setLayer(Element::Top);
                        }
		    } else if (simple == component + ".front.spr"){
                        int g=0, s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
                        bar.getFront().setSpriteData(g,s);
			bar.getFront().setSprite(sprites[g][s]);
		    } else if (simple == component + ".front.anim"){
                        int anim;
                        simple >> anim;
                        bar.getFront().setAction(animations[anim]);
		    } else if (simple == component + ".front.facing"){
                        int face;
                        simple >> face;
                        bar.getFront().setFacing(face);
		    } else if (simple == component + ".front.layerno"){
                        int layer = 0;
                        simple >> layer;
                        if (layer == 0){
                            bar.getFront().setLayer(Element::Background);
                        } else if (layer == 1){
                            bar.getFront().setLayer(Element::Foreground);
                        } else if (layer == 2){
                            bar.getFront().setLayer(Element::Top);
                        }
		    } else if (simple == component + ".range.x"){
                        int x=0,y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & e){
                        }
                        bar.setRange(x,y);
		    }
                }
            };

            BarWalk walk(*this,sprites,animations);
            section->walk(walk);
        } else if (head == "Face"){
            class FaceWalk: public Ast::Walker{
            public:
                FaceWalk(PlayerInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations):
                self(self),
		sprites(sprites),
                animations(animations){
                }
                PlayerInfo & self;
		Mugen::SpriteMap & sprites;
                std::map<int,MugenAnimation *> & animations;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (PaintownUtil::matchRegex(simple.toString(), "p1")){
                        getFace(simple,"p1",self.player1Face);
                    } else if (PaintownUtil::matchRegex(simple.toString(), "p2")){
                        getFace(simple,"p2",self.player2Face);
                    }
                }
                void getFace(const Ast::AttributeSimple & simple, const std::string & component, Face & face){
                    if (simple == component + ".pos"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			face.setPosition(x,y);
                    } else if (simple == component + ".bg.spr"){
			int g=0, s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
                        face.getBackground().setSpriteData(g,s);
			face.getBackground().setSprite(sprites[g][s]);
		    } else if (simple == component + ".bg.anim"){
                        int anim;
                        simple >> anim;
                        face.getBackground().setAction(animations[anim]);
		    } else if (simple == component + ".bg.offset"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			face.getBackground().setOffset(x,y);
		    } else if (simple == component + ".bg.scale"){
			double x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			face.getBackground().setScale(x,y);
		    } else if (simple == component + ".bg.facing"){
                        int facing;
                        simple >> facing;
                        face.getBackground().setFacing(facing);
		    } else if (simple == component + ".bg.layerno"){
                        int layer = 0;
                        simple >> layer;
                        if (layer == 0){
                            face.getBackground().setLayer(Element::Background);
                        } else if (layer == 1){
                            face.getBackground().setLayer(Element::Foreground);
                        } else if (layer == 2){
                            face.getBackground().setLayer(Element::Top);
                        }
		    } else if (simple == component + ".face.spr"){
			int g=0, s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
                        face.getFace().setSpriteData(g,s);
		    } else if (simple == component + ".face.offset"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			face.getFace().setOffset(x,y);
		    } else if (simple == component + ".face.scale"){
			double x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			face.getFace().setScale(x,y);
		    } else if (simple == component + ".face.facing"){
                        int facing;
                        simple >> facing;
                        face.getFace().setFacing(facing);
		    } else if (simple == component + ".face.layerno"){
                        int layer = 0;
                        simple >> layer;
                        if (layer == 0){
                            face.getFace().setLayer(Element::Background);
                        } else if (layer == 1){
                            face.getFace().setLayer(Element::Foreground);
                        } else if (layer == 2){
                            face.getFace().setLayer(Element::Top);
                        }
		    } 
                }
            };

            FaceWalk walk(*this,sprites,animations);
            section->walk(walk);
        }
    }
}

PlayerInfo::~PlayerInfo(){
    // Get rid of sprites
    for( Mugen::SpriteMap::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
	for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	    if (j->second){
                delete j->second;
            }
	}
    }
     // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if (i->second){
            delete i->second;
        }
    }
    // Get rid of fonts
    for (std::vector< MugenFont *>::iterator f = fonts.begin(); f != fonts.end(); ++f){
	if (*f){
            delete (*f);
        }
    }
}

void PlayerInfo::act(Mugen::Character & player1, Mugen::Character & player2){
    player1LifeBar.act(player1);
    player2LifeBar.act(player2);
    player1Face.act(player1);
    player2Face.act(player2);
}

void PlayerInfo::render(Element::Layer layer, const Bitmap &bmp){
    player1LifeBar.render(layer,bmp);
    player2LifeBar.render(layer,bmp);
    player1Face.render(layer,bmp);
    player2Face.render(layer,bmp);
}

void PlayerInfo::parseAnimations(Ast::AstParse & parsed){
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        head = Util::fixCase(head);
        if (PaintownUtil::matchRegex(head, "begin *action")){
            /* This creates the animations. It differs from character animation since
             * these are included in the stage.def file with the other defaults
             */
	    head.replace(0,13,"");
	    int h;
            istringstream out(head);
	    out >> h;
	    animations[h] = Mugen::Util::getAnimation(section, sprites);
	} 
    }
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

