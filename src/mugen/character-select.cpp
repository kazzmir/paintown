#include "character-select.h"

#include <iostream>
#include <exception>
#include <thread>

#include "ast/all.h"
#include "sound.h"
#include "config.h"
#include "util.h"

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/timedifference.h"
#include "r-tech1/input/input.h"
#include "r-tech1/input/input-manager.h"

using namespace Mugen;

/* Default size of the select screen. Make these variables to support hi-res */
static const int DEFAULT_SELECT_WIDTH = 320;
static const int DEFAULT_SELECT_HEIGHT = 240;

// Cell static members
Mugen::Effects Cell::effects = Mugen::Effects();
int Cell::offsetX = 0;
int Cell::offsetY = 0;

bool TeamMenu::wrapping = false;

int Player::randomSwitchTime = 4;

FontHandler::FontHandler():
state(Normal),
x(0),
y(0),
ticker(0),
blinkTime(10){
}

FontHandler::~FontHandler(){
}

void FontHandler::act(){
    switch(state){
        case Blink:
            ticker++;
            if (ticker >= (blinkTime*2)){
                ticker = 0;
            }
            break;
        case Normal:
        case Done:
        default:
            break;
    }
}

void FontHandler::draw(const Graphics::Bitmap & work, int offset){
    draw(text, work, offset);
}

void FontHandler::draw(const std::string & text, const Graphics::Bitmap & work, int offset){
    switch(state){
        default:
        case Normal:
            active.draw(x, y + offset, text, work);
            break;
        case Blink:
            if (ticker < blinkTime){
                active.draw(x, y + offset, text, work);
            } else if (ticker >= blinkTime){
                active2.draw(x, y + offset, text, work);
            }
            break;
        case Done:
            done.draw(x, y + offset, text, work);
            break;
    }
}

Cell::Cell(unsigned int index, const Gui::SelectListInterface * parent, const PaintownUtil::ReferenceCount<Sprite> & background, const PaintownUtil::ReferenceCount<Sprite> & randomIcon):
background(background),
randomIcon(randomIcon),
index(index),
parent(parent),
state(Empty),
flash(0){
}

Cell::~Cell(){
}

void Cell::act(){
    if (flash){
        flash--;
    }
}

void Cell::draw(int x, int y, int width, int height, const Graphics::Bitmap & work, const ::Font & font) const{
    if (background != NULL){
        background->render(x, y, work);
    }
    
    switch (state){
        case Random: {
            if (randomIcon != NULL){
                randomIcon->render(x + offsetX, y + offsetY, work, effects);
            }
            break;
        }
        case Empty: {
            break;
        }
        case Character: {
            character.drawIcon(x + offsetX, y + offsetY, work, effects);
            break;
        }
        case Unused: {
            /* FIXME: ??? */
            break;
        }
    }
    
    /*
    if (parent->getCurrentIndex(0) == parent->getCurrentIndex(1) &&
        parent->getCurrentIndex(0) == index){
        // Blink cursors
        if (parent->getCurrentState(0) == Gui::SelectListInterface::Active &&
            parent->getCurrentState(1) == Gui::SelectListInterface::Active){
            drawPlayer1Cursor(x, y, work);
            drawPlayer2Cursor(x, y, work, true);
        } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Active &&
                   parent->getCurrentState(1) == Gui::SelectListInterface::Done){
            drawPlayer2Cursor(x, y, work);
            drawPlayer1Cursor(x, y, work);
        } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Done &&
                   parent->getCurrentState(1) == Gui::SelectListInterface::Active){
            drawPlayer1Cursor(x, y, work);
            drawPlayer2Cursor(x, y, work);
        } else {
            drawPlayer2Cursor(x, y, work);
            drawPlayer1Cursor(x, y, work);
        }
    } else if (parent->getCurrentIndex(0) == index){
        drawPlayer1Cursor(x, y, work);
    } else if (parent->getCurrentIndex(1) == index){
        drawPlayer2Cursor(x, y, work);
    }
    */
    
    // Flash
    if (flash){
        // FIXME hot pink shows up after white
        // Graphics::Bitmap::transBlender( 0, 0, 0, int(25.5 * flash) );
        work.translucent(int(25.5 * flash)).rectangleFill(x-1, y-1, x-1+width, y-1+height, Graphics::makeColor(255,255,255));
    }
}

void Cell::setUnused(){
    this->state = Unused;
}

void Cell::setRandom(){
    this->state = Random;
}

void Cell::setEmpty(){
    this->state = Empty;
}

void Cell::setCharacter(const Mugen::ArcadeData::CharacterInfo & character){
    this->character = character;
    this->state = Character;
}

void Cell::select(){
    flash = 10;
}

void Cell::setBackground(PaintownUtil::ReferenceCount<Sprite> background){
    background = background;
}

void Cell::setRandomIcon(PaintownUtil::ReferenceCount<Sprite> randomIcon){
    randomIcon = randomIcon;
}

/*
void Cell::drawPlayer1Cursor(int x, int y, const Graphics::Bitmap & work) const {
    if (parent->getCurrentState(0) == Gui::SelectListInterface::Disabled){
    } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Active){
        if (player1ActiveCursor != NULL){
            player1ActiveCursor->render(x, y, work);
        }
    } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Done){
        if (player1DoneCursor != NULL){
            player1DoneCursor->render(x, y, work);
        }
    }
}

void Cell::drawPlayer2Cursor(int x, int y, const Graphics::Bitmap & work, bool blink) const {
    if (parent->getCurrentState(1) == Gui::SelectListInterface::Disabled){
    } else if (parent->getCurrentState(1) == Gui::SelectListInterface::Active){
        if (blinkCursor && blink){
            // Blink
            blinkTime++;
            if (blinkTime < 10){
                if (player2ActiveCursor != NULL){
                    player2ActiveCursor->render(x, y, work);
                }
            } else if (blinkTime >= 20){
                blinkTime=0;
            }
        } else {
            if (player2ActiveCursor != NULL){
                player2ActiveCursor->render(x, y, work);
            }
        }
    } else if (parent->getCurrentState(1) == Gui::SelectListInterface::Done){
        if (player2DoneCursor != NULL){
            player2DoneCursor->render(x, y, work);
        }
    }
}
*/

TeamMenu::TeamMenu():
current(Mugen::ArcadeData::CharacterCollection::Simultaneous),
turns(Mugen::ArcadeData::CharacterCollection::Turns2),
x(0),
y(0),
background(PaintownUtil::ReferenceCount<Sprite>(NULL)),
itemOffsetX(0),
itemOffsetY(0),
itemSpacingX(0),
itemSpacingY(0),
valueIconOffsetX(0),
valueIconOffsetY(0),
icon(PaintownUtil::ReferenceCount<Sprite>(NULL)),
emptyValueIconOffsetX(0),
emptyValueIconOffsetY(0),
emptyIcon(PaintownUtil::ReferenceCount<Sprite>(NULL)),
valueSpacingX(0),
valueSpacingY(0),
enabled(false){
    itemCurrentFont.setState(FontHandler::Blink);
}

TeamMenu::~TeamMenu(){
}

void TeamMenu::act(){
    if (!enabled){
        return;
    }
    itemCurrentFont.act();
}

void TeamMenu::draw(const Graphics::Bitmap & work, bool enemy){
    if (!enabled){
        return;
    }
    // Set location
    int currentX = x, currentY = y;
    if (background != NULL){
        background->render(currentX, currentY, work);
    }
    // Regular font or enemy ?
    if (!enemy){
        titleFont.setLocation(currentX, currentY);
        titleFont.draw(work);
    } else {
        enemyTitleFont.setLocation(currentX, currentY);
        enemyTitleFont.draw(work);
    }
    
    // First item (single)
    currentX+=itemOffsetX;
    currentY+=itemOffsetY;
    if (current == Mugen::ArcadeData::CharacterCollection::Single){
        itemCurrentFont.setLocation(currentX, currentY);
        itemCurrentFont.draw("Single", work);
    } else {
        itemFont.setLocation(currentX, currentY);
        itemFont.draw("Single", work);
    }
    
    // Second item (simultaneous)
    currentX+=itemSpacingX;
    currentY+=itemSpacingY;
    if (current == Mugen::ArcadeData::CharacterCollection::Simultaneous){
        itemCurrentFont.setLocation(currentX, currentY);
        itemCurrentFont.draw("Simul", work);
    } else {
        itemFont.setLocation(currentX, currentY);
        itemFont.draw("Simul", work);
    }
    if (emptyIcon != NULL){
        // Empty icons draw two
        int emptyX = currentX + emptyValueIconOffsetX, emptyY = currentY + emptyValueIconOffsetY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
    }
    if (icon != NULL){
        // Icons draw two for Simul
        int valueX = currentX + valueIconOffsetX, valueY = currentY + valueIconOffsetY;
        icon->render(valueX, valueY, work);
        valueX += valueSpacingX;
        valueY += valueSpacingY;
        icon->render(valueX, valueY, work);
    }
    // Third item (Turns)
    currentX+=itemSpacingX;
    currentY+=itemSpacingY;
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
        case Mugen::ArcadeData::CharacterCollection::Turns3:
        case Mugen::ArcadeData::CharacterCollection::Turns4:
            itemCurrentFont.setLocation(currentX, currentY);
            itemCurrentFont.draw("Turns", work);
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            itemFont.setLocation(currentX, currentY);
            itemFont.draw("Turns", work);
            break;
    }
    if (emptyIcon != NULL){
        // Empty icons draw four
        int emptyX = currentX + emptyValueIconOffsetX, emptyY = currentY + emptyValueIconOffsetY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
        emptyX += valueSpacingX;
        emptyY += valueSpacingY;
        emptyIcon->render(emptyX, emptyY, work);
    }
    if (icon != NULL){
        // Icons draw two by defualt
        int valueX = currentX + valueIconOffsetX, valueY = currentY + valueIconOffsetY;
        icon->render(valueX, valueY, work);
        valueX += valueSpacingX;
        valueY += valueSpacingY;
        icon->render(valueX, valueY, work);
        // Check which position it is on
        switch (turns){
            case Mugen::ArcadeData::CharacterCollection::Turns3:
                valueX += valueSpacingX;
                valueY += valueSpacingY;
                icon->render(valueX, valueY, work);
                break;
            case Mugen::ArcadeData::CharacterCollection::Turns4:
                valueX += valueSpacingX;
                valueY += valueSpacingY;
                icon->render(valueX, valueY, work);
                valueX += valueSpacingX;
                valueY += valueSpacingY;
                icon->render(valueX, valueY, work);
                break;
            case Mugen::ArcadeData::CharacterCollection::Turns2:
            default:
                break;
        }
    }
}

bool TeamMenu::up(){
    if (!enabled){
        return false;
    }
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Single:
            if (wrapping){
                current = Mugen::ArcadeData::CharacterCollection::Turns2;
                return true;
            }
            break;
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
            current = Mugen::ArcadeData::CharacterCollection::Single;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            current = Mugen::ArcadeData::CharacterCollection::Simultaneous;
            return true;
            break;
        default:
            break;
    };
    return false;
}

bool TeamMenu::down(){
    if (!enabled){
        return false;
    }
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Single:
            current = Mugen::ArcadeData::CharacterCollection::Simultaneous;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
            current = Mugen::ArcadeData::CharacterCollection::Turns2;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            if (wrapping){
                current = Mugen::ArcadeData::CharacterCollection::Single;
                return true;
            }
            break;
        default:
            break;
    };
    return false;
}

bool TeamMenu::left(){
    if (!enabled){
        return false;
    }
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            if (valueSpacingX >= 0){
                return valueLess();
            } else if (valueSpacingX < 0){
                return valueMore();
            }
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            return false;
            break;
    };
    return false;
}

bool TeamMenu::right(){
    if (!enabled){
        return false;
    }
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            if (valueSpacingX >= 0){
                return valueMore();
            } else if (valueSpacingX < 0){
                return valueLess();
            }
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            return false;
            break;
    };
    return false;
}

const Mugen::ArcadeData::CharacterCollection::Type & TeamMenu::select(){
    switch (current){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            return turns;
            break;
        case Mugen::ArcadeData::CharacterCollection::Single:
        case Mugen::ArcadeData::CharacterCollection::Simultaneous:
        default:
            return current;
            break;
    }
}

void TeamMenu::reset(){
    enabled = false;
    current = Mugen::ArcadeData::CharacterCollection::Simultaneous;
    turns = Mugen::ArcadeData::CharacterCollection::Turns2;
}

bool TeamMenu::valueLess(){
    switch (turns){
        case Mugen::ArcadeData::CharacterCollection::Turns4:
            turns = Mugen::ArcadeData::CharacterCollection::Turns3;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns3:
            turns = Mugen::ArcadeData::CharacterCollection::Turns2;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns2:
        default:
            return false;
            break;
    }
}

bool TeamMenu::valueMore(){
    switch (turns){
        case Mugen::ArcadeData::CharacterCollection::Turns2:
            turns = Mugen::ArcadeData::CharacterCollection::Turns3;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns3:
            turns = Mugen::ArcadeData::CharacterCollection::Turns4;
            return true;
            break;
        case Mugen::ArcadeData::CharacterCollection::Turns4:
        default:
            return false;
            break;
    }
}

StageMenu::StageMenu():
random(true),
current(0),
enabled(false),
finished(false){
    font.setState(FontHandler::Blink);
}
StageMenu::~StageMenu(){
}

void StageMenu::act(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (!enabled){
        return;
    }
    font.act();
}

void StageMenu::draw(const Graphics::Bitmap & work){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (!enabled){
        return;
    }
    // Stage
    if (random){
        font.draw("Stage: Random", work);
    } else {
        font.draw("Stage: " + names[current], work);
    }
}

void StageMenu::add(const Filesystem::AbsolutePath & stage){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    try {
        for (std::vector<Filesystem::AbsolutePath>::iterator i = stages.begin(); i != stages.end(); ++i){
            const Filesystem::AbsolutePath & check = *i;
            if (stage == check){
                return;
            }
        }
        AstRef parsed(Util::parseDef(stage));
        const std::string & name = Util::probeDef(parsed, "info", "name");
        stages.push_back(stage);
        names.push_back(name);
    } catch (const MugenException & ex){
        Global::debug(2) << "Warning! Tried to load file: '" << stage.path() << "'. Message: " << ex.getReason() << std::endl;
    }
}

bool StageMenu::up(){
    return false;
}

bool StageMenu::down(){
    return false;
}

bool StageMenu::left(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (!enabled || finished){
        return false;
    }
    if (stages.size() >= 1){
        if (random){
            random = false;
            current = stages.size()-1;
        } else if (current > 0){
            current--;
        } else if (!random){
            random = true;
        }
        return true;
    }
    return false;
}

bool StageMenu::right(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (!enabled || finished){
        return false;
    }
    if (stages.size() >= 1){
        if (random){
            random = false;
            current = 0;
        } else if (current < stages.size()-1){
            current++;
        } else if (!random){
            random = true;
        }
        return true;
    }
    return false;
}

void StageMenu::reset(){
    finished = enabled = false;
    random = true;
    current = 0;
    this->font.setState(FontHandler::Blink);
}

const Filesystem::AbsolutePath & StageMenu::select(){
    if (random){
        return stages[PaintownUtil::rnd(0, stages.size())];
    }
    return stages[current];
}

/*! Thrown in cooperative play once first player picks his character... 
 *  CharacterSelect::select utilizes Player::setCooperativeData to setup the second player when caught
 *  FIXME do not render profile
 */
class CooperativeException : public std::exception {
public:
    CooperativeException(){
    }
    virtual ~CooperativeException() throw(){
    }
};

Player::Player(unsigned int cursor, Gui::GridSelect & grid, std::vector< PaintownUtil::ReferenceCount<Cell> > & cells, std::vector<Mugen::ArcadeData::CharacterInfo> & characters, TeamMenu & teamMenu, TeamMenu & opponentTeamMenu, StageMenu & stageMenu, FontHandler & font, FontHandler & opponentFont, Mugen::SoundSystem<SelectSoundType> & sounds):
cursor(cursor),
grid(grid),
cells(cells),
characters(characters),
teamMenu(teamMenu),
opponentTeamMenu(opponentTeamMenu),
stageMenu(stageMenu),
font(font),
opponentFont(opponentFont),
sounds(sounds),
currentGameType(Mugen::Undefined),
cursorPosition(0),
opponentCursorPosition(0),
portraitX(0), 
portraitY(0),
opponentPortraitX(0), 
opponentPortraitY(0),
switchTime(0),
currentRandom(0),
collection(Mugen::ArcadeData::CharacterCollection::Single),
opponentCollection(Mugen::ArcadeData::CharacterCollection::Single),
selectState(NotStarted){
    if (cursor == 0){
        moveSound = Player1Move;
        doneSound = Player1Done;
        randomSound = Player1Random;
        teamMoveSound = Player1TeamMove;
        teamValueSound = Player1TeamValue;
        teamDoneSound = Player1TeamDone;
    } else if (cursor == 1){
        moveSound = Player2Move;
        doneSound = Player2Done;
        randomSound = Player2Random;
        teamMoveSound = Player2TeamMove;
        teamValueSound = Player2TeamValue;
        teamDoneSound = Player2TeamDone;
    }
}

Player::~Player(){
}

void Player::act(){
    if (cells[grid.getCurrentIndex(cursor)]->isRandom() && (selectState == Character || selectState == Opponent)){
        switchTime++;
        if (switchTime >= randomSwitchTime){
            switchTime = 0;
            currentRandom = PaintownUtil::rnd(0,characters.size());
            sounds.play(randomSound);
        }
    }
    switch (currentGameType){
        case Versus:
        case TeamVersus:
            // Make sure select stage is not stuck
            if (selectState == Stage && stageMenu.getFinished()){
                selectState = Finished;
            }
            break;
        case Arcade:
        case TeamArcade:
        case TeamCoop:
        case Survival:
        case SurvivalCoop:
        case Training:
        case Watch:
        case Undefined:
        default:
            break;
    }
}

void Player::draw(const Graphics::Bitmap & work){
    if (grid.getCurrentState(cursor) == Gui::SelectListInterface::Disabled){
        return;
    }
    switch (currentGameType){
        case Arcade:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            break;
        case Versus:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            break;
        case TeamArcade:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            break;
        case TeamVersus:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            break;
        case TeamCoop:
            switch (selectState){
                case Character:
                    drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
                    break;
                case Opponent:
                case Stage:
                    drawPortrait(opponentCollection, opponentPortraitEffects, opponentPortraitX, opponentPortraitY, opponentFont, work);
                    break;
                default:
                    break;
            }
            break;
        case Survival:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            break;
        case SurvivalCoop:
            switch (selectState){
                case Character:
                    drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
                    break;
                case Opponent:
                case Stage:
                    drawPortrait(opponentCollection, opponentPortraitEffects, opponentPortraitX, opponentPortraitY, opponentFont, work);
                    break;
                default:
                    break;
            }
            break;
        case Training:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            switch (selectState){
                case Opponent:
                case Stage:
                case Finished:
                    drawPortrait(opponentCollection, opponentPortraitEffects, opponentPortraitX, opponentPortraitY, opponentFont, work);
                    break;
                default:
                    break;
            }
            break;
        case Watch:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            switch (selectState){
                case Opponent:
                case Stage:
                case Finished:
                    drawPortrait(opponentCollection, opponentPortraitEffects, opponentPortraitX, opponentPortraitY, opponentFont, work);
                    break;
                default:
                    break;
            }
            break;
        case Undefined:
        default:
            break;
    }
}

void Player::up(){
    switch (currentGameType){
        case Mugen::Arcade:
            switch (selectState){
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Versus:
            switch (selectState){
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamArcade:
            switch (selectState){
                case Team:
                    if (teamMenu.up()){
                        sounds.play(Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.up()){
                        sounds.play(Player2TeamMove);
                    }
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamVersus:
            switch (selectState){
                case Team:
                    if (teamMenu.up()){
                        sounds.play(Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                case Stage:
                default:
                    break;
            }
            break;
        case Mugen::TeamCoop:
            switch (selectState){
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Survival:
            switch (selectState){
                case Team:
                    if (teamMenu.up()){
                        sounds.play(teamMoveSound);
                    }
                    break;
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.up()){
                        sounds.play(teamMoveSound);
                    }
                    break;
                case NotStarted:
                case Stage:
                default:
                    break;
            }
            break;
        case Mugen::SurvivalCoop:
            switch (selectState){
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                case Stage:
                default:
                    break;
            }
            break;
        case Mugen::Training:
            switch (selectState){
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Opponent:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Watch:
            switch (selectState){
                case Team:
                    if (teamMenu.up()){
                        sounds.play(Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.up()){
                        sounds.play(Player2TeamMove);
                    }
                    break;
                case Opponent:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void Player::down(){
    switch (currentGameType){
        case Mugen::Arcade:
            switch (selectState){
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Versus:
            switch (selectState){
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamArcade:
            switch (selectState){
                case Team:
                    if (teamMenu.down()){
                        sounds.play(Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.down()){
                        sounds.play(Player2TeamMove);
                    }
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamVersus:
            switch (selectState){
                case Team:
                    if (teamMenu.down()){
                        sounds.play(Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                case Stage:
                default:
                    break;
            }
            break;
        case Mugen::TeamCoop:
            switch (selectState){
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Survival:
            switch (selectState){
                case Team:
                    if (teamMenu.down()){
                        sounds.play(Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.down()){
                        sounds.play(teamMoveSound);
                    }
                    break;
                case NotStarted:
                case Stage:
                default:
                    break;
            }
            break;
        case Mugen::SurvivalCoop:
            switch (selectState){
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                case Stage:
                default:
                    break;
            }
            break;
        case Mugen::Training:
            switch (selectState){
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Opponent:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Watch:
            switch (selectState){
                case Team:
                    if (teamMenu.down()){
                        sounds.play(Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.down()){
                        sounds.play(Player2TeamMove);
                    }
                    break;
                case Opponent:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void Player::left(){
    switch (currentGameType){
        case Mugen::Arcade:
            switch (selectState){
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Versus:
            switch (selectState){
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(StageMove);
                    }
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamArcade:
            switch (selectState){
                case Team:
                    if (teamMenu.left()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.left()){
                        sounds.play(Player2TeamValue);
                    }
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamVersus:
            switch (selectState){
                case Team:
                    if (teamMenu.left()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamCoop:
            switch (selectState){
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Survival:
            switch (selectState){
                case Team:
                    if (teamMenu.left()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.left()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::SurvivalCoop:
            switch (selectState){
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(StageMove);
                    }
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Training:
            switch (selectState){
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Opponent:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Watch:
            switch (selectState){
                case Team:
                    if (teamMenu.left()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.left()){
                        sounds.play(Player2TeamValue);
                    }
                    break;
                case Opponent:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void Player::right(){
    switch (currentGameType){
        case Mugen::Arcade:
            switch (selectState){
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Versus:
            switch (selectState){
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(StageMove);
                    }
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamArcade:
            switch (selectState){
                case Team:
                    if (teamMenu.right()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.right()){
                        sounds.play(Player2TeamValue);
                    }
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamVersus:
            switch (selectState){
                case Team:
                    if (teamMenu.right()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::TeamCoop:
            switch (selectState){
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Survival:
            switch (selectState){
                case Team:
                    if (teamMenu.right()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.right()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::SurvivalCoop:
            switch (selectState){
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Training:
            switch (selectState){
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Opponent:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        case Mugen::Watch:
            switch (selectState){
                case Team:
                    if (teamMenu.right()){
                        sounds.play(teamValueSound);
                    }
                    break;
                case Character:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.right()){
                        sounds.play(Player2TeamValue);
                    }
                    break;
                case Opponent:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(StageMove);
                    }
                    break;
                case NotStarted:
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void Player::select(int act){
    switch (selectState){
        case Team:
            sounds.play(teamDoneSound);
            next(act);
            break;
        case Character:
            if (!cells[grid.getCurrentIndex(cursor)]->isEmpty()){
                sounds.play(doneSound);
                if (cells[grid.getCurrentIndex(cursor)]->isRandom()){
                    cells[currentRandom]->select();
                } else {
                    cells[grid.getCurrentIndex(cursor)]->select();
                }
                next(act);
            }
            break;
        case Opponent:
            if (!cells[grid.getCurrentIndex(cursor)]->isEmpty()){
                sounds.play(doneSound);
                if (cells[grid.getCurrentIndex(cursor)]->isRandom()){
                    cells[currentRandom]->select();
                } else {
                    cells[grid.getCurrentIndex(cursor)]->select();
                }
                next(act);
            }
            break;
        case OpponentTeam:
            sounds.play(teamDoneSound);
            next(act);
            break;
        case Stage:
            sounds.play(StageDone);
            next(act);
            break;
        case NotStarted:
        case Finished:
        default:
            break;
    }
}

void Player::setPortraitEffects(const Mugen::Effects & effects, const Mugen::Effects & opponentEffects){
    portraitEffects = effects;
    opponentPortraitEffects = opponentEffects;
}

void Player::setCurrentGameType(const Mugen::GameType & type){
    currentGameType = type;
    next();
}

void Player::next(int act){
    switch (currentGameType){
        case Mugen::Arcade:
            switch (selectState){
                case NotStarted:
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell(), act);
                    selectState = Finished;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    break;
                default:
                    break;
            }
            break;
        case Mugen::Versus:
            switch (selectState){
                case NotStarted:
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell(), act);
                    if (!stageMenu.getFinished()){
                        selectState = Stage;
                        stageMenu.setEnabled(true);
                    } else {
                        selectState = Finished;
                    }
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    break;
                case Stage:
                    selectState = Finished;
                    stageMenu.finish();
                    break;
                default:
                    break;
            }
            break;
        case Mugen::TeamArcade:
            switch (selectState){
                case NotStarted:
                    selectState = Team;
                    teamMenu.setEnabled(true);
                    break;
                case Team:
                    collection.setType(teamMenu.select());
                    teamMenu.setEnabled(false);
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    if (!collection.checkSet()){
                        collection.setNext(getCurrentCell(), act);
                        if (collection.checkSet()){
                            selectState = OpponentTeam;
                            opponentTeamMenu.setEnabled(true);
                            grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                        }
                    }
                    break;
                case OpponentTeam:
                    selectState = Finished;
                    opponentCollection.setType(opponentTeamMenu.select());
                    opponentTeamMenu.setEnabled(false);
                    break;
                default:
                    break;
            }
            break;
        case Mugen::TeamVersus:
            switch (selectState){
                case NotStarted:
                    selectState = Team;
                    teamMenu.setEnabled(true);
                    break;
                case Team:
                    collection.setType(teamMenu.select());
                    teamMenu.setEnabled(false);
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    if (!collection.checkSet()){
                        collection.setNext(getCurrentCell(), act);
                        if (collection.checkSet()){
                            if (!stageMenu.getFinished()){
                                selectState = Stage;
                                stageMenu.setEnabled(true);
                            } else {
                                selectState = Finished;
                            }
                            grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                        }
                    }
                    break;
                case Stage:
                    stageMenu.finish();
                    selectState = Finished;
                    break;
                default:
                    break;
            }
            break;
        case Mugen::TeamCoop:
            switch (selectState){
                case NotStarted:
                    collection.setType(Mugen::ArcadeData::CharacterCollection::Simultaneous);
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell(), act);
                    selectState = WaitFinished;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    throw CooperativeException();
                    break;
                case Opponent:
                    opponentCollection.setSecond(getCurrentCell());
                    selectState = Finished;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    break;
                default:
                    break;
            }
            break;
        case Mugen::Survival:
            switch (selectState){
                case NotStarted:
                    selectState = Team;
                    teamMenu.setEnabled(true);
                    break;
                case Team:
                    collection.setType(teamMenu.select());
                    teamMenu.setEnabled(false);
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    if (!collection.checkSet()){
                        collection.setNext(getCurrentCell(), act);
                        if (collection.checkSet()){
                            selectState = OpponentTeam;
                            opponentTeamMenu.setEnabled(true);
                            grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                        }
                    }
                    break;
                case OpponentTeam:
                    selectState = Stage;
                    opponentCollection.setType(opponentTeamMenu.select());
                    opponentTeamMenu.setEnabled(false);
                    stageMenu.setEnabled(true);
                    break;
                case Stage:
                    selectState = Finished;
                    stageMenu.finish();
                    break;
                default:
                    break;
            }
            break;
        case Mugen::SurvivalCoop:
            switch (selectState){
                case NotStarted:
                    collection.setType(Mugen::ArcadeData::CharacterCollection::Simultaneous);
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell(), act);
                    selectState = WaitFinished;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    throw CooperativeException();
                    break;
                case Opponent:
                    opponentCollection.setSecond(getCurrentCell());
                    selectState = Team;
                    teamMenu.setEnabled(true);
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    break;
                case Team:
                    selectState = Stage;
                    collection.setType(teamMenu.select());
                    teamMenu.setEnabled(false);
                    stageMenu.setEnabled(true);
                    break;
                case Stage:
                    selectState = Finished;
                    stageMenu.finish();
                    break;
                default:
                    break;
            }
            break;
        case Mugen::Training:
            switch (selectState){
                case NotStarted:
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell(), act);
                    selectState = Opponent;
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Opponent:
                    opponentCollection.setFirst(getCurrentCell() , act);
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    selectState = Stage;
                    stageMenu.setEnabled(true);
                    break;
                case Stage:
                    stageMenu.finish();
                    selectState = Finished;
                    break;
                default:
                    break;
            }
            break;
        case Mugen::Watch:
            switch (selectState){
                case NotStarted:
                    selectState = Team;
                    teamMenu.setEnabled(true);
                    break;
                case Team:
                    collection.setType(teamMenu.select());
                    teamMenu.setEnabled(false);
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    if (!collection.checkSet()){
                        collection.setNext(getCurrentCell(), act);
                        if (collection.checkSet()){
                            selectState = OpponentTeam;
                            opponentTeamMenu.setEnabled(true);
                            grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                        }
                    }
                    break;
                case OpponentTeam:
                    opponentCollection.setType(opponentTeamMenu.select());
                    selectState = Opponent;
                    opponentTeamMenu.setEnabled(false);
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, opponentCursorPosition);
                    break;
                case Opponent:
                    if (!opponentCollection.checkSet()){
                        opponentCollection.setNext(getCurrentCell() , act);
                        if (opponentCollection.checkSet()){
                            grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                            selectState = Stage;
                            stageMenu.setEnabled(true);
                        }
                    }
                    break;
                case Stage:
                    stageMenu.finish();
                    selectState = Finished;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


void Player::drawPortrait(const Mugen::ArcadeData::CharacterCollection & currentCollection, const Mugen::Effects & effects, int x, int y, FontHandler & currentFont, const Graphics::Bitmap & work){
    const Mugen::ArcadeData::CharacterInfo & character = currentCollection.checkSet() ? currentCollection.getLastSet() : getCurrentCell();
    
    character.drawPortrait(x, y, work, effects);
    
    int heightMod = 0;
    if (currentCollection.getFirstSet()){
        currentFont.draw(currentCollection.getFirst().getName(), work);
        heightMod += 15;
    }
    if (currentCollection.getSecondSet()){
        currentFont.draw(currentCollection.getSecond().getName(), work, heightMod);
        heightMod += 15;
    }
    if (currentCollection.getThirdSet()){
        currentFont.draw(currentCollection.getFirst().getName(), work, heightMod);
        heightMod += 15;
    }
    if (currentCollection.getFourthSet()){
        currentFont.draw(currentCollection.getFirst().getName(), work, heightMod);
    }
    
    if (!currentCollection.checkSet()){
        if (cells[grid.getCurrentIndex(cursor)]->isRandom()){
            // NOTE I'd prefer to randomize the name, but mugen originally just puts the words random there
            currentFont.draw("Random", work, heightMod);
        } else {
            currentFont.draw(character.getName(), work, heightMod);
        }
    }
}

void Player::reset(){
    collection = Mugen::ArcadeData::CharacterCollection(Mugen::ArcadeData::CharacterCollection::Single);
    opponentCollection = Mugen::ArcadeData::CharacterCollection(Mugen::ArcadeData::CharacterCollection::Single);
    selectState = NotStarted;
    grid.setCurrentState(cursor, Gui::SelectListInterface::Disabled);
    stageMenu.reset();
    teamMenu.reset();
    opponentTeamMenu.reset();
}

void Player::setCooperativeData(const Player & cooperativePlayer){
    // Second player
    currentGameType = cooperativePlayer.currentGameType;
    selectState = Opponent;
    opponentCollection = cooperativePlayer.collection;
    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
    grid.setCurrentIndex(cursor, grid.getCurrentIndex(cooperativePlayer.cursor));   
}

const Mugen::ArcadeData::CharacterInfo & Player::getCurrentCell(){
    if (cells[grid.getCurrentIndex(cursor)]->isRandom()){
        return characters[currentRandom];
    } 
    return cells[grid.getCurrentIndex(cursor)]->getCharacter();
}
    
CharacterSelect::CharacterSelect(const Filesystem::AbsolutePath & file):
file(file),
gridX(0),
gridY(0),
gridPositionX(0),
gridPositionY(0),
nextCell(0),
currentGameType(Undefined),
currentPlayer(Player1),
player1(0, grid, cells, characters, player1TeamMenu, player2TeamMenu, stages, player1Font, player2Font, sounds),
player2(1, grid, cells, characters, player2TeamMenu, player1TeamMenu, stages, player2Font, player1Font, sounds),
canceled(false),
demoLeftTime(0),
demoRightTime(0),
demoLeftRemaining(0),
demoRightRemaining(0),
blinkCursor(false),
blinkTime(0){
    Global::debug(1) << "Got file: " << file.path() << std::endl;
}

CharacterSelect::~CharacterSelect(){
}

void CharacterSelect::init(){
    try{
        Filesystem::AbsolutePath baseDir = file.getDirectory();

        Global::debug(1) << baseDir.path() << std::endl;

        if (file.isEmpty()){
            std::ostringstream out;
            out << "Cannot locate character select definition file for: " << file.path();
            throw MugenException( out.str(), __FILE__, __LINE__);
        }

        PaintownUtil::ReferenceCount<Sprite> cellBackground;
        PaintownUtil::ReferenceCount<Sprite> cellRandomIcon;

        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Util::parseDef(file));
        diff.endTime();
        Global::debug(1) << "Parsed mugen file " + file.path() + " in" + diff.printTime("") << std::endl;

        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            head = Mugen::Util::fixCase(head);
            if (head == "info"){
                /* Nothing right now */
            } else if (head == "files"){
                class FileWalker: public Ast::Walker{
                public:
                    FileWalker(Mugen::CharacterSelect & select, const Filesystem::AbsolutePath & baseDir):
                        select(select),
                        baseDir(baseDir){
                        }

                    Mugen::CharacterSelect & select;
                    const Filesystem::AbsolutePath & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            std::string sffFile;
                            simple.view() >> sffFile;
                            Global::debug(1) << "Got Sprite File: '" << sffFile << "'" << std::endl;
                            Mugen::Util::readSprites(Mugen::Util::findFile(Filesystem::RelativePath(sffFile)), Filesystem::AbsolutePath(), select.sprites, true);
                        } else if (simple == "snd"){
                            std::string sndFile;
                            simple.view() >> sndFile;
                            select.sounds.init(sndFile);
                        } else if (simple == "logo.storyboard"){
                            // Ignore
                        } else if (simple == "intro.storyboard"){
                            // Ignore
                        } else if (simple == "select"){
                            std::string file;
                            simple.view() >> file;
                            select.selectFile = Mugen::Util::fixFileName(baseDir, Mugen::Util::stripDir(file));
                            Global::debug(1) << "Got Select File: '" << file << "'" << std::endl;
                        } else if (simple == "fight"){
                            // Ignore
                        } else if (PaintownUtil::matchRegex(simple.idString(), PaintownUtil::Regex("^font"))){
                            std::string fontPath;
                            simple.view() >> fontPath;
                            select.fonts.add(fontPath);
                        } else {
                            throw MugenException("Unhandled option in Files Section: " + simple.toString(), __FILE__, __LINE__ );
                        }
                    }
                };

                FileWalker walker(*this, baseDir);
                section->walk(walker);
            } else if (head == "title info"){
                /* Nothing */
            } else if (PaintownUtil::matchRegex(head, PaintownUtil::Regex("^titlebg"))){
                /* Nothing */
            } else if (head == "select info"){
                class SelectInfoWalker: public Ast::Walker{
                public:
                    SelectInfoWalker(const AstRef & parsed, CharacterSelect & self, Mugen::SpriteMap & sprites, PaintownUtil::ReferenceCount<Sprite> & cellBackground, PaintownUtil::ReferenceCount<Sprite> & cellRandomIcon):
                        parsed(parsed),
                        self(self),
                        sprites(sprites),
                        cellBackground(cellBackground),
                        cellRandomIcon(cellRandomIcon){
                        }

                    const AstRef & parsed;
                    CharacterSelect & self;
                    Mugen::SpriteMap & sprites;
                    
                    Mugen::Effects player1Effects;
                    Mugen::Effects player2Effects;
                    PaintownUtil::ReferenceCount<Sprite> & cellBackground;
                    PaintownUtil::ReferenceCount<Sprite> & cellRandomIcon;

                    PaintownUtil::ReferenceCount<Animation> findAnimation(const AstRef & parse, int animation){
                        std::ostringstream all;
                        all << "begin action " << animation;
                        Ast::Section * section = parse->findSection(all.str());
                        if (section == NULL){
                            std::ostringstream out;
                            out << "No action for anim " << animation;
                            throw MugenException(out.str(), __FILE__, __LINE__);
                        }
                        return Util::getAnimation(section, sprites, true);
                    }

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "fadein.time" ){
                            try{
                                int time;
                                simple.view() >> time;
                                self.fader.setFadeInTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadein.color" ){
                            try{
                                int r,g,b;
                                simple.view() >> r >> g >> b;
                                self.fader.setFadeInColor(Graphics::makeColor(r,g,b));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadeout.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.fader.setFadeOutTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadeout.color"){
                            try{
                                int r,g,b;
                                simple.view() >> r >> g >> b;
                                self.fader.setFadeOutColor(Graphics::makeColor(r,g,b));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "rows"){
                            try{
                                simple.view() >> self.gridY;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "columns"){
                            try{
                                simple.view() >> self.gridX;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "wrapping"){
                            try{
                                bool wrap;
                                simple.view() >> wrap;
                                self.grid.setWrap(wrap);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "pos"){
                            try{
                                int x,y;
                                simple.view() >> self.gridPositionX >> self.gridPositionY;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "showemptyboxes"){
                            try{
                                bool boxes;
                                simple.view() >> boxes;
                                self.grid.setDrawEmpty(boxes);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "moveoveremptyboxes"){
                            try{
                                bool boxes;
                                simple.view() >> boxes;
                                /* if moveoveremptyboxes is 1 then we should
                                 * not allow the user to select an empty box
                                 */
                                self.grid.setAccessEmpty(!boxes);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.size"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.grid.setCellDimensions(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.spacing"){
                            try{
                                int spacing;
                                simple.view() >> spacing;
                                self.grid.setCellMargins(spacing, spacing);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.bg.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                cellBackground = PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                cellRandomIcon = PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.switchtime"){
                            try{
                                int time;
                                simple.view() >> time;
                                Player::setRandomSwitchTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.startcell"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                int index = 0;
                                for (int i = 0; i <= x; ++i){
                                    for (int j = 0; j < y; ++j){
                                        index++;
                                    }
                                }
                                self.player1.setCursorPosition(index);
                                self.player2.setOpponentCursorPosition(index);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.active.anim"){
                            try{
                                int animation;
                                simple.view() >> animation;
                                self.setPlayer1ActiveCursor(findAnimation(parsed, animation));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.anim"){
                            try{
                                int animation;
                                simple.view() >> animation;
                                self.setPlayer1DoneCursor(findAnimation(parsed, animation));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<Animation>(new Animation(sprites[group][sprite], true)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.setPlayer1DoneCursor(PaintownUtil::ReferenceCount<Animation>(new Animation(sprites[group][sprite], true)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player1Move, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player1Done, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player1Random, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.startcell"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                int index = 0;
                                for (int i = 0; i <= x; ++i){
                                    for (int j = 0; j < y; ++j){
                                        index++;
                                    }
                                }
                                self.player2.setCursorPosition(index);
                                self.player1.setOpponentCursorPosition(index);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.active.anim"){
                            try{
                                int animation;
                                simple.view() >> animation;
                                self.setPlayer2ActiveCursor(findAnimation(parsed, animation));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.done.anim"){
                            try{
                                int animation;
                                simple.view() >> animation;
                                self.setPlayer2DoneCursor(findAnimation(parsed, animation));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<Animation>(new Animation(sprites[group][sprite], true)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.setPlayer2DoneCursor(PaintownUtil::ReferenceCount<Animation>(new Animation(sprites[group][sprite], true)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.blink"){
                            try{
                                bool blink;
                                simple.view() >> blink;
                                self.setBlinkCursor(blink);
                            } catch (const Ast::Exception & e){
                            }
                        } 
                        else if ( simple == "p2.cursor.move.snd"){ 
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player2Move, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player2Done, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player2Random, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "random.move.snd.cancel"){
                            try{
                                bool cancel;
                                simple.view() >> cancel;
                                //FIXME
                                // Set cancel sound flag
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(StageMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(StageDone, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "cancel.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Cancel, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "portrait.offset"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                Cell::setOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "portrait.scale"){
                            try{
                                Mugen::Effects effects;
                                simple.view() >> effects.scalex >> effects.scaley;
                                Cell::setEffects(effects);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "title.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.titleFont.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "title.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.titleFont.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p1.face.offset"){
                            try{
                                int x = 0, y = 0;
                                simple.view() >> x >> y;
                                self.player1.setPortraitOffset(x, y);
                                self.player2.setOpponentPortraitOffset(x, y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.scale"){
                            try{
                                simple.view() >> player1Effects.scalex >> player1Effects.scaley;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                switch (face){
                                    case 1: player1Effects.facing = false; break;
                                    case -1: player1Effects.facing = true; break;
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.offset"){
                            try{
                                int x = 0, y = 0;
                                simple.view() >> x >> y;
                                self.player2.setPortraitOffset(x, y);
                                self.player1.setOpponentPortraitOffset(x, y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.scale"){
                            try{
                                simple.view() >> player2Effects.scalex >> player2Effects.scaley;
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                switch (face){
                                    case 1: player2Effects.facing = false; break;
                                    case -1: player2Effects.facing = true; break;
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1Font.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        }  else if ( simple == "p1.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player1Font.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p2.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2Font.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.name.font"){
                            int index=0, bank=0, position=0;
                            try{
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                            }
                            self.player2Font.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "stage.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.stages.font.setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.stages.font.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "stage.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.stages.font.setActive2(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "stage.done.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.stages.font.setDone(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "teammenu.move.wrapping"){
                            try {
                                bool wrap;
                                simple.view() >> wrap;
                                TeamMenu::setWrapping(wrap);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setPosition(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1TeamMenu.setBackgroundSprite(PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.selftitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.titleFont.setActive(self.fonts.getFont(index,bank,position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.selftitle.text"){
                            std::string text;
                            try {
                                simple.view() >> text;
                                self.player1TeamMenu.titleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.enemytitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.enemyTitleFont.setActive(self.fonts.getFont(index,bank,position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.enemytitle.text"){
                            std::string text;
                            try {
                                simple.view() >> text;
                                self.player1TeamMenu.enemyTitleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player1TeamMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player1TeamValue, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player1TeamDone, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.item.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setItemOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.item.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setItemSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.item.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player1TeamMenu.itemFont.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p1.teammenu.item.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player1TeamMenu.itemCurrentFont.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p1.teammenu.item.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.itemCurrentFont.setActive2(self.fonts.getFont(index,bank,position));
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                        } else if ( simple == "p1.teammenu.item.cursor.offset"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p1.teammenu.item.cursor.anim"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p1.teammenu.value.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1TeamMenu.setValueIconSprite(PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.empty.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setEmptyValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.empty.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1TeamMenu.setEmptyValueIconSprite(PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1TeamMenu.setValueSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setPosition(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.bg.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2TeamMenu.setBackgroundSprite(PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.selftitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.titleFont.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p2.teammenu.selftitle.text"){
                             std::string text;
                            try {
                                simple.view() >> text;
                                self.player2TeamMenu.titleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.enemytitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.enemyTitleFont.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p2.teammenu.enemytitle.text"){
                             std::string text;
                            try {
                                simple.view() >> text;
                                self.player2TeamMenu.enemyTitleFont.setText(text);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player2TeamMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player2TeamValue, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(Player2TeamDone, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.item.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setItemOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.item.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setItemSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.item.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemFont.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p2.teammenu.item.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemCurrentFont.setActive(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p2.teammenu.item.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemCurrentFont.setActive2(self.fonts.getFont(index,bank,position));
                        } else if ( simple == "p2.teammenu.item.cursor.offset"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p2.teammenu.item.cursor.anim"){
                            // TODO Not sure what this is, doesn't seem to do anything in mugen
                        } else if ( simple == "p2.teammenu.value.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2TeamMenu.setValueIconSprite(PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.empty.icon.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setEmptyValueIconOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.empty.icon.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2TeamMenu.setEmptyValueIconSprite(PaintownUtil::ReferenceCount<Sprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.spacing"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2TeamMenu.setValueSpacing(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        }
                    }
                };

                SelectInfoWalker walker(parsed, *this, sprites, cellBackground, cellRandomIcon);
                section->walk(walker);
                player1.setPortraitEffects(walker.player1Effects, walker.player2Effects);
                player2.setPortraitEffects(walker.player2Effects, walker.player1Effects);
            } else if (head == "selectbgdef"){ 
                /* Background management */
                background = PaintownUtil::ReferenceCount<Mugen::Background>(new Mugen::Background(file, "selectbg"));
            } else if (head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "vs screen" ){
                /* Don't handle versus here */
            }
            else if (head == "versusbgdef" ){
                /* Background management */
                //Mugen::Background *manager = new Mugen::Background(systemFile, "versusbg");
                //versus.setBackground(manager);
            }
            else if (head.find("versusbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "demo mode"){ /* Ignore for now */ }
            else if (head == "continue screen"){ /* Ignore for now */ }
            else if (head == "game over screen"){ /* Ignore for now */ }
            else if (head == "win screen"){ /* Ignore for now */ }
            else if (head == "default ending"){ /* Ignore for now */ }
            else if (head == "end credits"){ /* Ignore for now */ }
            else if (head == "survival results screen"){ /* Ignore for now */ }
            else if (head == "option info"){ /* Ignore for now */ }
            else if (head == "optionbgdef"){ /* Ignore for now */ }
            else if (head.find("optionbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "music"){ /* Ignore for now */ }
            else if (head.find("begin action") != std::string::npos){ /* Ignore for now */ }
            else {
                // throw MugenException("Unhandled Section in '" + systemFile.path() + "': " + head, __FILE__, __LINE__ ); 
                std::ostringstream context;
                context << __FILE__ << ":" << __LINE__;
                Global::debug(0, context.str()) << "Warning: Unhandled Section in '" + file.path() + "': " + head << std::endl;
            }
        }

        // Set up Grid set starting offset to 1 so it doesn't clip
        grid.setStartingOffset(1, 1);
        grid.setGridSize(gridX, gridY);
        // Set up cells
        for (unsigned int i = 0; i < (unsigned int)(gridX * gridY); ++i){
            PaintownUtil::ReferenceCount<Cell> cell = PaintownUtil::ReferenceCount<Cell>(new Cell(i, &grid, cellBackground, cellRandomIcon));
            cells.push_back(cell);
            grid.addItem(cell);
        }
        // Set up cursors
        grid.setCursors(2);
        //grid.setCurrentIndex(0, cursorPosition);
        grid.setCurrentState(0, Gui::SelectListInterface::Disabled);
        //grid.setCurrentIndex(1, opponentCursorPosition);
        grid.setCurrentState(1, Gui::SelectListInterface::Disabled);
        
        // Set up fader
        fader.setState(Gui::FadeTool::FadeIn);

    } catch (const Filesystem::NotFound & fail){
        std::ostringstream out;
        out << "Could not load select screen because " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
    
    parseSelect();
}

void CharacterSelect::cancel(){
    // Fade out
    fader.setState(Gui::FadeTool::FadeOut);
    sounds.play(Cancel);
    canceled = true;
}

bool CharacterSelect::isDone() {
    return (fader.getState() == Gui::FadeTool::EndFade);
}

void CharacterSelect::cursorsAct(){
    if (player1ActiveCursor != NULL){
        player1ActiveCursor->logic();
    }
    if (player1DoneCursor != NULL){
        player1DoneCursor->logic();
    }
    if (player2ActiveCursor != NULL){
        player2ActiveCursor->logic();
    }
    if (player2DoneCursor != NULL){
        player2DoneCursor->logic();
    }
            
    blinkTime++;
    if (blinkTime >= 20){
        blinkTime = 0;
    }
}

void CharacterSelect::act(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    for (std::vector< PaintownUtil::ReferenceCount<Cell> >::iterator i = cells.begin(); i != cells.end(); ++i){
        PaintownUtil::ReferenceCount<Cell> cell = *i;
        cell->act();
    }

    cursorsAct();

    background->act();
    grid.act();
    
    // Player portraits
    player1.act();
    player2.act();
    
    // Team Menu
    player1TeamMenu.act();
    player2TeamMenu.act();
    
    // Stages
    stages.act();
    
    // Fader
    fader.act();
    
    switch (currentPlayer){
        case Player1:
            if (player1.isFinished()){
                if (fader.getState() != Gui::FadeTool::FadeOut && !isDone()){
                    fader.setState(Gui::FadeTool::FadeOut);
                }
            }
            break;
        case Player2:
            if (player2.isFinished()){
                if (fader.getState() != Gui::FadeTool::FadeOut && !isDone()){
                    fader.setState(Gui::FadeTool::FadeOut);
                }
            }
            break;
        case Both:
            if (player1.isFinished() && player2.isFinished()){
                if (fader.getState() != Gui::FadeTool::FadeOut && !isDone()){
                    fader.setState(Gui::FadeTool::FadeOut);
                }
            }
            break;
        case Demo:
            if (!player1.isFinished()){
                if (demoLeftTime < 11){
                    demoLeftTime++;
                } else {
                    demoLeftTime = 0;
                    if (demoLeftRemaining > 0){
                        const int direction = PaintownUtil::rnd(0, 4);
                        switch (direction){
                            case 1:
                                down(0);
                                if (cells[grid.getCurrentIndex(0)]->isEmpty()){
                                    up(0);
                                }
                                break;
                            case 2:
                                left(0);
                                if (cells[grid.getCurrentIndex(0)]->isEmpty()){
                                    right(0);
                                }
                                break;
                            case 3:
                                right(0);
                                if (cells[grid.getCurrentIndex(0)]->isEmpty()){
                                    left(0);
                                }
                                break;
                            case 0:
                            default:
                                up(0);
                                if (cells[grid.getCurrentIndex(0)]->isEmpty()){
                                    down(0);
                                }
                                break;
                        }
                        demoLeftRemaining--;
                    } else {
                        select(0);
                    }
                }
            }
            if (!player2.isFinished()){
                if (demoRightTime < 11){
                    demoRightTime++;
                } else {
                    demoRightTime = 0;
                    if (demoRightRemaining > 0){
                        const int direction = PaintownUtil::rnd(0, 4);
                        switch (direction){
                            case 1:
                                down(1);
                                if (cells[grid.getCurrentIndex(1)]->isEmpty()){
                                    up(1);
                                }
                                break;
                            case 2:
                                left(1);
                                if (cells[grid.getCurrentIndex(1)]->isEmpty()){
                                    right(1);
                                }
                                break;
                            case 3:
                                right(1);
                                if (cells[grid.getCurrentIndex(1)]->isEmpty()){
                                    left(1);
                                }
                                break;
                            case 0:
                            default:
                                up(1);
                                if (cells[grid.getCurrentIndex(1)]->isEmpty()){
                                    down(1);
                                }
                                break;
                        }
                        demoRightRemaining--;
                    } else {
                        select(1);
                    }
                }
            }
            if (player1.isFinished() && player2.isFinished()){
                if (fader.getState() != Gui::FadeTool::FadeOut && !isDone()){
                    fader.setState(Gui::FadeTool::FadeOut);
                }
            }
        default:
            break;
    }   
}

static std::string getGameType(const Mugen::GameType & game){
    switch (game){
        case Mugen::Arcade:
            return "Arcade";
            break;
        case Mugen::Versus:
            return "Versus Mode";
            break;
        case Mugen::TeamArcade:
            return "Team Arcade";
            break;
        case Mugen::TeamVersus:
            return "Team Versus";
            break;
        case Mugen::TeamCoop:
            return "Team Cooperative";
            break;
        case Mugen::Survival:
            return "Survival";
            break;
        case Mugen::SurvivalCoop:
            return "Survival Cooperative";
            break;
        case Mugen::Training:
            return "Training Mode";
            break;
        case Mugen::Watch:
            return "Watch Mode";
            break;
        case Mugen::Undefined:
        default:
            return "Set GameType and Players";
            break;
    }
}

void CharacterSelect::drawPlayer1Cursor(int x, int y, Gui::SelectListInterface::CursorState state, const Graphics::Bitmap & work) const {
    if (state == Gui::SelectListInterface::Disabled){
    } else if (state == Gui::SelectListInterface::Active){
        if (player1ActiveCursor != NULL){
            player1ActiveCursor->render(x, y, work);
        }
    } else if (state == Gui::SelectListInterface::Done){
        if (player1DoneCursor != NULL){
            player1DoneCursor->render(x, y, work);
        }
    }
}

void CharacterSelect::drawPlayer2Cursor(int x, int y, Gui::SelectListInterface::CursorState state, const Graphics::Bitmap & work, bool blink) const {
    if (state == Gui::SelectListInterface::Disabled){
    } else if (state == Gui::SelectListInterface::Active){
        if (blinkCursor && blink){
            if (blinkTime < 10){
                if (player2ActiveCursor != NULL){
                    player2ActiveCursor->render(x, y, work);
                }
            }
        } else {
            if (player2ActiveCursor != NULL){
                player2ActiveCursor->render(x, y, work);
            }
        }
    } else if (state == Gui::SelectListInterface::Done){
        if (player2DoneCursor != NULL){
            player2DoneCursor->render(x, y, work);
        }
    }
}

void CharacterSelect::drawCursors(int xOffset, int yOffset, const Graphics::Bitmap & work) const {
    int cursorX1, cursorY1;
    int cursorX2, cursorY2;

    PaintownUtil::ReferenceCount<Gui::SelectItem> item1 = grid.getItemByCursor(0);
    PaintownUtil::ReferenceCount<Gui::SelectItem> item2 = grid.getItemByCursor(1);

    const Gui::SelectListInterface::CursorState state1 = grid.getCurrentState(0);
    const Gui::SelectListInterface::CursorState state2 = grid.getCurrentState(1);

    grid.getDrawLocation(item1, &cursorX1, &cursorY1);
    grid.getDrawLocation(item2, &cursorX2, &cursorY2);

    cursorX1 += xOffset;
    cursorX2 += xOffset;
    cursorY1 += yOffset;
    cursorY2 += yOffset;

    /* Overlapping */
    if (state1 == Gui::SelectListInterface::Active &&
        state2 == Gui::SelectListInterface::Active &&
        item1 == item2){
        drawPlayer1Cursor(cursorX1, cursorY1, state1, work);
        drawPlayer2Cursor(cursorX2, cursorY2, state2, work, true);
    } else if (state1 == Gui::SelectListInterface::Active &&
               state2 == Gui::SelectListInterface::Done){
        drawPlayer2Cursor(cursorX2, cursorY2, state2, work);
        drawPlayer1Cursor(cursorX1, cursorY1, state1, work);
    } else if (state1 == Gui::SelectListInterface::Done &&
               state2 == Gui::SelectListInterface::Active){
        drawPlayer1Cursor(cursorX1, cursorY1, state1, work);
        drawPlayer2Cursor(cursorX2, cursorY2, state2, work);
    } else {
        drawPlayer2Cursor(cursorX2, cursorY2, state2, work);
        drawPlayer1Cursor(cursorX1, cursorY1, state1, work);
    }
}

void CharacterSelect::draw(const Graphics::Bitmap & work){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    // Render Background
    background->renderBackground(0, 0, work);
    
    // Temporary bitmap for grid
    // const Graphics::Bitmap & temp = Graphics::Bitmap::temporaryBitmap(grid.getWidth(), grid.getHeight());
    /* FIXME: do we need this temporary bitmap? */
    Graphics::Bitmap temp(grid.getWidth(), grid.getHeight());
    temp.clearToMask();
    grid.render(temp, ::Font::getDefaultFont());
    // Minus 1 since it's been offset
    temp.draw(gridPositionX-1, gridPositionY-1, work);

    // render title based on Mugen::GameType
    titleFont.draw((!titleOverride.empty() ? titleOverride : getGameType(currentGameType)), work);
    
    // Draw portrait and name
    player1.draw(work);
    player2.draw(work);
    
    // Team Menu
    player1TeamMenu.draw(work);
    player2TeamMenu.draw(work);
    
    // Stages
    stages.draw(work);
    
    drawCursors(gridPositionX - 1, gridPositionY - 1, work);
    
    background->renderForeground(0, 0, work);
    
    // Fader
    fader.draw(work);
}

void CharacterSelect::setMode(const Mugen::GameType & game, const PlayerType & player){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (game == Mugen::Undefined){
        return;
    }
    // Set up fader
    fader.setState(Gui::FadeTool::FadeIn);
    currentGameType = game;
    currentPlayer = player;
    player1.reset();
    player2.reset();
    switch (currentPlayer){
        case Player1:
            player1.setCurrentGameType(currentGameType);
            break;
        case Player2:
            player2.setCurrentGameType(currentGameType);
            break;
        case Both:
        case Demo:
            player1.setCurrentGameType(currentGameType);
            player2.setCurrentGameType(currentGameType);
            demoLeftTime = PaintownUtil::rnd(0, 11);
            demoRightTime = PaintownUtil::rnd(0, 11);
            demoLeftRemaining = PaintownUtil::rnd(5, 11);
            demoRightRemaining = PaintownUtil::rnd(5, 11);
            grid.setCurrentIndex(0,0);
            grid.setCurrentIndex(1, nextCell-1);
            break;
        default:
            break;
    }
}

void CharacterSelect::up(unsigned int cursor){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (cursor == 0){
        player1.up();
    } else if (cursor == 1){
        player2.up();
    }
}

void CharacterSelect::down(unsigned int cursor){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (cursor == 0){
        player1.down();
    } else if (cursor == 1){
        player2.down();
    }
}

void CharacterSelect::left(unsigned int cursor){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (cursor == 0){
        player1.left();
    } else if (cursor == 1){
        player2.left();
    }
}

void CharacterSelect::right(unsigned int cursor){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (cursor == 0){
        player1.right();
    } else if (cursor == 1){
        player2.right();
    }
}

void CharacterSelect::select(unsigned int cursor, int act){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (cursor == 0){
        try {
            player1.select(act);
        } catch (const CooperativeException & ex){
            player2.reset();
            player2.setCooperativeData(player1);
            currentPlayer = Player2;
        }
    } else if (cursor ==1){
        try {
            player2.select(act);
        } catch (const CooperativeException & ex){
            player1.reset();
            player1.setCooperativeData(player2);
            currentPlayer = Player1;
        }
    }
}

bool CharacterSelect::uniqueCharacter(const Filesystem::AbsolutePath definitionPath) const {
    PaintownUtil::Thread::ScopedLock scoped(lock);
    for (std::vector<PaintownUtil::ReferenceCount<Cell> >::const_iterator it = cells.begin(); it != cells.end(); it++){
        PaintownUtil::ReferenceCount<Cell> cell = *it;
        if (cell->getCharacter().getDef() == definitionPath){
            return false;
        }
    }
    return true;
}

bool CharacterSelect::addCharacter(const Mugen::ArcadeData::CharacterInfo & character){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    // Check if the character exists
    /*
    for (std::vector<Mugen::ArcadeData::CharacterInfo>::iterator i = characters.begin(); i != characters.end(); ++i){
        const Mugen::ArcadeData::CharacterInfo & check = *i;
        if (character == check){
            return false;
        }
    }*/


    /* Find an unused cell if available */
    for (unsigned int i = 0; i < nextCell && i < cells.size(); i++){
        PaintownUtil::ReferenceCount<Cell> cell = cells[i];
        if (cell->isUnused()){
            characters.push_back(character);
            // Include stage if required
            if (character.getIncludeStage()){
                /* We hold the lock so call doAddStage directly */
                doAddStage(character.getStage());
            }

            cell->setCharacter(character);
            return true;
        }
    }

    // Check if we don't exceed the cell count of the current grid
    if (nextCell < cells.size()){
        // Add to list
        characters.push_back(character);
        // Include stage if required
        if (character.getIncludeStage()){
            /* We hold the lock so call doAddStage directly */
            doAddStage(character.getStage());
        }
        // Add to current cell
        cells[nextCell]->setCharacter(character);
        // Increment cell
        nextCell++;
        return true;
    } else {
        /* Otherwise find a cell set as 'randomselect' and use that instead.
         * Don't use blank cells because those are used to create open spaces
         * in the select screen where extra images can go.
         */
        for (std::vector<PaintownUtil::ReferenceCount<Cell> >::iterator it = cells.begin(); it != cells.end(); it++){
            PaintownUtil::ReferenceCount<Cell> cell = *it;
            if (cell->isRandom()){
                characters.push_back(character);
                // Include stage if required
                if (character.getIncludeStage()){
                    /* We hold the lock so call doAddStage directly */
                    doAddStage(character.getStage());
                }

                cell->setCharacter(character);
                return true;
            }
        }
    }
    return false;
}

void CharacterSelect::addEmpty(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (nextCell < cells.size()){
        cells[nextCell]->setEmpty();
        nextCell++;
    }
}

void CharacterSelect::addUnused(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (nextCell < cells.size()){
        cells[nextCell]->setUnused();
        nextCell++;
    }
}

void CharacterSelect::addRandom(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (nextCell < cells.size()){
        cells[nextCell]->setRandom();
        nextCell++;
    }
}
    
int CharacterSelect::stageCount() const {
    PaintownUtil::Thread::ScopedLock scoped(lock);
    return stages.getStages().size();
}

/* Only call this if you hold the lock */
void CharacterSelect::doAddStage(const Filesystem::AbsolutePath & stage){
    /* Check if it looks like a character, in which case ignore the stage */
    try{
        Util::probeDef(stage, "files", "cns");
    } catch (const MugenException & fail){
        try{
            /* Make sure its a real stage */
            Util::probeDef(stage, "info", "name");
            stages.add(stage);
        } catch (const MugenException & fail){
        }
    }
}

void CharacterSelect::addStage(const Filesystem::AbsolutePath & stage){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    doAddStage(stage);
}

Mugen::ArcadeData::MatchPath CharacterSelect::getArcadePath(){
    if (currentPlayer == Player2){
        return Mugen::ArcadeData::MatchPath(player2.getOpponentCollection().getType(), arcadeOrder, characters, stages.getStages());
    }
    
    return Mugen::ArcadeData::MatchPath(player1.getOpponentCollection().getType(), arcadeOrder, characters, stages.getStages());
}

Mugen::ArcadeData::MatchPath CharacterSelect::getTeamArcadePath(){
    if (currentPlayer == Player2){
        return Mugen::ArcadeData::MatchPath(player2.getOpponentCollection().getType(), teamArcadeOrder, characters, stages.getStages());
    }
    
    return Mugen::ArcadeData::MatchPath(player1.getOpponentCollection().getType(), teamArcadeOrder, characters, stages.getStages());
}

const std::vector<Mugen::ArcadeData::CharacterInfo> & CharacterSelect::getCharacters() const {
    return characters;
}

const std::vector<Filesystem::AbsolutePath> & CharacterSelect::getStages() const {
    return stages.getStages();
}
    
void CharacterSelect::setSelectInfo(const std::vector<SelectInfo> & infos){
    characterInfos.insert(characterInfos.end(), infos.begin(), infos.end());
}

void CharacterSelect::parseSelect(){
    const Filesystem::AbsolutePath file = Util::findFile(Filesystem::RelativePath(selectFile.getFilename().path()));
    
    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(file));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + file.path() + " in" + diff.printTime("") << std::endl;
    
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
        std::string head = section->getName();
        
        head = Mugen::Util::fixCase(head);

        if (head == "characters"){
            class CharacterWalker: public Ast::Walker{
            public:
                CharacterWalker(CharacterSelect & self):
                self(self){}
                virtual ~CharacterWalker(){}
            
                CharacterSelect & self;

                std::vector<SelectInfo> infos;
                
                virtual void onValueList(const Ast::ValueList & list){
                    // Grab Character
                    std::string temp;
                    Ast::View view = list.view();
                    view >> temp;

                    if (temp == "blank"){
                        self.addEmpty();
                    } else if (temp == "randomselect"){
                        self.addRandom();
                    } else {
                        if (Data::getInstance().getSearchType() == Data::SelectDef ||
                            Data::getInstance().getSearchType() == Data::SelectDefAndAuto){

                            SelectInfo info;
                            info.name = temp;

                            try{
                                /*
                                Filesystem::AbsolutePath tempPath;
                                tempPath = Util::findCharacterDef(temp);
                                Mugen::ArcadeData::CharacterInfo character(tempPath);              
                                */
                                try{
                                    // Grab stage
                                    view >> temp;
                                    if (PaintownUtil::matchRegex(temp, PaintownUtil::Regex("order = "))){
                                        temp.replace(0,std::string("order = ").size(),"");
                                        // character.setOrder((int)atoi(temp.c_str()));
                                        info.order = (int)atoi(temp.c_str());
                                    } else if (temp == "random"){
                                        // character.setRandomStage(true);
                                        info.randomStage = true;
                                    } else {
                                        try{
                                            info.stage = temp;
                                            // character.setStage(Util::findFile(Filesystem::RelativePath(temp)));
                                        } catch (const Filesystem::NotFound & ex){
                                            Global::debug(0) << "Could not find stage " << temp << ": " << ex.getTrace() << std::endl;
                                        }
                                    }
                                    // Grab options
                                    /* TODO: make the parser turn these into better AST nodes.
                                     * something like Assignment(Id(music), Filename(whatever))
                                     */
                                    while(true){
                                        view >> temp;
                                        if (PaintownUtil::matchRegex(temp, PaintownUtil::Regex("includestage = "))){
                                            temp.replace(0,std::string("includestage = ").size(),"");
                                            info.includeStage = (bool)atoi(temp.c_str());
                                            // character.setIncludeStage((bool)atoi(temp.c_str()));
                                        } else if (PaintownUtil::matchRegex(temp, PaintownUtil::Regex("music = "))){
                                            temp.replace(0,std::string("music = ").size(),"");
                                            info.music = temp;
                                            // character.setMusic(Util::findFile(Filesystem::RelativePath(temp)));
                                        } else if (PaintownUtil::matchRegex(temp, PaintownUtil::Regex("order = "))){
                                            temp.replace(0,std::string("order = ").size(),"");
                                            info.order = (int)atoi(temp.c_str());
                                            // character.setOrder((int)atoi(temp.c_str()));
                                        }
                                    }
                                } catch (const Ast::Exception & e){
                                }

                                self.addUnused();
                                // self.addCharacter(character);
                                infos.push_back(info);
                            } catch (const Storage::NotFound & fail){
                                Global::debug(0) << "Could not add character from " << temp << " because " << fail.getTrace() << std::endl;
                                self.addEmpty();
                            } catch (const MugenException & fail){
                                Global::debug(0) << "Could not add character from " << temp << " because " << fail.getTrace() << std::endl;
                                self.addEmpty();
                            }
                        }
                    }
                }
            };

            CharacterWalker walk(*this);
            section->walk(walk);
            setSelectInfo(walk.infos);
        } else if (head == "extrastages"){
            class StageWalker: public Ast::Walker{
            public:
                StageWalker(CharacterSelect & self):
                self(self){
                }

                virtual ~StageWalker(){
                }

                CharacterSelect & self;

                virtual void onValueList(const Ast::ValueList & list){
                    // Get Stage info and save it
                    try {
                        std::string temp;
                        list.view() >> temp;
                        Global::debug(1) << "stage: " << temp << std::endl;
                        self.addStage(Util::findFile(Filesystem::RelativePath(temp)));
                    } catch (const Ast::Exception & e){
                    } catch (const Filesystem::NotFound & ex){
                        Global::debug(0) << "Could not find extra stage: " << ex.getTrace() << std::endl;
                    }
                }
            };
            // Only add stages if auto search is off
            if (Data::getInstance().getSearchType() == Data::SelectDef ||
                Data::getInstance().getSearchType() == Data::SelectDefAndAuto){
                StageWalker walk(*this);
                section->walk(walk);
            }
        } else if (head == "options"){
            class OptionWalker: public Ast::Walker{
            public:
                OptionWalker(std::vector<int> & arcade, std::vector<int> & team):
                arcade(arcade),
                team(team){
                }
                virtual ~OptionWalker(){}
                std::vector<int> & arcade;
                std::vector<int> & team;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "arcade.maxmatches"){
                        Ast::View view = simple.view();
                        // only 10 max matches
                        for (int i = 0; i < 10; ++i){
                            try{
                                int matches;
                                view >> matches;
                                // No need to save the remaining of the matchup setup
                                if (matches == 0){
                                    break;
                                }
                                arcade.push_back(matches);
                            } catch (const Ast::Exception & e){
                                break;
                            }
                        }
                    } else if (simple == "team.maxmatches"){
                        Ast::View view = simple.view();
                        // only 10 max matches
                        for (int i = 0; i < 10; ++i){
                            try{
                                int matches;
                                view >> matches;
                                // No need to save the remaining of the matchup setup
                                if (matches == 0){
                                    break;
                                }
                                team.push_back(matches);
                            } catch (const Ast::Exception & e){
                                break;
                            }
                        }
                    }
                }
            };
            OptionWalker walk(arcadeOrder,teamArcadeOrder);
            section->walk(walk);
        } else {
            // throw MugenException("Unhandled Section in '" + file.path() + "': " + head, __FILE__, __LINE__); 
            std::ostringstream context;
            context << __FILE__ << ":" << __LINE__;
            Global::debug(0, context.str()) << "Warning: Unhandled Section in '" + file.path() + "': " + head << std::endl;
        }
    }
}

void CharacterSelect::setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<Animation> cursor){
    player1ActiveCursor = cursor;
}

void CharacterSelect::setPlayer1DoneCursor(PaintownUtil::ReferenceCount<Animation> cursor){
    player1DoneCursor = cursor;
}

void CharacterSelect::setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<Animation> cursor){
    player2ActiveCursor = cursor;
}

void CharacterSelect::setPlayer2DoneCursor(PaintownUtil::ReferenceCount<Animation> cursor){
    player2DoneCursor = cursor;
}

const std::vector<CharacterSelect::SelectInfo> & CharacterSelect::getSelectInfo() const {
    return characterInfos;
}

class SelectLogic: public PaintownUtil::Logic {
public:
    SelectLogic(InputMap<Mugen::Keys> & input1, InputMap<Mugen::Keys> & input2, Mugen::CharacterSelect & select, Searcher & search):
    is_done(false),
    canceled(false),
    input1(input1),
    input2(input2),
    select(select),
    search(search),
    // characterAddThread(PaintownUtil::Thread::uninitializedValue),
    subscription(*this),
    withSubscription(search, subscription, select.getSelectInfo(), select){
    }

    bool is_done, canceled;
    InputMap<Mugen::Keys> & input1, & input2;
    Mugen::CharacterSelect & select;
    Searcher & search;
    
    PaintownUtil::Thread::LockObject lock;
    
    // PaintownUtil::Thread::Id characterAddThread;
    PaintownUtil::Thread::LockObject addCharacterLock;
    // std::deque<Filesystem::AbsolutePath> addCharacters;

    class Subscriber: public Searcher::Subscriber {
    public:
        Subscriber(SelectLogic & owner):
        owner(owner),
        going(true),
        check(going, lock.getLock()){
            thread = std::thread([this](){
                this->process();
            });
        }

        void stop(){
            check.set(false);
            thread.join();
        }

        virtual ~Subscriber(){
            stop();
        }
    
        virtual void receiveCharacters(const std::vector<Filesystem::AbsolutePath> & paths){
            PaintownUtil::Thread::ScopedLock scoped(lock);
            characters.insert(characters.end(), paths.begin(), paths.end());
        }

        virtual void receiveStages(const std::vector<Filesystem::AbsolutePath> & paths){
            PaintownUtil::Thread::ScopedLock scoped(lock);
            stages.insert(stages.end(), paths.begin(), paths.end());
        }

        static void * doProcess(void * self_){
            Subscriber * self = (Subscriber*) self_;
            self->process();
            return NULL;
        }

        bool maybeAddCharacter(){
            Filesystem::AbsolutePath path;
            {
                PaintownUtil::Thread::ScopedLock scoped(lock);
                if (characters.size() > 0){
                    path = characters.front();
                    characters.erase(characters.begin());
                } else {
                    return false;
                }
            }
            owner.addCharacter(path);
            return true;
        }

        bool maybeAddStage(){
            Filesystem::AbsolutePath path;
            {
                PaintownUtil::Thread::ScopedLock scoped(lock);
                if (stages.size() > 0){
                    path = stages.front();
                    stages.erase(stages.begin());
                } else {
                    return false;
                }
            }
            owner.addStage(path);
            return true;
        }

        /* Actually process the characters/stages. Interleave them
         * so we don't end up delaying stages for too long
         */
        void process(){
            while (check.get()){
                bool didWork = maybeAddCharacter();
                
                /* Maybe quit early */
                if (!check.get()){
                    continue;
                }

                didWork |= maybeAddStage();

                /* Didn't do anything so wait for more work to appear.
                 * Might be able to use a condition variable instead..
                 */
                if (!didWork){
                    PaintownUtil::rest(1);
                }
            }
        }

        SelectLogic & owner;

        volatile bool going;

        /* order matters */
        PaintownUtil::Thread::LockObject lock;
        PaintownUtil::ThreadBoolean check;

        std::thread thread;
        std::vector<Filesystem::AbsolutePath> characters;
        std::vector<Filesystem::AbsolutePath> stages;
    };

    Subscriber subscription;
    
    class WithSubscription{
    public:
        WithSubscription(Searcher & search, Searcher::Subscriber & subscription, const std::vector<CharacterSelect::SelectInfo> & infos, CharacterSelect & select):
        subscribeThread(PaintownUtil::Thread::uninitializedValue),
        search(search),
        subscription(subscription),
        stop(false),
        check(stop, lock.getLock()),
        infos(infos),
        select(select){
            if (!PaintownUtil::Thread::createThread(&subscribeThread, NULL, (PaintownUtil::Thread::ThreadFunction) subscribe, this)){
                doSubscribe();
            }
        }

        PaintownUtil::Thread::Id subscribeThread;
        Searcher & search;
        Searcher::Subscriber & subscription;
        volatile bool stop;
        PaintownUtil::Thread::LockObject lock;
        PaintownUtil::ThreadBoolean check;

        std::vector<CharacterSelect::SelectInfo> infos;
        CharacterSelect & select;

        /* Start the subscription in a thread so that characters that are already found
         * will be added in a separate thread instead of the main one
         */
        static void * subscribe(void * me){
            WithSubscription * self = (WithSubscription*) me;
            self->doSubscribe();
            return NULL;
        }

        void addInfo(const CharacterSelect::SelectInfo & info){
            try{
                Mugen::ArcadeData::CharacterInfo character(Mugen::Util::findCharacterDef(info.name));
                character.setOrder(info.order);
                character.setRandomStage(info.randomStage);
                try{
                    character.setStage(Mugen::Util::findFile(Filesystem::RelativePath(info.stage)));
                } catch (const Filesystem::NotFound & ex){
                    Global::debug(0) << "Could not find stage " << info.stage << ": " << ex.getTrace() << std::endl;
                }
                character.setIncludeStage(info.includeStage);
                try{
                    character.setMusic(Mugen::Util::findFile(Filesystem::RelativePath(info.music)));
                } catch (const Filesystem::NotFound & ex){
                    Global::debug(0) << "Could not find music " << info.music << ":" << ex.getTrace() << std::endl;
                }

                select.addCharacter(character);
            } catch (const Storage::NotFound & fail){
                Global::debug(0) << "Could not add character " << info.name << " because " << fail.getTrace() << std::endl;
            } catch (const MugenException & fail){
                Global::debug(0) << "Could not add character " << info.name << " because " << fail.getTrace() << std::endl;
            }
        }

        void doSubscribe(){
            /* First add all infos that were specified in the select.def */
            while (!check.get() && infos.size() > 0){
                CharacterSelect::SelectInfo info = infos.front();
                infos.erase(infos.begin());
                addInfo(info);
            }

            /* Then start the subscriber and add characters that are found.
             * Only subscribe if auto search is enabled.
             */
            if (Data::getInstance().autoSearch()){
                search.subscribe(&subscription);
            }
        }


        virtual ~WithSubscription(){
            check.set(true);

            /* Make sure we wait for the initial join to finish before trying to unsubscribe.
             * Is it a race condition if the subscribeThread was never started before we get here?
             * I don't think so because the constructor will ensure that subscribeThread is either
             * the uninitialized value or is some thread value, possibly not started. If the thread
             * is uninitialized then joining is a no-op, which means the subscription must have been
             * started in the constructor (by virtue of the thread not being created), or if the
             * thread was created, even if not started, then it will eventually start and joinThread
             * will wait for it to finish.
             */
            PaintownUtil::Thread::joinThread(subscribeThread);
            search.unsubscribe(&subscription);
        }
    };
    
    WithSubscription withSubscription;
    
    void addCharacter(const Filesystem::AbsolutePath & path){
        /* We only care if the searcher adds the same def file twice. The user
         * is free to add kfm multiple times in select.def
         */
        // if (!done() && select.uniqueCharacter(path)){
        if (select.uniqueCharacter(path)){
            if (Storage::isContainer(path)){
                /* What exactly will happen if we add the same zip file twice?
                 * The old zip entries will get overwritten by new ones
                 * and the old zip container will go away once there are no
                 * more entries pointing to it.
                 */
                Storage::instance().addOverlay(path, path.getDirectory());
                try {
                    /* Found mugen/chars/foo.zip or something
                     * try to load mugen/chars/foo/foo.def
                     */
                    std::string where = Path::removeExtension(path.getFilename().path());
                    /* path/where/where.def */
                    Filesystem::AbsolutePath def = path.getDirectory().join(Filesystem::RelativePath(where)).join(Filesystem::RelativePath(where + ".def"));
                    if (!select.uniqueCharacter(def)){
                        Storage::instance().removeOverlay(path, path.getDirectory());
                    } else {
                        select.addCharacter(Mugen::ArcadeData::CharacterInfo(def));
                    }
                } catch (...){
                    Storage::instance().removeOverlay(path, path.getDirectory());
                }
            } else {
                try {
                    select.addCharacter(Mugen::ArcadeData::CharacterInfo(path));
                } catch (...){
                    // Can't add character ignore
                }
            }
        }
    }
    
    void addStage(const Filesystem::AbsolutePath & path){
        select.addStage(path);
    }
    
    bool done(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return is_done;
    }

    void doInput(int side){
        std::vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(input1, InputSource(true));
        for (std::vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<Mugen::Keys>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Esc){
                    if (!canceled){
                        select.cancel();
                        canceled = true;
                    }
                }
                if (event.out == Left){
                    select.left(side);
                }
                if (event.out == Down){
                    select.down(side);
                }
                if (event.out == Right){
                    select.right(side);
                }
                if (event.out == Up){
                    select.up(side);
                }
                if (event.out == A){
                    select.select(side, 0);
                }
                if (event.out == B){
                    select.select(side, 1);
                }
                if (event.out == C){
                    select.select(side, 2);
                }
                if (event.out == X){
                    select.select(side, 3);
                }
                if (event.out == Y){
                    select.select(side, 4);
                }
                if (event.out == Z){
                    select.select(side, 5);
                }
            }
        }
    }

    void run(){
        if (select.getCurrentPlayer() == CharacterSelect::Player1 ||
            select.getCurrentPlayer() == CharacterSelect::Both){
            doInput(0);
        }

        if (select.getCurrentPlayer() == CharacterSelect::Player2 ||
            select.getCurrentPlayer() == CharacterSelect::Both){
            doInput(1);
        }

        select.act();
        if (select.getCurrentPlayer() == CharacterSelect::Demo){
            if (InputManager::anyInput()){
                if (!canceled){
                    select.cancel();
                    canceled = true;
                }
            }
        }

        {
            PaintownUtil::Thread::ScopedLock scoped(lock);
            is_done = select.isDone();
        }
    }

    double ticks(double system){
        return system;
    }
};

class SelectDraw: public PaintownUtil::Draw {
public:
    SelectDraw(Mugen::CharacterSelect & select):
    select(select){
    }
    
    Mugen::CharacterSelect & select;

    void draw(const Graphics::Bitmap & buffer){
        // buffer.clear();
        Graphics::StretchedBitmap work(DEFAULT_SELECT_WIDTH, DEFAULT_SELECT_HEIGHT, buffer, Graphics::StretchedBitmap::NoClear, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
        work.start();
        select.draw(work);
        work.finish();
        // buffer.BlitToScreen();
    }
};

PaintownUtil::ReferenceCount<PaintownUtil::Logic> CharacterSelect::getLogic(InputMap<Mugen::Keys> & input1, InputMap<Mugen::Keys> & input2, Searcher & search){
    return PaintownUtil::ReferenceCount<SelectLogic>(new SelectLogic(input1, input2, *this, search));
}

PaintownUtil::ReferenceCount<PaintownUtil::Draw> CharacterSelect::getDraw(){
    return PaintownUtil::ReferenceCount<SelectDraw>(new SelectDraw(*this));
}
