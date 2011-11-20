#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/stretch-bitmap.h"
#include "character-select.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>
#include <sstream>
#include <iostream>

#include "stage.h"

#include "util/init.h"
#include "util/events.h"
#include "util/parameter.h"
#include "util/resource.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/music.h"
#include "util/timedifference.h"
#include "util/thread.h"
#include "globals.h"
#include "factory/font_render.h"
#include "exceptions/shutdown_exception.h"

#include "menu/menu_option.h"
#include "menu/options.h"

#include "animation.h"
#include "background.h"
#include "character.h"
#include "sound.h"
#include "reader.h"
#include "config.h"
#include "sprite.h"
#include "util.h"
#include "font.h"

#include "ast/all.h"
#include "parser/all.h"

#include "util/loading.h"

#include "util/input/input-manager.h"
#include "util/input/input-source.h"

#include "exceptions/exception.h"

namespace PaintownUtil = ::Util;

#ifdef WII
extern "C" void __sfp_lock_acquire();
extern "C" void __sfp_lock_release();
#endif

using namespace std;
using namespace Mugen;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

#if 0
static const Filesystem::AbsolutePath fixStageName(const std::string &stage){
    /* FIXME not a good solution to get file
     * jon: why isn't it good?
     */
    std::string defFile = stage;
    /* strip 'stage' from the def file if its already there */
    if (defFile.find("stages/") == 0){
        defFile = string("./") + defFile.substr(strlen("stages/"));
    }

    // Filesystem::AbsolutePath baseDir = Filesystem::find(Filesystem::RelativePath("mugen/stages/"));

    if (defFile.find(".def") == std::string::npos){
	defFile += ".def";
    }

    return Filesystem::findInsensitive(Filesystem::RelativePath("mugen/stages/").join(Filesystem::RelativePath(defFile)));

    /*
    // Get correct directory
    baseDir = Filesystem::AbsolutePath(Mugen::Util::getFileDir(baseDir.path() + ourDefFile));
    return Mugen::Util::getCorrectFileLocation(baseDir, Filesystem::RelativePath(ourDefFile).getFilename().path());
    */
}
#endif

FontHandler::FontHandler():
state(Normal),
font(0),
bank(0),
position(0),
blinkFont(0),
blinkBank(0),
blinkPosition(0),
doneFont(0),
doneBank(0),
donePosition(0),
ticker(0),
blinkTime(10),
blinkState(Normal){
}

FontHandler::~FontHandler(){
}

void FontHandler::act(){
    switch(state){
	case Blink:
	    ticker++;
	    if (ticker == blinkTime){
		ticker = 0;
		if (blinkState == Normal){
		    blinkState = Blink;
		} else if (blinkState == Blink){
		    blinkState = Normal;
		}
	    }
	    break;
	case Normal:
	case Done:
	default:
	    break;
    }
}

void FontHandler::render(const std::string &text, const Graphics::Bitmap &bmp){
    switch(state){
	default:
	case Normal:
            if (font){
                font->render(location.x, location.y, position, bank, bmp, text);
            }
	    break;
	case Blink:
	    if (blinkState == Normal){
                if (font){
                    font->render(location.x, location.y, position, bank, bmp, text);
                }
	    } else if (blinkState == Blink){
                if (blinkFont){
                    blinkFont->render(location.x, location.y, blinkPosition, blinkBank, bmp, text);
                }
	    }
	    break;
	case Done: {
            if (doneFont){
                doneFont->render(location.x, location.y, donePosition, doneBank, bmp, text);
            }
	    break;
        }
    }
}

CharacterInfo::CharacterInfo(const Filesystem::AbsolutePath &definitionFile):
definitionFile(definitionFile),
baseDirectory(definitionFile.getDirectory()),
currentPlayer1Act(1),
currentPlayer2Act(1),
icon(0),
portrait(0),
randomStage(false),
order(1),
referenceCell(0),
character1(0),
character2(0){
    try{
        AstRef parsed(Util::parseDef(definitionFile.path()));

        spriteFile = Filesystem::RelativePath(Util::probeDef(parsed, "files", "sprite"));
        name = Util::probeDef(parsed, "info", "name");
        displayName = Util::probeDef(parsed, "info", "displayname");

        /* Grab the act files, in mugen it's strictly capped at 12 so we'll do the same */
        for (int i = 0; i < 12; ++i){
            stringstream act;
            act << "pal" << i;
            try {
                std::string actFile = Util::probeDef(parsed, "files", act.str());
                actCollection.push_back(Filesystem::RelativePath(actFile));
            } catch (const MugenException &me){
                // Ran its course got what we needed
            }
        }

        if (actCollection.size() == 0){
            throw MugenException("No pal files specified");
        }

        // just a precaution
        // spriteFile = Util::removeSpaces(spriteFile);
        Filesystem::AbsolutePath realSpriteFile = Storage::instance().findInsensitive(Storage::instance().cleanse(baseDirectory).join(spriteFile));

        /* pull out the icon and the portrait from the sff */
        Util::getIconAndPortrait(realSpriteFile, baseDirectory.join(actCollection[0]), &icon, &portrait);
    } catch (...){
        /* barf! */
        cleanup();
        throw;
    }
}

void CharacterInfo::cleanup(){
    if (icon){
        delete icon;
        icon = NULL;
    }
    if (portrait){
        delete portrait;
        portrait = NULL;
    }
    if (character1){
	delete character1;
        character1 = NULL;
    }
    if (character2){
	delete character2;
        character2 = NULL;
    }
}

CharacterInfo::~CharacterInfo(){
    cleanup();
}

void CharacterInfo::loadPlayer1(){
    if (character1){
	return;
    }
    character1 = new Mugen::Character(definitionFile, Stage::Player1Side);
    character1->load(currentPlayer1Act);
}

void CharacterInfo::loadPlayer2(){
    if (character2){
	return;
    }
    character2 = new Mugen::Character(definitionFile, Stage::Player2Side);
    character2->load(currentPlayer2Act);
}

void CharacterInfo::setPlayer1Act(int number){
    currentPlayer1Act = number;
}

void CharacterInfo::setPlayer2Act(int number){
    currentPlayer2Act = number;
}

// Stage selector
StageHandler::StageHandler():
currentStage(0),
display(false),
selecting(true),
moveSound(0),
selectSound(0){
    stages.push_back(Filesystem::AbsolutePath()); // "Random"
    stageNames.push_back("Stage: Random");
}

StageHandler::~StageHandler(){
}

void StageHandler::act(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    font.act();
}

void StageHandler::render(const Graphics::Bitmap &bmp){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (display){
	font.render(stageNames[currentStage],bmp);
    }
}
	
//! Get current selected stage
const Filesystem::AbsolutePath & StageHandler::getStage(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    // check if random first;
    if (currentStage == 0){
	return getRandomStage();
    }
    return stages[currentStage];
}

//! Get random stage
const Filesystem::AbsolutePath & StageHandler::getRandomStage(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    return stages[PaintownUtil::rnd(1,stages.size())];
}

//! Set Next Stage
void StageHandler::next(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (!selecting){
	return;
    }
    if (currentStage < stages.size()-1){
	currentStage++;
    } else {
	currentStage = 0;
    }
    if (stages.size() > 1){
        if (moveSound){
            moveSound->play();
        }
    }
}

//! Set Prev Stage
void StageHandler::prev(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    if (!selecting){
	return;
    }
    if (currentStage > 0){
	currentStage--;
    } else {
	currentStage = stages.size()-1;
    }
    if (stages.size() > 1){
        if (moveSound){
            moveSound->play();
        }
    }
}

void StageHandler::toggleSelecting(){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    selecting = !selecting;
    if (!selecting) {
	font.setState(font.Done);
        if (selectSound){
            selectSound->play();
        }
    }
}

void StageHandler::addStage(const Filesystem::AbsolutePath & stage){
    PaintownUtil::Thread::ScopedLock scoped(lock);
    try{
        if (std::find(stages.begin(), stages.end(), stage) == stages.end()){
            stringstream temp;
            temp << "Stage " << stages.size() << ": " << Util::probeDef(stage, "info", "name");
            stageNames.push_back(temp.str());
            stages.push_back(stage);
        }
    } catch (const MugenException &ex){
	Global::debug(0) << "Problem adding stage. Reason: " << ex.getReason() << endl;
    }
}

//! Add stage to list
void StageHandler::addStage(const std::string & stage){
    try {
	/* FIXME not a good solution to get file.
         * 8/7/2011: why not? seems ok to me..
         */
        addStage(Util::findFile(Filesystem::RelativePath(stage)));
    } catch (const Filesystem::NotFound & ex){
	Global::debug(0) << "Problem adding stage. Reason: " << ex.getTrace() << endl;
    }
}

// Cell
Cell::Cell(int x, int y):
location(x,y),
background(NULL),
character(NULL),
randomSprite(NULL),
random(false),
blank(false),
empty(true),
characterScaleX(1),
characterScaleY(1),
flash(0),
cursors(None){
}

Cell::~Cell(){
}

void Cell::act(){
    if (flash){
	flash--;
    }
}

void Cell::randomize(std::vector<CharacterInfo *> &characters){
    if (random){
	unsigned int num = PaintownUtil::rnd(0,characters.size());
        character = characters[num];
    }
}

void Cell::render(const Graphics::Bitmap & bmp){
    if (!blank){
        if (background != 0){
            background->render(position.x,position.y,bmp);
        }
        if (!empty){
            Mugen::Effects effects;
            effects.scalex = characterScaleX;
            effects.scaley = characterScaleY;
            if (random){
                randomSprite->render(position.x + characterOffset.x, position.y + characterOffset.y, bmp,effects);
            } else {
                character->getIcon()->render(position.x + characterOffset.x, position.y + characterOffset.y, bmp,effects);
            }
        }
        if (flash){
            // Bitmap::drawingMode(Bitmap::MODE_TRANS);
            Graphics::Bitmap::transBlender( 0, 0, 0, int(25.5 * flash) );
            bmp.translucent().rectangleFill( position.x -1, position.y -1, (position.x -1) + dimensions.x, (position.y - 1) + dimensions.y,Graphics::makeColor(255,255,255));
            // Bitmap::drawingMode(Bitmap::MODE_SOLID);
        }
    }
}

Grid::Grid():
rows(0),
columns(0),
wrapping(false),
showEmptyBoxes(false),
moveOverEmptyBoxes(false),
cellSpacing(0),
cellBackgroundSprite(0),
cellRandomSprite(0),
cellRandomSwitchTime(0),
randomSwitchTimeTicker(0),
portraitScaleX(1),
portraitScaleY(1),
type(Mugen::Arcade){
}

Grid::~Grid(){
    // Destroy cell map
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
        std::vector< Cell *> &row = (*i);
        for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
            Cell *cell = (*column);
            if (cell){
                delete cell;
            }
        }
    }

}

void Grid::lock(){
    gridLock.acquire();
}

void Grid::unlock(){
    gridLock.release();
}
        
std::vector<CharacterInfo *> Grid::getCharacters() const {
    PaintownUtil::Thread::ScopedLock scoped(gridLock);
    /* a copy of characters will be returned */
    return characters;
}

void Grid::initialize(){
    Mugen::Point currentPosition;
    currentPosition.y = position.y;
    for (int row = 0; row < rows; ++row){
	currentPosition.x = position.x;
	std::vector< Cell *> cellRow;
	for (int column = 0; column < columns; ++column){
	    Cell *cell = new Cell(row,column);
	    cell->setBackground(cellBackgroundSprite);
	    cell->setRandomSprite(cellRandomSprite);
	    cell->setPosition(currentPosition.x,currentPosition.y);
	    cell->setDimensions(cellSize.x,cellSize.y);
	    cell->setCharacterOffset(portraitOffset.x, portraitOffset.y);
	    cell->setCharacterScale(portraitScaleX, portraitScaleY);
            // Set random cell with a default character
            if (cell->isRandom()){
                cell->setCharacter(characters.front());
            }
	    cellRow.push_back(cell);
	    currentPosition.x += cellSize.x + cellSpacing;
	}
	cells.push_back(cellRow);
	currentPosition.y += cellSize.y + cellSpacing;
    }
}

void Grid::act(Cursor & player1Cursor, Cursor & player2Cursor){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &row = (*i);
	for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
	    cell->act();
            if (randomSwitchTimeTicker == cellRandomSwitchTime){
                if (player1Cursor.getState() == Cursor::CharacterSelect){
                    if (player1Cursor.getCurrentCell() == cell && cell->isRandom()){
                        cell->randomize(characters);
                        player1Cursor.playRandomSound();
                    }
                }
                if (player2Cursor.getState() == Cursor::CharacterSelect){
                    if (player2Cursor.getCurrentCell() == cell && cell->isRandom()){
                        cell->randomize(characters);
                        player2Cursor.playRandomSound();
                    }
                }
            }
	}
    }

    if (randomSwitchTimeTicker == cellRandomSwitchTime){
        randomSwitchTimeTicker = 0;
    }
    randomSwitchTimeTicker++;

    stages.getFontHandler().act();
}

void Grid::render(const Graphics::Bitmap & bmp){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &row = (*i);
	for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
	    if (cell->isEmpty()){
		if (showEmptyBoxes && !cell->isBlank()){
		    cell->render(bmp);
		}
	    } else {
		cell->render(bmp);
	    }
	}
    }
}

void Grid::addBlank(){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &row = (*i);
	for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
	    if (cell->isEmpty()){
                cell->setBlank(true);
                return;
            }
        }
    }
}

bool Grid::isUniqueCharacter(CharacterInfo * character){
    lock();

    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	vector<Cell *> &row = (*i);
	for (vector<Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
            if (!cell->isEmpty() && cell->getCharacter() != NULL){
                CharacterInfo * his = cell->getCharacter();
                if (*his == *character){
                    unlock();
                    return false;
                }
            }
        }
    }

    unlock();

    return true;
}

bool Grid::addInfo(CharacterInfo * character){
    lock();
    vector<Cell*> candidates;

    /* first prefer empty cells */
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	vector<Cell *> &row = (*i);
	for (vector<Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
            if (cell->isEmpty()){
                candidates.push_back(cell);
            }
        }
    }

    /* then prefer blank cells */
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	vector<Cell *> &row = (*i);
	for (vector<Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
            if (cell->isBlank()){
                candidates.push_back(cell);
            }
        }
    }

    /* finally use random cells */
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	vector<Cell *> &row = (*i);
	for (vector<Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
            if (cell->isRandom()){
                candidates.push_back(cell);
            }
        }
    }

    bool success = false;

    if (candidates.size() > 0){
        Cell * cell = candidates[0];
        cell->setRandom(false);
        cell->setBlank(false);
        cell->setCharacter(character);
        character->setRandomStage(true);
        character->setReferenceCell(cell);
        characters.push_back(character);
        success = true;
    } else {
        /* failed to find an empty cell */
        delete character;
    }

    unlock();

    return success;
}

void Grid::addCharacter(CharacterInfo *character, bool isRandom){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &row = (*i);
	for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
	    if (cell->isEmpty()){
		if (isRandom){
		    cell->setRandom(true);
                    cell->setCharacter(character);
		    return;
		}
		character->setReferenceCell(cell);
		cell->setCharacter(character);
		characters.push_back(character);
		return;
	    } 
	}
    }
}

void Grid::setCursorPlayer1Start(Cursor &cursor){
    Cell *cell = getCell(player1Start.x,player1Start.y);
    cell->setCursorState(Cell::One);
    cursor.setCurrentCell(cell);
}

void Grid::setCursorPlayer2Start(Cursor &cursor){
    Cell *cell = getCell(player2Start.x,player2Start.y);
    cell->setCursorState(Cell::One);
    cursor.setCurrentCell(cell);
}

void Grid::setCursorStageSelect(Cursor & cursor){
    stages.setDisplay(true);
    cursor.setState(Cursor::StageSelect);
}

void Grid::moveCursor(Cursor & cursor, int Point::* field, int wrap, int direction){
    Mugen::Point current = cursor.getCurrentCell()->getLocation();

    Cell *cell = NULL;
    do{
        current.*field += direction;
        if (current.*field < 0){
            if (wrapping){
                current.*field = wrap - 1;
            } else {
                return;
            }
        } else if (current.*field >= wrap){
            if (wrapping){
                current.*field = 0;
            } else {
                return;
            }
        }

        try {
            cell = getCell(current.x, current.y);
        } catch (const MugenException &me){
            Global::debug(0) << "Could not get a cell at " << current.x << ", " << current.y << endl;
            // Shouldn't happen but you never know lets not continue
            return;
        }
        /* skip through blanks */
    } while (cell && cell->isBlank());
    
    if (cell->isEmpty()){
	if (!moveOverEmptyBoxes){
	    return;
	}
    }
    cursor.getCurrentCell()->popCursor();
    cell->pushCursor();
    cursor.setCurrentCell(cell);
    cursor.playMoveSound();
}

void Grid::moveCursorLeft(Cursor &cursor){
    moveCursor(cursor, &Point::y, columns, -1);
}

void Grid::moveCursorRight(Cursor &cursor){
    moveCursor(cursor, &Point::y, columns, 1);
}

void Grid::moveCursorUp(Cursor &cursor){
    moveCursor(cursor, &Point::x, rows, -1);
}

void Grid::moveCursorDown(Cursor &cursor){
    moveCursor(cursor, &Point::x, rows, 1);
}

void Grid::selectCell(Cursor &cursor, const Mugen::Keys & key, bool modifier){
    // *TODO use the key to determine which map(act) is used
    // Get the appropriate cell for flashing in case of random
    cursor.setActSelection(key, modifier);
    cursor.getCurrentCell()->getCharacter()->getReferenceCell()->startFlash();
    // set cursor state depending on state
    switch (type){
	case Arcade:
	    cursor.setState(Cursor::Done);
            cursor.playSelectSound();
	    break;
	case Versus:
	    setCursorStageSelect(cursor);
            cursor.playSelectSound();
	    break;
	case TeamArcade:
	    break;
	case TeamVersus:
	    break;
	case TeamCoop:
	    break;
	case Survival:
	    break;
	case SurvivalCoop:
	    break;
	case Training:
	    cursor.setState(Cursor::Done);
            cursor.playSelectSound();
	    break;
	case Watch:
	    cursor.setState(Cursor::Done);
            cursor.playSelectSound();
	    break;
	default:
	    break;
    }
}

void Grid::selectStage(){
    // Set stage so that doesn't infinitely toggle
    if (stages.isSelecting()){
	stages.toggleSelecting();
    }
}

Cell * Grid::getCell(int row, int column){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &rowIterator = (*i);
	for (std::vector< Cell *>::iterator columnIterator = rowIterator.begin(); columnIterator != rowIterator.end(); ++columnIterator){
	    Cell *cell = (*columnIterator);
	    if (cell->getLocation() == Mugen::Point(row,column)){
		return cell;
	    }
	}
    }
    
    std::ostringstream out;
    out << "Could not find cell for row " << row << " and column " << column << std::endl;
    throw MugenException(out.str());
}

Cursor::Cursor():
activeSprite(0),
doneSprite(0),
blink(false),
blinkRate(10),
blinkCounter(0),
hideForBlink(false),
faceScaleX(0),
faceScaleY(0),
facing(0),
state(NotActive),
moveSound(0),
selectSound(0),
randomSound(0),
cancelRandom(false),
actSelection(A),
actModifier(false){
}

Cursor::~Cursor(){
}

void Cursor::act(Grid &grid){
    vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(input, InputSource());
    for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
        const InputMap<Mugen::Keys>::InputEvent & event = *it;
        if (!event.enabled){
            continue;
        }

        switch (state){
            case NotActive:
                return;
                break;
            case TeamSelect:
                /* whats supposed to be here? */
                if (event[Up]){
                }
                if (event[Down]){
                }
                if (event[Left]){
                }
                if (event[Right]){
                }
                if (event[A]){
                }
                if (event[B]){
                }
                if (event[C]){
                }
                if (event[X]){
                }
                if (event[Y]){
                }
                if (event[Z]){
                }
                if (event[Start]){
                }
                break;
            case CharacterSelect:
                if (event[Up]){
                    grid.moveCursorUp(*this);
                }
                if (event[Down]){
                    grid.moveCursorDown(*this);
                }
                if (event[Left]){
                    grid.moveCursorLeft(*this);
                }
                if (event[Right]){
                    grid.moveCursorRight(*this);
                }

                if (!getCurrentCell()->isEmpty()){
                    Mugen::Keys selectable[] = {A, B, C, X, Y, Z};
                    for (unsigned int key = 0; key < sizeof(selectable) / sizeof(Mugen::Keys); key++){
                        if (event[selectable[key]]){
                            grid.selectCell(*this, selectable[key], event[Start]);
                        }
                    }
                }

                if (blink && (currentCell->getCursorState() == Cell::Two)){
                    blinkCounter++;
                    if (blinkCounter > blinkRate){
                        blinkCounter = 0;
                        hideForBlink = !hideForBlink;
                    }
                } else {
                    hideForBlink = false;
                }
                break;
            case StageSelect:
                if (event[Left]){
                    grid.getStageHandler().prev();
                }
                if (event[Right]){
                    grid.getStageHandler().next();
                }
                /* code re-use *cough* */
                if (event[A]){
                    grid.selectStage();
                }
                if (event[B]){
                    grid.selectStage();
                }
                if (event[C]){
                    grid.selectStage();
                }
                if (event[X]){
                    grid.selectStage();
                }
                if (event[Y]){
                    grid.selectStage();
                }
                if (event[Z]){
                    grid.selectStage();
                }
                if (event[Start]){
                }

                /* Check if selecting is still possible else set done state */
                if (!grid.getStageHandler().isSelecting()){
                    state = Done;
                }
                break;
            case Done:
            default:
                break;
        }
    }
}

void Cursor::render(Grid &grid, const Graphics::Bitmap & bmp){
    switch (state){
	case NotActive:
	    return;
	    break;
	case TeamSelect:
	    break;
	case CharacterSelect:
	    if (!hideForBlink){
                if (activeSprite){
                    activeSprite->render(currentCell->getPosition().x,currentCell->getPosition().y,bmp);
                }
	    }
	    renderPortrait(bmp);
	    break;
	case Done:
	default:
	    if (!hideForBlink){
                if (doneSprite){
                    doneSprite->render(currentCell->getPosition().x,currentCell->getPosition().y,bmp);
                }
	    }
	    renderPortrait(bmp);
    }
    
    /* Have to make sure stage select is prominent kinda stupid */
    grid.getStageHandler().render(bmp);
}

void Cursor::playMoveSound(){
    if (moveSound){
        moveSound->play();
    }
}

void Cursor::playSelectSound(){
    if (selectSound){
        selectSound->play();
    }
}

void Cursor::playRandomSound(){
    if (randomSound){
        if (cancelRandom){
            randomSound->stop();
        }
        randomSound->play();
    } 
}

void Cursor::renderPortrait(const Graphics::Bitmap &bmp){
    // Lets do the portrait and name
    if (!currentCell->isEmpty() && !currentCell->isBlank()){
	const CharacterInfo *character = currentCell->getCharacter();
	Mugen::Effects effects;
	effects.facing = facing;
	effects.scalex = faceScaleX;
	effects.scaley = faceScaleY;
	character->getPortrait()->render(faceOffset.x, faceOffset.y, bmp, effects);
	font.render(character->getDisplayName(),bmp);
    }
}

int Cursor::getActSelection(){
    switch (actSelection){
        case A:
            if (!actModifier){
                return 1;
            } else {
                return 7;
            }
        case B:
            if (!actModifier){
                return 2;
            } else {
                return 8;
            }
        case C:
            if (!actModifier){
                return 3;
            } else {
                return 9;
            }
        case X:
            if (!actModifier){
                return 4;
            } else {
                return 10;
            }
        case Y:
            if (!actModifier){
                return 5;
            } else {
                return 11;
            }
        case Z:
            if (!actModifier){
                return 6;
            } else {
                return 12;
            }
        default:
            break;
    }
    return PaintownUtil::rnd(1,12);
}

VersusScreen::VersusScreen():
background(0),
time(0){
}

VersusScreen::~VersusScreen(){
    delete background;
}

static const Graphics::Bitmap & getScreen(){
    return *Graphics::screenParameter.current();
}

void VersusScreen::render(CharacterInfo & player1, CharacterInfo & player2, Mugen::Stage * stage){
    // bool done = false;
    bool escaped = false;
    
    // int ticker = 0;
    // ParseCache cache;
    
    // Set the fade state
    fader.setState(Gui::FadeTool::FadeIn);
  
    /*
    double runCounter = 0;
    Global::speed_counter = 0;
    Global::second_counter = 0;
    */
    
    // Set game keys temporary
    InputMap<Mugen::Keys> gameInput;
    gameInput.set(Keyboard::Key_ESC, 10, true, Mugen::Esc);
    gameInput.set(Configuration::config(0).getJoystickQuit(), 10, true, Mugen::Esc);

    class PlayerLoader: public PaintownUtil::Future<int> {
    public:
        PlayerLoader(CharacterInfo & player1, CharacterInfo & player2):
            player1(player1),
            player2(player2){
                /* compute is a virtual function, is the virtual table set up
                 * by the time start() tries to call it? or is that a race condition?
                 */
                // start();
        }

        CharacterInfo & player1;
        CharacterInfo & player2;

        virtual void compute(){
            // Check acts lets make them use seperate ones
            if (player1.getDefinitionFile() == player2.getDefinitionFile()){
                if (player1.getPlayer1Act() == player2.getPlayer2Act()){
                    int act = player1.getPlayer1Act()-1;
                    if (act <= 0){
                        act += 2;
                    }
                    player2.setPlayer2Act(act);
                }
            }
            // Load player 1
            player1.loadPlayer1();
            // Load player 2
            player2.loadPlayer2();
#ifdef WII
            /* FIXME: this is a hack, im not sure why its even required but fopen() will hang on sfp_lock_acquire
             * in another thread without locking and unlocking the sfp lock.
             * related things
             *  newlib/libc/stdio/fopen.c -- calls sfp_lock_acquire
             *  newlib/libc/sys/linux/sys/libc-lock.h - uses pthread's as the lock implementation
             */
            /*
            __sfp_lock_acquire();
            __sfp_lock_release();
            */
#endif
            set(0);
        }
    };

    PlayerLoader playerLoader(player1, player2);
    playerLoader.start();

    /* loading has three phases.
     * 1. show the animations (portraits and backgrounds) for a second or two. if the
     * user presses a key then continue to the next phase immediately instead of waiting.
     * 2. if the players haven't been fully loaded yet then show a "Loading" screen and
     * wait until the players fully load.
     * 3. fade out.
     *
     * we can use 3 different logic classes to implement each phase
     */

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(CharacterInfo & player1, CharacterInfo & player2,
             Background * background, FontHandler & player1Font,
             FontHandler & player2Font,
             const Mugen::Point & player1Position,
             const Mugen::Point & player2Position,
             const Mugen::Effects & player1Effects,
             const Mugen::Effects & player2Effects,
             Gui::FadeTool & fader):
            player1(player1),
            player2(player2),
            background(background),
            player1Font(player1Font),
            player2Font(player2Font),
            player1Position(player1Position),
            player2Position(player2Position),
            player1Effects(player1Effects),
            player2Effects(player2Effects),
            fader(fader){
            }

        CharacterInfo & player1;
        CharacterInfo & player2;
        Background * background;
        FontHandler & player1Font;
        FontHandler & player2Font;
        const Mugen::Point & player1Position;
        const Mugen::Point & player2Position;
	const Mugen::Effects & player1Effects;
	const Mugen::Effects & player2Effects;
        Gui::FadeTool & fader;

        void draw(const Graphics::Bitmap & screen){
            Graphics::StretchedBitmap workArea(DEFAULT_WIDTH, DEFAULT_HEIGHT, screen);
            workArea.start();
            // render backgrounds
            background->renderBackground(0, 0, workArea);

            // render portraits
            player1.getPortrait()->render(player1Position.x, player1Position.y, workArea, player1Effects);
            player2.getPortrait()->render(player2Position.x, player2Position.y, workArea, player2Effects);

            // render fonts
            player1Font.render(player1.getDisplayName(), workArea);
            player2Font.render(player2.getDisplayName(), workArea);

            // render Foregrounds
            background->renderForeground(0,0,workArea);

            // render fades
            fader.draw(workArea);

            // Finally render to screen
            // workArea.Stretch(screen);
            workArea.finish();
            screen.BlitToScreen();
        }
    };

    /* FIXME: handle ESC like the original code did */
    class Logic1: public PaintownUtil::Logic {
    public:
        Logic1(Gui::FadeTool & fader,
               Background * background,
               FontHandler & player1Font,
               FontHandler & player2Font, int time):
            fader(fader),
            background(background),
            player1Font(player1Font),
            player2Font(player2Font),
            time(time),
            ticker(0){
        }
	
        Gui::FadeTool & fader;
        Background * background;
        FontHandler & player1Font;
        FontHandler & player2Font;
        int time;
        int ticker;

        double ticks(double system){
            return Util::gameTicks(system);
        }

        bool done(){
            return ticker >= time;
        }

        void run(){
            ticker += 1;

            // Fader
            fader.act();

            // Backgrounds
            background->act();

            // Player fonts
            player1Font.act();
            player2Font.act();
        }
    };

    /* phase 2 consists of running the loading screen, we don't need a logic class 
     * for that
     */
    /*
    class Logic2: public PaintownUtil::Logic {
    };
    */

    class Logic3: public PaintownUtil::Logic {
    public:
        Logic3(Gui::FadeTool & fader,
               Background * background,
               FontHandler & player1Font,
               FontHandler & player2Font):
            fader(fader),
            background(background),
            player1Font(player1Font),
            player2Font(player2Font){
        }
	
        Gui::FadeTool & fader;
        Background * background;
        FontHandler & player1Font;
        FontHandler & player2Font;

        bool done(){
            return fader.getState() == Gui::FadeTool::EndFade;
        }

        double ticks(double system){
            return Util::gameTicks(system);
        }

        void run(){
            fader.act();
            background->act();
            player1Font.act();
            player2Font.act();
        }
    };

    Draw drawer(player1, player2, background,
                player1Font, player2Font,
                player1Position, player2Position,
                player1Effects, player2Effects, fader);

    Logic1 logic1(fader, background, player1Font, player2Font, time);

    PaintownUtil::standardLoop(logic1, drawer);

    try{
        Loader::Info info;
        Graphics::Bitmap background(getScreen(), true);
        info.setBackground(&background);
        info.setLoadingMessage("Loading...");
        info.setPosition(10, 200);

        class Context: public Loader::LoadingContext {
        public:
            Context(PlayerLoader & playerLoader, Mugen::Stage *& stage):
                playerLoader(playerLoader),
                stage(stage),
                fail(NULL){
                }

            virtual void maybeFail(){
                if (fail != NULL){
                    fail->throwSelf();
                }
            }

            virtual ~Context(){
                delete fail;
            }

            virtual void load(){
                try{
                    /* future */
                    int ok = playerLoader.get();

                    // Load stage
                    stage->addPlayer1(playerLoader.player1.getPlayer1());
                    stage->addPlayer2(playerLoader.player2.getPlayer2());
                    stage->load();
                } catch (const MugenException & fail){
                    this->fail = new MugenException(fail);
                } catch (const LoadException & fail){
                    this->fail = new LoadException(fail);
                }
            }

            PlayerLoader & playerLoader;
            Mugen::Stage *& stage;
            Exception::Base * fail;
        };

        Context context(playerLoader, stage);
        /* FIXME: the wii has problems loading stuff in a background thread
         * while the load screen is going on.
         */
#ifdef WII
        context.load();
#else
        Loader::loadScreen(context, info);
#endif
        context.maybeFail();

        fader.setState(Gui::FadeTool::FadeOut);
    } catch (const MugenException & e){
        throw e;
    }

    fader.setState(Gui::FadeTool::FadeOut);
    Logic3 logic3(fader, background, player1Font, player2Font);
    PaintownUtil::standardLoop(logic3, drawer);
    
#if 0
    while (!done || fader.getState() != Gui::FadeTool::EndFade){

        bool draw = false;

        if ( Global::speed_counter > 0 ){
            draw = true;
            runCounter += Util::gameTicks(Global::speed_counter);
            Global::speed_counter = 0;
            while ( runCounter >= 1.0 ){
                // tick tock
                ticker++;

                runCounter -= 1;
                // Key handler
                InputManager::poll();

                if (Global::shutdown()){
                    throw ShutdownException();
                }

                vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(gameInput);
                for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                    const InputMap<Mugen::Keys>::InputEvent & event = *it;
                    if (!event.enabled){
                        continue;
                    }

                    if (event[Mugen::Esc]){
                        done = escaped = true;
                        fader.setState(Gui::FadeTool::FadeOut);
                        InputManager::waitForRelease(gameInput, Mugen::Esc);
                    }
                }

                // Logic
                /* if done, dont run the loader again, just wait for the fadeout */
                if (ticker >= time && !done){
                    // PaintownUtil::Thread::Id loader;
                    try{
                        Level::LevelInfo info;
                        info.setBackground(&bmp);
                        info.setLoadingMessage("Loading...");
                        info.setPosition(-1, 400);

                        class Context: public Loader::LoadingContext {
                            public:
                                Context(PlayerLoader & playerLoader, Mugen::Stage *& stage):
                                    playerLoader(playerLoader),
                                    stage(stage),
                                    fail(NULL){
                                    }

                                virtual void maybeFail(){
                                    if (fail != NULL){
                                        fail->throwSelf();
                                    }
                                }

                                virtual ~Context(){
                                    delete fail;
                                }

                                virtual void load(){
                                    try{
                                        /* future */
                                        int ok = playerLoader.get();

                                        // Load stage
                                        stage->addPlayer1(playerLoader.player1.getPlayer1());
                                        stage->addPlayer2(playerLoader.player2.getPlayer2());
                                        stage->load();
                                    } catch (const MugenException & fail){
                                        this->fail = new MugenException(fail);
                                    } catch (const LoadException & fail){
                                        this->fail = new LoadException(fail);
                                    }
                                }

                                PlayerLoader & playerLoader;
                                Mugen::Stage *& stage;
                                Exception::Base * fail;
                        };

                        Context context(playerLoader, stage);
                        // Loader::startLoading(&loader, (void*) &info);
                        Loader::loadScreen(context, info);
                        context.maybeFail();

                        // Loader::stopLoading(loader);
                        done = true;
                        fader.setState(Gui::FadeTool::FadeOut);
                    } catch (const MugenException & e){
                        // Loader::stopLoading(loader);
                        throw e;
                    }
                }

                // Fader
                fader.act();

                // Backgrounds
                background->act();

                // Player fonts
                player1Font.act();
                player2Font.act();
            }
        }

        if (draw){
            // render backgrounds
            background->renderBackground(0,0,workArea);

            // render portraits
            player1.getPortrait()->render(player1Position.x,player1Position.y,workArea,player1Effects);
            player2.getPortrait()->render(player2Position.x,player2Position.y,workArea,player2Effects);

            // render fonts
            player1Font.render(player1.getDisplayName(), workArea);
            player2Font.render(player2.getDisplayName(), workArea);

            // render Foregrounds
            background->renderForeground(0,0,workArea);

            // render fades
            fader.draw(workArea);

            // Finally render to screen
            workArea.Stretch(bmp);
            bmp.BlitToScreen();
        }

        while (Global::speed_counter < 1){
            PaintownUtil::rest(1);
        }
    }
#endif
    
    // **FIXME Hack figure something out
    if (escaped){
	throw Exception::Return(__FILE__, __LINE__);
    }
}

static std::vector<Ast::Section*> collectSelectStuff(Ast::AstParse::section_iterator & iterator, Ast::AstParse::section_iterator end){
    Ast::AstParse::section_iterator last = iterator;
    vector<Ast::Section*> stuff;

    Ast::Section * section = *iterator;
    std::string head = section->getName();
    /* better to do case insensitive regex matching rather than
     * screw up the original string
     */
    stuff.push_back(section);
    iterator++;

    while (true){
        if (iterator == end){
            break;
        }

        section = *iterator;
        string sectionName = section->getName();
        sectionName = Mugen::Util::fixCase(sectionName);
        // Global::debug(2, __FILE__) << "Match '" << (prefix + name + ".*") << "' against '" << sectionName << "'" << endl;
        if (PaintownUtil::matchRegex(sectionName, "select")){
            stuff.push_back(section);
        } else {
            break;
        }

        last = iterator;
        iterator++;
    }
    iterator = last;
    return stuff;
}

CharacterSelect::CharacterSelect(const Filesystem::AbsolutePath & file, const PlayerType & playerType, const GameType & gameType):
systemFile(file),
sffFile(""),
sndFile(""),
selectFile(""),
gameType(gameType),
cancelSound(0),
background(NULL),
currentPlayer1(0),
currentPlayer2(0),
currentStage(0),
playerType(playerType),
quitSearching(false),
searchingCheck(quitSearching, searchingLock.getLock()),
characterAddThread(PaintownUtil::Thread::uninitializedValue),
subscription(*this){
    grid.setGameType(gameType);

    // Set defaults
    reset();
}

CharacterSelect::~CharacterSelect(){
    searchingCheck.set(true);

    /* signal the add thread in case its waiting */
    addCharacterLock.acquire();
    addCharacterLock.signal();
    addCharacterLock.release();

    PaintownUtil::Thread::joinThread(characterAddThread);

    // Get rid of sprites
    for( Mugen::SpriteMap::iterator i = sprites.begin(); i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    // Get rid of sounds
    for( std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
      for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    // Cleanup fonts
    for (std::vector< MugenFont *>::iterator f = fonts.begin(); f != fonts.end(); ++f){
        if (*f) delete (*f);
    }

    for (vector<CharacterInfo*>::iterator it = characters.begin(); it != characters.end(); it++){
        delete *it;
    }

    /* background */
    if (background){
	delete background;
    }
    // Kill stage
    if (currentStage){
	delete currentStage;
    }
}

void CharacterSelect::load(){
    try{
        Filesystem::AbsolutePath baseDir = systemFile.getDirectory();

        Global::debug(1) << baseDir.path() << endl;

        if (systemFile.isEmpty()){
            throw MugenException( "Cannot locate character select definition file for: " + systemFile.path());
        }

        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Util::parseDef(systemFile.path()));
        diff.endTime();
        Global::debug(1) << "Parsed mugen file " + systemFile.path() + " in" + diff.printTime("") << endl;

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
                            simple.view() >> select.sffFile;
                            Global::debug(1) << "Got Sprite File: '" << select.sffFile << "'" << endl;
                            Mugen::Util::readSprites(Mugen::Util::findFile(Filesystem::RelativePath(select.sffFile)), Filesystem::AbsolutePath(), select.sprites, true);
                            for( Mugen::SpriteMap::iterator i = select.sprites.begin() ; i != select.sprites.end() ; ++i ){
                                // Load these sprites so they are ready to use
                                for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
                                    if( j->second )j->second->load();
                                }
                            }
                        } else if (simple == "snd"){
                            simple.view() >> select.sndFile;
                            Mugen::Util::readSounds(Util::findFile(Filesystem::RelativePath(select.sndFile)), select.sounds);
                            Global::debug(1) << "Got Sound File: '" << select.sndFile << "'" << endl;
                        } else if (simple == "logo.storyboard"){
                            // Ignore
                        } else if (simple == "intro.storyboard"){
                            // Ignore
                        } else if (simple == "select"){
                            simple.view() >> select.selectFile;
                            Global::debug(1) << "Got Select File: '" << select.selectFile << "'" << endl;
                        } else if (simple == "fight"){
                            // Ignore
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            string path;
                            simple.view() >> path;
                            select.fonts.push_back(new MugenFont(Util::findFile(Filesystem::RelativePath(path))));
                            Global::debug(1) << "Got Font File: '" << path << "'" << endl;

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
                                int rows;
                                simple.view() >> rows;
                                self.grid.setRows(rows);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "columns"){
                            try{
                                int columns;
                                simple.view() >> columns;
                                self.grid.setColumns(columns);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "wrapping"){
                            try{
                                bool wrap;
                                simple.view() >> wrap;
                                self.grid.setWrapping(wrap);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "pos"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                self.grid.setPosition(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "showemptyboxes"){
                            try{
                                bool boxes;
                                simple.view() >> boxes;
                                self.grid.setShowEmptyBoxes(boxes);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "moveoveremptyboxes"){
                            try{
                                bool boxes;
                                simple.view() >> boxes;
                                self.grid.setMoveOverEmptyBoxes(boxes);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.size"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.grid.setCellSize(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.spacing"){
                            try{
                                int spacing;
                                simple.view() >> spacing;
                                self.grid.setCellSpacing(spacing);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.bg.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.grid.setCellBackgroundSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.grid.setCellRandomSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cell.random.switchtime"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.grid.setCellRandomSwitchTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.startcell"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                self.grid.setPlayer1Start(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1Cursor.setActiveSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player1Cursor.setDoneSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.player1Cursor.setMoveSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.player1Cursor.setSelectSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.player1Cursor.setRandomSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.startcell"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                self.grid.setPlayer2Start(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.active.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2Cursor.setActiveSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.done.spr"){
                            try{
                                int group, sprite;
                                simple.view() >> group >> sprite;
                                self.player2Cursor.setDoneSprite(sprites[group][sprite]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.cursor.blink"){
                            try{
                                bool blink;
                                simple.view() >> blink;
                                self.player2Cursor.setBlink(blink);
                            } catch (const Ast::Exception & e){
                            }
                        } 
                        else if ( simple == "p2.cursor.move.snd"){ 
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.player2Cursor.setMoveSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.cursor.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.player2Cursor.setSelectSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.random.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.player2Cursor.setRandomSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "random.move.snd.cancel"){
                            try{
                                bool cancel;
                                simple.view() >> cancel;
                                self.player1Cursor.setRandomCancel(cancel);
                                self.player2Cursor.setRandomCancel(cancel);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.move.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.grid.getStageHandler().setMoveSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.done.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.grid.getStageHandler().setSelectSound(self.sounds[group][sound]);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "cancel.snd"){
                            try{
                                int group, sound;
                                simple.view() >> group >> sound;
                                self.cancelSound = self.sounds[group][sound];
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "portrait.offset"){
                            try{
                                int x,y;
                                simple.view() >> x >> y;
                                self.grid.setPortraitOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "portrait.scale"){
                            try{
                                double x,y;
                                simple.view() >> x >> y;
                                self.grid.setPortraitScale(x,y);
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
                            /* -1 indicates no font */
                            if (index != -1){
                                /* should this be set even if the parse fails? */
                                self.titleFont.setPrimary(self.getFont(index), bank, position);
                            }
                        } else if ( simple == "p1.face.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1Cursor.setFaceOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.scale"){
                            try{
                                double x, y;
                                simple.view() >> x >> y;
                                self.player1Cursor.setFaceScale(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.face.facing"){
                            try{
                                int f;
                                simple.view() >> f;
                                self.player1Cursor.setFacing(f);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2Cursor.setFaceOffset(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.scale"){
                            try{
                                double x, y;
                                simple.view() >> x >> y;
                                self.player2Cursor.setFaceScale(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.face.facing"){
                            try{
                                int f;
                                simple.view() >> f;
                                self.player2Cursor.setFacing(f);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p1.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player1Cursor.getFontHandler().setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        }  else if ( simple == "p1.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }

                            if (index > 0){
                                self.player1Cursor.getFontHandler().setPrimary(self.getFont(index),bank,position);
                            }
                        } else if ( simple == "p2.name.offset"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.player2Cursor.getFontHandler().setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "p2.name.font"){
                            try{
                                int index, bank, position;
                                simple.view() >> index >> bank >> position;
                                if (index > 0){
                                    self.player2Cursor.getFontHandler().setPrimary(self.getFont(index),bank,position);
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.pos"){
                            try{
                                int x, y;
                                simple.view() >> x >> y;
                                self.grid.getStageHandler().getFontHandler().setLocation(x,y);
                            } catch (const Ast::Exception & e){
                            }
                        } else if ( simple == "stage.active.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }

                            if (index > 0){
                                self.grid.getStageHandler().getFontHandler().setPrimary(self.getFont(index),bank,position);
                            }
                        } else if ( simple == "stage.active2.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }

                            if (index > 0){
                                self.grid.getStageHandler().getFontHandler().setBlink(self.getFont(index),bank,position);
                            }
                        } else if ( simple == "stage.done.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            if (index > 0){
                                self.grid.getStageHandler().getFontHandler().setDone(self.getFont(index),bank,position);
                            }
                        }
#if 0
                        else if ( simple.find("teammenu")!=std::string::npos ){
                            /* Ignore for now */
                        }
#endif
                        //else throw MugenException( "Unhandled option in Select Info Section: " + itemhead );
                    }
                };

                SelectInfoWalker walker(*this, sprites);
                section->walk(walker);
            } else if (head == "selectbgdef"){ 
                /* Background management */
                Mugen::Background *manager = new Mugen::Background(systemFile, "selectbg");
                background = manager;
            } else if (head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "vs screen" ){
                class VersusWalker: public Ast::Walker{
                public:
                    VersusWalker(VersusScreen & self, const CharacterSelect & select):
                        self(self),
                        select(select){
                        }

                    VersusScreen & self;
                    const CharacterSelect & select;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "time" ){
                            try{
                                int time;
                                simple.view() >> time;
                                self.setTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadein.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.getFadeTool().setFadeInTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "fadeout.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                self.getFadeTool().setFadeOutTime(time);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.pos"){
                            int x=0,y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (Ast::Exception & e){
                            }
                            self.setPlayer1Position(Mugen::Point(x,y));
                        } else if (simple == "p1.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                self.setPlayer1Facing(face);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p1.scale"){
                            double x,y;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.setPlayer1Scale(x,y);
                        } else if (simple == "p2.pos"){
                            int x=0,y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.setPlayer2Position(Mugen::Point(x,y));
                        } else if (simple == "p2.facing"){
                            try{
                                int face;
                                simple.view() >> face;
                                self.setPlayer2Facing(face);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "p2.scale"){
                            double x,y;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.setPlayer2Scale(x,y);
                        } else if (simple == "p1.name.pos"){
                            int x, y;
                            try {
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.getPlayer1Font().setLocation(x,y);
                        } else if (simple == "p1.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            if (index > 0){
                                self.getPlayer1Font().setPrimary(select.getFont(index),bank,position);
                            }
                        } else if (simple == "p2.name.pos"){
                            int x, y;
                            try {
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            self.getPlayer2Font().setLocation(x,y);
                        } else if (simple == "p2.name.font"){
                            int index=0, bank=0, position=0;
                            try {
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                                //ignore for now
                            }
                            if (index > 0){
                                self.getPlayer2Font().setPrimary(select.getFont(index),bank,position);
                            }
                        }
                    }
                };

                VersusWalker walker(versus, *this);
                section->walk(walker);
            }
            else if (head == "versusbgdef" ){
                /* Background management */
                Mugen::Background *manager = new Mugen::Background(systemFile, "versusbg");
                versus.setBackground(manager);
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
                ostringstream context;
                context << __FILE__ << ":" << __LINE__;
                Global::debug(0, context.str()) << "Warning: Unhandled Section in '" + systemFile.path() + "': " + head << endl;
            }
        }

        // Set up Grid
        grid.initialize();

        // Setup cursors
        switch (gameType){
            case Versus:
                grid.setCursorPlayer1Start(player1Cursor);
                grid.setCursorPlayer2Start(player2Cursor);
                break;
            case Arcade:
            case Training:
                if (playerType == Player1){
                    grid.setCursorPlayer1Start(player1Cursor);
                } else if (playerType == Player2){
                    grid.setCursorPlayer2Start(player2Cursor);
                }
            case Watch:
            default:
                if (playerType == Player1){
                    grid.setCursorPlayer1Start(player1Cursor);
                } else if (playerType == Player2){
                    grid.setCursorPlayer2Start(player2Cursor);
                }
                break;
        }
        // Now load up our characters
        parseSelect(Mugen::Util::fixFileName(baseDir, Mugen::Util::stripDir(selectFile)));
    } catch (const Filesystem::NotFound & fail){
        ostringstream out;
        out << "Could not load select screen because " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

//! Get group of characters by order number
std::vector<CharacterInfo *> CharacterSelect::getCharacterGroup(int orderNumber){
    std::vector<CharacterInfo *> tempCharacters;
    for (std::vector<CharacterInfo *>::iterator i = characters.begin(); i != characters.end(); ++i){
	CharacterInfo *character = *i;
	if (character->getOrder() == orderNumber){
	    tempCharacters.push_back(character);
	}
    }
    return tempCharacters;
}

/* TODO: add a description of this structure */
class CharacterCollect{
public:
    CharacterCollect():
        random(false),
        randomStage(false),
        name(""),
        stage(""),
        includeStage(true),
        blank(false),
        order(1),
        song(""){
        }

    ~CharacterCollect(){
    }

    bool random;
    bool randomStage;
    std::string name;
    std::string stage;
    bool includeStage;
    bool blank;
    int order;
    std::string song;
};
        
bool CharacterSelect::addInfo(CharacterInfo * info){
    return grid.addInfo(info);
}
        
bool CharacterSelect::isUniqueCharacter(CharacterInfo * character){
    return grid.isUniqueCharacter(character);
}

static vector<Filesystem::AbsolutePath> findFiles(const Filesystem::AbsolutePath & path){
    try{
        return Storage::instance().getFilesRecursive(path, "*.def");
    } catch (const Filesystem::NotFound & fail){
        return vector<Filesystem::AbsolutePath>();
    }
}

static vector<Filesystem::AbsolutePath> findFiles(const Filesystem::RelativePath & path){
    try{
        return findFiles(Storage::instance().find(path));
    } catch (const Filesystem::NotFound & fail){
        return vector<Filesystem::AbsolutePath>();
    }
}

bool CharacterSelect::maybeAddCharacter(const Filesystem::AbsolutePath & path){
    Global::debug(1) << "Checking character " << path.path() << endl;
    try{
        CharacterInfo * info = new CharacterInfo(path);
        Global::debug(1) << path.path() << " is good" << endl;
        if (isUniqueCharacter(info)){
            return addInfo(info);
        } else {
            delete info;
        }
        return true;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(1) << "Failed to load " << path.path() << " because " << fail.getTrace() << endl;
    } catch (const Filesystem::Exception & fail){
        Global::debug(1) << "Failed to load " << path.path() << " because " << fail.getTrace() << endl;
    } catch (const Exception::Base & fail){
        Global::debug(1) << "Failed to load " << path.path() << " because " << fail.getTrace() << endl;
    } catch (...){
        Global::debug(1) << "Failed to load " << path.path() << " for an unknown reason" << endl;
    }

    return true;
}

void * CharacterSelect::doAddCharacters(void * arg){
    CharacterSelect * select = (CharacterSelect*) arg;
    while (!select->searchingCheck.get()){
        select->addCharacterLock.acquire();
        /* if we have no more characters to process then wait for a signal */
        if (select->addCharacters.size() == 0){
            select->addCharacterLock.wait();
        }
        Filesystem::AbsolutePath path;
        bool got = false;
        /* we might have been signaled because the character select screen is over
         * even though there are no more characters to process.
         */
        if (select->addCharacters.size() > 0){
            path = select->addCharacters[0];
            select->addCharacters.pop_front();
            got = true;
        }
        select->addCharacterLock.release();

        if (got){
            if (!select->maybeAddCharacter(path)){
                /* couldn't add any more characters, so just quit */
                break;
            }
            /* yield the thread */
            PaintownUtil::rest(0);
        }
    }

    return NULL;
}

/* add a single character */
void CharacterSelect::addFile(const Filesystem::AbsolutePath & file){
    vector<Filesystem::AbsolutePath> one;
    one.push_back(file);
    addFiles(one);
}

void CharacterSelect::addFiles(const vector<Filesystem::AbsolutePath> & files){
    /* Add all the found files in one swoop */
    addCharacterLock.acquire();
    addCharacterLock.signal();
    for (vector<Filesystem::AbsolutePath>::const_iterator it = files.begin(); it != files.end(); it++){
        const Filesystem::AbsolutePath & path = *it;
        addCharacters.push_back(path);
    }
    addCharacterLock.release();
}

void CharacterSelect::startAddThread(){
    characterAddThread = PaintownUtil::Thread::uninitializedValue;
    if (!PaintownUtil::Thread::createThread(&characterAddThread, NULL, (PaintownUtil::Thread::ThreadFunction) doAddCharacters, this)){
        Global::debug(0) << "Could not create character add thread" << endl;
    }
}

void CharacterSelect::parseSelect(const Filesystem::AbsolutePath &selectFile){
    startAddThread();

    const Filesystem::AbsolutePath file = Util::findFile(Filesystem::RelativePath(selectFile.getFilename().path()));
    
    TimeDifference diff;
    diff.startTime();
    AstRef parsed(Util::parseDef(file.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + file.path() + " in" + diff.printTime("") << endl;
    
    // Characters
    std::vector<CharacterCollect> characterCollection;
    // Stages
    std::vector<std::string> stageNames;
    
    // Arcade max matches
    std::vector<int> arcadeMaxMatches;
    // Team max matches
    std::vector<int> teamMaxMatches;
    
    for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
	Ast::Section * section = *section_it;
	std::string head = section->getName();
        
	head = Mugen::Util::fixCase(head);

        if (head == "characters"){
            class CharacterWalker: public Ast::Walker{
            public:
		CharacterWalker(std::vector< CharacterCollect > & characters):
		characters(characters){}
		virtual ~CharacterWalker(){}
		
		std::vector< CharacterCollect > &characters;
                virtual void onValueList(const Ast::ValueList & list){
                    CharacterCollect character;
		    // Grab Character
		    std::string temp;
                    Ast::View view = list.view();
		    view >> temp;

                    if (temp == "blank"){
                        character.blank = true;
                    } else if (temp == "randomselect"){
			character.random = true;
		    } else {
			character.name = temp;
		    }

                    try{
                        // Grab stage
                        view >> temp;
                        if (PaintownUtil::matchRegex(temp, "order = ")){
                            temp.replace(0,std::string("order = ").size(),"");
                            character.order = (bool)atoi(temp.c_str());
                        } else if (temp == "random"){
                            character.randomStage = true;
                        } else {
                            character.stage = temp;
                        }
                        // Grab options
                        /* TODO: make the parser turn these into better AST nodes.
                         * something like Assignment(Id(music), Filename(whatever))
                         */
                        while(true){
                            view >> temp;
                            if (PaintownUtil::matchRegex(temp,"includestage = ")){
                                temp.replace(0,std::string("includestage = ").size(),"");
                                character.includeStage = (bool)atoi(temp.c_str());
                            } else if (PaintownUtil::matchRegex(temp,"music = ")){
                                temp.replace(0,std::string("music = ").size(),"");
                                character.song = temp;
                            } else if (PaintownUtil::matchRegex(temp,"order = ")){
                                temp.replace(0,std::string("order = ").size(),"");
                                character.order = (bool)atoi(temp.c_str());
                            }
                        }
                    } catch (const Ast::Exception & e){
                    }

		    characters.push_back(character);
                }
            };

            CharacterWalker walk(characterCollection);
            section->walk(walk);
	} else if (head == "extrastages"){
	    class StageWalker: public Ast::Walker{
            public:
		StageWalker(std::vector< std::string > &names):
		names(names){
		}

		virtual ~StageWalker(){
                }

		std::vector< std::string > &names;

                virtual void onValueList(const Ast::ValueList & list){
		    // Get Stage info and save it
		    try {
			std::string temp;
			list.view() >> temp;
			Global::debug(1) << "stage: " << temp << endl;
			names.push_back(temp);
		    } catch (const Ast::Exception & e){
		    }
                }
            };
	    StageWalker walk(stageNames);
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
	    OptionWalker walk(arcadeMaxMatches,teamMaxMatches);
	    section->walk(walk);
	} else {
	    // throw MugenException("Unhandled Section in '" + file.path() + "': " + head, __FILE__, __LINE__); 
            ostringstream context;
            context << __FILE__ << ":" << __LINE__;
            Global::debug(0, context.str()) << "Warning: Unhandled Section in '" + file.path() + "': " + head << endl;
	}
    }
    
    for (std::vector<CharacterCollect>::iterator i = characterCollection.begin(); i != characterCollection.end();++i){
        try{
            CharacterCollect & character = *i;
            const Filesystem::AbsolutePath def = Util::findCharacterDef(character.name);
            addFile(def);
            stageNames.push_back(character.stage);
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Error loading mugen character: " << fail.getTrace() << std::endl;
        }
    }

    if (stageNames.size() == 0){
        throw MugenException("No stages listed");
    }

    // Prepare stages
    for (std::vector<std::string>::iterator i = stageNames.begin(); i != stageNames.end(); ++i){
	grid.getStageHandler().addStage((*i));
    }
    
    /* FIXME: implement this stuff again */
#if 0
    // Set up our characters along the grid (excluding random select)
    // Offset for stage placement
    int stageOffset = 0;
    for (std::vector<CharacterCollect>::iterator i = characterCollection.begin(); i != characterCollection.end();++i){
	CharacterCollect & character = *i;
        try{
            if (!character.random && !character.blank){
                // Get character
                // *FIXME Not an elegant solution for character location

                // look up the character's directory first in the <motif>/chars/...
                // and then in mugen/chars/...
                /* Mugen::Util::findDirectory(character.name) */
                /*
                const Filesystem::AbsolutePath baseDir = Filesystem::findInsensitive(Filesystem::RelativePath("mugen/chars/" + character.name));
                Filesystem::RelativePath str = Filesystem::RelativePath(character.name).getFilename();
                const Filesystem::AbsolutePath charDefFile = Util::fixFileName(baseDir, str.path() + ".def");
                */
                const Filesystem::AbsolutePath defFile = Util::findCharacterDef(character.name);

                // const std::string charDefFile = Filesystem::cleanse(Mugen::Util::fixFileName(baseDir, std::string(str + ".def")));
                Global::debug(1) << "Got character def: " << defFile.path() << endl;
                CharacterInfo *charInfo = new CharacterInfo(defFile);
                charInfo->setRandomStage(character.randomStage);
                // Set stage
                if (character.stage.empty()){
                    // lets assume random then
                    charInfo->setRandomStage(true);
                } else {
                    // Fix the stage name before handing it the character
                    charInfo->setStage(Filesystem::RelativePath(character.stage));
                    // also add the stage
                    if (character.includeStage){
                        // Pass base stage name, StageHandler will fix the stage name
                        stageNames.insert(stageNames.begin() + stageOffset, character.stage);
                        stageOffset++;
                    }
                }
                charInfo->setOrder(character.order);
                charInfo->setMusic(character.song);
                characters.push_back(charInfo);
            }
        } catch (const Filesystem::NotFound & error){
            Global::debug(0) << "Could not load a tile: " << error.getTrace() << endl;
        } catch (const MugenException & error){
            Global::debug(0) << "Could not load a tile: " << error.getReason() << endl;
        }
    }
    
    // Now setup Grid
    if (characters.size() == 0){
        ostringstream out;
        out << "No characters available to select. Please add some to " << selectFile.path();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    std::vector<CharacterInfo *>::iterator nextChar = characters.begin();
    for (std::vector<CharacterCollect>::iterator i = characterCollection.begin(); i != characterCollection.end(); ++i){
	CharacterCollect & character = *i;
        if (character.blank){
            grid.addBlank();
        } else if (character.random){
            if (characters.size() > 0){
                grid.addCharacter(characters.front(), true);
            }
	} else {
            if (nextChar != characters.end()){
                grid.addCharacter(*nextChar);
                nextChar++;
            }
	}
    }
    
    // Prepare stages
    for (std::vector<std::string>::iterator i = stageNames.begin(); i != stageNames.end(); ++i){
	grid.getStageHandler().addStage((*i));
    }
    
    // Setup arcade matches
    int order = 1;
    for (std::vector<int>::iterator i = arcadeMaxMatches.begin(); i != arcadeMaxMatches.end();++i){
	std::vector<CharacterInfo *> tempCharacters = getCharacterGroup(order);
	std::random_shuffle(tempCharacters.begin(),tempCharacters.end());
	std::vector<CharacterInfo *>::iterator currentCharacter = tempCharacters.begin();
	std::queue<CharacterInfo *> characters;
	for (int m = 0; m < *i; ++m){
	    if (currentCharacter != tempCharacters.end()){
		characters.push(*currentCharacter);
		currentCharacter++;
	    } else {
		// No more
		break;
	    }
	}
	if (!characters.empty()){
	    arcadeMatches.push(characters);
	}
	order++;
    }
    
    // Setup team matches
    order = 1;
    for (std::vector<int>::iterator i =  teamMaxMatches.begin(); i != teamMaxMatches.end();++i){
	std::vector<CharacterInfo *> tempCharacters = getCharacterGroup(order);
	std::random_shuffle(tempCharacters.begin(),tempCharacters.end());
	std::vector<CharacterInfo *>::iterator currentCharacter = tempCharacters.begin();
	std::queue<CharacterInfo *> characters;
	for (int m = 0; m < *i; ++m){
	    if (currentCharacter != tempCharacters.end()){
		characters.push(*currentCharacter);
		currentCharacter++;
	    } else {
		// No more
		break;
	    }
	}
	if (!characters.empty()){
	    teamMatches.push(characters);
	}
	order++;
    }
#endif
}

static void startMusic(const Filesystem::AbsolutePath & systemFile, const string & which){
    try {
	string music = Mugen::Util::probeDef(systemFile, "music", which);
	Music::loadSong(Storage::instance().find(Filesystem::RelativePath(Mugen::Data::getInstance().getDirectory().path() + "/sound/" + music)).path());
	Music::pause();
	Music::play();
    } catch (const MugenException & ex){
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not load music: " << fail.getTrace() << endl;
    }
}

void CharacterSelect::run(const std::string & title, Searcher & search){
    bool escaped = false;

    class WithSubscription{
    public:
        WithSubscription(Searcher & search, Searcher::Subscriber & subscription):
        search(search),
        subscription(subscription){
            search.subscribe(&subscription);
        }

        Searcher & search;
        Searcher::Subscriber & subscription;

        ~WithSubscription(){
            search.unsubscribe(&subscription);
        }
    };

    WithSubscription(search, subscription);

    Gui::FadeTool fader;
    // Set the fade state
    fader.setState(Gui::FadeTool::FadeIn);
  
    // Run select screen background music
    startMusic(systemFile, "select.bgm");

    class Logic: public PaintownUtil::Logic {
    public:
        Logic(Gui::FadeTool & fader, MugenSound * cancelSound, CharacterSelect & select, Background * background, Grid & grid, Cursor & player1Cursor, Cursor & player2Cursor, FontHandler & titleFont):
        is_done(false),
        escaped(false),
        fader(fader),
        cancelSound(cancelSound),
        select(select),
        background(background),
        grid(grid),
        player1Cursor(player1Cursor),
        player2Cursor(player2Cursor),
        titleFont(titleFont){
            gameInput = Mugen::getPlayer1Keys(20);
        }

        bool is_done;
        bool escaped;
        Gui::FadeTool & fader;
        MugenSound * cancelSound;
        InputMap<Mugen::Keys> gameInput;
        CharacterSelect & select;
        Background * background;
        Grid & grid;
        Cursor & player1Cursor;
        Cursor & player2Cursor;
        FontHandler & titleFont;

        bool didEscape() const {
            return escaped;
        }

        double ticks(double system){
            return Util::gameTicks(system);
        }

        void run(){
            InputSource source;
            vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(gameInput, source);
            for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }

                if (event[Mugen::Esc]){
                    is_done = escaped = true;
                    fader.setState(Gui::FadeTool::FadeOut);
                    // play cancel sound
                    if (cancelSound){
                        cancelSound->play();
                    }
                    InputManager::waitForRelease(gameInput, source, Mugen::Esc);
                }
            }

            /* *FIXME remove later when solution is found */
            if (select.checkPlayerData()){
                is_done = true;
                fader.setState(Gui::FadeTool::FadeOut);
            }

            fader.act();

            background->act();

            grid.lock();
            grid.act(player1Cursor, player2Cursor);

            player1Cursor.act(grid);
            player2Cursor.act(grid);
            grid.unlock();

            titleFont.act();
        }

        bool done(){
            return is_done || fader.getState() == Gui::FadeTool::EndFade;
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Background * background, Grid & grid, Cursor & player1Cursor, Cursor & player2Cursor, FontHandler & titleFont, Gui::FadeTool & fader, const string & title):
        background(background),
        grid(grid),
        player1Cursor(player1Cursor),
        player2Cursor(player2Cursor),
        titleFont(titleFont),
        fader(fader),
        title(title){
        }

        Background * background;
        Grid & grid;
        Cursor & player1Cursor;
        Cursor & player2Cursor;
        FontHandler & titleFont;
        Gui::FadeTool & fader;
        const string & title;

        void draw(const Graphics::Bitmap & buffer){
            Graphics::StretchedBitmap workArea(DEFAULT_WIDTH, DEFAULT_HEIGHT, buffer);
            workArea.start();
            background->renderBackground(0,0,workArea);
	    // Render Grid
            grid.lock();
	    grid.render(workArea);
	    // Render cursors
	    player1Cursor.render(grid, workArea);
	    player2Cursor.render(grid, workArea);
            grid.unlock();
	    
	    // render title
	    titleFont.render(title, workArea);
	    
	    // render Foregrounds
	    background->renderForeground(0,0,workArea);
	    
	    // render fades
	    fader.draw(workArea);
	    
	    // Finally render to screen
	    // workArea.Stretch(buffer);
            workArea.finish();
	    buffer.BlitToScreen();
        }
    };

    Logic logic(fader, cancelSound, *this, background, grid, player1Cursor, player2Cursor, titleFont);
    Draw draw(background, grid, player1Cursor, player2Cursor, titleFont, fader, title);

    PaintownUtil::standardLoop(logic, draw);
    
#if 0
    while ( ! done && fader.getState() != Gui::FadeTool::EndFade ){
    
	bool draw = false;
	
        if ( Global::speed_counter3 > 0 ){
            draw = true;
            runCounter += Util::gameTicks(Global::speed_counter3);
            Global::speed_counter3 = 0;
            while ( runCounter >= 1.0 ){
                runCounter -= 1;
                // Key handler
                InputManager::poll();

                vector<InputMap<Mugen::Keys>::InputEvent> out = InputManager::getEvents(gameInput);
                for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                    const InputMap<Mugen::Keys>::InputEvent & event = *it;
                    if (!event.enabled){
                        continue;
                    }

                    if (event[Mugen::Esc]){
                        done = escaped = true;
                        fader.setState(Gui::FadeTool::FadeOut);
                        // play cancel sound
                        if (cancelSound){
                            cancelSound->play();
                        }
                        InputManager::waitForRelease(gameInput, Mugen::Esc);
                    }
                }

                /* *FIXME remove later when solution is found */
                if (checkPlayerData()){
                    done = true;
                    fader.setState(Gui::FadeTool::FadeOut);
                }

                // Logic

                // Fader
                fader.act();

                // Backgrounds
                background->act();

                // Grid
                grid.lock();
                grid.act(player1Cursor, player2Cursor);

                // Cursors
                player1Cursor.act(grid);
                player2Cursor.act(grid);
                grid.unlock();

                // Title
                titleFont.act();
            }
        }
		
        /* is this needed here? */
	while ( Global::second_counter > 0 ){
	    game_time--;
	    Global::second_counter--;
	    if (game_time < 0){
                game_time = 0;
	    }
	}

	if ( draw ){
	    // render backgrounds
	    background->renderBackground(0,0,workArea);
	    // Render Grid
            grid.lock();
	    grid.render(workArea);
	    // Render cursors
	    player1Cursor.render(grid, workArea);
	    player2Cursor.render(grid, workArea);
            grid.unlock();
	    
	    // render title
	    titleFont.render(title, workArea);
	    
	    // render Foregrounds
	    background->renderForeground(0,0,workArea);
	    
	    // render fades
	    fader.draw(workArea);
	    
	    // Finally render to screen
	    workArea.Stretch(bmp);
	    bmp.BlitToScreen();
	}

	while (Global::speed_counter3 < 1){
            PaintownUtil::rest(1);
	}
    }
#endif

    quitSearching = true;
    
    // **FIXME Hack figure something out
    if (logic.didEscape()){
	throw Exception::Return(__FILE__, __LINE__);
    }
}

void CharacterSelect::reset(){
    switch (gameType){
	case Arcade:
	    // set first cursor1
	    if (playerType == Player1){
		player1Cursor.setState(Cursor::CharacterSelect);
		player2Cursor.setState(Cursor::NotActive);
                if (currentStage){
                    delete currentStage;
                    if (currentPlayer2->hasRandomStage()){
                        currentStage = new Mugen::Stage(grid.getStageHandler().getRandomStage());
                    } else {
                        currentStage = new Mugen::Stage(Util::findFile(currentPlayer2->getStage()));
                    }
                }
	    } else if (playerType == Player2){
		player2Cursor.setState(Cursor::CharacterSelect);
		player1Cursor.setState(Cursor::NotActive);
                if (currentStage){
                    delete currentStage;
                    if (currentPlayer1->hasRandomStage()){
                        currentStage = new Mugen::Stage(grid.getStageHandler().getRandomStage());
                    } else {
                        currentStage = new Mugen::Stage(Util::findFile(currentPlayer1->getStage()));
                    }
                }
	    }
	    break;
	case Versus:
	    player1Cursor.setState(Cursor::CharacterSelect);
	    player2Cursor.setState(Cursor::CharacterSelect);
	    break;
	case TeamArcade:
	    player1Cursor.setState(Cursor::TeamSelect);
	    break;
	case TeamVersus:
	    player1Cursor.setState(Cursor::TeamSelect);
	    player2Cursor.setState(Cursor::TeamSelect);
	    break;
	case TeamCoop:
	    player1Cursor.setState(Cursor::CharacterSelect);
	    break;
	case Survival:
	    player1Cursor.setState(Cursor::TeamSelect);
	    break;
	case SurvivalCoop:
	    player1Cursor.setState(Cursor::CharacterSelect);
	    break;
	case Training:
	    // set first cursor1
	    if (playerType == Player1){
		player1Cursor.setState(Cursor::CharacterSelect);
		player2Cursor.setState(Cursor::NotActive);
	    } else if (playerType == Player2){
		player2Cursor.setState(Cursor::CharacterSelect);
		player1Cursor.setState(Cursor::NotActive);
	    }
	    break;
	case Watch:
	    // set first cursor1
	    if (playerType == Player1){
		player1Cursor.setState(Cursor::CharacterSelect);
		player2Cursor.setState(Cursor::NotActive);
	    } else if (playerType == Player2){
		player2Cursor.setState(Cursor::CharacterSelect);
		player1Cursor.setState(Cursor::NotActive);
	    }
	    break;
	default:
	    break;
    }
}

void CharacterSelect::renderVersusScreen(){

    // start bgm
    try {
        std::string music = Util::probeDef(systemFile, "music", "vs.bgm");
        Music::loadSong(Util::findFile(Filesystem::RelativePath(music)).path());
        Music::pause();
        Music::play();
    } catch (const MugenException & ex){
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not load music: " << fail.getTrace() << endl;
    }
    
    versus.render(*currentPlayer1, *currentPlayer2, currentStage);
}

bool CharacterSelect::setNextArcadeMatch(){
    vector<CharacterInfo*> characters = grid.getCharacters();
    if (characters.size() == 0){
        return false;
    }
    std::random_shuffle(characters.begin(), characters.end());

    if (playerType == Player1){
	// tempPlayer = currentPlayer2 = characters.front();
        currentPlayer2 = characters.front();
        currentPlayer2->setPlayer2Act(PaintownUtil::rnd(1, 12));
    } else if (playerType == Player2){
	// tempPlayer = currentPlayer1 = characters.front();
	currentPlayer1 = characters.front();
        currentPlayer1->setPlayer1Act(PaintownUtil::rnd(1, 12));
    }
        
    currentStage = new Mugen::Stage(grid.getStageHandler().getRandomStage());
    
    return true;

    /* FIXME: redo the arcade stuff */
    /*
    if (arcadeMatches.empty()){
	return false;
    }
    std::queue<CharacterInfo *> & characters = arcadeMatches.front();
    if (characters.empty()){
	arcadeMatches.pop();
	if (arcadeMatches.empty()){
	    return false;
	}
	characters = arcadeMatches.front();
    }
    CharacterInfo * tempPlayer = NULL;
    if (playerType == Player1){
	tempPlayer = currentPlayer2 = characters.front();
        currentPlayer2->setPlayer2Act(PaintownUtil::rnd(1,12));
    } else if (playerType == Player2){
	tempPlayer = currentPlayer1 = characters.front();
        currentPlayer1->setPlayer1Act(PaintownUtil::rnd(1,12));
    }
    characters.pop();
    if (currentStage){
        delete currentStage;
        currentStage = NULL;
    }
    if (tempPlayer->hasRandomStage()){
        currentStage = new Mugen::Stage(Util::findFile(grid.getStageHandler().getRandomStage()));
    } else {
        currentStage = new Mugen::Stage(Util::findFile(currentPlayer2->getStage()));
    }
    return true;
    */
}

bool CharacterSelect::setNextTeamMatch(){
    return false;
}

bool CharacterSelect::checkPlayerData(){
    /* FIXME: theres a bunch of copy/pasted code in here, clean it up */
    switch (gameType){
	case Arcade:
	    if (playerType == Player1){
		if (player1Cursor.getState() == Cursor::Done){
		    currentPlayer1 = player1Cursor.getCurrentCell()->getCharacter();
                    currentPlayer1->setPlayer1Act(player1Cursor.getActSelection());
		    return true;
		}
	    } else if (playerType == Player2){
		if (player2Cursor.getState() == Cursor::Done){
		    currentPlayer2 = player2Cursor.getCurrentCell()->getCharacter();
                    currentPlayer2->setPlayer1Act(player2Cursor.getActSelection());
		    return true;
		}
	    }
	    break;
	case Versus:
	    if ((player1Cursor.getState() == Cursor::Done) && (player2Cursor.getState() == Cursor::Done)){
		currentPlayer1 = player1Cursor.getCurrentCell()->getCharacter();
                currentPlayer1->setPlayer1Act(player1Cursor.getActSelection());
		currentPlayer2 = player2Cursor.getCurrentCell()->getCharacter();
                currentPlayer2->setPlayer1Act(player2Cursor.getActSelection());
		if (currentStage){
		    delete currentStage;
		}
		currentStage = new Mugen::Stage(grid.getStageHandler().getStage());
		return true;
	    }
	    break;
	case TeamArcade:
	    /* Ignore */
	    break;
	case TeamVersus:
	    /* Ignore */
	    break;
	case TeamCoop:
	    /* Ignore */
	    break;
	case Survival:
	    /* Ignore */
	    break;
	case SurvivalCoop:
	    /* Ignore */
	    break;
	case Training:
            /* FIXME: I copy/pasted this from watch, is it right? */
            if (playerType == Player1){
		if (player1Cursor.getState() == Cursor::Done && !currentPlayer1){
		    // Store 
		    currentPlayer1 = player1Cursor.getCurrentCell()->getCharacter();
                    currentPlayer1->setPlayer1Act(player1Cursor.getActSelection());
		    // Reset state and pick next player
		    player1Cursor.setState(Cursor::CharacterSelect);
		    grid.setCursorPlayer2Start(player1Cursor);
		    return false;
		} else if (player1Cursor.getState() == Cursor::Done && !currentPlayer2){
		    currentPlayer2 = player1Cursor.getCurrentCell()->getCharacter();
                    currentPlayer2->setPlayer2Act(player1Cursor.getActSelection());
		    grid.setCursorStageSelect(player1Cursor);
		    return false;
		} else if (player1Cursor.getState() == Cursor::Done){
		    // Finish up
		    currentStage = new Mugen::Stage(grid.getStageHandler().getStage());
		    return true;
		}
	    } else if (playerType == Player2){
		if (player2Cursor.getState() == Cursor::Done && !currentPlayer2){
		    // Store
		    currentPlayer2 = player2Cursor.getCurrentCell()->getCharacter();
                    currentPlayer2->setPlayer2Act(player2Cursor.getActSelection());
		    // Reset state and pick next player
		    player2Cursor.setState(Cursor::CharacterSelect);
		    grid.setCursorPlayer1Start(player2Cursor);
		    return false;
		} else if (player2Cursor.getState() == Cursor::Done && !currentPlayer1){
		    currentPlayer1 = player2Cursor.getCurrentCell()->getCharacter();
                    currentPlayer1->setPlayer1Act(player2Cursor.getActSelection());
		    grid.setCursorStageSelect(player2Cursor);
		    return false;
		} else if (player2Cursor.getState() == Cursor::Done){
		    // Finish up
		    currentStage = new Mugen::Stage(grid.getStageHandler().getStage());
		    return true;
		}
	    }
	    break;
	case Watch:
	    if (playerType == Player1){
		if (player1Cursor.getState() == Cursor::Done && !currentPlayer1){
		    // Store
		    currentPlayer1 = player1Cursor.getCurrentCell()->getCharacter();
                    currentPlayer1->setPlayer1Act(player1Cursor.getActSelection());
		    // Reset state and pick next player
		    player1Cursor.setState(Cursor::CharacterSelect);
		    grid.setCursorPlayer2Start(player1Cursor);
		    return false;
		} else if (player1Cursor.getState() == Cursor::Done && !currentPlayer2){
		    currentPlayer2 = player1Cursor.getCurrentCell()->getCharacter();
                    currentPlayer2->setPlayer2Act(player1Cursor.getActSelection());
		    grid.setCursorStageSelect(player1Cursor);
		    return false;
		} else if (player1Cursor.getState() == Cursor::Done){
		    // Finish up
		    currentStage = new Mugen::Stage(grid.getStageHandler().getStage());
		    return true;
		}
	    } else if (playerType == Player2){
		if (player2Cursor.getState() == Cursor::Done && !currentPlayer2){
		    // Store
		    currentPlayer2 = player2Cursor.getCurrentCell()->getCharacter();
                    currentPlayer2->setPlayer2Act(player2Cursor.getActSelection());
		    // Reset state and pick next player
		    player2Cursor.setState(Cursor::CharacterSelect);
		    grid.setCursorPlayer1Start(player2Cursor);
		    return false;
		} else if (player2Cursor.getState() == Cursor::Done && !currentPlayer1){
		    currentPlayer1 = player2Cursor.getCurrentCell()->getCharacter();
                    currentPlayer1->setPlayer1Act(player2Cursor.getActSelection());
		    grid.setCursorStageSelect(player2Cursor);
		    return false;
		} else if (player2Cursor.getState() == Cursor::Done){
		    // Finish up
		    currentStage = new Mugen::Stage(grid.getStageHandler().getStage());
		    return true;
		}
	    }
	    break;
	default:
	    /* Ignore */
	    break;
    }
    return false;
}

/* indexes start at 1 */
MugenFont * CharacterSelect::getFont(int index) const {
    if (index - 1 >= 0 && index - 1 < (signed) fonts.size()){
        return fonts[index - 1];
    } else {
        ostringstream out;
        out << "No font for index " << index;
        throw MugenException(out.str(), __FILE__, __LINE__);
    }
}

CharacterSelect::Subscriber::Subscriber(CharacterSelect & owner):
owner(owner){
}

CharacterSelect::Subscriber::~Subscriber(){
}
        
void CharacterSelect::Subscriber::receiveCharacters(const std::vector<Filesystem::AbsolutePath> & paths){
    owner.addFiles(paths);
}

void CharacterSelect::Subscriber::receiveStages(const std::vector<Filesystem::AbsolutePath> & paths){
    for (vector<Filesystem::AbsolutePath>::const_iterator it = paths.begin(); it != paths.end(); it++){
        const Filesystem::AbsolutePath & path = *it;
        owner.grid.getStageHandler().addStage(path);
    }
}
