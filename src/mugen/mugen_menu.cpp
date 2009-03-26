#include "mugen/mugen_menu.h"

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
#include "util/bitmap.h"
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
#include "mugen_character.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"
#include "mugen_storyboard.h"

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;
const int DEFAULT_SCREEN_X_AXIS = 160;
const int DEFAULT_SCREEN_Y_AXIS = 0;

MugenCharacterSelect::MugenCharacterSelect(const unsigned long int &ticker, std::vector<MugenFont *> &fonts):
cellBackgroundBitmap(0),
cellRandomBitmap(0),
selectTicker(ticker),
fonts(fonts){
}
MugenCharacterSelect::~MugenCharacterSelect(){
    if (cellBackgroundBitmap){
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
    }
    if (background){
	delete background;
    }
    for (std::vector< std::vector< MugenCell *> >::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< MugenCell *> &row = *i;
	for (std::vector< MugenCell *>::iterator c = row.begin(); c != row.end(); ++c){
	    MugenCell *cell = *c;
	    if (cell) delete cell;
	}
    }
    // Characters
    for (std::vector< MugenCharacter *>::iterator c = characters.begin(); c != characters.end(); ++c){
	    if (*c) delete (*c);
    }
    // STages
    for (std::vector< MugenStage *>::iterator i = stages.begin(); i != stages.end(); ++i){
	    if (*i) delete (*i);
    }
}
void MugenCharacterSelect::load(const std::string &selectFile, unsigned int &index, std::vector< MugenSection * > &collection, 
			   std::map< unsigned int, std::map< unsigned int, MugenSprite * > > &sprites) throw (MugenException){
    /* Extract info for our first section of our select screen */
    for( ; index < collection.size(); ++index ){
	std::string head = collection[index]->getHeader();
	MugenUtil::fixCase(head);
	if( head == "select info" ){ 
	    while( collection[index]->hasItems() ){
		MugenItemContent *content = collection[index]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		Global::debug(1) << "Got itemhead: '" << itemhead << "'" << endl;
		if ( itemhead == "fadein.time" ){
		    int time;
		    *content->getNext() >> time;
		    fader.setFadeInTime(time);
		} else if ( itemhead == "fadein.color" ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    fader.setFadeInColor(Bitmap::makeColor(r,g,b));
		} else if ( itemhead == "fadeout.time" ){
		    int time;
		    *content->getNext() >> time;
		    fader.setFadeOutTime(time);
		} else if ( itemhead == "fadeout.color" ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
		} else if ( itemhead == "rows" ){
		    *content->getNext() >> rows;
		} else if ( itemhead == "columns" ){
		    *content->getNext() >> columns;
		} else if ( itemhead == "wrapping"){
		    *content->getNext() >> wrapping;
		} else if ( itemhead == "pos" ){
		    *content->getNext() >> position.x;
		    *content->getNext() >> position.y;
		} else if ( itemhead == "showemptyboxes" ){
		    *content->getNext() >> showEmptyBoxes;
		} else if ( itemhead == "moveoveremptyboxes" ){
		    *content->getNext() >> moveOverEmptyBoxes;
		} else if ( itemhead == "cell.size" ){
		    *content->getNext() >> cellSize.x;
		    *content->getNext() >> cellSize.y;
		} else if ( itemhead == "cell.spacing" ){
		    *content->getNext() >> cellSpacing;
		} else if ( itemhead == "cell.bg.spr" ){
		    int group, sprite;
		    *content->getNext() >> group;
		    *content->getNext() >> sprite;
		    cellBackgroundSprite = sprites[group][sprite];
		    cellBackgroundBitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) cellBackgroundSprite->pcx, cellBackgroundSprite->newlength, true));
		} else if ( itemhead == "cell.random.spr" ){
		    int group, sprite;
		    *content->getNext() >> group;
		    *content->getNext() >> sprite;
		    cellRandomSprite = sprites[group][sprite];
		    cellRandomBitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) cellRandomSprite->pcx, cellRandomSprite->newlength, true));
		} else if ( itemhead == "cell.random.switchtime" ){
		    *content->getNext() >> cellRandomSwitchTime;
		} else if ( itemhead == "p1.cursor.startcell" ){
		    *content->getNext() >> p1Cursor.cursor.x;
		    *content->getNext() >> p1Cursor.cursor.y;
		    p1Cursor.start.x = p1Cursor.cursor.x;
		    p1Cursor.start.y = p1Cursor.cursor.y;
		} else if ( itemhead == "p1.cursor.active.spr"){
		    int group, sprite;
		    *content->getNext() >> group;
		    *content->getNext() >> sprite;
		    p1Cursor.cursorActiveSprite = sprites[group][sprite];
		    p1Cursor.active = new Bitmap(Bitmap::memoryPCX((unsigned char*) p1Cursor.cursorActiveSprite->pcx, p1Cursor.cursorActiveSprite->newlength, true));
		} else if ( itemhead == "p1.cursor.done.spr"){
		    int group, sprite;
		    *content->getNext() >> group;
		    *content->getNext() >> sprite;
		    p1Cursor.cursorDoneSprite = sprites[group][sprite];
		    p1Cursor.done = new Bitmap(Bitmap::memoryPCX((unsigned char*) p1Cursor.cursorDoneSprite->pcx, p1Cursor.cursorDoneSprite->newlength, true));
		} 
		else if ( itemhead == "p1.cursor.move.snd" ){ /* nothing */ }
		else if ( itemhead == "p1.cursor.done.snd"){ /* nothing */ }
		else if ( itemhead == "p1.random.move.snd"){ /* nothing */ }
		else if ( itemhead == "p2.cursor.startcell"){
		    *content->getNext() >> p2Cursor.cursor.x;
		    *content->getNext() >> p2Cursor.cursor.y;
		    p2Cursor.start.x = p2Cursor.cursor.x;
		    p2Cursor.start.y = p2Cursor.cursor.y;
		} if ( itemhead == "p2.cursor.active.spr"){
		    int group, sprite;
		    *content->getNext() >> group;
		    *content->getNext() >> sprite;
		    p2Cursor.cursorActiveSprite = sprites[group][sprite];
		    p2Cursor.active = new Bitmap(Bitmap::memoryPCX((unsigned char*) p2Cursor.cursorActiveSprite->pcx, p2Cursor.cursorActiveSprite->newlength, true));
		} else if ( itemhead == "p2.cursor.done.spr"){
		    int group, sprite;
		    *content->getNext() >> group;
		    *content->getNext() >> sprite;
		    p2Cursor.cursorDoneSprite = sprites[group][sprite];
		    p2Cursor.done = new Bitmap(Bitmap::memoryPCX((unsigned char*) p2Cursor.cursorDoneSprite->pcx, p2Cursor.cursorDoneSprite->newlength, true));
		} else if ( itemhead == "p2.cursor.blink"){
		    *content->getNext() >> p2Cursor.blink;
		    p2Cursor.blinkCounter = 0;
		} 
		else if ( itemhead == "p2.cursor.move.snd"){ /* nothing */ }
		else if ( itemhead == "p2.cursor.done.snd"){ /* nothing */ }
		else if ( itemhead == "p2.random.move.snd"){ /* nothing */ }
		else if ( itemhead == "stage.move.snd"){ /* nothing */ }
		else if ( itemhead == "stage.done.snd"){ /* nothing */ }
		else if ( itemhead == "cancel.snd"){ /* nothing */ }
		else if ( itemhead == "portrait.offset"){
		    *content->getNext() >> portraitOffset.x;
		    *content->getNext() >> portraitOffset.y;
		} else if ( itemhead == "portrait.scale"){
		    *content->getNext() >> portraitScale.x;
		    *content->getNext() >> portraitScale.y;
		} else if ( itemhead == "title.offset"){
		    *content->getNext() >> titleOffset.x;
		    *content->getNext() >> titleOffset.y;
		} else if ( itemhead == "title.font"){
		    *content->getNext() >> titleFont.index;
		    *content->getNext() >> titleFont.bank;
		    *content->getNext() >> titleFont.position;
		} else if ( itemhead == "p1.face.offset"){
		    *content->getNext() >> p1Cursor.faceOffset.x;
		    *content->getNext() >> p1Cursor.faceOffset.y;
		} else if ( itemhead == "p1.face.scale"){
		    *content->getNext() >> p1Cursor.faceScalex;
		    *content->getNext() >> p1Cursor.faceScaley;
		} else if ( itemhead == "p1.face.facing"){
		    *content->getNext() >> p1Cursor.facing;
		} else if ( itemhead == "p2.face.offset"){
		    *content->getNext() >> p2Cursor.faceOffset.x;
		    *content->getNext() >> p2Cursor.faceOffset.y;
		} else if ( itemhead == "p2.face.scale"){
		    *content->getNext() >> p2Cursor.faceScalex;
		    *content->getNext() >> p2Cursor.faceScaley;
		} else if ( itemhead == "p2.face.facing"){
		    *content->getNext() >> p2Cursor.facing;
		} else if ( itemhead == "p1.name.offset"){
		    *content->getNext() >> p1Cursor.nameOffset.x;
		    *content->getNext() >> p1Cursor.nameOffset.y;
		}  else if ( itemhead == "p1.name.font"){
		    *content->getNext() >> p1Cursor.nameFont.index;
		    *content->getNext() >> p1Cursor.nameFont.bank;
		    *content->getNext() >> p1Cursor.nameFont.position;
		} else if ( itemhead == "p2.name.offset"){
		    *content->getNext() >> p2Cursor.nameOffset.x;
		    *content->getNext() >> p2Cursor.nameOffset.y;
		} else if ( itemhead == "p2.name.font"){
		    *content->getNext() >> p2Cursor.nameFont.index;
		    *content->getNext() >> p2Cursor.nameFont.bank;
		    *content->getNext() >> p2Cursor.nameFont.position;
		} else if ( itemhead == "stage.pos"){
		    *content->getNext() >> stageInfo.stagePosition.x;
		    *content->getNext() >> stageInfo.stagePosition.y;
		} else if ( itemhead == "stage.active.font"){
		    *content->getNext() >> stageInfo.stageActiveFont.index;
		    *content->getNext() >> stageInfo.stageActiveFont.bank;
		    *content->getNext() >> stageInfo.stageActiveFont.position;
		} else if ( itemhead == "stage.active2.font"){
		    *content->getNext() >> stageInfo.stageActiveFont2.index;
		    *content->getNext() >> stageInfo.stageActiveFont2.bank;
		    *content->getNext() >> stageInfo.stageActiveFont2.position;
		} else if ( itemhead == "stage.done.font"){
		    *content->getNext() >> stageInfo.stageDoneFont.index;
		    *content->getNext() >> stageInfo.stageDoneFont.bank;
		    *content->getNext() >> stageInfo.stageDoneFont.position;
		} else if ( itemhead.find("teammenu")!=std::string::npos ){ /* Ignore for now */ }
		//else throw MugenException( "Unhandled option in Select Info Section: " + itemhead );
	    }
	}
	else if( head == "selectbgdef" ){ 
	    // Background management
	    MugenBackgroundManager *manager = new MugenBackgroundManager(MugenUtil::getFileDir( selectFile ),collection, index,selectTicker,&sprites);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	}
	else {
	    // Done collecting
	    index--;
	    break;
	}
    }
    fonts = fonts;
    // Set up cell table
    MugenMenuPoint currentPosition;
    currentPosition.y = position.y;
    for (int row = 0; row < rows; ++row){
	currentPosition.x = position.x;
	std::vector< MugenCell *> cellRow;
	for (int column = 0; column < columns; ++column){
	    MugenCell *cell = new MugenCell;
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
    loadCharacters(selectFile);
    
    // Set stage info
    stageInfo.selected = false;
    stageInfo.altCounter = 0;
    
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    if (background) background->preload(DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
}
MugenSelectedChars MugenCharacterSelect::run(const std::string &title, const int players, const bool selectStage, Bitmap *work){
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
    unsigned int random = Util::rnd(0,stages.size()-1);
    stageSelections.push_back(stages[random]);
    for (std::vector<MugenStage *>::iterator i = stages.begin(); i != stages.end(); ++i){
	stageSelections.push_back(*i);
    }
    unsigned int currentStage =  0;
    
    MugenSelectedChars characterList;
    
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
				characterList.team1.push_back(cells[p1Cursor.cursor.x][p1Cursor.cursor.y]->character);
			    } else if (!p1Cursor.selecting && selectStage && !stageInfo.selected){
				stageInfo.selected = true;
				characterList.selectedStage = stageSelections[currentStage];
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
		Util::rest( 1 );
		keyInputManager::update();
	}
    }
	
	return characterList;
}

void MugenCharacterSelect::drawCursors(const int players, Bitmap *work){
    MugenMenuPoint currentPosition;
    currentPosition.y = position.y;
    for (int row = 0; row < rows; ++row){
	currentPosition.x = position.x;
	for (int column = 0; column < columns; ++column){
	    MugenCell *cell = cells[row][column];
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
	MugenCell *cell = cells[p1Cursor.cursor.x][p1Cursor.cursor.y];
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
	    MugenCell *cell = cells[p2Cursor.cursor.x][p2Cursor.cursor.y];
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

void MugenCharacterSelect::movePlayer1Cursor(int x, int y){
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
	    int cury = p1Cursor.cursor.y;
	    while (!cells[p1Cursor.cursor.x][cury]->empty){
		cury+=y;
		if (wrapping && cury >= columns){
		    cury = 0;
		} else if (cury >= columns) {
		    cury = p1Cursor.cursor.y;
		}
		if (cury == p1Cursor.cursor.y){
		    break;
		}
	    }
	    if (!cells[p1Cursor.cursor.x][cury]->empty)p1Cursor.cursor.y = cury;
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

void MugenCharacterSelect::movePlayer2Cursor(int x, int y){
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

void MugenCharacterSelect::loadCharacters(const std::string &selectFile) throw (MugenException){
    std::string dir = MugenUtil::getFileDir(selectFile);
    std::string file = MugenUtil::stripDir(selectFile);
    MugenReader reader( MugenUtil::getCorrectFileLocation(dir,file) );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    std::vector< std::string > stageNames;
    /* Extract info for our first section of our menu */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	MugenUtil::fixCase(head);
	if( head == "characters" ){
	    int row = 0;
	    int column = 0;
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if (itemhead=="random"){
		    // set random flag
		    cells[row][column]->random = true;
		    cells[row][column]->empty = false;
		} else {
		    // Get character
		    MugenCharacter *character = new MugenCharacter(itemhead);
		    try{
			character->load();
		    } catch (MugenException &ex){
			throw MugenException(ex);
		    }
		    characters.push_back(character);
		    Global::debug(1) << "Got character: " << character->getName() << endl;
		    // set cell 
		    cells[row][column]->character = character;
		    cells[row][column]->empty = false;
		    if (content->hasItems()){
			// Next item will be a stage lets add it to the list of stages
			std::string temp;
			*content->getNext() >> temp;
			stageNames.push_back(temp);
			Global::debug(1) << "Got stage: " << temp << endl;
		    }
		    // Need to add in other options and assign their respective stages to them....
		}
		column++;
		if (column >=columns){
		    column = 0;
		    row++;
		    // Have we met our quota?
		    if (row >= rows){
			// can't add any more characters... breakage
			break;
		    }
		}
	    }
	}
	else if( head == "extrastages" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		// Next item will be a stage lets add it to the list of stages
		stageNames.push_back(itemhead);
		Global::debug(1) << "Got stage: " << itemhead << endl;
	    }
	}
	else if( head == "options" ){ /* ignore for now */}
	else throw MugenException( "Unhandled Section in '" + selectFile + "': " + head ); 
    }
    
    // Prepare stages
    for (std::vector<std::string>::iterator i = stageNames.begin(); i != stageNames.end(); ++i){
	MugenStage *stage = new MugenStage(*i);
	try{
	    stage->load();
	} catch (MugenException &ex){
	    throw MugenException(ex);
	}
	stages.push_back(stage);
    }
}

MugenMenu::MugenMenu(const std::string &filename):
optionLocation(0),
location(filename),
spriteFile(""),
soundFile(""),
logoFile(""),
introFile(""),
selectFile(""),
fightFile(""),
windowVisibleItems(0),
showBoxCursor(false),
ticker(0),
background(0),
logo(0),
intro(0){
}

void MugenMenu::load() throw (MugenException){
     // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string baseDir = Util::getDataPath() + "mugen/data/" + MugenUtil::getFileDir(location);
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, MugenUtil::stripDir(location) );
    // get real basedir
    //baseDir = MugenUtil::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate menu definition file for: " + location );
    
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    /* Extract info for our first section of our menu */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	MugenUtil::fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("name")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
		    setName(temp);
                    Global::debug(1) << "Read name '" << getName() << "'" << endl;
		} else if ( itemhead.find("author")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
                    Global::debug(1) << "Made by: '" << temp << "'" << endl;
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "files" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		if ( itemhead.find("spr")!=std::string::npos ){
		    *content->getNext() >> spriteFile;
		    Global::debug(1) << "Got Sprite File: '" << spriteFile << "'" << endl;
		    MugenUtil::readSprites( MugenUtil::getCorrectFileLocation(baseDir, spriteFile), "", sprites );
		} else if ( itemhead.find("snd")!=std::string::npos ){
		    *content->getNext() >> soundFile;
                    Global::debug(1) << "Got Sound File: '" << soundFile << "'" << endl;
		} else if ( itemhead.find("logo.storyboard")!=std::string::npos ){
		    if( content->hasItems()){
			*content->getNext() >> logoFile;
			try{
			    Global::debug(1) << baseDir << " / " << logoFile << endl;
			    logo = new MugenStoryboard(MugenUtil::getCorrectFileLocation(baseDir, logoFile));
			    logo->load();
			}
			catch (MugenException &ex){
			    throw MugenException( "Error loading logo storyboard: " + ex.getReason() );
			}
		    }
                    Global::debug(1) << "Got Logo Storyboard File: '" << logoFile << "'" << endl;
		} else if ( itemhead.find("intro.storyboard")!=std::string::npos ){
		    if (content->hasItems()){
			*content->getNext() >> introFile;
			try{
			    intro = new MugenStoryboard(MugenUtil::getCorrectFileLocation(baseDir, introFile));
			    intro->load();
			}
			catch (MugenException &ex){
			    throw MugenException( "Error loading intro storyboard: " + ex.getReason() );
			}
		    }
                    Global::debug(1) << "Got Intro Storyboard File: '" << introFile << "'" << endl;
		} else if ( itemhead.find("select")!=std::string::npos ){
		    *content->getNext() >> selectFile;
                    Global::debug(1) << "Got Select File: '" << selectFile << "'" << endl;
		} else if ( itemhead.find("fight")!=std::string::npos ){
		    *content->getNext() >> fightFile;
                    Global::debug(1) << "Got Fight File: '" << fightFile << "'" << endl;
		} else if ( itemhead.find("font")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
		    MugenUtil::removeSpaces(temp);
		    fonts.push_back(new MugenFont(MugenUtil::getCorrectFileLocation(baseDir, temp)));
                    Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
		} else throw MugenException( "Unhandled option in Files Section: " + itemhead );
	    }
	}
	else if( head == "title info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		if ( itemhead.find("fadein.time")!=std::string::npos ){
		    int time;
		    *content->getNext() >> time;
		    fader.setFadeInTime(time);
		} else if ( itemhead.find("fadein.color")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    fader.setFadeInColor(Bitmap::makeColor(r,g,b));
		} else if ( itemhead.find("fadeout.time")!=std::string::npos ){
		    int time;
		    *content->getNext() >> time;
		    fader.setFadeOutTime(time);
		} else if ( itemhead.find("fadeout.color")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
		} else if ( itemhead.find("menu.pos")!=std::string::npos ){
		    *content->getNext() >> position.x;
		    *content->getNext() >> position.y;
		} else if ( itemhead.find("menu.item.font")!=std::string::npos ){
		    *content->getNext() >> fontItem.index;
		    *content->getNext() >> fontItem.bank;
		    *content->getNext() >> fontItem.position;
		} else if ( itemhead.find("menu.item.active.font")!=std::string::npos ){
		    *content->getNext() >> fontActive.index;
		    *content->getNext() >> fontActive.bank;
		    *content->getNext() >> fontActive.position;
		} else if ( itemhead.find("menu.item.spacing")!=std::string::npos ){
		    *content->getNext() >> fontSpacing.x;
		    *content->getNext() >> fontSpacing.y;
		} else if ( itemhead.find("menu.itemname.")!=std::string::npos ){
		   if (itemhead == "menu.itemname.arcade"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.versus"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.teamarcade"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.teamversus"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.teamcoop"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.survival"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.survivalcoop"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.training"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.watch"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.options"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionDummy *dummy = new OptionDummy(temp);
				    addOption(dummy);
			    }
		       }
		   } else if (itemhead == "menu.itemname.exit"){
		       if (content->hasItems()){
			    std::string temp;
			    *content->getNext() >> temp;
			    if (!temp.empty()){
				    OptionQuit *quit = new OptionQuit(temp);
				    addOption(quit);
			    }
		       }
		   }
		} else if ( itemhead.find("menu.window.margins.x")!=std::string::npos ){
		    *content->getNext() >> windowMarginX.x;
		    *content->getNext() >> windowMarginX.y;
		} else if ( itemhead.find("menu.window.margins.y")!=std::string::npos ){
		    *content->getNext() >> windowMarginY.x;
		    *content->getNext() >> windowMarginY.y;
		} else if ( itemhead.find("menu.window.visibleitems")!=std::string::npos ){
		    *content->getNext() >> windowVisibleItems;
		} else if ( itemhead.find("menu.boxcursor.visible")!=std::string::npos ){
		    *content->getNext() >> showBoxCursor;
		} else if ( itemhead.find("menu.boxcursor.coords")!=std::string::npos ){
		    *content->getNext() >> boxCursorCoords.x1;
		    *content->getNext() >> boxCursorCoords.y1;
		    *content->getNext() >> boxCursorCoords.x2;
		    *content->getNext() >> boxCursorCoords.y2;
		    boxCursorCoords.alpha = 128;
		    boxCursorCoords.alphaMove = -6;
		} else if ( itemhead.find("cursor.move.snd")!=std::string::npos ){
		    // Configure later
		} else if ( itemhead.find("cursor.done.snd")!=std::string::npos ){
		    // Configure later
		} else if ( itemhead.find("cancel.snd")!=std::string::npos ){
		    // Configure later
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "titlebgdef" ){
	    // Background management
	    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir,collection, i,ticker,&sprites);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	}
	else if( head == "select info" ){ 
	    // Pass off to selectInfo
	    characterSelect = new MugenCharacterSelect(ticker,fonts);
	    try{
		characterSelect->load(baseDir + selectFile,i,collection,sprites);
	    }
	    catch (MugenException &ex){
		throw MugenException(ex);
	    }
	}
	else if( head == "selectbgdef" ){ /* Ignore for now */ }
	else if( head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
	else if( head == "music" ){ /* Ignore for now */ }
	else if( head == "vs screen" ){ /* Ignore for now */ }
	else if( head == "versusbgdef" ){ /* Ignore for now */ }
	else if( head.find("versusbg" ) != std::string::npos ){ /* Ignore for now */ }
	else if( head == "demo mode" ){ /* Ignore for now */ }
	else if( head == "continue screen" ){ /* Ignore for now */ }
	else if( head == "game over screen" ){ /* Ignore for now */ }
	else if( head == "win screen" ){ /* Ignore for now */ }
	else if( head == "default ending" ){ /* Ignore for now */ }
	else if( head == "end credits" ){ /* Ignore for now */ }
	else if( head == "survival results screen" ){ /* Ignore for now */ }
	else if( head == "option info" ){ /* Ignore for now */ }
	else if( head == "optionbgdef" ){ /* Ignore for now */ }
	else if( head.find("optionbg") != std::string::npos ){ /* Ignore for now */ }
	else if( head == "music" ){ /* Ignore for now */ }
	else if( head.find("begin action") != std::string::npos ){ /* Ignore  fornow */ }
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
    }
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    if (background) background->preload(DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
}

MugenMenu::~MugenMenu(){
    // cleanup
    cleanup();
}

void MugenMenu::run(){
    Bitmap workArea(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    bool done = false;
    bool endGame = false;
    
    if ( menuOptions.empty() ){
	    return;
    }
    
    selectedOption = menuOptions.begin();
    optionLocation = 0;
    menuOptions.front()->setState(MenuOption::Selected);
  /*  
    if ( !music.empty() ){
	    MenuGlobals::setMusic(music);
    }
    
    if ( !selectSound.empty() ){
	    MenuGlobals::setSelectSound(selectSound);
    }
    */
  // Set the fade state
  fader.setState(FADEIN);
  
    // Do we have logos or intros?
    // Logo run it no repeat
    if (logo){
	logo->run( work,false);
    }
    // Intro run it no repeat
    if (intro){
	intro->run( work,false);
    }
  
    double runCounter = 0;
    while( ! endGame ){
	    Global::speed_counter = 0;
	    Global::second_counter = 0;
	    int game_time = 100;
	 
	    while ( ! done && (*selectedOption)->getState() != MenuOption::Run && fader.getState() != RUNFADE ){
    
		    bool draw = false;
		    
		    keyInputManager::update();
    
		    if ( Global::speed_counter > 0 ){
			    draw = true;
			    runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
			    while ( runCounter >= 1.0 ){
				ticker++;
				runCounter -= 1;
				// Keys
				if (fader.getState() == NOFADE){
				    if ( keyInputManager::keyState(keys::UP, true ) ||
					    /* for vi people like me */
					keyInputManager::keyState('k', true )){	
					    (*selectedOption)->setState(MenuOption::Deselected);
					    if ( selectedOption > menuOptions.begin() ){
						    selectedOption--;
						    optionLocation--;
					    } else { 
						selectedOption = menuOptions.end() -1;
						optionLocation = menuOptions.size() -1;
					    }
					    (*selectedOption)->setState(MenuOption::Selected);
					    //if(menuOptions.size() > 1)MenuGlobals::playSelectSound();
				    }

				    if ( keyInputManager::keyState(keys::DOWN, true ) ||
					    /* for vi people like me */
					keyInputManager::keyState('j', true )){
					    (*selectedOption)->setState(MenuOption::Deselected);
					    if ( selectedOption < menuOptions.begin()+menuOptions.size()-1 ){
						    selectedOption++;
						    optionLocation++;
					    } else {
						selectedOption = menuOptions.begin();
						optionLocation = 0;
					    }
					    (*selectedOption)->setState(MenuOption::Selected);
					    //if(menuOptions.size() > 1)MenuGlobals::playSelectSound();
				    }
				    
				    if ( keyInputManager::keyState(keys::LEFT, true) ||
					keyInputManager::keyState('h', true)){
					    if ( (*selectedOption)->leftKey()){
						/* ??? */
					    }
				    }
				    
				    if ( keyInputManager::keyState(keys::RIGHT, true )||
					keyInputManager::keyState('l', true )){
					    if ( (*selectedOption)->rightKey()){
						/* ??? */
					    }
				    }
				    
				    if ( keyInputManager::keyState(keys::ENTER, true ) ){
					    if((*selectedOption)->isRunnable())(*selectedOption)->setState( MenuOption::Run );
					    // Set the fade state
					    fader.setState(FADEOUT);
				    }
				    
				    if ( keyInputManager::keyState(keys::ESC, true ) ){
					    endGame = done = true;
					    // Set the fade state
					    fader.setState(FADEOUT);
				    }
				}
				// Fader
				fader.act();
				
				// Options
				for( vector< MenuOption *>::iterator b = menuOptions.begin(); b != menuOptions.end(); ++b ){
					(*b)->logic();
				}
				
				// Backgrounds
				background->logic( 0, 0, 0, 0 );
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
			    // Draw any misc stuff in the background of the menu of selected object 
			    (*selectedOption)->draw(work);
			    // Draw text
			    drawText(&workArea);
			    // Foregrounds
			    background->renderFront(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
			    // Do fades
			    fader.draw(&workArea);
			    // Finally render to screen
			    workArea.Stretch(*work);
			    work->BlitToScreen();
		    }
    
		    while ( Global::speed_counter < 1 ){
			    Util::rest( 1 );
			    keyInputManager::update();
		    }
	    }
	    
	    // do we got an option to run, lets do it
	    if ((*selectedOption)->getState() == MenuOption::Run){
		   try{
			/*if (backSound != ""){
			    Sound * ok = Resource::getSound(okSound);
			    ok->play();
			}*/
			(*selectedOption)->run(endGame);
			characterSelect->run((*selectedOption)->getText(), 2, true, work);
		    } catch ( const ReturnException & re ){
		    }
		    // Reset it's state
		    (*selectedOption)->setState(MenuOption::Selected);
		    /*if ( !music.empty() ){
			    MenuGlobals::setMusic(music);
		    }
		    if ( !selectSound.empty() ){
			    MenuGlobals::setSelectSound(selectSound);
		    }*/
		    
		    // reset the fade state
		    fader.setState(FADEIN);
	    }
/*
	    if (!music.empty()){
		    if(MenuGlobals::currentMusic() != music){
			    MenuGlobals::popMusic();
		    }
	    }
	    
	    if (!selectSound.empty()){
		    if(MenuGlobals::currentSelectSound() != selectSound){
			    MenuGlobals::popSelectSound();
		    }
	    }
*/
	    if (endGame){
		    // Deselect selected entry
		    (*selectedOption)->setState(MenuOption::Deselected);
		    /*if (backSound != ""){
			Sound * back = Resource::getSound(backSound);
			back->play();
		    }*/
	    }
    }
    
    return;
}


void MugenMenu::cleanup(){
    
    //Backgrounds
    if (background) delete background;
    
    // Character select
    if (characterSelect) delete characterSelect;
    
    // Get rid of sprites
    cleanupSprites();
}

void MugenMenu::cleanupSprites(){
    // Get rid of sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
}

// Draw text
void MugenMenu::drawText(Bitmap *bmp){
    
    int xplacement = position.x;
    int yplacement = position.y;
    int visibleCounter = 0;
    int offset = optionLocation >= windowVisibleItems ? optionLocation - windowVisibleItems + 1 : 0;
    for( std::vector <MenuOption *>::iterator i = menuOptions.begin() + offset; i != menuOptions.end(); ++i){
	MenuOption *option = *i;
	if (option->getState() == MenuOption::Selected){
	    MugenFont *font = fonts[fontActive.index-1];
	    if(showBoxCursor){
		boxCursorCoords.alpha += boxCursorCoords.alphaMove;
		if (boxCursorCoords.alpha <= 0){
		    boxCursorCoords.alpha = 0;
		    boxCursorCoords.alphaMove = 6;
		}
		else if (boxCursorCoords.alpha >= 128){
		    boxCursorCoords.alpha = 128;
		    boxCursorCoords.alphaMove = -6;
		}
		Bitmap::drawingMode(Bitmap::MODE_TRANS);
		Bitmap::transBlender(0,0,0,boxCursorCoords.alpha);
		bmp->rectangleFill(xplacement + boxCursorCoords.x1, yplacement + boxCursorCoords.y1, xplacement + boxCursorCoords.x2,yplacement + boxCursorCoords.y2,Bitmap::makeColor(255,255,255));
		Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    }
	    font->render(xplacement, yplacement, fontActive.position, fontActive.bank, *bmp, option->getText());
	    xplacement += fontSpacing.x;
	    yplacement += fontSpacing.y;
	} else {
	    MugenFont *font = fonts[fontItem.index-1];
	    font->render(xplacement, yplacement, fontItem.position, fontItem.bank, *bmp, option->getText());
	    xplacement += fontSpacing.x;
	    yplacement += fontSpacing.y;
	}
	
	// Visible counter
	visibleCounter++;
	if (visibleCounter >= windowVisibleItems)break;
    }
}


