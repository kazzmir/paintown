#include "util/bitmap.h"
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
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"
#include "mugen_storyboard.h"

#include "mugen/option_versus.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;

using namespace std;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

MugenCharacterSelect::MugenCharacterSelect(const unsigned long int &ticker, std::vector<MugenFont *> &fonts):
cellBackgroundBitmap(0),
cellRandomBitmap(0),
selectTicker(ticker),
fonts(fonts),
characterList(0){
}
MugenCharacterSelect::~MugenCharacterSelect(){
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
    for (std::vector< std::vector< MugenCell *> >::iterator i = cells.begin(); i != cells.end(); ++i){
	std::vector< MugenCell *> &row = *i;
	for (std::vector< MugenCell *>::iterator c = row.begin(); c != row.end(); ++c){
	    MugenCell *cell = *c;
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
}

void MugenCharacterSelect::load(const std::string &selectFile, const std::vector<Ast::Section*> & sections, MugenSprites & sprites){

    for (vector<Ast::Section*>::const_iterator it = sections.begin(); it != sections.end(); it++){
        Ast::Section * section = *it;
        std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	Mugen::Util::fixCase(head);

	if (head == "select info"){ 
            class SelectInfoWalker: public Ast::Walker{
            public:
                SelectInfoWalker(MugenCharacterSelect & self, MugenSprites & sprites):
                    self(self),
                    sprites(sprites){
                    }

                MugenCharacterSelect & self;
                MugenSprites & sprites;

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

    fonts = fonts;
    // Set up cell table
    Mugen::Point currentPosition;
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
MugenSelectedChars *MugenCharacterSelect::run(const std::string &title, const int players, const bool selectStage, Bitmap *work){
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
    
    if (characterList){
	delete characterList;
    }
    characterList = new MugenSelectedChars();
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
		Util::rest( 1 );
		keyInputManager::update();
	}
    }
	if (selectStage && !stageInfo.selected)return 0;
	return characterList;
}

void MugenCharacterSelect::drawCursors(const int players, Bitmap *work){
    Mugen::Point currentPosition;
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
    std::string dir = Mugen::Util::getFileDir(selectFile);
    std::string file = Mugen::Util::stripDir(selectFile);
    /* FIXME!! Replace with peg parser */
    MugenReader reader( Mugen::Util::getCorrectFileLocation(dir,file) );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    std::vector< std::string > stageNames;
    /* Extract info for our first section of our menu */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	Mugen::Util::fixCase(head);
	if( head == "characters" ){
	    int row = 0;
	    int column = 0;
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		Mugen::Util::removeSpaces(itemhead);
		if (itemhead=="random"){
		    // set random flag
		    cells[row][column]->random = true;
		    cells[row][column]->empty = false;
		} else {
		    // Get character
                    Mugen::Character *character = new Mugen::Character(itemhead);
		    try{
			character->load();
		    } catch (const MugenException & ex){
                        throw ex;
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
		Mugen::Util::removeSpaces(itemhead);
		// Next item will be a stage lets add it to the list of stages
		stageNames.push_back(itemhead);
		Global::debug(1) << "Got stage: " << itemhead << endl;
	    }
	}
	else if( head == "options" ){ /* ignore for now */}
	else throw MugenException("Unhandled Section in '" + selectFile + "': " + head, __FILE__, __LINE__); 
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
intro(0),
characterSelect(0){
}

static vector<Ast::Section*> collectSelectStuff(Ast::AstParse::section_iterator & iterator, Ast::AstParse::section_iterator end){
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
        Mugen::Util::fixCase(sectionName);
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

void MugenMenu::loadData() throw (MugenException){
     // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Filesystem::find("mugen/data/" + Mugen::Util::getFileDir(location));
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(location) );
    // get real basedir
    //baseDir = Mugen::Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate menu definition file for: " + location );
    }

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;
    
    /*
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    */

    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	Mugen::Util::fixCase(head);
        if (head == "info"){
            class InfoWalker: public Ast::Walker{
            public:
                InfoWalker(MugenMenu & menu):
                menu(menu){
                }

                MugenMenu & menu;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "name"){
                        menu.setName(simple.valueAsString());
                        Global::debug(1) << "Read name '" << menu.getName() << "'" << endl;
                    } else if (simple == "author"){
                        string temp;
                        simple >> temp;
                        Global::debug(1) << "Made by: '" << temp << "'" << endl;
                    } else {
                        throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                    }
                }
            };

            InfoWalker walker(*this);
            section->walk(walker);
        } else if (head == "files"){
            class FileWalker: public Ast::Walker{
                public:
                    FileWalker(MugenMenu & menu, const string & baseDir):
                        menu(menu),
                        baseDir(baseDir){
                        }

                    MugenMenu & menu;
                    const string & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            simple >> menu.spriteFile;
                            Global::debug(1) << "Got Sprite File: '" << menu.spriteFile << "'" << endl;
                            Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(baseDir, menu.spriteFile), "", menu.sprites);
                        } else if (simple == "snd"){
                            simple >> menu.soundFile;
                            Global::debug(1) << "Got Sound File: '" << menu.soundFile << "'" << endl;
                        } else if (simple == "logo.storyboard"){
                            try{
                                simple >> menu.logoFile;
                                try{
                                    Global::debug(1) << "Logo file " << baseDir << "/" << menu.logoFile << endl;
                                    menu.logo = new MugenStoryboard(Mugen::Util::getCorrectFileLocation(baseDir, menu.logoFile));
                                    menu.logo->load();
                                    Global::debug(1) << "Got Logo Storyboard File: '" << menu.logoFile << "'" << endl;
                                } catch (const MugenException &ex){
                                    throw MugenException( "Error loading logo storyboard: " + ex.getReason(), __FILE__, __LINE__);
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "intro.storyboard"){
                            try{
                                simple >> menu.introFile;
                                try{
                                    Global::debug(1) << "Intro file " << baseDir << "/" << menu.introFile << endl;
                                    menu.intro = new MugenStoryboard(Mugen::Util::getCorrectFileLocation(baseDir, menu.logoFile));
                                    menu.intro->load();
                                    Global::debug(1) << "Got Intro Storyboard File: '" << menu.introFile << "'" << endl;
                                } catch (const MugenException &ex){
                                    throw MugenException( "Error loading intro storyboard: " + ex.getReason(), __FILE__, __LINE__);
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "select"){
                            simple >> menu.selectFile;
                            Global::debug(1) << "Got Select File: '" << menu.selectFile << "'" << endl;
                        } else if (simple == "fight"){
                            simple >> menu.fightFile;
                            Global::debug(1) << "Got Fight File: '" << menu.fightFile << "'" << endl;
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            string temp;
                            simple >> temp;
                            menu.fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;

                        } else {
                            throw MugenException("Unhandled option in Files Section: " + simple.toString(), __FILE__, __LINE__ );
                        }
                    }
            };
            
            FileWalker walker(*this, baseDir);
            section->walk(walker);
        } else if (head == "music"){
            /* FIXME! parse music here */
        } else if (head == "title info"){
            class TitleInfoWalker: public Ast::Walker{
            public:
                TitleInfoWalker(MugenMenu & menu):
                menu(menu){
                }

                MugenMenu & menu;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "fadein.time"){
                        int time;
                        simple >> time;
                        menu.fader.setFadeInTime(time);
                    } else if (simple == "fadein.color"){
                        int r,g,b;
                        simple >> r >> g >> b;
                        menu.fader.setFadeInColor(Bitmap::makeColor(r,g,b));
                    } else if (simple == "fadeout.time"){
                        int time;
                        simple >> time;
                        menu.fader.setFadeOutTime(time);
                    } else if (simple == "fadeout.color"){
                        int r,g,b;
                        simple >> r >> g >> b;
                        menu.fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
                    } else if (simple == "menu.pos"){
                        simple >> menu.position.x;
                        simple >> menu.position.y;
                    } else if (simple == "menu.item.font"){
                        simple >> menu.fontItem.index;
                        simple >> menu.fontItem.bank;
                        simple >> menu.fontItem.position;
                    } else if (simple == "menu.item.active.font"){
                        simple >> menu.fontActive.index;
                        simple >> menu.fontActive.bank;
                        simple >> menu.fontActive.position;
                    } else if (simple == "menu.item.spacing"){
                        simple >> menu.fontSpacing.x;
                        simple >> menu.fontSpacing.y;
		    } else if (simple == "menu.itemname.arcade"){
                        try{
                            menu.addOption(new OptionDummy(simple.valueAsString()));
                        } catch (const Ast::Exception & e){
                        }
                    } else if (simple == "menu.itemname.versus"){
                        try{
                            menu.addOption(new MugenOptionVersus(simple.valueAsString()));
                        } catch (const Ast::Exception & e){
                        }
		   } else if (simple == "menu.itemname.teamarcade"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.teamversus"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.teamcoop"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.survival"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.survivalcoop"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.training"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.watch"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.options"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.exit"){
                       try{
                           menu.addOption(new OptionQuit(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
                   } else if (simple == "menu.window.margins.x"){
                       simple >> menu.windowMarginX.x;
                       simple >> menu.windowMarginX.y;
                   } else if (simple == "menu.window.margins.y"){
                       simple >> menu.windowMarginY.x;
                       simple >> menu.windowMarginY.y;
                   } else if (simple == "menu.window.visibleitems"){
                       simple >> menu.windowVisibleItems;
                   } else if (simple == "menu.boxcursor.visible"){
                       simple >> menu.showBoxCursor;
                   } else if (simple == "menu.boxcursor.coords"){
                       simple >> menu.boxCursorCoords.x1;
                       simple >> menu.boxCursorCoords.y1;
                       simple >> menu.boxCursorCoords.x2;
                       simple >> menu.boxCursorCoords.y2;
                       menu.boxCursorCoords.alpha = 128;
                       menu.boxCursorCoords.alphaMove = -6;
                   } else if (simple == "cursor.move.snd"){
                       /* FIXME! parse cursor.move.snd */
                   } else if (simple == "cursor.done.snd"){
                       /* FIXME! parse cursor.done.snd */
                   } else if (simple == "cancel.snd"){
                       /* FIXME! parse cancel.snd */
                   } else {
                       throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                   }
                }
	    };
            
            TitleInfoWalker walker(*this);
            section->walk(walker);
        } else if (PaintownUtil::matchRegex(head, "^titlebgdef")){
            vector<Ast::Section*> backgroundStuff = Mugen::Util::collectBackgroundStuff(section_it, parsed.getSections()->end());
	    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir, backgroundStuff, ticker, &sprites);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
        } else if (head == "select info"){ 
	    // Pass off to selectInfo
	    characterSelect = new MugenCharacterSelect(ticker, fonts);
            characterSelect->load(baseDir + selectFile, collectSelectStuff(section_it, parsed.getSections()->end()), sprites);
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
    
    /* Set up the animations for those that have action numbers assigned (not -1 )
     * Also do their preload
     */
    if (background){
        background->preload(DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
    }
}

MugenMenu::~MugenMenu(){
    // cleanup
    cleanup();
}

void MugenMenu::run() throw (ReturnException) {
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
					    (*selectedOption)->setState(MenuOption::Deselected);
					    throw ReturnException();
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
			    (*selectedOption)->drawBelow(work);
			    // Draw text
			    renderText(&workArea);
			    // Foregrounds
			    background->renderFront(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
			    // Draw any misc stuff in the foreground of the menu of selected object 
			    (*selectedOption)->drawAbove(work);
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
			if (!endGame){
			    //characterSelect->run((*selectedOption)->getText(), 1, true, work);
			}
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
void MugenMenu::renderText(Bitmap *bmp){
    
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
