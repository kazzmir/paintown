#include "character-select.h"

#include <iostream>

#include "util/timedifference.h"
#include "util/trans-bitmap.h"
#include "mugen/ast/all.h"
#include "mugen/sound.h"

using namespace Mugen;

// Cell static members
PaintownUtil::ReferenceCount<MugenSprite> Cell::background = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::randomIcon = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player1ActiveCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player1DoneCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player2ActiveCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
PaintownUtil::ReferenceCount<MugenSprite> Cell::player2DoneCursor = PaintownUtil::ReferenceCount<MugenSprite>(NULL);
bool Cell::blinkCursor = false;
int Cell::blinkTime = 0;
Mugen::Effects Cell::effects = Mugen::Effects();
int Cell::offsetX = 0;
int Cell::offsetY = 0;

bool TeamMenu::wrapping = false;

int Player::randomSwitchTime = 4;

SelectFont::SelectFont():
font(PaintownUtil::ReferenceCount<MugenFont>(NULL)),
bank(0),
position(0){
}

SelectFont::SelectFont(PaintownUtil::ReferenceCount<MugenFont> font, int bank, int position):
font(font),
bank(bank),
position(position){
}

SelectFont::SelectFont(const SelectFont & copy):
font(copy.font),
bank(copy.bank),
position(copy.position){
}

SelectFont::~SelectFont(){
}

const SelectFont & SelectFont::operator=(const SelectFont & copy){
    font = copy.font;
    bank = copy.bank;
    position = copy.position;
    return * this;
}

void SelectFont::draw(int x, int y, const std::string & text, const Graphics::Bitmap & work){
    if (font != NULL){
        font->render(x, y, position, bank, work, text);
    }
}

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

Cell::Cell(unsigned int index, const Gui::SelectListInterface * parent):
index(index),
parent(parent),
empty(true),
isRandom(false),
flash(0){
}

Cell::~Cell(){
}

void Cell::act(){
    if (flash){
        flash--;
    }
}

void Cell::draw(int x, int y, int width, int height, const Graphics::Bitmap & work, const Font & font) const{
    if (background != NULL){
        background->render(x, y, work);
    }
    
    // If random draw random icon otherwise draw character image if available
    if (isRandom){
        if (randomIcon != NULL){
            randomIcon->render(x + offsetX, y + offsetY, work, effects);
        }
    } else if (!empty) {
        character.drawIcon(x + offsetX, y + offsetY, work, effects);
    }
    
    if (parent->getCurrentIndex(0) == parent->getCurrentIndex(1) && parent->getCurrentIndex(0) == index){
        // Blink cursors
        if (parent->getCurrentState(0) == Gui::SelectListInterface::Active && parent->getCurrentState(1) == Gui::SelectListInterface::Active){
            drawPlayer1Cursor(x, y, work);
            drawPlayer2Cursor(x, y, work, true);
        } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Active && parent->getCurrentState(1) == Gui::SelectListInterface::Done){
            drawPlayer2Cursor(x, y, work);
            drawPlayer1Cursor(x, y, work);
        } else if (parent->getCurrentState(0) == Gui::SelectListInterface::Done && parent->getCurrentState(1) == Gui::SelectListInterface::Active){
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
    
    // Flash
    if (flash){
        // FIXME hot pink shows up after white
        Graphics::Bitmap::transBlender( 0, 0, 0, int(25.5 * flash) );
        work.translucent().rectangleFill(x-1, y-1, x-1+width, y-1+height, Graphics::makeColor(255,255,255));
    }
}

bool Cell::isEmpty() const{
    return empty;
}

void Cell::setRandom(bool r){
    empty = !r;
    isRandom = r;
}

void Cell::setCharacter(const Mugen::ArcadeData::CharacterInfo & character){
    this->character = character;
    empty = false;
}

void Cell::select(){
    flash = 10;
}

void Cell::setBackground(PaintownUtil::ReferenceCount<MugenSprite> background){
    Cell::background = background;
}

void Cell::setRandomIcon(PaintownUtil::ReferenceCount<MugenSprite> randomIcon){
    Cell::randomIcon = randomIcon;
}

void Cell::setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player1ActiveCursor = cursor;
}

void Cell::setPlayer1DoneCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player1DoneCursor = cursor;
}

void Cell::setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player2ActiveCursor = cursor;
}

void Cell::setPlayer2DoneCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor){
    Cell::player2DoneCursor = cursor;
}

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


TeamMenu::TeamMenu():
current(Mugen::ArcadeData::CharacterCollection::Simultaneous),
turns(Mugen::ArcadeData::CharacterCollection::Turns2),
x(0),
y(0),
background(PaintownUtil::ReferenceCount<MugenSprite>(NULL)),
itemOffsetX(0),
itemOffsetY(0),
itemSpacingX(0),
itemSpacingY(0),
valueIconOffsetX(0),
valueIconOffsetY(0),
icon(PaintownUtil::ReferenceCount<MugenSprite>(NULL)),
emptyValueIconOffsetX(0),
emptyValueIconOffsetY(0),
emptyIcon(PaintownUtil::ReferenceCount<MugenSprite>(NULL)),
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
    if (!enabled){
        return;
    }
    font.act();
}

void StageMenu::draw(const Graphics::Bitmap & work){
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
    try {
        for (std::vector<Filesystem::AbsolutePath>::iterator i = stages.begin(); i != stages.end(); ++i){
            const Filesystem::AbsolutePath & check = *i;
            if (stage == check){
                return;
            }
        }
        AstRef parsed(Util::parseDef(stage.path()));
        const std::string & name = Util::probeDef(parsed, "info", "name");
        stages.push_back(stage);
        names.push_back(name);
    } catch (const MugenException & ex){
        Global::debug(0) << "Warning! Tried to load file: '" << stage.path() << "'. Message: " << ex.getReason() << std::endl;
    }
}

bool StageMenu::up(){
    return false;
}

bool StageMenu::down(){
    return false;
}

bool StageMenu::left(){
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

SoundSystem::SoundSystem(){
}

SoundSystem::~SoundSystem(){
}

void SoundSystem::init(const std::string & file){
    Mugen::Util::readSounds(Util::findFile(Filesystem::RelativePath(file)), sounds);
    Global::debug(1) << "Got Sound File: '" << file << "'" << std::endl;
}

void SoundSystem::play(const Type & type){
    MugenSound * sound = sounds[soundLookup[type].group][soundLookup[type].index];
    if (sound){
        sound->play();
    }
}

void SoundSystem::set(const Type & type, int group, int sound){
    IndexValue values;
    values.group = group;
    values.index = sound;
    soundLookup[type] = values;
}

Player::Player(unsigned int cursor, Gui::GridSelect & grid, std::vector< PaintownUtil::ReferenceCount<Cell> > & cells, std::vector<Mugen::ArcadeData::CharacterInfo> & characters, TeamMenu & teamMenu, TeamMenu & opponentTeamMenu, StageMenu & stageMenu, FontHandler & font, FontHandler & opponentFont, SoundSystem & sounds):
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
        moveSound = SoundSystem::Player1Move;
        doneSound = SoundSystem::Player1Done;
        randomSound = SoundSystem::Player1Random;
        teamMoveSound = SoundSystem::Player1TeamMove;
        teamValueSound = SoundSystem::Player1TeamValue;
        teamDoneSound = SoundSystem::Player1TeamDone;
    } else if (cursor == 1){
        moveSound = SoundSystem::Player2Move;
        doneSound = SoundSystem::Player2Done;
        randomSound = SoundSystem::Player2Random;
        teamMoveSound = SoundSystem::Player2TeamMove;
        teamValueSound = SoundSystem::Player2TeamValue;
        teamDoneSound = SoundSystem::Player2TeamDone;
    }
}

Player::~Player(){
}

void Player::act(){
    if (cells[grid.getCurrentIndex(cursor)]->getRandom() && (selectState == Character || selectState == Opponent)){
        switchTime++;
        if (switchTime >= randomSwitchTime){
            switchTime = 0;
            currentRandom = PaintownUtil::rnd(0,characters.size());
            sounds.play(randomSound);
        }
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
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            break;
        case Survival:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
            break;
        case SurvivalCoop:
            drawPortrait(collection, portraitEffects, portraitX, portraitY, font, work);
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
    /*
    const Mugen::ArcadeData::CharacterCollection & currentCollection = selectState == Opponent ? opponentCollection : collection;
    const Mugen::Effects & effects = selectState == Opponent ? opponentPortraitEffects : portraitEffects;
    const int x = selectState == Opponent ? opponentPortraitX : portraitX, y = selectState == Opponent ? opponentPortraitY : portraitY;
    FontHandler & currentFont = selectState == Opponent ? opponentFont : font;
    
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
        if (cells[grid.getCurrentIndex(cursor)]->getRandom()){
            // NOTE I'd prefer to randomize the name, but mugen originally just puts the words random there
            currentFont.draw("Random", work, heightMod);
        } else {
            currentFont.draw(character.getName(), work, heightMod);
        }
    }*/
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
                        sounds.play(SoundSystem::Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.up()){
                        sounds.play(SoundSystem::Player2TeamMove);
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
                        sounds.play(SoundSystem::Player1TeamMove);
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
                        sounds.play(SoundSystem::Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.up(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.up()){
                        sounds.play(SoundSystem::Player2TeamMove);
                    }
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
                        sounds.play(SoundSystem::Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.down()){
                        sounds.play(SoundSystem::Player2TeamMove);
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
                        sounds.play(SoundSystem::Player1TeamMove);
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
                        sounds.play(SoundSystem::Player1TeamMove);
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
                        sounds.play(SoundSystem::Player1TeamMove);
                    }
                    break;
                case Character:
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case OpponentTeam:
                    if (opponentTeamMenu.down()){
                        sounds.play(SoundSystem::Player2TeamMove);
                    }
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
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::Player2TeamValue);
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
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::StageMove);
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
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::Player2TeamValue);
                    }
                case Opponent:
                    if (grid.left(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.left()){
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::Player2TeamValue);
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
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::StageMove);
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
                    if (grid.down(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::StageMove);
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
                        sounds.play(SoundSystem::Player2TeamValue);
                    }
                case Opponent:
                    if (grid.right(cursor)){
                        sounds.play(moveSound);
                    }
                    break;
                case Stage:
                    if (stageMenu.right()){
                        sounds.play(SoundSystem::StageMove);
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

void Player::select(){
    switch (selectState){
        case Team:
            sounds.play(teamDoneSound);
            next();
            break;
        case Character:
            sounds.play(doneSound);
            if (cells[grid.getCurrentIndex(cursor)]->getRandom()){
                cells[currentRandom]->select();
            } else {
                cells[grid.getCurrentIndex(cursor)]->select();
            }
            next();
            break;
        case Opponent:
            sounds.play(doneSound);
            if (cells[grid.getCurrentIndex(cursor)]->getRandom()){
                cells[currentRandom]->select();
            } else {
                cells[grid.getCurrentIndex(cursor)]->select();
            }
            next();
            break;
        case OpponentTeam:
            sounds.play(teamDoneSound);
            next();
            break;
        case Stage:
            sounds.play(SoundSystem::StageDone);
            next();
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

void Player::next(){
    switch (currentGameType){
        case Mugen::Arcade:
            switch (selectState){
                case NotStarted:
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell());
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
                    collection.setFirst(getCurrentCell());
                    selectState = Stage;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
                    stageMenu.setEnabled(true);
                    break;
                case Stage:
                    selectState = Finished;
                    stageMenu.finish();
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
                        collection.setNext(getCurrentCell());
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
                        collection.setNext(getCurrentCell());
                        if (collection.checkSet()){
                            selectState = Stage;
                            grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
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
        case Mugen::TeamCoop:
            switch (selectState){
                case NotStarted:
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell());
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
                        collection.setNext(getCurrentCell());
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
                    selectState = Character;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Active);
                    grid.setCurrentIndex(cursor, cursorPosition);
                    break;
                case Character:
                    collection.setFirst(getCurrentCell());
                    selectState = Finished;
                    grid.setCurrentState(cursor, Gui::SelectListInterface::Done);
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
                    collection.setFirst(getCurrentCell());
                    selectState = Opponent;
                    grid.setCurrentIndex(cursor, opponentCursorPosition);
                    break;
                case Opponent:
                    opponentCollection.setFirst(getCurrentCell());
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
                        collection.setNext(getCurrentCell());
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
                        opponentCollection.setNext(getCurrentCell());
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
        if (cells[grid.getCurrentIndex(cursor)]->getRandom()){
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

const Mugen::ArcadeData::CharacterInfo & Player::getCurrentCell(){
    if (cells[grid.getCurrentIndex(cursor)]->getRandom()){
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
player2(1, grid, cells, characters, player2TeamMenu, player1TeamMenu, stages, player2Font, player1Font, sounds){
    Global::debug(0) << "Got file: " << file.path() << std::endl;
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

        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Util::parseDef(file.path()));
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
                            /*for( Mugen::SpriteMap::iterator i = select.sprites.begin() ; i != select.sprites.end() ; ++i ){
                                // Load these sprites so they are ready to use
                                for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
                                    if( j->second )j->second->load();
                                }
                            }*/
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
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            std::string fontPath;
                            simple.view() >> fontPath;
                            select.fonts.push_back(PaintownUtil::ReferenceCount<MugenFont>(new MugenFont(Util::findFile(Filesystem::RelativePath(fontPath)))));
                            Global::debug(1) << "Got Font File: '" << fontPath << "'" << std::endl;

                        } else {
                            throw MugenException("Unhandled option in Files Section: " + simple.toString(), __FILE__, __LINE__ );
                        }
                    }
                };

                FileWalker walker(*this, baseDir);
                section->walk(walker);
            } else if (head == "title info"){
                /* Nothing */
            } else if (PaintownUtil::matchRegex(head, "^titlebg")){
                /* Nothing */
            } else if (head == "select info"){ 
                class SelectInfoWalker: public Ast::Walker{
                public:
                    SelectInfoWalker(CharacterSelect & self, Mugen::SpriteMap & sprites):
                        self(self),
                        sprites(sprites){
                        }

                    CharacterSelect & self;
                    Mugen::SpriteMap & sprites;
                    
                    Mugen::Effects player1Effects;
                    Mugen::Effects player2Effects;

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
                                self.grid.setAccessEmpty(boxes);
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
                                Cell::setBackground(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setRandomIcon(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
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
                        } else if (simple == "p1.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer1DoneCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player1Move, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player1Done, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player1Random, group, sound);
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
                        } else if (simple == "p2.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                Cell::setPlayer2DoneCursor(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.blink"){
                            try{
                                bool blink;
                                simple.view() >> blink;
                                Cell::setBlinkCursor(blink);
                            } catch (const Ast::Exception & e){
                            }
                        } 
                        else if ( simple == "p2.cursor.move.snd"){ 
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player2Move, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player2Done, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player2Random, group, sound);
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
                                self.sounds.set(SoundSystem::StageMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::StageDone, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "cancel.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Cancel, group, sound);
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
                            self.titleFont.setActive(SelectFont(self.getFont(index), bank, position));
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
                                simple.view() >> player1Effects.facing;
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
                                simple.view() >> player2Effects.facing;
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
                            self.player1Font.setActive(SelectFont(self.getFont(index), bank, position));
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
                            self.player2Font.setActive(SelectFont(self.getFont(index), bank, position));
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
                            self.stages.font.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "stage.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.stages.font.setActive2(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "stage.done.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.stages.font.setDone(SelectFont(self.getFont(index), bank, position));
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
                                self.player1TeamMenu.setBackgroundSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.selftitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.titleFont.setActive(SelectFont(self.getFont(index), bank, position));
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
                                self.player1TeamMenu.enemyTitleFont.setActive(SelectFont(self.getFont(index), bank, position));
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
                                self.sounds.set(SoundSystem::Player1TeamMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.value.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player1TeamValue, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.teammenu.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player1TeamDone, group, sound);
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
                            self.player1TeamMenu.itemFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p1.teammenu.item.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player1TeamMenu.itemCurrentFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p1.teammenu.item.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                                self.player1TeamMenu.itemCurrentFont.setActive2(SelectFont(self.getFont(index), bank, position));
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
                                self.player1TeamMenu.setValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
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
                                self.player1TeamMenu.setEmptyValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
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
                                self.player2TeamMenu.setBackgroundSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.selftitle.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.titleFont.setActive(SelectFont(self.getFont(index), bank, position));
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
                            self.player2TeamMenu.enemyTitleFont.setActive(SelectFont(self.getFont(index), bank, position));
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
                                self.sounds.set(SoundSystem::Player2TeamMove, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.value.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player2TeamValue, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.teammenu.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.sounds.set(SoundSystem::Player2TeamDone, group, sound);
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
                            self.player2TeamMenu.itemFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.teammenu.item.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemCurrentFont.setActive(SelectFont(self.getFont(index), bank, position));
                        } else if ( simple == "p2.teammenu.item.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            self.player2TeamMenu.itemCurrentFont.setActive2(SelectFont(self.getFont(index), bank, position));
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
                                self.player2TeamMenu.setValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
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
                                self.player2TeamMenu.setEmptyValueIconSprite(PaintownUtil::ReferenceCount<MugenSprite>(sprites[group][sprite]));
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

                SelectInfoWalker walker(*this, sprites);
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
            else if (head.find("versusbg" ) != std::string::npos ){ /* Ignore for now */ }
            else if (head == "demo mode" ){ /* Ignore for now */ }
            else if (head == "continue screen" ){ /* Ignore for now */ }
            else if (head == "game over screen" ){ /* Ignore for now */ }
            else if (head == "win screen" ){ /* Ignore for now */ }
            else if (head == "default ending" ){ /* Ignore for now */ }
            else if (head == "end credits" ){ /* Ignore for now */ }
            else if (head == "survival results screen" ){ /* Ignore for now */ }
            else if (head == "option info" ){ /* Ignore for now */ }
            else if (head == "optionbgdef" ){ /* Ignore for now */ }
            else if (head.find("optionbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "music" ){ /* Ignore for now */ }
            else if (head.find("begin action") != std::string::npos ){ /* Ignore for now */ }
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
            PaintownUtil::ReferenceCount<Cell> cell = PaintownUtil::ReferenceCount<Cell>(new Cell(i, &grid));
            cells.push_back(cell);
            grid.addItem(cell.convert<Gui::SelectItem>());
        }
        // Set up cursors
        grid.setCursors(2);
        //grid.setCurrentIndex(0, cursorPosition);
        grid.setCurrentState(0, Gui::SelectListInterface::Disabled);
        //grid.setCurrentIndex(1, opponentCursorPosition);
        grid.setCurrentState(1, Gui::SelectListInterface::Disabled);

    } catch (const Filesystem::NotFound & fail){
        std::ostringstream out;
        out << "Could not load select screen because " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
    
    parseSelect();
}

void CharacterSelect::act(){
    for (std::vector< PaintownUtil::ReferenceCount<Cell> >::iterator i = cells.begin(); i != cells.end(); ++i){
        PaintownUtil::ReferenceCount<Cell> cell = *i;
        cell->act();
    }
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

void CharacterSelect::draw(const Graphics::Bitmap & work){
    // Render Background
    background->renderBackground(0,0,work);
    
    // Temporary bitmap for grid
    const Graphics::Bitmap & temp = Graphics::Bitmap::temporaryBitmap(grid.getWidth(), grid.getHeight());
    temp.clearToMask();
    grid.render(temp, Font::getDefaultFont());
    // Minus 1 since it's been offset
    temp.draw(gridPositionX-1, gridPositionY-1, work);
    
    // render title based on Mugen::GameType
    titleFont.draw(getGameType(currentGameType), work);
    
    // Draw portrait and name
    player1.draw(work);
    player2.draw(work);
    
    // Team Menu
    player1TeamMenu.draw(work);
    player2TeamMenu.draw(work);
    
    // Stages
    stages.draw(work);
    
    background->renderForeground(0,0,work);
}

void CharacterSelect::setMode(const Mugen::GameType & game, const PlayerType & player){
    if (game == Mugen::Undefined){
        return;
    }
    currentGameType = game;
    currentPlayer = player;
    player1.reset();
    player2.reset();
    switch (currentPlayer){
        case Player1:
            //nextPlayer1Selection();
            player1.setCurrentGameType(currentGameType);
            break;
        case Player2:
            player2.setCurrentGameType(currentGameType);
            break;
        case Both:
            player1.setCurrentGameType(currentGameType);
            player2.setCurrentGameType(currentGameType);
            break;
        default:
            break;
    }
}

void CharacterSelect::up(unsigned int cursor){
    if (cursor == 0){
        player1.up();
    } else if (cursor == 1){
        player2.up();
    }
}

void CharacterSelect::down(unsigned int cursor){
    if (cursor == 0){
        player1.down();
    } else if (cursor == 1){
        player2.down();
    }
}

void CharacterSelect::left(unsigned int cursor){
    if (cursor == 0){
        player1.left();
    } else if (cursor == 1){
        player2.left();
    }
}

void CharacterSelect::right(unsigned int cursor){
    if (cursor == 0){
        player1.right();
    } else if (cursor == 1){
        player2.right();
    }
}

void CharacterSelect::select(unsigned int cursor){
    if (cursor == 0){
        player1.select();
    } else if (cursor ==1){
        player2.select();
    }
}

bool CharacterSelect::addCharacter(const Mugen::ArcadeData::CharacterInfo & character){
    // Check if we don't exceed the cell count of the current grid
    if (nextCell < cells.size()){
        // Add to list
        characters.push_back(character);
        // Include stage if required
        if (character.getIncludeStage()){
            addStage(character.getStage());
        }
        // Add to current cell
        cells[nextCell]->setCharacter(character);
        // Increment cell
        nextCell++;
        return true;
    }
    return false;
}

void CharacterSelect::addEmpty(){
    if (nextCell < cells.size()){
        cells[nextCell]->setEmpty(true);
        nextCell++;
    }
}

void CharacterSelect::addRandom(){
    if (nextCell < cells.size()){
        cells[nextCell]->setRandom(true);
        nextCell++;
    }
}

void CharacterSelect::addStage(const Filesystem::AbsolutePath & stage){
    stages.add(stage);
}

/* indexes start at 1 */
PaintownUtil::ReferenceCount<MugenFont> CharacterSelect::getFont(int index) const {
    if (index - 1 >= 0 && index - 1 < (signed) fonts.size()){
        return fonts[index - 1];
    } else {
        std::ostringstream out;
        out << "No font for index " << index;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

void CharacterSelect::parseSelect(){
    const Filesystem::AbsolutePath file = Util::findFile(Filesystem::RelativePath(selectFile.getFilename().path()));
    
    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(file.path()));
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
                        Mugen::ArcadeData::CharacterInfo character(Util::findCharacterDef(temp));              
                        try{
                            // Grab stage
                            view >> temp;
                            if (PaintownUtil::matchRegex(temp, "order = ")){
                                temp.replace(0,std::string("order = ").size(),"");
                                character.setOrder((int)atoi(temp.c_str()));
                            } else if (temp == "random"){
                                character.setRandomStage(true);
                            } else {
                                character.setStage(Util::findFile(Filesystem::RelativePath(temp)));
                            }
                            // Grab options
                            /* TODO: make the parser turn these into better AST nodes.
                            * something like Assignment(Id(music), Filename(whatever))
                            */
                            while(true){
                                view >> temp;
                                if (PaintownUtil::matchRegex(temp,"includestage = ")){
                                    temp.replace(0,std::string("includestage = ").size(),"");
                                    character.setIncludeStage((bool)atoi(temp.c_str()));
                                } else if (PaintownUtil::matchRegex(temp,"music = ")){
                                    temp.replace(0,std::string("music = ").size(),"");
                                    character.setMusic(Util::findFile(Filesystem::RelativePath(temp)));
                                } else if (PaintownUtil::matchRegex(temp,"order = ")){
                                    temp.replace(0,std::string("order = ").size(),"");
                                    character.setOrder((int)atoi(temp.c_str()));
                                }
                            }
                        } catch (const Ast::Exception & e){
                        }
                        self.addCharacter(character);
                    }
                }
            };

            CharacterWalker walk(*this);
            section->walk(walk);
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
                    }
                }
            };
            StageWalker walk(*this);
            section->walk(walk);
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