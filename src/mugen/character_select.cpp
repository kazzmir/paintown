#include "util/bitmap.h"
#include "mugen/character_select.h"

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

#include "mugen_stage.h"

#include "init.h"
#include "resource.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "globals.h"
#include "factory/font_render.h"

#include "menu/menu_option.h"
#include "menu/menu_global.h"
#include "menu/option_quit.h"
#include "menu/option_dummy.h"

#include "gui/keyinput_manager.h"
#include "gui/keys.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "character.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"
#include "mugen_storyboard.h"

#include "mugen/option_versus.h"
#include "ast/all.h"
#include "parser/all.h"

#include "input/input-manager.h"

namespace PaintownUtil = ::Util;

using namespace std;
using namespace Mugen;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

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

void FontHandler::render(const std::string &text, const Bitmap &bmp){
    switch(state){
	default:
	case Normal:
	    font->render(location.x, location.y, position, bank, bmp, text);
	    break;
	case Blink:
	    if (blinkState == Normal){
		font->render(location.x, location.y, position, bank, bmp, text);
	    } else if (blinkState == Blink){
		blinkFont->render(location.x, location.y, blinkPosition, blinkBank, bmp, text);
	    }
	    break;
	case Done:
	    doneFont->render(location.x, location.y, donePosition, doneBank, bmp, text);
	    break;
    }
}

CharacterInfo::CharacterInfo(const std::string &definitionFile):
definitionFile(definitionFile),
baseDirectory(Util::getFileDir(definitionFile)),
spriteFile(Util::probeDef(definitionFile,"files","sprite")),
name(Util::probeDef(definitionFile,"info","name")),
displayName(Util::probeDef(definitionFile,"info","displayname")),
currentAct(0),
icon(0),
portrait(0),
referenceCell(0){
    /* Grab the act files, in mugen it's strictly capped at 12 so we'll do the same */
    for (int i = 0; i < 12; ++i){
        stringstream act;
        act << "pal" << i;
        try {
            std::string actFile = Util::probeDef(definitionFile,"files",act.str());
            actCollection.push_back(actFile);
        } catch (const MugenException &me){
            // Ran its course got what we needed
        }
    }
    // just a precaution
    spriteFile = Util::removeSpaces(spriteFile);
    icon = Util::probeSff(baseDirectory + spriteFile,9000,0,baseDirectory + actCollection[currentAct]);
    portrait = Util::probeSff(baseDirectory + spriteFile,9000,1,baseDirectory + actCollection[currentAct]);
}

CharacterInfo::~CharacterInfo(){
    if (icon){
        delete icon;
    }
    if (portrait){
        delete portrait;
    }
}

void CharacterInfo::setAct(int number){
    if (number == currentAct)return;
    currentAct = number;
    if (icon){
        delete icon;
    }
    if (portrait){
        delete portrait;
    }
    icon = Util::probeSff(baseDirectory + spriteFile,9000,0,baseDirectory + actCollection[currentAct]);
    portrait = Util::probeSff(baseDirectory + spriteFile,9000,1,baseDirectory + actCollection[currentAct]);
}

// Stage selector
StageHandler::StageHandler():
currentStage(0),
selecting(true){
    stages.push_back("Random");
    stageNames.push_back("Random");
}

StageHandler::~StageHandler(){
}

void StageHandler::act(){
    font.act();
}

void StageHandler::render(const Bitmap &bmp){
    font.render(stageNames[currentStage],bmp);
}
	
//! Get current selected stage
const std::string &StageHandler::getStage(){
    // check if random first;
    if (currentStage == 0){
	return stages[PaintownUtil::rnd(0,stages.size()-1)];
    }
    return stages[currentStage];
}

//! Set Next Stage
void StageHandler::next(){
    if (!selecting){
	return;
    }
    if (currentStage < stages.size()-1){
	currentStage++;
    } else {
	currentStage = 0;
    }
}

//! Set Prev Stage
void StageHandler::prev(){
    if (!selecting){
	return;
    }
    if (currentStage > 0){
	currentStage--;
    } else {
	currentStage = stages.size()-1;
    }
}

//! Add stage to list
void StageHandler::addStage(const std::string &stage){
    try {
	// *FIXME not a good solution to get file
	std::string ourDefFile = stage;
	std::string baseDir = Filesystem::find("mugen/stages/");
	if (ourDefFile.find(".def")==std::string::npos){
	    ourDefFile+=".def";
	}
	// Get correct directory
	baseDir = Mugen::Util::getFileDir(baseDir + ourDefFile);
	ourDefFile = Mugen::Util::getCorrectFileLocation(baseDir, Mugen::Util::stripDir(ourDefFile));
	std::string temp = Util::probeDef(ourDefFile,"info","name");
	stageNames.push_back(temp);
	stages.push_back(stage);
    } catch (const MugenException &ex){
	Global::debug(0) << "Problem adding stage. Reason: " << ex.getReason() << endl;
    }
}

// Cell
Cell::Cell(int x, int y):
location(x,y),
background(0),
randomSprite(0),
random(false),
empty(true),
characterScaleX(1),
characterScaleY(1),
flash(0),
cursors(None){
}

Cell::~Cell(){
}

void Cell::act(std::vector<CharacterInfo *> &characters){
    if (random){
	unsigned int num = PaintownUtil::rnd(0,characters.size()-1);
	character = characters[num];
    }
    if (flash){
	flash--;
    }
}

void Cell::render(const Bitmap & bmp){
    background->render(position.x,position.y,bmp);
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
	Bitmap::drawingMode(Bitmap::MODE_TRANS);
	Bitmap::transBlender( 0, 0, 0, int(25.5 * flash) );
	bmp.rectangleFill( position.x -1, position.y -1, (position.x -1) + dimensions.x, (position.y - 1) + dimensions.y,Bitmap::makeColor(255,255,255));
	Bitmap::drawingMode(Bitmap::MODE_SOLID);
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
	    cellRow.push_back(cell);
	    currentPosition.x += cellSize.x + cellSpacing;
	}
	cells.push_back(cellRow);
	currentPosition.y += cellSize.y + cellSpacing;
    }
}

void Grid::act(){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &row = (*i);
	for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
	    cell->act(characters);
	}
    }
    
    stages.getFontHandler().act();
}

void Grid::render(const Bitmap & bmp){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &row = (*i);
	for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
	    if (cell->isEmpty()){
		if (showEmptyBoxes){
		    cell->render(bmp);
		}
	    } else {
		cell->render(bmp);
	    }
	}
    }
}

void Grid::addCharacter(CharacterInfo *character, bool isRandom){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &row = (*i);
	for (std::vector< Cell *>::iterator column = row.begin(); column != row.end(); ++column){
	    Cell *cell = (*column);
	    if (cell->isEmpty()){
		if (isRandom){
		    cell->setRandom(true);
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

void Grid::setCursorStart(Cursor &cursor){
    Cell *cell = getCell(cursor.getStart().x,cursor.getStart().y);
    cell->setCursorState(Cell::One);
    cursor.setCurrentCell(cell);
}

void Grid::moveCursorLeft(Cursor &cursor){
    Mugen::Point current = cursor.getCurrentCell()->getLocation();
    current.y--;
    if (current.y < 0){
	if (wrapping){
	    current.y = columns-1;
	} else {
	    return;
	}
    }
    Cell *cell;
    try {
	cell = getCell(current.x,current.y);
    } catch (MugenException &me){
	// Shouldn't happen but you never know lets not continue
	return;
    }
    
    if (cell->isEmpty()){
	if (!moveOverEmptyBoxes){
	    return;
	}
    }
    cursor.getCurrentCell()->popCursor();
    cell->pushCursor();
    cursor.setCurrentCell(cell);
}
void Grid::moveCursorRight(Cursor &cursor){
    Mugen::Point current = cursor.getCurrentCell()->getLocation();
    current.y++;
    if (current.y >= columns){
	if (wrapping){
	    current.y = 0;
	} else {
	    return;
	}
    }
    Cell *cell;
    try {
	cell = getCell(current.x,current.y);
    } catch (MugenException &me){
	// Shouldn't happen but you never know lets not continue
	return;
    }
    
    if (cell->isEmpty()){
	if (!moveOverEmptyBoxes){
	    return;
	}
    }
    cursor.getCurrentCell()->popCursor();
    cell->pushCursor();
    cursor.setCurrentCell(cell);
}
void Grid::moveCursorUp(Cursor &cursor){
    Mugen::Point current = cursor.getCurrentCell()->getLocation();
    current.x--;
    if (current.x < 0){
	if (wrapping){
	    current.x = rows-1;
	} else {
	    return;
	}
    }
    Cell *cell;
    try {
	cell = getCell(current.x,current.y);
    } catch (MugenException &me){
	// Shouldn't happen but you never know lets not continue
	return;
    }
    
    if (cell->isEmpty()){
	if (!moveOverEmptyBoxes){
	    return;
	}
    }
    cursor.getCurrentCell()->popCursor();
    cell->pushCursor();
    cursor.setCurrentCell(cell);
}
void Grid::moveCursorDown(Cursor &cursor){
    Mugen::Point current = cursor.getCurrentCell()->getLocation();
    current.x++;
    if (current.x >= rows){
	if (wrapping){
	    current.x = 0;
	} else {
	    return;
	}
    }
    Cell *cell;
    try {
	cell = getCell(current.x,current.y);
    } catch (MugenException &me){
	// Shouldn't happen but you never know lets not continue
	return;
    }
    
    if (cell->isEmpty()){
	if (!moveOverEmptyBoxes){
	    return;
	}
    }
    cursor.getCurrentCell()->popCursor();
    cell->pushCursor();
    cursor.setCurrentCell(cell);
}

void Grid::selectCell(Cursor &cursor, const CharacterKeys & key){
    // *TODO use the key to determine which map(act) is used
    // Get the appropriate cell for flashing in case of random
    cursor.getCurrentCell()->getCharacter()->getReferenceCell()->startFlash();
    // set cursor state depending on state
    switch (type){
	case Arcade:
	    cursor.setState(Cursor::Done);
	    break;
	case Versus:
	    cursor.setState(Cursor::StageSelect);
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
	    break;
	case Watch:
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

Cell *Grid::getCell(int row, int column) throw (MugenException){
    for (CellMap::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< Cell *> &rowIterator = (*i);
	for (std::vector< Cell *>::iterator columnIterator = rowIterator.begin(); columnIterator != rowIterator.end(); ++columnIterator){
	    Cell *cell = (*columnIterator);
	    if (cell->getLocation() == Mugen::Point(row,column)){
		return cell;
	    }
	}
    }
    
    throw MugenException("Could not find cell.");
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
state(NotActive){
}

Cursor::~Cursor(){
}

void Cursor::act(Grid &grid){
    InputMap<CharacterKeys>::Output out = InputManager::getMap(input);
    switch (state){
	case NotActive:
	    return;
	    break;
	case TeamSelect:
	    if (out[Up]){
	    }
	    if (out[Down]){
	    }
	    if (out[Left]){
	    }
	    if (out[Right]){
	    }
	    if (out[A]){
	    }
	    if (out[B]){
	    }
	    if (out[C]){
	    }
	    if (out[X]){
	    }
	    if (out[Y]){
	    }
	    if (out[Z]){
	    }
	    if (out[Start]){
	    }
	    break;
	case CharacterSelect:
	    if (out[Up]){
		grid.moveCursorUp(*this);
	    }
	    if (out[Down]){
		grid.moveCursorDown(*this);
	    }
	    if (out[Left]){
		grid.moveCursorLeft(*this);
	    }
	    if (out[Right]){
		grid.moveCursorRight(*this);
	    }
	    if (out[A]){
		grid.selectCell(*this,A);
	    }
	    if (out[B]){
		grid.selectCell(*this,B);
	    }
	    if (out[C]){
		grid.selectCell(*this,C);
	    }
	    if (out[X]){
		grid.selectCell(*this,X);
	    }
	    if (out[Y]){
		grid.selectCell(*this,Y);
	    }
	    if (out[Z]){
		grid.selectCell(*this,Z);
	    }
	    if (out[Start]){
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
	    if (out[Left]){
		grid.getStageHandler().prev();
	    }
	    if (out[Right]){
		grid.getStageHandler().next();
	    }
	    if (out[A]){
		grid.selectStage();
	    }
	    if (out[B]){
		grid.selectStage();
	    }
	    if (out[C]){
		grid.selectStage();
	    }
	    if (out[X]){
		grid.selectStage();
	    }
	    if (out[Y]){
		grid.selectStage();
	    }
	    if (out[Z]){
		grid.selectStage();
	    }
	    if (out[Start]){
	    }
	    break;
	case Done:
	default:
	    break;
    }
}

void Cursor::render(Grid &grid, const Bitmap & bmp){
    switch (state){
	case NotActive:
	    return;
	    break;
	case TeamSelect:
	    break;
	case CharacterSelect:
	    if (!hideForBlink){
		activeSprite->render(currentCell->getPosition().x,currentCell->getPosition().y,bmp);
	    }
	    renderPortrait(bmp);
	    break;
	case Done:
	default:
	    if (!hideForBlink){
		doneSprite->render(currentCell->getPosition().x,currentCell->getPosition().y,bmp);
	    }
	    renderPortrait(bmp);
    }
    
    /* Have to make sure stage select is prominent kinda stupid */
    if (state == StageSelect){
	grid.getStageHandler().render(bmp);
    }
}

void Cursor::renderPortrait(const Bitmap &bmp){
    // Lets do the portrait and name
    if (!currentCell->isEmpty()){
	const CharacterInfo *character = currentCell->getCharacter();
	Mugen::Effects effects;
	effects.facing = facing;
	effects.scalex = faceScaleX;
	effects.scaley = faceScaleY;
	character->getPortrait()->render(faceOffset.x,faceOffset.y,bmp,effects);
	font.render(character->getName(),bmp);
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

New::CharacterSelect::CharacterSelect(const std::string &file, const GameType &type):
systemFile(file),
sffFile(""),
sndFile(""),
selectFile(""),
type(type){
    grid.setGameType(type);
    
    switch (type){
	case Arcade:
	    // set first cursor1
	    player1.setState(Cursor::CharacterSelect);
	    break;
	case Versus:
	    player1.setState(Cursor::CharacterSelect);
	    player2.setState(Cursor::CharacterSelect);
	    break;
	case TeamArcade:
	    player1.setState(Cursor::TeamSelect);
	    break;
	case TeamVersus:
	    player1.setState(Cursor::TeamSelect);
	    player2.setState(Cursor::TeamSelect);
	    break;
	case TeamCoop:
	    player1.setState(Cursor::CharacterSelect);
	    break;
	case Survival:
	    player1.setState(Cursor::TeamSelect);
	    break;
	case SurvivalCoop:
	    player1.setState(Cursor::CharacterSelect);
	    break;
	case Training:
	    player1.setState(Cursor::CharacterSelect);
	    break;
	case Watch:
	    break;
	default:
	    break;
    }
}

New::CharacterSelect::~CharacterSelect(){
    // Get rid of sprites
    for( Mugen::SpriteMap::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    // Cleanup fonts
    for (std::vector< MugenFont *>::iterator f = fonts.begin(); f != fonts.end(); ++f){
	    if (*f) delete (*f);
    }
    /* background */
    if (background){
	delete background;
    }
}

void New::CharacterSelect::load() throw (MugenException){
    // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Mugen::Util::getFileDir(systemFile);
    
    Global::debug(1) << baseDir << endl;
    
    if (systemFile.empty()){
        throw MugenException( "Cannot locate character select definition file for: " + systemFile );
    }

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(systemFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + systemFile + " in" + diff.printTime("") << endl;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	head = Mugen::Util::fixCase(head);
        if (head == "info"){
	    /* Nothing right now */
        } else if (head == "files"){
            class FileWalker: public Ast::Walker{
                public:
                    FileWalker(Mugen::New::CharacterSelect & select, const string & baseDir):
                        select(select),
                        baseDir(baseDir){
                        }

                    Mugen::New::CharacterSelect & select;
                    const string & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            simple >> select.sffFile;
                            Global::debug(1) << "Got Sprite File: '" << select.sffFile << "'" << endl;
                            Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(baseDir, select.sffFile), "", select.sprites);
			    for( Mugen::SpriteMap::iterator i = select.sprites.begin() ; i != select.sprites.end() ; ++i ){
				// Load these sprites so they are ready to use
				for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
				    if( j->second )j->second->load();
				}
			    }
                        } else if (simple == "snd"){
                            simple >> select.sndFile;
                            Global::debug(1) << "Got Sound File: '" << select.sndFile << "'" << endl;
                        } else if (simple == "logo.storyboard"){
                            // Ignore
                        } else if (simple == "intro.storyboard"){
                            // Ignore
                        } else if (simple == "select"){
                            simple >> select.selectFile;
                            Global::debug(1) << "Got Select File: '" << select.selectFile << "'" << endl;
                        } else if (simple == "fight"){
                            // Ignore
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            string temp;
                            simple >> temp;
                            select.fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;

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
                SelectInfoWalker(New::CharacterSelect & self, Mugen::SpriteMap & sprites):
                    self(self),
                    sprites(sprites){
                    }

                New::CharacterSelect & self;
                Mugen::SpriteMap & sprites;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
		if (simple == "fadein.time" ){
		    int time;
                    simple >> time;
		    self.fader.setFadeInTime(time);
		} else if (simple == "fadein.color" ){
		    int r,g,b;
                    simple >> r >> g >> b;
		    self.fader.setFadeInColor(Bitmap::makeColor(r,g,b));
		} else if (simple == "fadeout.time"){
		    int time;
                    simple >> time;
		    self.fader.setFadeOutTime(time);
		} else if (simple == "fadeout.color"){
		    int r,g,b;
                    simple >> r >> g >> b;
		    self.fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
		} else if (simple == "rows"){
                    int rows;
		    simple >> rows;
		    self.grid.setRows(rows);
		} else if (simple == "columns"){
                    int columns;
		    simple >> columns;
		    self.grid.setColumns(columns);
		} else if (simple == "wrapping"){
		    bool wrap;
                    simple >> wrap;
		    self.grid.setWrapping(wrap);
		} else if (simple == "pos"){
		    int x,y;
                    simple >> x >> y;
		    self.grid.setPosition(x,y);
		} else if (simple == "showemptyboxes"){
                    bool boxes;
		    simple >> boxes;
		    self.grid.setShowEmptyBoxes(boxes);
		} else if (simple == "moveoveremptyboxes"){
		    bool boxes;
                    simple >> boxes;
		    self.grid.setMoveOverEmptyBoxes(boxes);
		} else if (simple == "cell.size"){
                    int x, y;
		    simple >> x >> y;
		    self.grid.setCellSize(x,y);
		} else if (simple == "cell.spacing"){
		    int spacing;
                    simple >> spacing;
		    self.grid.setCellSpacing(spacing);
		} else if (simple == "cell.bg.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.grid.setCellBackgroundSprite(sprites[group][sprite]);
		} else if (simple == "cell.random.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.grid.setCellRandomSprite(sprites[group][sprite]);
		} else if (simple == "cell.random.switchtime"){
		    int time;
		    simple >> time;
		    self.grid.setCellRandomSwitchTime(time);
		} else if (simple == "p1.cursor.startcell"){
		    int x,y;
                    simple >> x >> y;
		    self.player1.setStart(x,y);
		} else if (simple == "p1.cursor.active.spr"){
		    int group, sprite;
		    simple >> group >> sprite;
		    self.player1.setActiveSprite(sprites[group][sprite]);
		} else if (simple == "p1.cursor.done.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.player1.setDoneSprite(sprites[group][sprite]);
		} else if (simple == "p1.cursor.move.snd"){ /* nothing */ }
                else if (simple == "p1.cursor.done.snd"){ /* nothing */ }
		else if (simple == "p1.random.move.snd"){ /* nothing */ }
		else if (simple == "p2.cursor.startcell"){
                    int x,y;
                    simple >> x >> y;
		    self.player2.setStart(x,y);
		} if (simple == "p2.cursor.active.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.player2.setActiveSprite(sprites[group][sprite]);
		} else if (simple == "p2.cursor.done.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.player2.setDoneSprite(sprites[group][sprite]);
		} else if (simple == "p2.cursor.blink"){
		    bool blink;
		    simple >> blink;
		    self.player2.setBlink(blink);
		} 
		else if ( simple == "p2.cursor.move.snd"){ /* nothing */ }
		else if ( simple == "p2.cursor.done.snd"){ /* nothing */ }
		else if ( simple == "p2.random.move.snd"){ /* nothing */ }
		else if ( simple == "stage.move.snd"){ /* nothing */ }
		else if ( simple == "stage.done.snd"){ /* nothing */ }
		else if ( simple == "cancel.snd"){ /* nothing */ }
		else if (simple == "portrait.offset"){
		    int x,y;
		    simple >> x >> y;
		    self.grid.setPortraitOffset(x,y);
		} else if (simple == "portrait.scale"){
		    double x,y;
		    simple >> x >> y;
		    self.grid.setPortraitScale(x,y);
		} else if ( simple == "title.offset"){
		    int x, y;
		    simple >> x >> y;
		    self.titleFont.setLocation(x,y);
		} else if ( simple == "title.font"){
		    int index=0, bank=0, position=0;
		    try {
			simple >> index >> bank >> position;
		    } catch (const Ast::Exception & e){
			//ignore for now
		    }
		    self.titleFont.setPrimary(self.fonts[index-1],bank,position);
		} else if ( simple == "p1.face.offset"){
		    int x, y;
		    simple >> x >> y;
		    self.player1.setFaceOffset(x,y);
		} else if ( simple == "p1.face.scale"){
		    double x, y;
		    simple >> x >> y;
		    self.player1.setFaceScale(x,y);
		} else if ( simple == "p1.face.facing"){
		    int f;
		    simple >> f;
		    self.player1.setFacing(f);
		} else if ( simple == "p2.face.offset"){
		    int x, y;
		    simple >> x >> y;
		    self.player2.setFaceOffset(x,y);
		} else if ( simple == "p2.face.scale"){
		    double x, y;
		    simple >> x >> y;
		    self.player2.setFaceScale(x,y);
		} else if ( simple == "p2.face.facing"){
		    int f;
		    simple >> f;
		    self.player2.setFacing(f);
		} else if ( simple == "p1.name.offset"){
		    int x, y;
		    simple >> x >> y;
		    self.player1.getFontHandler().setLocation(x,y);
		}  else if ( simple == "p1.name.font"){
		    int index=0, bank=0, position=0;
		    try {
			simple >> index >> bank >> position;
		    } catch (const Ast::Exception & e){
			//ignore for now
		    } self.player1.getFontHandler().setPrimary(self.fonts[index-1],bank,position);
		} else if ( simple == "p2.name.offset"){
		    int x, y;
		    simple >> x >> y;
		    self.player2.getFontHandler().setLocation(x,y);
		} else if ( simple == "p2.name.font"){
		    int index, bank, position;
		    simple >> index >> bank >> position;
		    self.player2.getFontHandler().setPrimary(self.fonts[index-1],bank,position);
		} else if ( simple == "stage.pos"){
		    int x, y;
		    simple >> x >> y;
		    self.grid.getStageHandler().getFontHandler().setLocation(x,y);
		} else if ( simple == "stage.active.font"){
		    int index=0, bank=0, position=0;
		    try {
			simple >> index >> bank >> position;
		    } catch (const Ast::Exception & e){
			//ignore for now
		    }
		    self.grid.getStageHandler().getFontHandler().setPrimary(self.fonts[index-1],bank,position);
		} else if ( simple == "stage.active2.font"){
                    int index=0, bank=0, position=0;
		    try {
			simple >> index >> bank >> position;
		    } catch (const Ast::Exception & e){
			//ignore for now
		    }
		    self.grid.getStageHandler().getFontHandler().setBlink(self.fonts[index-1],bank,position);
		} else if ( simple == "stage.done.font"){
                    int index=0, bank=0, position=0;
		    try {
			simple >> index >> bank >> position;
		    } catch (const Ast::Exception & e){
			//ignore for now
		    }
		    self.grid.getStageHandler().getFontHandler().setDone(self.fonts[index-1],bank,position);
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
	    const int ticker = 0;
	    MugenBackgroundManager *manager = new MugenBackgroundManager(Mugen::Util::getFileDir(selectFile), collectSelectStuff(section_it, parsed.getSections()->end()), ticker, &sprites, "selectbg");
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	} else if (head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
	else if (head == "vs screen" ){ /* Ignore for now */ }
	else if (head == "versusbgdef" ){ /* Ignore for now */ }
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
            throw MugenException("Unhandled Section in '" + systemFile + "': " + head, __FILE__, __LINE__ ); 
        }
    }
    
    // Set up Grid
    grid.initialize();
    
    // Setup cursors
    grid.setCursorStart(player1);
    grid.setCursorStart(player2);
    
    // Now load up our characters
    parseSelect(Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(selectFile)));
    
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    if (background){
	background->preload(DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
    }
}

void New::CharacterSelect::parseSelect(const std::string &selectFile){
    const std::string file = Mugen::Util::getCorrectFileLocation(Mugen::Util::getFileDir(selectFile), Mugen::Util::stripDir(selectFile));
    
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(file));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + file + " in" + diff.printTime("") << endl;
    
    std::vector< std::string > stageNames;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
	Ast::Section * section = *section_it;
	std::string head = section->getName();
        
	head = Mugen::Util::fixCase(head);

        if (head == "characters"){
            class CharacterWalker: public Ast::Walker{
            public:
		CharacterWalker(Grid &grid, std::vector< CharacterInfo *> &characters, std::vector< std::string > &stageNames):
		grid(grid),
		characters(characters),
		stageNames(stageNames){}
		virtual ~CharacterWalker(){}
		
		Grid &grid;
		std::vector< CharacterInfo *> &characters;
		std::vector< std::string > &stageNames;
                virtual void onValueList(const Ast::ValueList & list){
                    // Get Stage info and save it
                    std::string temp;
                    list >> temp;
                    
		    // Check if it's random
		    if (temp == "randomselect"){
                        // Add random
                        grid.addCharacter(0,true);
                    } else {
                        // Get character
			// *FIXME Not an elegant solution for character location
			const std::string baseDir = Filesystem::find("mugen/chars/" + temp + "/");
			std::string str = Mugen::Util::stripDir(temp);
			const std::string charDefFile = Mugen::Util::fixFileName(baseDir, std::string(str + ".def"));
			Global::debug(0) << "Got character def: " << charDefFile << endl;
			CharacterInfo *character = new CharacterInfo(charDefFile);
                        
			// Add to our character list
                        characters.push_back(character);
                        Global::debug(1) << "Got character: " << character->getName() << endl;
                        
			// Add to grid 
                        grid.addCharacter(character);

                        /* **TODO** get values for:
				    stage
				    includestage
				    random
				    music
				    order
				    
				    For now we'll assume a stage
			*/
                        std::string stageTemp;
                        list >> stageTemp;
                        // and add it to our stages
                        stageNames.push_back(stageTemp);
                        Global::debug(1) << "Got stage: " << stageTemp << endl;
                    }
                }
            };

            CharacterWalker walk(grid, characters, stageNames);
            section->walk(walk);
	} else if (head == "extrastages"){
	    class StageWalker: public Ast::Walker{
            public:
		StageWalker(std::vector< std::string > &names):
		names(names){
		}
		virtual ~StageWalker(){}
		std::vector< std::string > &names;
                virtual void onValueList(const Ast::ValueList & list){
		    // Get Stage info and save it
		    try {
			std::string temp;
			list >> temp;
			Global::debug(0) << "stage: " << temp << endl;
			names.push_back(temp);
		    } catch (...){
		    }
                }
            };
	    StageWalker walk(stageNames);
	    section->walk(walk);
	} else if (head == "options"){
	    /* Add in later */
	} else {
	    throw MugenException("Unhandled Section in '" + file + "': " + head, __FILE__, __LINE__); 
	}
    }
    
    // Prepare stages
    for (std::vector<std::string>::iterator i = stageNames.begin(); i != stageNames.end(); ++i){
	grid.getStageHandler().addStage((*i));
    }
}

void New::CharacterSelect::run(const std::string & title, const Bitmap &bmp){
    Bitmap workArea(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    bool done = false;
    
    // Set the fade state
    fader.setState(FADEIN);
  
    double runCounter = 0;
    Global::speed_counter = 0;
    Global::second_counter = 0;
    int game_time = 100;
    
    // Set game keys temporary
    InputMap<int> gameInput;
    gameInput.set(Keyboard::Key_ESC, 10, true, 0);
    
    while ( ! done && fader.getState() != RUNFADE ){
    
	bool draw = false;
	
	if ( Global::speed_counter > 0 ){
	    draw = true;
	    runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
	    while ( runCounter >= 1.0 ){
		runCounter -= 1;
		// Key handler
		InputManager::poll();
		
		InputMap<int>::Output out = InputManager::getMap(gameInput);
		if (out[0]){
		    done = true;
		    fader.setState(FADEOUT);
		}
		
		// Logic
		
		// Fader
		fader.act();
		
		// Backgrounds
		background->logic( 0, 0, 0, 0 );
		
		// Grid
		grid.act();
		
		// Cursors
		player1.act(grid);
		player2.act(grid);
		
		// Title
		titleFont.act();
	    }
	    
	    Global::speed_counter = 0;
	}
		
	while ( Global::second_counter > 0 ){
	    game_time--;
	    Global::second_counter--;
	    if ( game_time < 0 ){
		    game_time = 0;
	    }
	}

	if ( draw ){
	    // render backgrounds
	    background->renderBack(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
	    // Render Grid
	    grid.render(workArea);
	    // Render cursors
	    player1.render(grid, workArea);
	    player2.render(grid, workArea);
	    
	    // render title
	    titleFont.render(title,workArea);
	    
	    // render stage
	    
	    // render Foregrounds
	    background->renderFront(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
	    
	    // render fades
	    fader.draw(&workArea);
	    
	    // Finally render to screen
	    workArea.Stretch(bmp);
	    bmp.BlitToScreen();
	}

	while ( Global::speed_counter < 1 ){
		PaintownUtil::rest( 1 );
	}
    }
}

/* OLD STUFF */

Mugen::CharacterSelect::CharacterSelect(const unsigned long int &ticker, const std::string &filename):
location(filename),
cellBackgroundBitmap(0),
cellRandomBitmap(0),
background(0),
selectTicker(ticker),
characterList(0){
}
Mugen::CharacterSelect::~CharacterSelect(){
    /*if (cellBackgroundBitmap){
	delete cellBackgroundBitmap;
    }
    if (cellRandomBitmap){
	delete cellRandomBitmap;
    }
    if (p1Cursor.active){
	delete p1Cursor.active;
    }
    if (p1Cursor.done){
	delete p1Cursor.done;
    }
    if (p2Cursor.active){
	delete p2Cursor.active;
    }
    if (p2Cursor.done){
	delete p2Cursor.done;
    }*/
    if (background){
	delete background;
    }
    for (std::vector< std::vector< OldCell *> >::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< OldCell *> &row = *i;
	for (std::vector< OldCell *>::iterator c = row.begin(); c != row.end(); ++c){
	    OldCell *cell = *c;
	    if (cell) delete cell;
	}
    }
    // Characters
    for (std::vector< Mugen::Character *>::iterator c = characters.begin(); c != characters.end(); ++c){
	    if (*c) delete (*c);
    }
    // STages
    for (std::vector< MugenStage *>::iterator i = stages.begin(); i != stages.end(); ++i){
	    if (*i) delete (*i);
    }
    // Delete character select list struct
    if (characterList){
	delete characterList;
    }
    // Get rid of sprites
    for( Mugen::SpriteMap::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    // Cleanup fonts
    for (std::vector< MugenFont *>::iterator f = fonts.begin(); f != fonts.end(); ++f){
	    if (*f) delete (*f);
    }
}

void Mugen::CharacterSelect::load(){
     // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Mugen::Util::getFileDir(location);
    const std::string ourDefFile = location;
    
    Global::debug(1) << baseDir << endl;
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate character select definition file for: " + location );
    }

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
        if (head == "info"){
	    // Not really needed right now but can add later
	    /*
            class InfoWalker: public Ast::Walker{
            public:
                InfoWalker(){
                }

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "name"){
			string temp;
			simple >> temp;
                        Global::debug(1) << "Read name '" << temp << "'" << endl;
                    } else if (simple == "author"){
                        string temp;
                        simple >> temp;
                        Global::debug(1) << "Made by: '" << temp << "'" << endl;
                    } else {
                        throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                    }
                }
            };

            InfoWalker walker();
            section->walk(walker);
	    */
        } else if (head == "files"){
            class FileWalker: public Ast::Walker{
                public:
                    FileWalker(Mugen::CharacterSelect & select, const string & baseDir):
                        select(select),
                        baseDir(baseDir){
                        }

                    Mugen::CharacterSelect & select;
                    const string & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            simple >> select.sffFile;
                            Global::debug(1) << "Got Sprite File: '" << select.sffFile << "'" << endl;
                            Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(baseDir, select.sffFile), "", select.sprites);
                        } else if (simple == "snd"){
                            simple >> select.sndFile;
                            Global::debug(1) << "Got Sound File: '" << select.sndFile << "'" << endl;
                        } else if (simple == "logo.storyboard"){
                            // Ignore
                        } else if (simple == "intro.storyboard"){
                            // Ignore
                        } else if (simple == "select"){
                            simple >> select.selectFile;
                            Global::debug(1) << "Got Select File: '" << select.selectFile << "'" << endl;
                        } else if (simple == "fight"){
                            // Ignore
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            string temp;
                            simple >> temp;
                            select.fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;

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
	    // Pass off to selectInfo
	    parseSelectInfo(collectSelectStuff(section_it, parsed.getSections()->end()));
        } else if (head == "selectbgdef" ){ /* Ignore for now */ }
	else if (head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
	else if (head == "vs screen" ){ /* Ignore for now */ }
	else if (head == "versusbgdef" ){ /* Ignore for now */ }
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
            throw MugenException("Unhandled Section in '" + ourDefFile + "': " + head, __FILE__, __LINE__ ); 
        }
    }
    
    // Set up cell table
    Mugen::Point currentPosition;
    currentPosition.y = position.y;
    for (int row = 0; row < rows; ++row){
	currentPosition.x = position.x;
	std::vector< OldCell *> cellRow;
	for (int column = 0; column < columns; ++column){
	    OldCell *cell = new OldCell;
	    cell->position.x = currentPosition.x;
	    cell->position.y = currentPosition.y;
	    cell->character = 0;
	    cell->random = false;
	    cell->empty = true;
	    cellRow.push_back(cell);
	    currentPosition.x += cellSize.x + cellSpacing;
	}
	cells.push_back(cellRow);
	currentPosition.y += cellSize.y + cellSpacing;
    }
    
    // Now load up our characters
    loadCharacters(Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(selectFile)));
    
    // Set stage info
    stageInfo.selected = false;
    stageInfo.altCounter = 0;
    
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    if (background) background->preload(DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
}

Mugen::SelectedChars *Mugen::CharacterSelect::run(const std::string &title, const int players, const bool selectStage, Bitmap *work){
    Bitmap workArea(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    bool done = false;
    
    // Set the fade state
    fader.setState(FADEIN);
  
    double runCounter = 0;
    Global::speed_counter = 0;
    Global::second_counter = 0;
    int game_time = 100;
    // Set player 1 and 2 cursors
    p1Cursor.selecting = true;
    p1Cursor.cursor.x = p1Cursor.start.x;
    p1Cursor.cursor.y = p1Cursor.start.y;
    p2Cursor.selecting = true;
    p2Cursor.cursor.x = p2Cursor.start.x;
    p2Cursor.cursor.y = p2Cursor.start.y;
    
    // Stage list
    stageInfo.selected = false;
    stageInfo.altCounter = 0;
    std::vector< MugenStage *> stageSelections;
    if (stages.size() == 0){
        throw MugenException("No stages available");
    }
    unsigned int random = PaintownUtil::rnd(0,stages.size()-1);
    stageSelections.push_back(stages[random]);
    for (std::vector<MugenStage *>::iterator i = stages.begin(); i != stages.end(); ++i){
	stageSelections.push_back(*i);
    }
    unsigned int currentStage =  0;
    
    if (characterList){
	delete characterList;
    }
    characterList = new Mugen::SelectedChars();
    characterList->selectedStage = 0;
    
    while ( ! done && fader.getState() != RUNFADE ){
    
	bool draw = false;
	
	keyInputManager::update();

	if ( Global::speed_counter > 0 ){
		draw = true;
		runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
		while ( runCounter >= 1.0 ){
		    runCounter -= 1;
		    // Keys
		    if (fader.getState() == NOFADE){
			if ( keyInputManager::keyState(keys::UP, true ) ||
				/* for vi people like me */
			    keyInputManager::keyState('k', true )){
			    movePlayer1Cursor(-1,0);
			}

			if ( keyInputManager::keyState(keys::DOWN, true ) ||
				/* for vi people like me */
			    keyInputManager::keyState('j', true )){
			    movePlayer1Cursor(1,0);
			}
			
			if ( keyInputManager::keyState(keys::LEFT, true) ||
			    keyInputManager::keyState('h', true)){
			    if (p1Cursor.selecting){
				movePlayer1Cursor(0,-1);
			    } else {
				if (selectStage && !stageInfo.selected){
				    if (currentStage > 0){
					currentStage--;
				    } else {
					currentStage = stageSelections.size() -1;
				    }
				}
			    }
			}
			
			if ( keyInputManager::keyState(keys::RIGHT, true )||
			    keyInputManager::keyState('l', true )){
			    if (p1Cursor.selecting){
				movePlayer1Cursor(0,1);
			    } else {
				if (selectStage && !stageInfo.selected){
				    if (currentStage < stageSelections.size()-1){
					currentStage++;
				    } else {
					currentStage = 0;
				    }
				}
			    }
			}
			
			if ( keyInputManager::keyState(keys::ENTER, true ) ){
			    if (!cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->empty && p1Cursor.selecting){
				p1Cursor.selecting = false;
				characterList->team1.push_back(cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->character);
			    } else if (!p1Cursor.selecting && selectStage && !stageInfo.selected){
				stageInfo.selected = true;
				characterList->selectedStage = stageSelections[currentStage];
			    }
			}
			
			if ( keyInputManager::keyState(keys::ESC, true ) ){
			    done = true;
			    fader.setState(FADEOUT);
			}
		    }
		    // Fader
		    fader.act();
		    
		    // Backgrounds
		    background->logic( 0, 0, 0, 0 );
		    
		    // Check status
		    if (players == 2){
			if (!p1Cursor.selecting && !p2Cursor.selecting && stageInfo.selected){
			    done = true;
			    fader.setState(FADEOUT);
			}
		    } else if (players == 1){
			if (!p1Cursor.selecting && ((selectStage && stageInfo.selected) || !selectStage)){
			    done = true;
			    fader.setState(FADEOUT);
			}
		    }
		}
		
		Global::speed_counter = 0;
	}
		
	while ( Global::second_counter > 0 ){
		game_time--;
		Global::second_counter--;
		if ( game_time < 0 ){
			game_time = 0;
		}
	}

	if ( draw ){
		// backgrounds
		background->renderBack(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
		// Stuff
		drawCursors(players, &workArea);
		// Draw title
		MugenFont *font = fonts[titleFont.index-1];
		font->render(titleOffset.x, titleOffset.y, titleFont.position, titleFont.bank, workArea, title);
		// Draw stage
		if (selectStage && (!p1Cursor.selecting || !p2Cursor.selecting)){
		    std::string stageName = "Stage";
		    if (currentStage == 0){
			stageName += ": Random";
		    } else {
			std::stringstream s;
			s << " " << currentStage << ": " << stageSelections[currentStage]->getName();
			stageName += s.str();
		    }
		    if (!stageInfo.selected){
			if (stageInfo.altCounter % 2 == 0){
			    // reuse Font
			    font = fonts[stageInfo.stageActiveFont.index-1];
			    font->render(stageInfo.stagePosition.x, stageInfo.stagePosition.y, stageInfo.stageActiveFont.position, stageInfo.stageActiveFont.bank, workArea, stageName);
			} else {
			    // reuse Font
			    font = fonts[stageInfo.stageActiveFont2.index-1];
			    font->render(stageInfo.stagePosition.x, stageInfo.stagePosition.y, stageInfo.stageActiveFont2.position, stageInfo.stageActiveFont2.bank, workArea, stageName);
			}
			stageInfo.altCounter++;
			if (stageInfo.altCounter == 10){
			    stageInfo.altCounter = 0;
			}
		    } else {
			// Done selecting
			// reuse Font
			font = fonts[stageInfo.stageDoneFont.index-1];
			font->render(stageInfo.stagePosition.x, stageInfo.stagePosition.y, stageInfo.stageDoneFont.position, stageInfo.stageDoneFont.bank, workArea, stageName);
		    }
		}
		// Foregrounds
		background->renderFront(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
		// Do fades
		fader.draw(&workArea);
		// Finally render to screen
		workArea.Stretch(*work);
		work->BlitToScreen();
	}

	while ( Global::speed_counter < 1 ){
		PaintownUtil::rest( 1 );
		keyInputManager::update();
	}
    }
	if (selectStage && !stageInfo.selected)return 0;
	return characterList;
}

void Mugen::CharacterSelect::drawCursors(const int players, Bitmap *work){
    Mugen::Point currentPosition;
    currentPosition.y = position.y;
    for (int row = 0; row < rows; ++row){
	currentPosition.x = position.x;
	for (int column = 0; column < columns; ++column){
	    OldCell *cell = cells[row][column];
	    if (!cell->empty){
		if (!cell->random){
		    cell->character->renderSprite(cell->position.x,cell->position.y,9000,0,work);
		    cellBackgroundBitmap->draw(currentPosition.x,currentPosition.y,*work);
		} else {
		    cellRandomBitmap->draw(cell->position.x,cell->position.y,*work);
		    cellBackgroundBitmap->draw(currentPosition.x,currentPosition.y,*work);
		}
	    } else if (showEmptyBoxes){
		cellBackgroundBitmap->draw(currentPosition.x,currentPosition.y,*work);
	    }
	    currentPosition.x += cellSize.x + cellSpacing;
	}
	currentPosition.y += cellSize.y + cellSpacing;
    }
    
    // Player cursors player 1
    if (p1Cursor.selecting){
	p1Cursor.active->draw(cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->position.x,cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->position.y,*work);
    } else {
	p1Cursor.done->draw(cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->position.x,cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->position.y,*work);
    }
    if ( !cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->empty ){
	OldCell *cell = cells[p1Cursor.cursor.x][p1Cursor.cursor.y];
	if (!cell->empty){
	    if (!cell->random){
		// Portrait
		cell->character->renderSprite(p1Cursor.faceOffset.x,p1Cursor.faceOffset.y,9000,1,work,p1Cursor.facing, p1Cursor.faceScalex,p1Cursor.faceScaley);
		// Name
		MugenFont *font = fonts[p1Cursor.nameFont.index-1];
		font->render(p1Cursor.nameOffset.x, p1Cursor.nameOffset.y, p1Cursor.nameFont.position, p1Cursor.nameFont.bank, *work, cell->character->getName());
	    } else {
	    }
	}
    }
    // Player cursors player 2
    if (players > 1){
	if (p2Cursor.blink && ((p1Cursor.cursor.x == p2Cursor.cursor.x) && (p1Cursor.cursor.y == p2Cursor.cursor.y))){
	    if (p2Cursor.blinkCounter % 2 == 0){
		if (p2Cursor.selecting){
		    p2Cursor.active->draw(cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.x,cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.y,*work);
		} else {
		    p2Cursor.done->draw(cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.x,cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.y,*work);
		}
	    }
	    p2Cursor.blinkCounter++;
	    if (p2Cursor.blinkCounter == 10){
		p2Cursor.blinkCounter = 0;
	    }
	} else {
	    if (p2Cursor.selecting){
		p2Cursor.active->draw(cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.x,cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.y,*work);
	    } else {
		p2Cursor.done->draw(cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.x,cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->position.y,*work);
	    }
	}
	if ( !cells[p2Cursor.cursor.x][p2Cursor.cursor.y]->empty ){
	    OldCell *cell = cells[p2Cursor.cursor.x][p2Cursor.cursor.y];
	    if (!cell->empty){
		if (!cell->random){
		    // Portrait
		    cell->character->renderSprite(p2Cursor.faceOffset.x,p2Cursor.faceOffset.y,9000,1,work,p2Cursor.facing, p2Cursor.faceScalex,p2Cursor.faceScaley);
		    // Name
		    MugenFont *font = fonts[p2Cursor.nameFont.index-1];
		    font->render(p2Cursor.nameOffset.x, p2Cursor.nameOffset.y, p2Cursor.nameFont.position, p2Cursor.nameFont.bank, *work, cell->character->getName());
		} else {
		}
	    }
	}
    }
}

void Mugen::CharacterSelect::movePlayer1Cursor(int x, int y){
    if (!p1Cursor.selecting){
	return;
    }
    if (x > 0){
	if (moveOverEmptyBoxes){
	    p1Cursor.cursor.x += x;
	    if (wrapping && p1Cursor.cursor.x >= rows){
		p1Cursor.cursor.x = 0;
	    } else if (p1Cursor.cursor.x >= rows) {
		p1Cursor.cursor.x = rows-1;
	    }
	}
	else {
	    int curx = p1Cursor.cursor.x;
	    while (!cells[curx][p1Cursor.cursor.y]->empty){
		curx+=x;
		if (wrapping && curx >= rows){
		    curx = 0;
		} else if (curx >= rows) {
		    curx = p1Cursor.cursor.x;
		}
		if (curx == p1Cursor.cursor.x){
		    break;
		}
	    }
	    if (!cells[curx][p1Cursor.cursor.y]->empty)p1Cursor.cursor.x = curx;
	}
    } else if (x < 0){
	if (moveOverEmptyBoxes){
	    p1Cursor.cursor.x += x;
	    if (wrapping && p1Cursor.cursor.x < 0){
		p1Cursor.cursor.x = rows-1;
	    } else if (p1Cursor.cursor.x < 0) {
		p1Cursor.cursor.x = 0;
	    }
	}
	else {
	    int curx = p1Cursor.cursor.x;
	    while (!cells[curx][p1Cursor.cursor.y]->empty){
		curx+=x;
		if (wrapping && curx < 0){
		    curx = rows-1;
		} else if (curx < 0) {
		    curx = p1Cursor.cursor.x;
		}
		if (curx == p1Cursor.cursor.x){
		    break;
		}
	    }
	    if (!cells[curx][p1Cursor.cursor.y]->empty)p1Cursor.cursor.x = curx;
	}
    }
    if (y > 0){
	if (moveOverEmptyBoxes){
	    p1Cursor.cursor.y += y;
	    if (wrapping && p1Cursor.cursor.y >= columns){
		p1Cursor.cursor.y = 0;
	    } else if (p1Cursor.cursor.y >= columns) {
		p1Cursor.cursor.y = columns-1;
	    }
	}
	else {
	    int cury = (p1Cursor.cursor.y += y) % columns;
	    while (cells[p1Cursor.cursor.x][cury]->empty){
		cury += y;
		if (wrapping && cury >= columns){
		    cury = 0;
		} else if (cury >= columns) {
		    cury = p1Cursor.cursor.y;
		}
		if (cury == p1Cursor.cursor.y){
		    break;
		}
	    }
	    if (!cells[p1Cursor.cursor.x][cury]->empty){
                p1Cursor.cursor.y = cury;
            }
	}
    } else if (y < 0){
	if (moveOverEmptyBoxes){
	    p1Cursor.cursor.y += y;
	    if (wrapping && p1Cursor.cursor.y < 0){
		p1Cursor.cursor.y = columns-1;
	    } else if (p1Cursor.cursor.y < 0) {
		p1Cursor.cursor.y = 0;
	    }
	}
	else {
	    int cury = p1Cursor.cursor.y;
	    while (!cells[p1Cursor.cursor.x][cury]->empty){
		cury+=y;
		if (wrapping && cury < 0){
		    cury = columns-1;
		} else if (cury < 0) {
		    cury = p1Cursor.cursor.y;
		}
		if (cury == p1Cursor.cursor.y){
		    break;
		}
	    }
	    if (!cells[p1Cursor.cursor.x][cury]->empty)p1Cursor.cursor.y = cury;
	}
    }
}

void Mugen::CharacterSelect::movePlayer2Cursor(int x, int y){
    if (!p2Cursor.selecting){
	return;
    }
    if (x > 0){
	if (moveOverEmptyBoxes){
	    p2Cursor.cursor.x += x;
	    if (wrapping && p2Cursor.cursor.x >= rows){
		p2Cursor.cursor.x = 0;
	    } else if (p2Cursor.cursor.x >= rows) {
		p2Cursor.cursor.x = rows-1;
	    }
	}
	else {
	    int curx = p2Cursor.cursor.x;
	    while (!cells[curx][p2Cursor.cursor.y]->empty){
		curx+=x;
		if (wrapping && curx >= rows){
		    curx = 0;
		} else if (curx >= rows) {
		    curx = p2Cursor.cursor.x;
		}
		if (curx == p2Cursor.cursor.x){
		    break;
		}
	    }
	    if (!cells[curx][p2Cursor.cursor.y]->empty)p2Cursor.cursor.x = curx;
	}
    } else if (x < 0){
	if (moveOverEmptyBoxes){
	    p2Cursor.cursor.x += x;
	    if (wrapping && p2Cursor.cursor.x < 0){
		p2Cursor.cursor.x = rows-1;
	    } else if (p2Cursor.cursor.x < 0) {
		p2Cursor.cursor.x = 0;
	    }
	}
	else {
	    int curx = p2Cursor.cursor.x;
	    while (!cells[curx][p2Cursor.cursor.y]->empty){
		curx+=x;
		if (wrapping && curx < 0){
		    curx = rows-1;
		} else if (curx < 0) {
		    curx = p2Cursor.cursor.x;
		}
		if (curx == p2Cursor.cursor.x){
		    break;
		}
	    }
	    if (!cells[curx][p2Cursor.cursor.y]->empty)p2Cursor.cursor.x = curx;
	}
    }
    if (y > 0){
	if (moveOverEmptyBoxes){
	    p2Cursor.cursor.y += y;
	    if (wrapping && p2Cursor.cursor.y >= columns){
		p2Cursor.cursor.y = 0;
	    } else if (p2Cursor.cursor.y >= columns) {
		p2Cursor.cursor.y = columns-1;
	    }
	}
	else {
	    int cury = p2Cursor.cursor.y;
	    while (!cells[p2Cursor.cursor.x][cury]->empty){
		cury+=y;
		if (wrapping && cury >= columns){
		    cury = 0;
		} else if (cury >= columns) {
		    cury = p2Cursor.cursor.y;
		}
		if (cury == p2Cursor.cursor.y){
		    break;
		}
	    }
	    if (!cells[p1Cursor.cursor.x][cury]->empty)p2Cursor.cursor.y = cury;
	}
    } else if (y < 0){
	if (moveOverEmptyBoxes){
	    p2Cursor.cursor.y += y;
	    if (wrapping && p2Cursor.cursor.y < 0){
		p2Cursor.cursor.y = columns-1;
	    } else if (p2Cursor.cursor.y < 0) {
		p2Cursor.cursor.y = 0;
	    }
	}
	else {
	    int cury = p2Cursor.cursor.y;
	    while (!cells[p2Cursor.cursor.x][cury]->empty){
		cury+=y;
		if (wrapping && cury < 0){
		    cury = columns-1;
		} else if (cury < 0) {
		    cury = p2Cursor.cursor.y;
		}
		if (cury == p2Cursor.cursor.y){
		    break;
		}
	    }
	    if (!cells[p1Cursor.cursor.x][cury]->empty)p2Cursor.cursor.y = cury;
	}
    }
}

void Mugen::CharacterSelect::loadCharacters(const std::string &selectFile){
    const std::string file = Mugen::Util::getCorrectFileLocation(Mugen::Util::getFileDir(selectFile), Mugen::Util::stripDir(selectFile));
    
    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(file));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + file + " in" + diff.printTime("") << endl;
    
    std::vector< std::string > stageNames;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
	Ast::Section * section = *section_it;
	std::string head = section->getName();
        
	head = Mugen::Util::fixCase(head);

        if (head == "characters"){
            class CharacterWalker: public Ast::Walker{
            public:
		CharacterWalker(const int rows, const int columns, std::vector< std::vector< OldCell *> > &cells,std::vector< Mugen::Character *> &characters, std::vector< std::string > &stageNames):
		row(0),
		column(0),
		rows(rows),
		columns(columns),
		cells(cells),
		characters(characters),
		stageNames(stageNames){}
		virtual ~CharacterWalker(){}
		
		int row, column;
		const int rows,columns;
		std::vector< std::vector< OldCell *> > &cells;
		std::vector< Mugen::Character *> &characters;
		std::vector< std::string > &stageNames;
                virtual void onValueList(const Ast::ValueList & list){
                    // Get Stage info and save it
                    std::string temp;
                    list >> temp;
                    /* **TODO** need to create identifier to house Character 
                       base data so we don't have to load the entire thing
                       - Filename
                       - Character Name
                       - Character Display Name
                       - Image 9000 and 9001
                       - Is stage random?
                       - Stage name def
                       - music
                       - includestage
                       - order
                       */

                    /* **TODO** check whether it is one of the below 
                       - the character directory
                       - a specified character def file
                       - randomselect
                       */
                    temp = Mugen::Util::removeSpaces(temp);
                    if (temp == "random"){
                        // set random flag
                        cells[row][column]->random = true;
                        cells[row][column]->empty = false;
                    } else {
                        // Get character
                        Mugen::Character *character = new Mugen::Character(temp);
                        //character->load();
                        characters.push_back(character);
                        Global::debug(1) << "Got character: " << character->getName() << endl;
                        // set cell 
                        cells[row][column]->character = character;
                        cells[row][column]->empty = false;

                        // Hack get stage
                        std::string stageTemp;
                        list >> stageTemp;
                        // Next item will be a stage lets add it to the list of stages
                        stageNames.push_back(stageTemp);
                        Global::debug(1) << "Got stage: " << stageTemp << endl;
                    }

                    column++;
                    if (column >=columns){
                        column = 0;
                        row++;
                        // Have we met our quota?
                        if (row >= rows){
                            // can't add any more characters... breakage
                            return;
                        }
                    }
                }
            };

            CharacterWalker walk(rows, columns, cells, characters, stageNames);
            section->walk(walk);
	} else if (head == "extrastages"){
	    class StageWalker: public Ast::Walker{
            public:
		StageWalker(std::vector< std::string > &names):
		names(names){
		}
		virtual ~StageWalker(){}
		std::vector< std::string > &names;
                virtual void onValueList(const Ast::ValueList & list){
		    // Get Stage info and save it
		    try {
			std::string temp;
			list >> temp;
			Global::debug(0) << "stage: " << temp << endl;
			names.push_back(temp);
		    } catch (...){
		    }
                }
            };
	    StageWalker walk(stageNames);
	    section->walk(walk);
	} else if (head == "options"){
	} else {
	    throw MugenException("Unhandled Section in '" + file + "': " + head, __FILE__, __LINE__); 
	}
    }
    
    // Prepare stages
    for (std::vector<std::string>::iterator i = stageNames.begin(); i != stageNames.end(); ++i){
	MugenStage *stage = new MugenStage(*i);
	// No need to load them.... Load only when using the stage...
	/*try{
	    stage->load();
	} catch (MugenException &ex){
	    throw MugenException(ex);
	}*/
	
	stages.push_back(stage);
    }
}

void Mugen::CharacterSelect::parseSelectInfo(const std::vector<Ast::Section*> & sections){
    for (vector<Ast::Section*>::const_iterator it = sections.begin(); it != sections.end(); it++){
        Ast::Section * section = *it;
        std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	head = Mugen::Util::fixCase(head);

	if (head == "select info"){ 
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
		    int time;
                    simple >> time;
		    self.fader.setFadeInTime(time);
		} else if (simple == "fadein.color" ){
		    int r,g,b;
                    simple >> r >> g >> b;
		    self.fader.setFadeInColor(Bitmap::makeColor(r,g,b));
		} else if (simple == "fadeout.time"){
		    int time;
                    simple >> time;
		    self.fader.setFadeOutTime(time);
		} else if (simple == "fadeout.color"){
		    int r,g,b;
                    simple >> r >> g >> b;
		    self.fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
		} else if (simple == "rows"){
                    simple >> self.rows;
		} else if (simple == "columns"){
                    simple >> self.columns;
		} else if (simple == "wrapping"){
                    simple >> self.wrapping;
		} else if (simple == "pos"){
                    simple >> self.position.x >> self.position.y;
		} else if (simple == "showemptyboxes"){
                    simple >> self.showEmptyBoxes;
		} else if (simple == "moveoveremptyboxes"){
                    simple >> self.moveOverEmptyBoxes;
		} else if (simple == "cell.size"){
                    simple >> self.cellSize.x >> self.cellSize.y;
		} else if (simple == "cell.spacing"){
                    simple >> self.cellSpacing;
		} else if (simple == "cell.bg.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.cellBackgroundSprite = sprites[group][sprite];
		    self.cellBackgroundSprite->load();
		    self.cellBackgroundBitmap = self.cellBackgroundSprite->getBitmap();
		} else if (simple == "cell.random.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.cellRandomSprite = sprites[group][sprite];
		    self.cellRandomSprite->load();
		    self.cellRandomBitmap = self.cellRandomSprite->getBitmap();
		} else if (simple == "cell.random.switchtime"){
                    simple >> self.cellRandomSwitchTime;
		} else if (simple == "p1.cursor.startcell"){
                    simple >> self.p1Cursor.cursor.x;
		    simple >> self.p1Cursor.cursor.y;
		    self.p1Cursor.start.x = self.p1Cursor.cursor.x;
		    self.p1Cursor.start.y = self.p1Cursor.cursor.y;
		} else if (simple == "p1.cursor.active.spr"){
		    int group, sprite;
		    simple >> group >> sprite;
		    self.p1Cursor.cursorActiveSprite = sprites[group][sprite];
		    self.p1Cursor.cursorActiveSprite->load();
		    self.p1Cursor.active = self.p1Cursor.cursorActiveSprite->getBitmap();
		} else if (simple == "p1.cursor.done.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.p1Cursor.cursorDoneSprite = sprites[group][sprite];
		    self.p1Cursor.cursorDoneSprite->load();
		    self.p1Cursor.done = self.p1Cursor.cursorDoneSprite->getBitmap();
		} else if (simple == "p1.cursor.move.snd"){ /* nothing */ }
                else if (simple == "p1.cursor.done.snd"){ /* nothing */ }
		else if (simple == "p1.random.move.snd"){ /* nothing */ }
		else if (simple == "p2.cursor.startcell"){
                    simple >> self.p2Cursor.cursor.x;
		    simple >> self.p2Cursor.cursor.y;
		    self.p2Cursor.start.x = self.p2Cursor.cursor.x;
		    self.p2Cursor.start.y = self.p2Cursor.cursor.y;
		} if (simple == "p2.cursor.active.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.p2Cursor.cursorActiveSprite = sprites[group][sprite];
		    self.p2Cursor.cursorActiveSprite->load();
		    self.p2Cursor.active = self.p2Cursor.cursorActiveSprite->getBitmap();
		} else if (simple == "p2.cursor.done.spr"){
		    int group, sprite;
                    simple >> group >> sprite;
		    self.p2Cursor.cursorDoneSprite = sprites[group][sprite];
		    self.p2Cursor.cursorDoneSprite->load();
		    self.p2Cursor.done = self.p2Cursor.cursorDoneSprite->getBitmap();
		} else if (simple == "p2.cursor.blink"){
		    simple >> self.p2Cursor.blink;
		    self.p2Cursor.blinkCounter = 0;
		} 
		else if ( simple == "p2.cursor.move.snd"){ /* nothing */ }
		else if ( simple == "p2.cursor.done.snd"){ /* nothing */ }
		else if ( simple == "p2.random.move.snd"){ /* nothing */ }
		else if ( simple == "stage.move.snd"){ /* nothing */ }
		else if ( simple == "stage.done.snd"){ /* nothing */ }
		else if ( simple == "cancel.snd"){ /* nothing */ }
		else if (simple == "portrait.offset"){
		    simple >> self.portraitOffset.x;
		    simple >> self.portraitOffset.y;
		} else if (simple == "portrait.scale"){
		    simple >> self.portraitScale.x;
		    simple >> self.portraitScale.y;
		} else if ( simple == "title.offset"){
		    simple >> self.titleOffset.x;
		    simple >> self.titleOffset.y;
		} else if ( simple == "title.font"){
		    simple >> self.titleFont.index;
		    simple >> self.titleFont.bank;
		    simple >> self.titleFont.position;
		} else if ( simple == "p1.face.offset"){
		    simple >> self.p1Cursor.faceOffset.x;
		    simple >> self.p1Cursor.faceOffset.y;
		} else if ( simple == "p1.face.scale"){
		    simple >> self.p1Cursor.faceScalex;
		    simple >> self.p1Cursor.faceScaley;
		} else if ( simple == "p1.face.facing"){
		    simple >> self.p1Cursor.facing;
		} else if ( simple == "p2.face.offset"){
		    simple >> self.p2Cursor.faceOffset.x;
		    simple >> self.p2Cursor.faceOffset.y;
		} else if ( simple == "p2.face.scale"){
		    simple >> self.p2Cursor.faceScalex;
		    simple >> self.p2Cursor.faceScaley;
		} else if ( simple == "p2.face.facing"){
		    simple >> self.p2Cursor.facing;
		} else if ( simple == "p1.name.offset"){
		    simple >> self.p1Cursor.nameOffset.x;
		    simple >> self.p1Cursor.nameOffset.y;
		}  else if ( simple == "p1.name.font"){
		    simple >> self.p1Cursor.nameFont.index;
		    simple >> self.p1Cursor.nameFont.bank;
		    simple >> self.p1Cursor.nameFont.position;
		} else if ( simple == "p2.name.offset"){
		    simple >> self.p2Cursor.nameOffset.x;
		    simple >> self.p2Cursor.nameOffset.y;
		} else if ( simple == "p2.name.font"){
		    simple >> self.p2Cursor.nameFont.index;
		    simple >> self.p2Cursor.nameFont.bank;
		    simple >> self.p2Cursor.nameFont.position;
		} else if ( simple == "stage.pos"){
		    simple >> self.stageInfo.stagePosition.x;
		    simple >> self.stageInfo.stagePosition.y;
		} else if ( simple == "stage.active.font"){
		    simple >> self.stageInfo.stageActiveFont.index;
		    simple >> self.stageInfo.stageActiveFont.bank;
		    simple >> self.stageInfo.stageActiveFont.position;
		} else if ( simple == "stage.active2.font"){
                    try{
                        simple >> self.stageInfo.stageActiveFont2.index;
                        simple >> self.stageInfo.stageActiveFont2.bank;
                        simple >> self.stageInfo.stageActiveFont2.position;
                    } catch (const Ast::Exception & e){
                        /* ignore I guess */
                    }
		} else if ( simple == "stage.done.font"){
                    try{
                        simple >> self.stageInfo.stageDoneFont.index;
                        simple >> self.stageInfo.stageDoneFont.bank;
                        simple >> self.stageInfo.stageDoneFont.position;
                    } catch (const Ast::Exception & e){
                        /* ignore */
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
	    MugenBackgroundManager *manager = new MugenBackgroundManager(Mugen::Util::getFileDir(selectFile), sections, selectTicker, &sprites, "selectbg");
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	}
    }
}
