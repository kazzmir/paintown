#include "util/bitmap.h"
#include "characterhud.h"

#include <ostream>

// #include "init.h"
// #include "resource.h"
// #include "util/funcs.h"
// #include "object/animation.h"
// #include "object/object.h"
// #include "object/character.h"
// #include "object/object_attack.h"
// #include "object/player.h"
// #include "globals.h"
// #include "factory/font_render.h"

#include "mugen/config.h"
#include "mugen_sprite.h"
#include "mugen_animation.h"
#include "mugen_font.h"
#include "mugen_sound.h"
#include "character.h"

#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;

using namespace std;
using namespace Mugen;

static std::string replaceString(const std::string & replace, const std::string & replaceWith, const std::string & string){
    std::string temp = string;
    for(std::string::size_type i = 0; (i = temp.find(replace, i)) != std::string::npos;)
    {
	    temp.replace(i, replace.length(), replaceWith);
	    i += replaceWith.length() - replace.length() + 1;
    }
    return temp;
}

FightElement::FightElement():
type(IS_NOTSET),
action(0),
sprite(0),
font(0),
sound(0),
offset(0,0),
displaytime(0),
soundtime(0),
text(""),
bank(0),
position(0),
useDisplayTime(false),
useSoundTime(false),
ticker(0),
soundTicker(0){
}

FightElement::~FightElement(){
}

void FightElement::act(){
    switch (type){
	case IS_SOUND_AND_ACTION:
	case IS_ACTION:
	    action->logic();
	    break;
	case IS_SOUND_AND_SPRITE:
	case IS_SPRITE:
            break;
	case IS_SOUND_AND_FONT:
	case IS_FONT:
            break;
        case IS_SOUND:
            break;
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
    
    if (useDisplayTime && ticker < displaytime){
	ticker++;
    }
    if (useSoundTime && soundTicker == soundtime){
	if (sound){
	    sound->play();
	}
	soundTicker++;
    } else if (useSoundTime && soundTicker < soundtime){
	soundTicker++;
    }
}

void FightElement::render(int x, int y, const Bitmap & bmp){
    if (useDisplayTime && ticker == displaytime){
	return;
    }
    switch (type){
	case IS_SOUND_AND_ACTION:
	case IS_ACTION:
            action->render(effects.facing == -1, effects.vfacing == -1, x + offset.x, y+ offset.y , bmp, effects.scalex, effects.scaley);
            break;
	case IS_SOUND_AND_SPRITE:
	case IS_SPRITE:
            sprite->render(x + offset.x, y + offset.y, bmp,effects);
            break;
	case IS_SOUND_AND_FONT:
	case IS_FONT:
	    font->render(x + offset.x, y + offset.y, bank, position, bmp, text);
            break;
        case IS_SOUND:
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void FightElement::render(const Element::Layer & layer, int x, int y, const Bitmap & bmp){
    if (useDisplayTime && ticker == displaytime){
	return;
    }
    switch (type){
	case IS_SOUND_AND_ACTION:
	case IS_ACTION:
            if (layer == getLayer()){
	        action->render(effects.facing == -1, effects.vfacing == -1, x + offset.x, y + offset.y, bmp, effects.scalex, effects.scaley);
            }
	    break;
	case IS_SOUND_AND_SPRITE:
	case IS_SPRITE:
            if (layer == getLayer()){
	        sprite->render(x + offset.x, y + offset.y, bmp,effects);
            }
	    break;
	case IS_SOUND_AND_FONT:
	case IS_FONT:
            if (layer == getLayer()){
		font->render(x + offset.x, y + offset.y, bank, position, bmp, text);
            }
	    break;
        case IS_SOUND:
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void FightElement::play(){
    switch (type){
	case IS_SOUND_AND_ACTION:
	    sound->play();
	case IS_ACTION:
	    ticker = 0;
            break;
	case IS_SOUND_AND_SPRITE:
	    if (useSoundTime){
		soundTicker = 0;
	    } else {
		sound->play();
	    }
	case IS_SPRITE:
	    ticker = 0;
            break;
	case IS_SOUND_AND_FONT:
	    sound->play();
	case IS_FONT:
            break;
        case IS_SOUND:
	    sound->play();
	    break;
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void FightElement::setAction(MugenAnimation *anim){
    if (anim){
	if (type == IS_SOUND){
	    setType(IS_SOUND_AND_ACTION);
	} else {
	    setType(IS_ACTION);
	}
	action = anim;
    }
}
void FightElement::setSprite(MugenSprite *spr){
    if (spr){
	if (type == IS_SOUND){
	    setType(IS_SOUND_AND_SPRITE);
	} else {
	    setType(IS_SPRITE);
	}
	sprite = spr;
    }
}
void FightElement::setFont(MugenFont *fnt, int bank, int position){
    if (type == IS_ACTION){
	// Animation trumps font
	return;
    }
    if (fnt){
	if (type == IS_SOUND){
	    setType(IS_SOUND_AND_FONT);
	} else {
	    setType(IS_FONT);
	}
	font = fnt;
	this->bank = bank;
	this->position = position;
    }
}

void FightElement::setSound(MugenSound * sound){
    if (sound){
	if (type == IS_FONT){
	    setType(IS_SOUND_AND_FONT);
	} else if (type == IS_SPRITE){
	    setType(IS_SOUND_AND_SPRITE);
	} else if (type == IS_ACTION){
	    setType(IS_SOUND_AND_ACTION);
	} else {
	    setType(IS_SOUND);
	}
	this->sound = sound;
    }
}

int FightElement::getWidth(){
    switch (type){
	case IS_ACTION:
	    return 0;
            break;
	case IS_SPRITE:
	    return sprite->getWidth();
            break;
	case IS_FONT:
	    return font->textLength(text.c_str());
            break;
        case IS_SOUND:
	    return 0;
	    break;
	case IS_NOTSET:
	default:
	    return 0;
	    break;
    }
}


int FightElement::getHeight(){
    switch (type){
	case IS_ACTION:
	    return 0;
            break;
	case IS_SPRITE:
	    return sprite->getHeight();
            break;
	case IS_FONT:
	    return font->getHeight();
            break;
        case IS_SOUND:
	    return 0;
	    break;
	case IS_NOTSET:
	default:
	    return 0;
	    break;
    }
}

Bar::Bar():
type(Health),
maxHealth(1000),
currentHealth(1000),
damage(0),
powerLevel(Level0){
}

Bar::~Bar(){
}

void Bar::act(Mugen::Character & character){
    switch (type){
        default:
        case Health:
            maxHealth = character.getMaxHealth();
            currentHealth = character.getHealth();
            // Update damage counter if char has been damaged
            // x1 = current health, x2 = max health, y1 = place in the bar, y2 = maximum bar amount
            if (character.hasControl()){
                if (damage > currentHealth){
                    damage--;
                } else {
                    damage = currentHealth;
                }
            }
            break;
        case Power:
	    maxHealth = 3000;
	    currentHealth = (int)character.getPower();
	    // Update power bar and count number counter.setText()
	    // Play sounds at level 1 2 3
	    // Level 1 = 1000
	    // Level 2 = 2000
	    // Level 3 = 3000
	    if (character.getPower() == 3000){
		if (powerLevel != Level3){
		    level3Sound.play();
		}
		powerLevel = Level3;
		counter.setText("3");
	    } else if (character.getPower() >= 2000){
		if (powerLevel != Level2){
		    level2Sound.play();
		}
		powerLevel = Level2;
		counter.setText("2");
	    } else if (character.getPower() >= 1000){
		if (powerLevel != Level1){
		    level1Sound.play();
		}
		powerLevel = Level1;
		counter.setText("1");
	    } else {
		powerLevel = Level0;
		counter.setText("0");
	    }
            break;
    }
}

void Bar::render(Element::Layer layer, const Bitmap & bmp){
    // Background is full range
    back0.render(layer, position.x, position.y, bmp);
    // This is a container just render it normally 
    back1.render(layer, position.x, position.y, bmp);
    // Middle is the damage indicator
    if (range.y < 0){
	bmp.setClipRect(position.x + ((damage*range.y)/maxHealth),position.y,position.x + middle.getWidth() + range.x,middle.getHeight());
    } else {
	//bmp.setClipRect(position.x + range.x,position.y,position.x + (damage*range.y)/maxHealth,middle.getHeight());
    }
    middle.render(layer, position.x, position.y, bmp);
    bmp.setClipRect(0,0,bmp.getWidth(),bmp.getHeight());
    // Front is the actual current health
    if (range.y < 0){
	bmp.setClipRect(position.x + ((currentHealth*range.y)/maxHealth),position.y,position.x + middle.getWidth() + range.x,middle.getHeight());
    } else {
	//bmp.setClipRect(position.x + range.x,position.y,position.x + (currentHealth*range.y)/maxHealth,middle.getHeight());
    }
    front.render(layer, position.x, position.y, bmp);
    bmp.setClipRect(0,0,bmp.getWidth(),bmp.getHeight());
    // Counter Number for powerbars
    counter.render(layer, position.x, position.y, bmp);
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
    font.setText(character.getName());
}

void Name::render(const Element::Layer & layer, const Bitmap & bmp){
    background.render(layer, position.x, position.y, bmp);
    font.render(layer, position.x, position.y, bmp);
}

static void getElementProperties(const Ast::AttributeSimple & simple, const std::string & component, const std::string & elementName, FightElement & element, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations, std::vector<MugenFont *> & fonts){
    std::string compCopy = component;
    if (!compCopy.empty()){
	compCopy+=".";
    }
    if (simple == compCopy + elementName + ".spr"){
        int g=0, s=0;
        try{
            simple >> g >> s;
        } catch (const Ast::Exception & e){
        }
        element.setSpriteData(g,s);
        element.setSprite(sprites[g][s]);
    } else if (simple == compCopy + elementName + ".anim"){
        int anim;
        simple >> anim;
        element.setAction(animations[anim]);
    } else if ( simple == compCopy + elementName + ".font"){
	int index=0, bank=0, position=0;
	try {
	    simple >> index >> bank >> position;
	} catch (const Ast::Exception & e){
	    //ignore for now
	}
	element.setFont(fonts[index-1],bank,position);
    } else if (simple == compCopy + elementName + ".facing"){
        int face;
        simple >> face;
        element.setFacing(face);
    } else if (simple == compCopy + elementName + ".vfacing"){
        int face;
        simple >> face;
        element.setVFacing(face);
    } else if (simple == compCopy + elementName + ".layerno"){
        int layer = 0;
        simple >> layer;
        if (layer == 0){
            element.setLayer(Element::Background);
        } else if (layer == 1){
            element.setLayer(Element::Foreground);
        } else if (layer == 2){
            element.setLayer(Element::Top);
        }
    } else if (simple == compCopy +  elementName + ".offset"){
        int x=0, y=0;
        try{
            simple >> x >> y;
        } catch (const Ast::Exception & e){
        }
        element.setOffset(x,y);
    } else if (simple == compCopy + elementName + ".scale"){
        double x=1, y=1;
        try{
            simple >> x >> y;
        } catch (const Ast::Exception & e){
        }
        element.setScale(x,y);
    } else if (simple == compCopy + elementName + ".displaytime"){
        int time;
        simple >> time;
        element.setDisplayTime(time);
    } 
}

GameTime::GameTime():
frameCount(0),
time(Mugen::Data::getInstance().getTime()),
ticker(0),
started(false){
}

GameTime::~GameTime(){
}

void GameTime::act(){
    if (started){
	ticker++;
	if (ticker >= frameCount && time > 0){
	    time--;
	    ticker = 0;
	}
    }
    std::ostringstream str;
    if (time <= 9){
	str << "0" << time;
    } else {
	str << time;
    }
    timer.setText(str.str());
}

void GameTime::render(const Element::Layer & layer, const Bitmap & bmp){
    background.render(layer, position.x, position.y, bmp);
    timer.render(layer, position.x, position.y, bmp);
}
void GameTime::start(){
    // Resets the time just in case
    time = Mugen::Data::getInstance().getTime();
    started = true;
    ticker = 0;
}
void GameTime::stop(){
    started = false;
}

Combo::Combo():
side(Left),
startOffset(0),
displayTime(0),
ticker(0),
shake(false),
shakeTime(0),
total(0),
state(Disabled){
}
Combo::~Combo(){
}
void Combo::act(Mugen::Character & character){
    if (character.getCurrentCombo() >= 2 && total !=character.getCurrentCombo()){
	if (state == Disabled){
	    switch (side){
		case Left:
		    currentPosition = position;
		    currentPosition.x += startOffset;
		    break;
		case Right:
		    currentPosition = position;
		    currentPosition.x += abs(startOffset);
		    break;
		default:
		    break;
	    }
	}
	state = Forward;
	if (shake){
	    shakeTime = 10;
	}
	total = character.getCurrentCombo();
	std::ostringstream str;
	str << total;
	combo.setText(str.str());
	std::string temp = replaceString("%i",str.str(),message);
	text.setText(temp);
    }
    if (shakeTime > 0){
	shakeTime--;
    }
    switch (state){
	case Forward:
	    switch (side){
		case Left:
		    currentPosition.x+=2;
		    if (currentPosition.x >= position.x){
			currentPosition = position;
			state = Wait;
			ticker = 0;
		    }
		    break;
		case Right:
		    currentPosition.x-=2;
		    if (currentPosition.x <= position.x){
			currentPosition = position;
			state = Wait;
			ticker = 0;
		    }
		    break;
		default:
		    break;
	    }
	    break;
	case Wait:
	    if (ticker == displayTime){
		state = Retracting;
	    }
	    ticker++;
	    break;
	case Retracting:
	    switch (side){
		case Left:
		    currentPosition.x-=2;
		    if (currentPosition.x <= (position.x + startOffset)){
			state = Disabled;
		    }
		    break;
		case Right:
		    currentPosition.x+=2;
		    if (currentPosition.x >= (position.x + abs(startOffset))){
			state = Disabled;
		    }
		    break;
		default:
		    break;
	    }
	    break;
	case Disabled:
	default:
	    break;
    }
    total = character.getCurrentCombo();
}

void Combo::render(const Element::Layer & layer, const Bitmap & bmp){
    if (state != Disabled){
	const int modifierX = (shakeTime > 0 ? PaintownUtil::rnd( 8 ) - 4 : 0);
	const int modifierY = (shakeTime > 0 ? PaintownUtil::rnd( 8 ) - 4 : 0);
	combo.render(layer,currentPosition.x + modifierX,currentPosition.y + modifierY,bmp);
	// Hacking in adjustment for now, need to get correct width from font which is wrong
	text.render(layer,currentPosition.x + combo.getWidth() + (total > 9 ? 10 : 15),currentPosition.y,bmp);
    }
}
	
Round::Round(){
}
Round::~Round(){
}
void Round::act(){
}
void Round::render(const Element::Layer &, const Bitmap &){
}

GameInfo::GameInfo(const std::string & fightFile):
state(NotStarted){
    std::string baseDir = Mugen::Util::getFileDir(fightFile);
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(fightFile) );
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate fight definition file for: " + fightFile );
    }
    
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
                FileWalk(std::string & baseDir, GameInfo & self):
                baseDir(baseDir),
		self(self){
                }
                std::string & baseDir;
		GameInfo & self;
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
                    } else if (simple == "snd"){
			string temp;
                        simple >> temp;
                        Mugen::Util::readSounds( Mugen::Util::getCorrectFileLocation(baseDir, temp ), self.sounds);
                        Global::debug(1) << "Got Sound File: '" << temp << "'" << endl;
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
                BarWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations, std::vector<MugenFont *> & fonts):
                self(self),
		sprites(sprites),
                animations(animations),
                fonts(fonts){
                }
                GameInfo & self;
		Mugen::SpriteMap & sprites;
                std::map<int,MugenAnimation *> & animations;
                std::vector<MugenFont *> & fonts;
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
                    } else if (simple == component + ".range.x"){
                        int x=0,y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & e){
                        }
                        bar.setRange(x,y);
		    }
                    getElementProperties(simple,component,"bg0", bar.getBack0(),sprites,animations,fonts);
                    getElementProperties(simple,component,"bg1", bar.getBack1(),sprites,animations,fonts);
                    getElementProperties(simple,component,"mid", bar.getMiddle(),sprites,animations,fonts);
                    getElementProperties(simple,component,"front", bar.getFront(),sprites,animations,fonts);
                }
            };

            BarWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        } else if (head == "Powerbar"){
            class BarWalk: public Ast::Walker{
            public:
                BarWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations, std::vector<MugenFont *> & fonts, Mugen::SoundMap & sounds):
                self(self),
		sprites(sprites),
                animations(animations),
                fonts(fonts),
		sounds(sounds){
                }
                GameInfo & self;
		Mugen::SpriteMap & sprites;
                std::map<int,MugenAnimation *> & animations;
                std::vector<MugenFont *> & fonts;
		Mugen::SoundMap & sounds;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (PaintownUtil::matchRegex(simple.toString(), "p1")){
                        getBar(simple,"p1",self.player1PowerBar);
                    } else if (PaintownUtil::matchRegex(simple.toString(), "p2")){
                        getBar(simple,"p2",self.player2PowerBar);
                    } else if (simple == "level1.snd"){
			int g=0,s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
			self.player1PowerBar.getLevel1Sound().setSound(sounds[g][s]);
			self.player2PowerBar.getLevel1Sound().setSound(sounds[g][s]);
		    } else if (simple == "level2.snd"){
			int g=0,s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
			self.player1PowerBar.getLevel2Sound().setSound(sounds[g][s]);
			self.player2PowerBar.getLevel2Sound().setSound(sounds[g][s]);
		    } else if (simple == "level3.snd"){
			int g=0,s=0;
			try{
			    simple >> g >> s;
			} catch (const Ast::Exception & e){
			}
			self.player1PowerBar.getLevel3Sound().setSound(sounds[g][s]);
			self.player2PowerBar.getLevel3Sound().setSound(sounds[g][s]);
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
                    } else if (simple == component + ".range.x"){
                        int x=0,y=0;
                        try{
                            simple >> x >> y;
                        } catch (const Ast::Exception & e){
                        }
                        bar.setRange(x,y);
		    }
                    getElementProperties(simple,component,"bg0", bar.getBack0(),sprites,animations,fonts);
                    getElementProperties(simple,component,"bg1", bar.getBack1(),sprites,animations,fonts);
                    getElementProperties(simple,component,"mid", bar.getMiddle(),sprites,animations,fonts);
                    getElementProperties(simple,component,"front", bar.getFront(),sprites,animations,fonts);
                    getElementProperties(simple,component,"counter", bar.getCounter(),sprites,animations,fonts);
                }
            };

            BarWalk walk(*this,sprites,animations,fonts,sounds);
            section->walk(walk);
        } else if (head == "Face"){
            class FaceWalk: public Ast::Walker{
            public:
                FaceWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations, std::vector<MugenFont *> & fonts):
                self(self),
		sprites(sprites),
                animations(animations),
                fonts(fonts){
                }
                GameInfo & self;
		Mugen::SpriteMap & sprites;
                std::map<int,MugenAnimation *> & animations;
                std::vector<MugenFont *> & fonts;
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
                    }
                    getElementProperties(simple,component,"bg", face.getBackground(),sprites,animations,fonts);
                    getElementProperties(simple,component,"face", face.getFace(),sprites,animations,fonts);
                }
            };

            FaceWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        } else if (head == "Name"){
            class NameWalk: public Ast::Walker{
            public:
                NameWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations, std::vector<MugenFont *> & fonts):
                self(self),
		sprites(sprites),
		animations(animations),
		fonts(fonts){
                }
                GameInfo & self;
		Mugen::SpriteMap & sprites;
		std::map<int,MugenAnimation *> & animations;
		std::vector<MugenFont *> & fonts;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (PaintownUtil::matchRegex(simple.toString(), "p1")){
                        getName(simple,"p1",self.player1Name);
                    } else if (PaintownUtil::matchRegex(simple.toString(), "p2")){
                        getName(simple,"p2",self.player2Name);
                    }
                }
                void getName(const Ast::AttributeSimple & simple, const std::string & component, Name & name){
                    if (simple == component + ".pos"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			name.setPosition(x,y);
                    }
                    getElementProperties(simple,component,"bg", name.getBackground(),sprites,animations,fonts);
                    getElementProperties(simple,component,"name", name.getFont(),sprites,animations,fonts); 
                }
            };

            NameWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        } else if (head == "Time"){
            class TimeWalk: public Ast::Walker{
            public:
                TimeWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations, std::vector<MugenFont *> & fonts):
                self(self),
		sprites(sprites),
		animations(animations),
		fonts(fonts){
                }
                GameInfo & self;
		Mugen::SpriteMap & sprites;
		std::map<int,MugenAnimation *> & animations;
		std::vector<MugenFont *> & fonts;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "pos"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			self.timer.setPosition(x,y);
		    } else if (simple == "framespercount"){
			int x=0;
			simple >> x;
			self.timer.setFrameCount(x);
		    } 
		    getElementProperties(simple,"","bg", self.timer.getBackground(),sprites,animations,fonts);
		    getElementProperties(simple,"","counter", self.timer.getTimer(),sprites,animations,fonts);
		}
            };

            TimeWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        } else if (head == "Combo"){
            class ComboWalk: public Ast::Walker{
            public:
                ComboWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int,MugenAnimation *> & animations, std::vector<MugenFont *> & fonts):
                self(self),
		sprites(sprites),
		animations(animations),
		fonts(fonts){
                }
                GameInfo & self;
		Mugen::SpriteMap & sprites;
		std::map<int,MugenAnimation *> & animations;
		std::vector<MugenFont *> & fonts;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "pos"){
			int x=0, y=0;
			try{
			    simple >> x >> y;
			} catch (const Ast::Exception & e){
			}
			self.team1Combo.setPosition(x,y);
			self.team2Combo.setPosition(x,y);
		    } else if (simple == "start.x"){
			int x=0;
			simple >> x;
			self.team1Combo.setStartOffset(x);
			self.team2Combo.setStartOffset(x);
		    } else if (simple == "counter.shake"){
			bool shake;
			simple >> shake;
			self.team1Combo.setShake(shake);
			self.team2Combo.setShake(shake);
		    } else if (simple == "text.text"){
			std::string text;
			simple >> text;
			self.team1Combo.setMessage(text);
			self.team2Combo.setMessage(text);
		    } else if (simple == "displaytime"){
			int time;
			simple >> time;
			self.team1Combo.setDisplayTime(time);
			self.team2Combo.setDisplayTime(time);
		    } 
		    getElementProperties(simple,"","counter", self.team1Combo.getCombo(),sprites,animations,fonts);
		    getElementProperties(simple,"","text", self.team1Combo.getText(),sprites,animations,fonts);
		}
            };

            ComboWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        }
    }

    // Set power bars
    player1PowerBar.setType(Bar::Power);
    player2PowerBar.setType(Bar::Power);
}

GameInfo::~GameInfo(){
    // Get rid of sprites
    for (Mugen::SpriteMap::iterator i = sprites.begin() ; i != sprites.end() ; ++i){
	for (std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j){
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
    
    // Get rid of sounds
    for( Mugen::SoundMap::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
      for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second ){
	      delete j->second;
	  }
      }
    }
}

void GameInfo::act(Mugen::Character & player1, Mugen::Character & player2){
    player1LifeBar.act(player1);
    player2LifeBar.act(player2);
    player1PowerBar.act(player1);
    player2PowerBar.act(player2);
    player1Face.act(player1);
    player2Face.act(player2);
    player1Name.act(player1);
    player2Name.act(player2);
    timer.act();
    team1Combo.act(player1);
    team2Combo.act(player2);
}

void GameInfo::render(Element::Layer layer, const Bitmap &bmp){
    player1LifeBar.render(layer,bmp);
    player2LifeBar.render(layer,bmp);
    player1PowerBar.render(layer,bmp);
    player2PowerBar.render(layer,bmp);
    player1Face.render(layer,bmp);
    player2Face.render(layer,bmp);
    player1Name.render(layer,bmp);
    player2Name.render(layer,bmp);
    timer.render(layer,bmp);
    team1Combo.render(layer,bmp);
    team2Combo.render(layer,bmp);
}

void GameInfo::setState(const State & state, Character & player1, Character & player2){
    this->state = state;
    switch (state){
	case Intro:
	    // Set both character intro state
	    break;
	case RoundIndicator:
	    // Set both character Control to no control stand
	    break;
	case StartGame:
	    // Resume control state to characters and start time
	    timer.start();
	    break;
	case KO:
	    // Set character state to win for player that wins
	    break;
	case Draw:
	    // Set both character state to draw
	    break;
	case EndGame:
	    timer.stop();
	    // End the game
	    break;
	case NotStarted:
	default:
	    break;
    }
}

void GameInfo::parseAnimations(Ast::AstParse & parsed){
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
