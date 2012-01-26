#include "util/bitmap.h"
#include "characterhud.h"

#include <ostream>

#include "config.h"
#include "sprite.h"
#include "animation.h"
#include "parse-cache.h"
#include "font.h"
#include "sound.h"
#include "stage.h"
#include "character.h"
#include "state.h"

#include "util/debug.h"
#include "util/timedifference.h"
#include "ast/all.h"
#include "parser/all.h"

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;

namespace PaintownUtil = ::Util;

using namespace std;
using namespace Mugen;

static string replaceString(const string & replace, const string & replaceWith, const string & subject){
    string temp = subject;
    for (string::size_type i = 0; (i = temp.find(replace, i)) != string::npos;) {
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
displayState(NoDisplayTimer),
soundState(NoSoundTimer),
ticker(0),
soundTicker(0){
}

FightElement::~FightElement(){
}

void FightElement::act(){
    if (isDone()){
        return;
    }
    switch (type){
	case IS_ACTION:
	    action->logic();
	    break;
	case IS_SPRITE:
            break;
	case IS_FONT:
            break;
        case IS_SOUND:
            break;
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
    
    switch (displayState){
	case DisplayStarted:
	    if (ticker >= displaytime){
		displayState = DisplayEnded;
	    } else {
		ticker++;
	    }
	    break;
	case DisplayEnded:
	case NoDisplayTimer:
	default:
	    break;
    }
    
    switch (soundState){
	case Waiting:
	    if (soundTicker >= soundtime){
		if (sound){
		    sound->play();
		}
	    } else {
		soundTicker++;
	    }
	    break;
	case Played:
	case NoSoundTimer:
	default:
	    break;
    }
	    
}

void FightElement::render(int x, int y, const Graphics::Bitmap & bmp, Graphics::Bitmap::Filter * filter){
    if (isDone()){
	return;
    }
    switch (type){
	case IS_ACTION:
            action->render(effects.facing == -1, effects.vfacing == -1, x + offset.x, y+ offset.y , bmp, effects.scalex, effects.scaley);
            break;
	case IS_SPRITE:
            sprite->render(x + offset.x, y + offset.y, bmp,effects);
            break;
	case IS_FONT:
	    font->render(x + offset.x, y + offset.y, position, bank, bmp, text);
            break;
        case IS_SOUND:
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void FightElement::render(const Element::Layer & layer, int x, int y, const Graphics::Bitmap & bmp, int width = -99999){
    if (isDone()){
	return;
    }

    int realX = x + offset.x;
    int realY = y + offset.y;

    switch (type){
	case IS_ACTION:
            if (layer == getLayer()){
	        action->render(effects.facing == -1, effects.vfacing == -1, realX, realY, bmp, effects.scalex, effects.scaley);
            }
	    break;
	case IS_SPRITE:
            if (layer == getLayer()){
                // Global::debug(0) << "Draw bar with width " << width << endl;
                Effects realEffects(effects);
                if (width != -99999){
                    if (width < 0){
                        /* flip facing */
                        // realEffects.facing *= -1;
                        // realEffects.dimension.x2 = -width;
                        realEffects.dimension.x1 = sprite->getWidth() + width;
                        // realX += width;
                    } else {
                        realEffects.dimension.x2 = width;
                    }
                }
                sprite->render(realX, realY, bmp, realEffects);
            }
	    break;
	case IS_FONT:
            if (layer == getLayer()){
		font->render(realX, realY, position, bank, bmp, text);
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
	case IS_ACTION:
	    action->play();
	    break;
	case IS_SPRITE:
	case IS_FONT:
	    if (displayState == DisplayNotStarted || displayState == DisplayEnded){
		ticker = 0;
		displayState = DisplayStarted;
	    }
            break;
        case IS_SOUND:
	    if (soundState != NoSoundTimer){
		soundTicker = 0;
		soundState = Waiting;
	    } else {
		sound->play();
	    }
	    break;
	case IS_NOTSET:
	default:
	    // nothing
	    break;
    }
}

void FightElement::setAction(PaintownUtil::ReferenceCount<MugenAnimation> anim){
    if (anim != NULL){
	setType(IS_ACTION);
	action = anim;
        action->setAsOneTime(true);
    }
}
void FightElement::setSprite(MugenSprite *spr){
    if (spr){
	setType(IS_SPRITE);
	sprite = spr;
    }
}
void FightElement::setFont(MugenFont *fnt, int bank, int position){
    if (type == IS_ACTION){
	// Animation trumps font
	return;
    }
    if (fnt){
	setType(IS_FONT);
	font = fnt;
	this->bank = bank;
	this->position = position;
    }
}

void FightElement::setSound(MugenSound * sound){
    if (sound){
	setType(IS_SOUND);
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

bool FightElement::notStarted(){
    switch (type){
	case IS_ACTION:
	    return !action->hasStarted();
            break;
	case IS_SPRITE:
	case IS_FONT:
	    return (displayState == DisplayNotStarted);
            break;
        case IS_SOUND:
	    break;
	case IS_NOTSET:
	default:
	    break;
    }
    return false;
}

bool FightElement::isDone(){
    switch (type){
	case IS_ACTION:
	    return action->isDone();
            break;
	case IS_SPRITE:
	case IS_FONT:
	    switch (displayState){
		case DisplayNotStarted:
		case NoDisplayTimer:
		case DisplayStarted:
		    return false;
		    break;
		case DisplayEnded:
		default:
		    return true;
	    }
            break;
        case IS_SOUND:
	    break;
	case IS_NOTSET:
	default:
	    break;
    }
    return false;
}

void FightElement::reset(){
    switch (type){
	case IS_ACTION:
	    action->reset();
            break;
	case IS_SPRITE:
	case IS_FONT:
	    ticker = 0;
            displayState = DisplayNotStarted;
            break;
        case IS_SOUND:
	    break;
	case IS_NOTSET:
	default:
	    break;
    }
}

static const int DAMAGE_WAIT_TIME = 30;

Bar::Bar():
type(None),
maxHealth(1000),
currentHealth(1000),
damage(1000),
wait(DAMAGE_WAIT_TIME),
powerLevel(Level0){
}

Bar::~Bar(){
}

void Bar::act(Mugen::Character & character){
    switch (type){
        case Health: {
            maxHealth = character.getMaxHealth();
	    if (currentHealth != character.getHealth()){
		wait = DAMAGE_WAIT_TIME;
	    }
            currentHealth = character.getHealth();
            if (maxHealth == 0){
                throw MugenException("Character max health is 0. It should have been set in the [Data] section of the constants file", __FILE__, __LINE__);
            }
            // Update damage counter if char has been damaged
            // x1 = current health, x2 = max health, y1 = place in the bar, y2 = maximum bar amount
            if (damage == currentHealth){
		wait = DAMAGE_WAIT_TIME;
	    } else {
		if (wait <= 0){
		    damage -= (int)((double) (damage - currentHealth) / 8.0 + 0.5);
		} else {
		    wait -= 1;
		}
	    }
            break;
        }
        case Power: {
	    maxHealth = 3000;
            /* FIXME: characters power should never go above 3000. fix in the character */
	    currentHealth = (int)character.getPower();
            if (currentHealth > 3000){
                currentHealth = 3000;
            }
	    // Update power bar and count number counter.setText()
	    // Play sounds at level 1 2 3
	    // Level 1 = 1000
	    // Level 2 = 2000
	    // Level 3 = 3000
	    if (character.getPower() >= 3000){
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
        case None : break;
        default: break;
    }
}

void Bar::render(Element::Layer layer, const Graphics::Bitmap & bmp){
    if (type != None){
        // Background is full range
        back0.render(layer, position.x, position.y, bmp);
        // This is a container just render it normally 
        back1.render(layer, position.x, position.y, bmp);

        /* Q: how is range.x supposed to be used? isn't it always 0? */
        /* TODO: show the damage number */
        middle.render(layer, position.x, position.y, bmp, (int)(damage * range.y / maxHealth));

        double width = currentHealth * range.y / maxHealth;
        front.render(layer, position.x, position.y, bmp, (int) width);

#if 0
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
#endif
        // Counter Number for powerbars
        counter.render(layer, position.x, position.y, bmp);
    }
}

Face::Face(){
}

Face::~Face(){
}

void Face::act(Character & character){
    face.setSprite(character.getSprite(face.getSpriteData().x,face.getSpriteData().y));
}

void Face::render(const Element::Layer & layer, const Graphics::Bitmap & bmp){
    background.render(layer, position.x, position.y, bmp);
    face.render(layer, position.x, position.y, bmp);
}

Name::Name(){
}

Name::~Name(){
}

void Name::act(Mugen::Character & character){
    font.setText(character.getDisplayName());
}

void Name::render(const Element::Layer & layer, const Graphics::Bitmap & bmp){
    background.render(layer, position.x, position.y, bmp);
    font.render(layer, position.x, position.y, bmp);
}

static void getElementProperties(const Ast::AttributeSimple & simple, const std::string & component, const std::string & elementName, FightElement & element, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts){
    std::string compCopy = component;
    if (!compCopy.empty()){
        compCopy+=".";
    }
    if (simple == compCopy + elementName + ".spr"){
        int g=0, s=0;
        try{
            simple.view() >> g >> s;
        } catch (const Ast::Exception & e){
        }
        element.setSpriteData(g,s);
        element.setSprite(sprites[g][s]);
    } else if (simple == compCopy + elementName + ".anim"){
        int anim;
        simple.view() >> anim;
        element.setAction(animations[anim]);
    } else if ( simple == compCopy + elementName + ".font"){
        int index=0, bank=0, position=0;
        try {
            simple.view() >> index >> bank >> position;
        } catch (const Ast::Exception & e){
            //ignore for now
        }
        element.setFont(fonts[index-1],bank,position);
    } else if (simple == compCopy + elementName + ".facing"){
        int face;
        simple.view() >> face;
        element.setFacing(face);
    } else if (simple == compCopy + elementName + ".vfacing"){
        int face;
        simple.view() >> face;
        element.setVFacing(face);
    } else if (simple == compCopy + elementName + ".layerno"){
        int layer = 0;
        simple.view() >> layer;
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
            simple.view() >> x >> y;
        } catch (const Ast::Exception & e){
        }
        element.setOffset(x,y);
    } else if (simple == compCopy + elementName + ".scale"){
        double x=1, y=1;
        try{
            simple.view() >> x >> y;
        } catch (const Ast::Exception & e){
        }
        element.setScale(x,y);
    } else if (simple == compCopy + elementName + ".displaytime"){
        int time;
        simple.view() >> time;
        element.setDisplayTime(time);
    } else if (simple == compCopy + elementName + ".text"){
        std::string text;
        simple.view() >> text;
        element.setText(text);
    } 
}

GameTime::GameTime():
frameCount(0),
time(Mugen::Data::getInstance().getTime()),
ticker(0),
started(false),
disabled(false){
}

GameTime::~GameTime(){
}

void GameTime::act(){
    if (started){
	ticker++;
	if (disabled){
	    return;
	}
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

void GameTime::render(const Element::Layer & layer, const Graphics::Bitmap & bmp){
    background.render(layer, position.x, position.y, bmp);
    timer.render(layer, position.x, position.y, bmp);
}

void GameTime::reset(){
    // Resets the time
    time = Mugen::Data::getInstance().getTime();
    // 0 or less is considered disabled
    if (time <=0){
        disabled = true;
        // Mugen hides the infinity symbol in the num font and sets it to o
        timer.setText("o");
    }
}

void GameTime::start(){
    started = true;
    ticker = 0;
}

void GameTime::stop(){
    started = false;
}

Combo::Combo():
startOffset(0),
displayTime(0),
player1Ticker(0),
player2Ticker(0),
shake(false),
player1ShakeTime(0),
player2ShakeTime(0),
player1Total(0),
player2Total(0),
player1State(Disabled),
player2State(Disabled){
    combo.setLayer(Element::Top);
    text.setLayer(Element::Top);
}
Combo::~Combo(){
}
void Combo::act(Mugen::Character & player1, Mugen::Character & player2){
    if (player1.getCurrentCombo() >= 2 && player1Total !=player1.getCurrentCombo()){
	if (player1State == Disabled){
	    player1Position = position;
	    player1Position.x += startOffset;
	}
	player1State = Forward;
	if (shake){
	    player1ShakeTime = 10;
	}
	player1Total = player1.getCurrentCombo();
	std::ostringstream str;
	str << player1Total;
        player1Combo = str.str();
	player1Text = replaceString("%i",player1Combo,message);
    }
    if (player2.getCurrentCombo() >= 2 && player2Total !=player2.getCurrentCombo()){
	if (player2State == Disabled){
	    player2Position.set(DEFAULT_WIDTH - position.x, position.y);
	    player2Position.x += abs(startOffset);
	}
	player2State = Forward;
	if (shake){
	    player2ShakeTime = 10;
	}
	player2Total = player2.getCurrentCombo();
	std::ostringstream str;
	str << player2Total;
        player2Combo = str.str();
	player2Text = replaceString("%i",player2Combo,message);
    }
    if (player1ShakeTime > 0){
	player1ShakeTime--;
    }
    if (player2ShakeTime > 0){
        player2ShakeTime--;
    }
    switch (player1State){
	case Forward:
            player1Position.x+=2;
            if (player1Position.x >= position.x){
                player1Position = position;
                player1State = Wait;
                player1Ticker = 0;
            }
	    break;
	case Wait:
	    if (player1Ticker == displayTime){
		player1State = Retracting;
	    }
	    player1Ticker++;
	    break;
	case Retracting:
	    player1Position.x-=2;
            if (player1Position.x <= (position.x + startOffset)){
                player1State = Disabled;
            }
	    break;
	case Disabled:
	default:
	    break;
    }
    switch (player2State){
	case Forward:
            player2Position.x-=2;
            if (player2Position.x <= (DEFAULT_WIDTH - position.x)){
                player2Position.set(DEFAULT_WIDTH - position.x, position.y);
                player2State = Wait;
                player2Ticker = 0;
            }
	    break;
	case Wait:
	    if (player2Ticker == displayTime){
		player2State = Retracting;
	    }
	    player2Ticker++;
	    break;
	case Retracting:
            player2Position.x+=2;
            if (player2Position.x >= (DEFAULT_WIDTH - position.x + abs(startOffset))){
                player2State = Disabled;
            }
	    break;
	case Disabled:
	default:
	    break;
    }
    player1Total = player1.getCurrentCombo();
    player2Total = player2.getCurrentCombo();
}

void Combo::render(const Element::Layer & layer, const Graphics::Bitmap & bmp){
    if (player1State != Disabled){
	const int modifierX = (player1ShakeTime > 0 ? PaintownUtil::rnd( 8 ) - 4 : 0);
	const int modifierY = (player1ShakeTime > 0 ? PaintownUtil::rnd( 8 ) - 4 : 0);
        combo.setText(player1Combo);
        combo.render(layer,player1Position.x + modifierX,player1Position.y + modifierY,bmp);
	// Hacking in adjustment for now, need to get correct width from font which is wrong
        text.setText(player1Text);
	text.render(layer,player1Position.x + combo.getWidth() + (player1Total > 9 ? 10 : 15),player1Position.y,bmp);
    }
    if (player2State != Disabled){
	const int modifierX = (player2ShakeTime > 0 ? PaintownUtil::rnd( 8 ) - 4 : 0);
	const int modifierY = (player2ShakeTime > 0 ? PaintownUtil::rnd( 8 ) - 4 : 0);
	// Hacking in adjustment for now, need to get correct width from font which is wrong
        text.setText(player2Text);
	text.render(layer,player2Position.x - text.getWidth() + (player1Total > 9 ? 10 : 15), player2Position.y,bmp);
        combo.setText(player2Combo);
	combo.render(layer,player2Position.x - text.getWidth() - (player1Total > 9 ? 10 : 15) + modifierX, player2Position.y + modifierY,bmp);
    }
}
	
Round::Round():
state(WaitForIntro),
currentRound(1),
totalDraws(0),
matchWins(0),
matchMaxDrawGames(0),
startWaitTime(0),
roundDisplayTime(0),
fightDisplayTime(0),
controlTime(0),
KODisplayTime(0),
overByKO(false),
slowTime(0),
overWaitTime(0),
overHitTime(0),
overWinTime(0),
overTime(0),
winDisplayTime(0),
roundEnd(false),
winStateSet(false),
ticker(0),
player1Behavior(0),
player2Behavior(0){
    // Set layers for top
    defaultRound.setLayer(Element::Top);
    fight.setLayer(Element::Top);
    KO.setLayer(Element::Top);
    DKO.setLayer(Element::Top);
    TO.setLayer(Element::Top);
    win.setLayer(Element::Top);
    win2.setLayer(Element::Top);
    draw.setLayer(Element::Top);
    // Setup fade tool
    fader.setState(Gui::FadeTool::FadeIn);
    fader.setFadeInTime(15);
    fader.setFadeOutTime(10);
}

Round::~Round(){
    for (std::map< unsigned int, FightElement * >::iterator i = roundSounds.begin(); i != roundSounds.end(); ++i){
	delete i->second;
    }
    for (std::map< unsigned int, FightElement * >::iterator i = rounds.begin(); i != rounds.end(); ++i){
	delete i->second;
    }
}

void Round::doWin(Mugen::Stage & stage, Mugen::Character & winner, Mugen::Character & loser){
    winner.changeState(stage, Mugen::Win);

    WinGame::WinType winType;
    if (overByKO){
        winType = WinGame::Normal;
    } else {
        winType = WinGame::TimeOver;
    }

    // Add win to character
    WinGame win;
    win.type = winType;
    win.perfect = winner.getHealth() == winner.getMaxHealth();
    winner.addWin(win);

    /* If the loser still has health then its a loss by time and they should go into
     * their loss state.
     */
    if (loser.getHealth() > 0){
        loser.changeState(stage, Mugen::Lose);
    }
}

void Round::act(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2){
    switch (state){
	case WaitForIntro:
	    if (ticker >= startWaitTime){
		setState(DisplayIntro, stage, player1, player2);
	    }
	    Global::debug(1) << "Round Ticker: " << ticker << " | Waiting for Intro: " << startWaitTime << endl;
	    break;
	case DisplayIntro:
	    // Check if player states are done with intro move on to next
	    // for now just go ahead and start the round
            if ((player1.getCurrentState() == Mugen::Standing) && (player2.getCurrentState() == Mugen::Standing)){
                setState(WaitForRound,stage,player1,player2);
            }
	    Global::debug(1) << "Round Ticker: " << ticker << " | DisplayIntro "  << endl;
	    break;
	case WaitForRound:
	    if (ticker >= roundDisplayTime && fader.getState() != Gui::FadeTool::FadeIn){
		setState(DisplayRound,stage,player1,player2);
	    }
	    Global::debug(1) << "Round Ticker: " << ticker << " | Wait for round: " << roundDisplayTime << endl;
	    break;
	case DisplayRound:
	    if (getRoundElement().isDone()){
		setState(WaitForFight,stage,player1,player2);
	    }
	    Global::debug(1) << "Round Ticker: " << ticker << " | Playing round. " << endl;
	    break;
	case WaitForFight:
	    if (ticker >= fightDisplayTime){
		setState(DisplayFight,stage,player1,player2);
	    }
	    Global::debug(1) << "Round Ticker: " << ticker << " | Wait for Fight: " << fightDisplayTime << endl;
	    break;
	case DisplayFight:
            if (ticker >= controlTime){
		setState(PlayingGame, stage, player1, player2);
		Global::debug(1) << "Round Ticker: " << ticker << " | Wait for Fight to end." << endl;
	    }
	    Global::debug(1) << "Round Ticker: " << ticker << " | Playing Fight. " << endl;
	    break;
	case PlayingGame:
	    // Evaluate players and then go to the appropriate finish
	    break;
        case RoundOver:
            /*
	    // Set slow time
	    if (ticker < 2){
		// Set slow time
	    }
            */
	    // Remove hittable state
	    if (ticker >= overHitTime){
                //player1.setUnHurtable();
                //player2.setUnHurtable();
            }
	    // Only show KO after slowtime
	    if (ticker < slowTime){
		break;
	    } else if (ticker == slowTime){
		stage.setGameRate(1);
	    }

            /* FIXME: handle ko echo here */

	    // Check KO State
	    if (player1.getHealth() <= 0 && player2.getHealth() <= 0){
		if (ticker >= KODisplayTime){
		    if (DKO.notStarted()){
			DKO.play();
			DKOSound.play();
		    } else if (DKO.isDone()){
                        setState(DoWin, stage, player1, player2);
                    }
		}
	    } else {
		if (ticker >= KODisplayTime){
		    if (KO.notStarted()){
			KO.play();
			KOSound.play();
		    } else if (KO.isDone()){
                        setState(DoWin, stage, player1, player2);
                    }
		}
	    }
            // Check wait time, remove player control
	    if (ticker >= overWaitTime){
                // Remove player control
	        player1.setBehavior(&dummyBehavior);
                player2.setBehavior(&dummyBehavior);
            }
	    break;
	case DoTimeOver:
	    // Run Time Over
	    if (TO.notStarted()){
		TO.play();
		TOSound.play();
	    } else if (TO.isDone()){
                setState(DoWin, stage, player1, player2);
            }
            // Check wait time, remove player control
	    if (ticker >= overWaitTime){
                // Remove player control
	        player1.setBehavior(&dummyBehavior);
                player2.setBehavior(&dummyBehavior);
            }
	    break;
        case DoWin: {
            // Just a precaution
            player1.setBehavior(&dummyBehavior);
            player2.setBehavior(&dummyBehavior);
            // Change into win state
            if (ticker >= overWinTime){
                // Check ko / dko
                if (player1.getHealth() <= 0 && player2.getHealth() <= 0){
                    if (ticker >= overWinTime + winDisplayTime){
                        if (draw.notStarted()){
                            draw.play();
                            drawSound.play();
                            // Increment draw for now, though I think this actually counts as win for both
                            totalDraws++;
                        }
                    }
                } else if (player1.getHealth() == player2.getHealth()){
                    if (!winStateSet){
                        winStateSet = true;
                        std::vector<std::string> vec;
                        player1.changeState(stage, Mugen::Draw, vec);
                        player2.changeState(stage, Mugen::Draw, vec);
                    }
                    if (ticker >= overWinTime + winDisplayTime){
                        if (draw.notStarted()){
                            draw.play();
                            drawSound.play();
                            // Increment draw
                            totalDraws++;
                        }
                    }
                } else {
                    // Set win state
                    if (player1.getHealth() > player2.getHealth() && !winStateSet){
                        winStateSet = true;
                        doWin(stage, player1, player2);
                    } else if (player2.getHealth() > player1.getHealth() && !winStateSet){
                        winStateSet = true;
                        doWin(stage, player2, player1);
                    }

                    if (ticker >= overWinTime + winDisplayTime){
                        if (win.notStarted()){
                            std::string temp;
                            if (player1.getHealth() > player2.getHealth()){
                                temp = replaceString("%s", player1.getDisplayName(), winText);
                            } else if (player2.getHealth() > player1.getHealth()){
                                temp = replaceString("%s", player2.getDisplayName(), winText);
                            }
                            win.setText(temp);
                            win.play();
                            winSound.play();
                        }
                    }
                }
            }
            // End round
            if (ticker >= overTime){
                if (fader.getState() != Gui::FadeTool::FadeOut){
                    fader.setState(Gui::FadeTool::FadeOut);
                } else if (ticker >= overTime + fader.getFadeOutTime()) {
                    currentRound++;
                    stage.reset();
                    player1.roundEnd();
                    player2.roundEnd();
                    // Lets check draws
                    if (matchMaxDrawGames != -1){
                        // Exit match
                        if (totalDraws > matchMaxDrawGames){
                            stage.setMatchOver(true);
                        }
                        // Check current match and draws assign winner / loser info and exit match if needed
                    }
                   
                    if (matchWins != -1){
                        if (player1.getWins().size() >= (unsigned int)matchWins){
                            player1.addMatchWin();
                            // Later on add lose information to other player
                            // Exit match
                            stage.setMatchOver(true);
                            setState(MatchOver, stage, player1, player2);
                        } else if (player2.getWins().size() >= (unsigned int)matchWins){
                            player2.addMatchWin();
                            // Same as above
                            // Exit match
                            stage.setMatchOver(true);
                            setState(MatchOver, stage, player1, player2);
                        }
                    }
                }
            }
            break;
        }
	default:
	    break;
    }
    
    if (state != MatchOver){
        getRoundElement().act();
        getRoundSoundElement().act();
        fight.act();
        KO.act();
        DKO.act();
        TO.act();
        win.act();
        win2.act();
        draw.act();
        ticker++;
        fader.act();
    }
}

void Round::render(const Element::Layer & layer, const Graphics::Bitmap & bmp){
    FightElement & round = getRoundElement();
    if (!round.notStarted() && !round.isDone()){
        round.render(layer, position.x, position.y, bmp);
    }
    if (!fight.notStarted() && !fight.isDone()){
	fight.render(layer, position.x, position.y, bmp);
    }
    if (!KO.notStarted() && !KO.isDone()){
	KO.render(layer, position.x, position.y, bmp);
    }
    if (!DKO.notStarted() && !DKO.isDone()){
	DKO.render(layer, position.x, position.y, bmp);
    }
    if (!TO.notStarted() && !TO.isDone()){
	TO.render(layer, position.x, position.y, bmp);
    }
    if (!win.notStarted() && !win.isDone()){
	win.render(layer, position.x, position.y, bmp);
    }
    if (!win2.notStarted() && !win2.isDone()){
	win2.render(layer, position.x, position.y, bmp);
    }
    if (!draw.notStarted() && !draw.isDone()){
	draw.render(layer, position.x, position.y, bmp);
    }

    if (layer == Element::Top){
        fader.draw(bmp);
    }
}

void Round::reset(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2){
    if (!player1Behavior){
        player1Behavior = player1.getBehavior();
    }
    if (!player2Behavior){
        player2Behavior = player2.getBehavior();
    }
    // Set dummy behaviors until control is given back
    player1.setBehavior(&dummyBehavior);
    player2.setBehavior(&dummyBehavior);
    roundEnd = false;
    winStateSet = false;
    if (currentRound == 1){
        setState(WaitForIntro, stage, player1, player2);
    } else {
        setState(WaitForRound, stage, player1, player2);
    }

    // Reset objects
    getRoundElement().reset();
    getRoundSoundElement().reset();
    fight.reset();
    KO.reset();
    DKO.reset();
    TO.reset();
    win.reset();
    win2.reset();
    draw.reset();
    fader.setState(Gui::FadeTool::FadeIn);
    overByKO = false;
}

void Round::setState(const State & state, Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2){
    this->state = state;
    std::vector<std::string> vec;
    switch (this->state){
	case WaitForIntro:
            player1.changeState(stage, Mugen::Initialize, vec);
	    player2.changeState(stage, Mugen::Initialize, vec);
	    break;
	case DisplayIntro:
            /* The initialize state (5900) will call a change state controller
             * to change to Intro (190) so there doesn't seem to be a need
             * to force the characters to change to their intro's.
             * Look in common1.cns
             */
            /*
            player1.changeState(stage, Mugen::Intro, vec);
            player2.changeState(stage, Mugen::Intro, vec);
            */
	    break;
	case WaitForRound:
            player1.changeState(stage, Mugen::Standing, vec);
            player2.changeState(stage, Mugen::Standing, vec);
	    break;
	case DisplayRound:
            {
                FightElement & element = getRoundElement();
                FightElement & soundElement = getRoundSoundElement();
                if (element.notStarted()){
	    	    ostringstream str;
		    str << currentRound;
		    std::string temp = replaceString("%i",str.str(),defaultText);
		    Global::debug(1) << "Displaying Round info: " << temp << endl;
		    element.setText(temp);
		    element.play();
		    soundElement.play();
	        } 
            }
	    break;
	case WaitForFight:
            break;
	case DisplayFight:
            if (fight.notStarted()){
		fight.play();
		fightSound.play();
	    }
	    break;
	case PlayingGame:
	    // Give control back
            player1.setBehavior(player1Behavior);
            player2.setBehavior(player2Behavior);
	    break;
	case RoundOver:
	    roundEnd = true;
            overByKO = true;
	    // Start slow time
	    stage.setGameRate(0.4);
	    break;
        case DoTimeOver:
            overByKO = false;
            break;
	default:
	    break;
    }
    ticker = 0;
}

FightElement & Round::getRoundSound(unsigned int number){
    std::map< unsigned int, FightElement *>::iterator element = roundSounds.find(number);
    if (element == roundSounds.end()){
        roundSounds[number] = new FightElement();
    }
    return *this->roundSounds[number];
}

void Round::setRoundSoundTime(int time){
    for (std::map< unsigned int, FightElement * >::iterator i = roundSounds.begin(); i != roundSounds.end(); ++i){
	i->second->setSoundTime(time);
    }
}

FightElement & Round::getRound(unsigned int number){
    std::map< unsigned int, FightElement *>::iterator element = rounds.find(number);
    if (element == rounds.end()){
        rounds[number] = new FightElement();
        rounds[number]->setLayer(Element::Top);
    }
    return *this->rounds[number];
}

//! Get current stage element in use
FightElement & Round::getRoundElement(){
    const unsigned int round = currentRound;
    std::map< unsigned int, FightElement * >::iterator element = rounds.find(round);
    if (element != rounds.end()){
        if (element->second->isSet()){
            return *element->second;
        }
    }
    return defaultRound;
}

FightElement & Round::getRoundSoundElement(){
    const unsigned int round = currentRound;
    std::map< unsigned int, FightElement * >::iterator element = roundSounds.find(round);
    if (element != roundSounds.end()){
        if (element->second->isSet()){
            return *element->second;
        }
    }   
    return defaultRoundSound;
}

WinIcon::WinIcon():
useIconUpTo(0){
}
WinIcon::~WinIcon(){
    for (std::map<WinGame::WinType, FightElement *>::iterator i =  player1Icons.begin(); i != player1Icons.end(); ++i){
	if (i->second){
	    delete i->second;
	}
    }
    for (std::map<WinGame::WinType, FightElement *>::iterator i =  player2Icons.begin(); i != player2Icons.end(); ++i){
	if (i->second){
	    delete i->second;
	}
    }
}
void WinIcon::act(const Character & p1, const Character & p2){
    player1Wins = p1.getWins();
    player2Wins = p2.getWins();
    for (std::map<WinGame::WinType, FightElement *>::iterator i =  player1Icons.begin(); i != player1Icons.end(); ++i){
	if (i->second){
	    i->second->act();
	}
    }
    for (std::map<WinGame::WinType, FightElement *>::iterator i =  player2Icons.begin(); i != player2Icons.end(); ++i){
	if (i->second){
	    i->second->act();
	}
    }
}
void WinIcon::render(const Element::Layer & layer, const Graphics::Bitmap &bmp){
    if (player1Wins.size() < useIconUpTo){
	Mugen::Point position = player1Position;
	for (std::vector<WinGame>::const_iterator i = player1Wins.begin(); i != player1Wins.end(); ++i){
            const WinGame & win = *i;
	    FightElement & element = getPlayer1Win(win.type);
	    element.render(layer, position.x, position.y, bmp);
            if (win.perfect){
                FightElement & perfect = getPlayer1Win(WinGame::Perfect);
                perfect.render(layer, position.x, position.y, bmp);
            }
	    position.x += player1Offset.x;
	    position.y += player1Offset.y;
	}
    } else {
	ostringstream str;
	str << player1Wins.size();
	player1Counter.setText(str.str());
	player1Counter.render(layer, player1Position.x, player1Position.y, bmp);
    }
    if (player2Wins.size() < useIconUpTo){
	Mugen::Point position = player2Position;
	for (std::vector<WinGame>::const_iterator i = player2Wins.begin(); i != player2Wins.end(); ++i){
            const WinGame & win = *i;
	    FightElement & element = getPlayer2Win(win.type);
	    element.render(layer, position.x, position.y, bmp);
            if (win.perfect){
                FightElement & perfect = getPlayer2Win(WinGame::Perfect);
                perfect.render(layer, position.x, position.y, bmp);
            }
	    position.x += player2Offset.x;
	    position.y += player2Offset.y;
	}
    } else {
	ostringstream str;
	str << player2Wins.size();
	player2Counter.setText(str.str());
	player2Counter.render(layer, player2Position.x, player2Position.y, bmp);
    }
}

FightElement &WinIcon::getPlayer1Win(const WinGame::WinType &win){
	std::map<WinGame::WinType, FightElement *>::iterator icon = player1Icons.find(win);
	if (icon == player1Icons.end()){
		player1Icons[win] = new FightElement();
	}
	return *player1Icons[win];
}

FightElement &WinIcon::getPlayer2Win(const WinGame::WinType &win){
	std::map<WinGame::WinType, FightElement *>::iterator icon = player2Icons.find(win);
	if (icon == player2Icons.end()){
		player2Icons[win] = new FightElement();
	}
	return *player2Icons[win];
}

GameInfo::GameInfo(const Filesystem::AbsolutePath & fightFile){
    Filesystem::AbsolutePath baseDir = fightFile.getDirectory();
    const Filesystem::AbsolutePath ourDefFile = Mugen::Util::fixFileName(baseDir, fightFile.getFilename().path());
    
    if (ourDefFile.isEmpty()){
        throw MugenException("Cannot locate fight definition file for: " + fightFile.path(), __FILE__, __LINE__);
    }
    
    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(ourDefFile.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile.path() + " in" + diff.printTime("") << endl;

    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
    if (head == "Files"){
        class FileWalk: public Ast::Walker{
            public:
                FileWalk(const Filesystem::AbsolutePath & baseDir, GameInfo & self):
                    baseDir(baseDir),
                    self(self){
                    }
                const Filesystem::AbsolutePath & baseDir;
                GameInfo & self;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "sff"){
                        std::string sff;
                        simple.view() >> sff;
                        Global::debug(1) << "Got Sprite File: '" << sff << "'" << endl;
                        Util::readSprites(Util::findFile(Filesystem::RelativePath(sff)), Filesystem::AbsolutePath(), self.sprites, true);
                        /*
                        for( Mugen::SpriteMap::iterator i = self.sprites.begin() ; i != self.sprites.end() ; ++i ){
                            // Load these sprites so they are ready to use
                            for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
                                if (j->second){
                                    j->second->load();
                                }
                            }
                        }
                        */
                    } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                        string path;
                        simple.view() >> path;
                        self.fonts.push_back(new MugenFont(Util::findFile(Filesystem::RelativePath(path))));
                        Global::debug(1) << "Got Font File: '" << path << "'" << endl;
                    } else if (simple == "snd"){
                        string temp;
                        simple.view() >> temp;
                        Util::readSounds(Util::findFile(Filesystem::RelativePath(temp)), self.sounds);
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
                BarWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts):
                    self(self),
                    sprites(sprites),
                    animations(animations),
                    fonts(fonts){
                    }

                GameInfo & self;
                Mugen::SpriteMap & sprites;
                std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
                std::vector<MugenFont *> & fonts;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (PaintownUtil::matchRegex(simple.toString(), "p1")){
                        getBar(simple,"p1", self.player1LifeBar);
                    } else if (PaintownUtil::matchRegex(simple.toString(), "p2")){
                        getBar(simple,"p2", self.player2LifeBar);
                    }
                }

                void getBar(const Ast::AttributeSimple & simple, const std::string & component, Bar & bar){
                    bar.setType(Bar::Health);
                    if (simple == component + ".pos"){
                        int x=0, y=0;
                        try{
                            simple.view() >> x >> y;
                        } catch (const Ast::Exception & e){
                        }
                        bar.setPosition(x,y);
                    } else if (simple == component + ".range.x"){
                        int x = 0, y = 0;
                        try{
                            simple.view() >> x >> y;
                        } catch (const Ast::Exception & e){
                        }
                        bar.setRange(x, y);
                    }

                    getElementProperties(simple, component, "bg0", bar.getBack0(), sprites, animations, fonts);
                    getElementProperties(simple, component, "bg1", bar.getBack1(), sprites, animations, fonts);
                    getElementProperties(simple, component, "mid", bar.getMiddle(), sprites, animations, fonts);
                    getElementProperties(simple, component, "front", bar.getFront(), sprites, animations, fonts);
                }
        };

        BarWalk walk(*this, sprites, animations, fonts);
        section->walk(walk);
    } else if (head == "Powerbar"){
            class BarWalk: public Ast::Walker{
                public:
                    BarWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts, Mugen::SoundMap & sounds):
                        self(self),
                        sprites(sprites),
                        animations(animations),
                        fonts(fonts),
                        sounds(sounds){
                        }

                    GameInfo & self;
                    Mugen::SpriteMap & sprites;
                    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
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
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.player1PowerBar.getLevel1Sound().setSound(sounds[g][s]);
                            self.player2PowerBar.getLevel1Sound().setSound(sounds[g][s]);
                        } else if (simple == "level2.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.player1PowerBar.getLevel2Sound().setSound(sounds[g][s]);
                            self.player2PowerBar.getLevel2Sound().setSound(sounds[g][s]);
                        } else if (simple == "level3.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.player1PowerBar.getLevel3Sound().setSound(sounds[g][s]);
                            self.player2PowerBar.getLevel3Sound().setSound(sounds[g][s]);
                        }
                    }

                    void getBar(const Ast::AttributeSimple & simple, const std::string & component, Bar & bar){
                        bar.setType(Bar::Power);
                        if (simple == component + ".pos"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            bar.setPosition(x,y);
                        } else if (simple == component + ".range.x"){
                            int x=0,y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            bar.setRange(x,y);
                        }
                        getElementProperties(simple,component,"bg0", bar.getBack0(), sprites, animations,fonts);
                        getElementProperties(simple,component,"bg1", bar.getBack1(),sprites,animations,fonts);
                        getElementProperties(simple,component,"mid", bar.getMiddle(),sprites,animations,fonts);
                        getElementProperties(simple,component,"front", bar.getFront(),sprites,animations,fonts);
                        getElementProperties(simple,component,"counter", bar.getCounter(),sprites,animations,fonts);
                    }
            };

            BarWalk walk(*this, sprites, animations, fonts, sounds);
            section->walk(walk);
        } else if (head == "Face"){
            class FaceWalk: public Ast::Walker{
                public:
                    FaceWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts):
                        self(self),
                        sprites(sprites),
                        animations(animations),
                        fonts(fonts){
                        }
                    GameInfo & self;
                    Mugen::SpriteMap & sprites;
                    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
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
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            face.setPosition(x,y);
                        }
                        getElementProperties(simple,component,"bg", face.getBackground(), sprites, animations,fonts);
                        getElementProperties(simple,component,"face", face.getFace(), sprites, animations,fonts);
                    }
            };

            FaceWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        } else if (head == "Name"){
            class NameWalk: public Ast::Walker{
                public:
                    NameWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts):
                        self(self),
                        sprites(sprites),
                        animations(animations),
                        fonts(fonts){
                        }
                    GameInfo & self;
                    Mugen::SpriteMap & sprites;
                    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
                    std::vector<MugenFont *> & fonts;
                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (PaintownUtil::matchRegex(simple.toString(), "p1")){
                            getName(simple,"p1", self.player1Name);
                        } else if (PaintownUtil::matchRegex(simple.toString(), "p2")){
                            getName(simple,"p2", self.player2Name);
                        }
                    }
                    void getName(const Ast::AttributeSimple & simple, const std::string & component, Name & name){
                        if (simple == component + ".pos"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
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
                    TimeWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts):
                        self(self),
                        sprites(sprites),
                        animations(animations),
                        fonts(fonts){
                        }

                    GameInfo & self;
                    Mugen::SpriteMap & sprites;
                    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
                    std::vector<MugenFont *> & fonts;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "pos"){
                            int x = 0, y = 0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.timer.setPosition(x,y);
                        } else if (simple == "framespercount"){
                            /* its very strange for this not to be 60, maybe print
                             * a warning if its anything other than 60?
                             */
                            int x = 0;
                            simple.view() >> x;
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
                    ComboWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts):
                        self(self),
                        sprites(sprites),
                        animations(animations),
                        fonts(fonts){
                        }
                    GameInfo & self;
                    Mugen::SpriteMap & sprites;
                    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
                    std::vector<MugenFont *> & fonts;
                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "pos"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.combo.setPosition(x,y);
                        } else if (simple == "start.x"){
                            int x=0;
                            simple.view() >> x;
                            self.combo.setStartOffset(x);
                        } else if (simple == "counter.shake"){
                            bool shake;
                            simple.view() >> shake;
                            self.combo.setShake(shake);
                        } else if (simple == "text.text"){
                            std::string text;
                            simple.view() >> text;
                            self.combo.setMessage(text);
                        } else if (simple == "displaytime"){
                            int time;
                            simple.view() >> time;
                            self.combo.setDisplayTime(time);
                        } 
                        getElementProperties(simple,"","counter", self.combo.getCombo(),sprites,animations,fonts);
                        getElementProperties(simple,"","text", self.combo.getText(),sprites,animations,fonts);
                    }
            };

            ComboWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        } else if (head == "Round"){
            // Sound time to apply to round sounds
            int soundTime = 0;
            class RoundWalk: public Ast::Walker{
                public:
                    RoundWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts, int & soundTime):
                        self(self),
                        sprites(sprites),
                        animations(animations),
                        fonts(fonts),
                        soundTime(soundTime){
                        }
                    GameInfo & self;
                    Mugen::SpriteMap & sprites;
                    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
                    std::vector<MugenFont *> & fonts;
                    int & soundTime;
                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "match.wins"){
                            int wins;
                            simple.view() >> wins;
                            self.roundControl.setMatchWins(wins);
                        } else if (simple == "match.maxdrawgames"){
                            int draws;
                            simple.view() >> draws;
                            self.roundControl.setMatchMaxDrawGames(draws);
                        } else if (simple == "start.waittime"){
                            int time = 0;
                            simple.view() >> time;
                            self.roundControl.setStartWaitTime(time);
                        } else if (simple == "pos"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.setPosition(x,y);
                        } else if (simple == "round.time"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setRoundDisplayTime(time);
                        } else if (simple == "round.default.text"){
                            std::string text;
                            simple.view() >> text;
                            self.roundControl.setDefaultText(text);
                        } else if (simple == "fight.time"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setFightDisplayTime(time);
                        } else if (simple == "KO.time"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setKODisplayTime(time);
                        } else if (simple == "win.time"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setWinDisplayTime(time);
                        } else if (simple == "slow.time"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setSlowTime(time);
                        } else if (simple == "over.waittime"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setOverWaitTime(time);
                        } else if (simple == "over.hittime"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setOverHitTime(time);
                        } else if (simple == "over.time"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setOverTime(time);
                        } else if (simple == "ctrl.time"){
                            int time=0;
                            simple.view() >> time;
                            self.roundControl.setControlTime(time);
                        } else if (simple == "round.sndtime"){
                            int time = 0;
                            simple.view() >> soundTime;
                        } else if (simple == "fight.sndtime"){
                            int time = 0;
                            simple.view() >> time;
                            self.roundControl.getFightSound().setSoundTime(time);
                        } else if (simple == "KO.sndtime"){
                            int time = 0;
                            simple.view() >> time;
                            self.roundControl.getKOSound().setSoundTime(time);
                            self.roundControl.getDKOSound().setSoundTime(time);
                            self.roundControl.getTOSound().setSoundTime(time);
                        } else if (simple == "fight.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.getFightSound().setSound(self.sounds[g][s]);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^fight.")){
                            getElementProperties(simple,"","fight", self.roundControl.getFight(), sprites, animations,fonts);
                        } else if (simple == "ko.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.getKOSound().setSound(self.sounds[g][s]);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^KO.")){
                            getElementProperties(simple,"","ko", self.roundControl.getKO(),sprites,animations,fonts);
                        } else if (simple == "dko.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.getDKOSound().setSound(self.sounds[g][s]);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^DKO.")){
                            getElementProperties(simple,"","dko", self.roundControl.getDKO(),sprites,animations,fonts);
                        } else if (simple == "to.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.getTOSound().setSound(self.sounds[g][s]);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^TO.")){
                            getElementProperties(simple,"","to", self.roundControl.getTO(),sprites,animations,fonts);
                        } else if (simple == "win.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.getWinSound().setSound(self.sounds[g][s]);
                        } else if (simple == "win.text"){
                            std::string text;
                            simple.view() >> text;
                            self.roundControl.setWinText(text);
                        } else if (simple == "win2.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.getWin2Sound().setSound(self.sounds[g][s]);
                        } else if (simple == "win2.text"){
                            std::string text;
                            simple.view() >> text;
                            self.roundControl.setWin2Text(text);
                        } else if (simple == "draw.snd"){
                            int g=0,s=0;
                            try{
                                simple.view() >> g >> s;
                            } catch (const Ast::Exception & e){
                            }
                            self.roundControl.getDrawSound().setSound(self.sounds[g][s]);
                        }
                        if (PaintownUtil::matchRegex(simple.toString(),"^win.")){
                            getElementProperties(simple,"","win", self.roundControl.getWin(),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^win2.")){
                            getElementProperties(simple,"","win2", self.roundControl.getWin2(),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^draw.")){
                            getElementProperties(simple,"","draw", self.roundControl.getDraw(),sprites,animations,fonts);
                        }
                        getElementProperties(simple,"round","default", self.roundControl.getDefaultRound(),sprites,animations,fonts);
                        if (PaintownUtil::matchRegex(simple.toString(), "round[0-9]+")){
                            std::string temp = PaintownUtil::captureRegex(simple.toString(), "round([0-9]+)",0);
                            int num = atoi(temp.c_str());
                            ostringstream str;
                            str << "round" << num;
                            if (simple == str.str() + ".snd"){
                                int g=0,s=0;
                                try{
                                    simple.view() >> g >> s;
                                } catch (const Ast::Exception & e){
                                }
                                self.roundControl.getRoundSound(num).setSound(self.sounds[g][s]);
                            } else if (PaintownUtil::matchRegex(simple.toString(),"^"+str.str())){
                                getElementProperties(simple,"",str.str(), self.roundControl.getRound(num),sprites,animations,fonts);
                            }
                        }
                    }
            };

            RoundWalk walk(*this, sprites, animations, fonts, soundTime);
            section->walk(walk);

            // Set sound time
            roundControl.setRoundSoundTime(soundTime);
        } else if (head == "WinIcon"){
            class WinIconWalk: public Ast::Walker{
                public:
                    WinIconWalk(GameInfo & self, Mugen::SpriteMap & sprites, std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations, std::vector<MugenFont *> & fonts):
                        self(self),
                        sprites(sprites),
                        animations(animations),
                        fonts(fonts){
                        }
                    GameInfo & self;
                    Mugen::SpriteMap & sprites;
                    std::map<int, PaintownUtil::ReferenceCount<MugenAnimation> > & animations;
                    std::vector<MugenFont *> & fonts;
                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "p1.pos"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.winIconDisplay.setPlayer1Position(x,y);
                        } else if (simple == "p2.pos"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.winIconDisplay.setPlayer2Position(x,y);
                        } else if (simple == "p1.iconoffset"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.winIconDisplay.setPlayer1Offset(x,y);
                        } else if (simple == "p2.iconoffset"){
                            int x=0, y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.winIconDisplay.setPlayer2Offset(x,y);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.counter.")){
                            getElementProperties(simple,"p1","counter", self.winIconDisplay.getPlayer1Counter(),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.counter.")){
                            getElementProperties(simple,"p2","counter", self.winIconDisplay.getPlayer1Counter(),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.n.")){
                            getElementProperties(simple,"p1","n", self.winIconDisplay.getPlayer1Win(WinGame::Normal),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.n.")){
                            getElementProperties(simple,"p2","n", self.winIconDisplay.getPlayer2Win(WinGame::Normal),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.s.")){
                            getElementProperties(simple,"p1","s", self.winIconDisplay.getPlayer1Win(WinGame::Special),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.s.")){
                            getElementProperties(simple,"p2","s", self.winIconDisplay.getPlayer2Win(WinGame::Special),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.h.")){
                            getElementProperties(simple,"p1","h", self.winIconDisplay.getPlayer1Win(WinGame::Hyper),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.h.")){
                            getElementProperties(simple,"p2","h", self.winIconDisplay.getPlayer2Win(WinGame::Hyper),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.throw.")){
                            getElementProperties(simple,"p1","throw", self.winIconDisplay.getPlayer1Win(WinGame::NormalThrow),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.throw.")){
                            getElementProperties(simple,"p2","throw", self.winIconDisplay.getPlayer2Win(WinGame::NormalThrow),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.c.")){
                            getElementProperties(simple,"p1","c", self.winIconDisplay.getPlayer1Win(WinGame::Cheese),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.c.")){
                            getElementProperties(simple,"p2","c", self.winIconDisplay.getPlayer2Win(WinGame::Cheese),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.t.")){
                            getElementProperties(simple,"p1","t", self.winIconDisplay.getPlayer1Win(WinGame::TimeOver),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.t.")){
                            getElementProperties(simple,"p2","t", self.winIconDisplay.getPlayer2Win(WinGame::TimeOver),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.suicide.")){
                            getElementProperties(simple,"p1","suicide", self.winIconDisplay.getPlayer1Win(WinGame::Suicide),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.suicide.")){
                            getElementProperties(simple,"p2","suicide", self.winIconDisplay.getPlayer2Win(WinGame::Suicide),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.teammate.")){
                            getElementProperties(simple,"p1","teammate", self.winIconDisplay.getPlayer1Win(WinGame::Teammate),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.teammate.")){
                            getElementProperties(simple,"p2","teammate", self.winIconDisplay.getPlayer2Win(WinGame::Teammate),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p1.perfect.")){
                            getElementProperties(simple,"p1","perfect", self.winIconDisplay.getPlayer1Win(WinGame::Perfect),sprites,animations,fonts);
                        } else if (PaintownUtil::matchRegex(simple.toString(),"^p2.perfect.")){
                            getElementProperties(simple,"p2","perfect", self.winIconDisplay.getPlayer2Win(WinGame::Perfect),sprites,animations,fonts);
                        } else if (simple == "useiconupto"){
                            int num;
                            simple.view() >> num;
                            self.winIconDisplay.setUseIconUpTo(num);
                        }
                    }
            };

            WinIconWalk walk(*this,sprites,animations,fonts);
            section->walk(walk);
        } 
    }
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

    animations.clear();

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

void GameInfo::act(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2){
    player1LifeBar.act(player1);
    player2LifeBar.act(player2);
    player1PowerBar.act(player1);
    player2PowerBar.act(player2);
    player1Face.act(player1);
    player2Face.act(player2);
    player1Name.act(player1);
    player2Name.act(player2);
    timer.act();
    combo.act(player1, player2);
    roundControl.act(stage, player1, player2);
    winIconDisplay.act(player1, player2);
    if (roundControl.getState() == Round::PlayingGame){
        if (!timer.isStarted()){
            timer.reset();
            timer.start();
        }
        if (timer.isStarted() && timer.hasExpired()){
            roundControl.setState(Round::DoTimeOver, stage, player1, player2);
            timer.stop();
        }
    }
    if ((player1.getHealth() <=0 || player2.getHealth() <=0) && !roundControl.isRoundOver()){
        roundControl.setState(Round::RoundOver, stage, player1, player2);
        timer.stop();
    }
}

void GameInfo::render(const Element::Layer & layer, const Graphics::Bitmap &bmp){
    player1LifeBar.render(layer,bmp);

    // Program received signal SIGFPE, Arithmetic exception.
    player2LifeBar.render(layer,bmp);

    player1PowerBar.render(layer, bmp);
    player2PowerBar.render(layer, bmp);

    player1Face.render(layer, bmp);
    player2Face.render(layer, bmp);

    player1Name.render(layer, bmp);
    player2Name.render(layer, bmp);

    timer.render(layer, bmp);
    combo.render(layer, bmp);
    roundControl.render(layer, bmp);
    winIconDisplay.render(layer, bmp);
}

void GameInfo::reset(Mugen::Stage & stage, Mugen::Character & player1, Mugen::Character & player2){
    timer.reset();
    roundControl.reset(stage, player1, player2);
}

void GameInfo::parseAnimations(const AstRef & parsed){
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
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
            animations[h] = Mugen::Util::getAnimation(section, sprites, true);
        } 
    }
}
