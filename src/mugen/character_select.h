#ifndef _mugen_character_select_h
#define _mugen_character_select_h

#include <vector>
#include <map>

#include "mugen_exception.h"
#include "mugen_fadetool.h"

#include "menu/menu.h"

#include "input/input-map.h"

#include "mugen_util.h"
#include "return_exception.h"

#include "ast/all.h"

/*
 * Character Select Screen
 */

class MugenAnimation;
class MugenFont;
class MugenSprite;
class MugenStoryboard;
class MugenSection;

namespace Ast{
    class Section;
}

class MugenStage;

/* Encapsulate in Mugen namespace */
namespace Mugen{
    
class Character;
class Background;

/*! Font Handling */
class FontHandler{
    public:
	FontHandler();
	virtual ~FontHandler();
	
	void act();
	void render(const std::string &text, const Bitmap &);
	
	enum State{
	    Normal,
	    Blink,
	    Done
	};
	
	virtual inline void setLocation(int x, int y){
	    location.x = x;
	    location.y = y;
	}
	
	virtual inline void setState(const State &state){
	    this->state = state;
	}
	
	virtual inline const State & getState() const {
	    return state;
	}
	
	virtual inline void setBlinkTime(int time){
	    this->blinkTime = time;
	}
	
	virtual inline void setPrimary(MugenFont *font, int bank, int position){
	    this->font = font;
	    this->bank = bank;
	    this->position = position;
	}
	virtual inline void setBlink(MugenFont *font, int bank, int position){
	    this->state = Blink;
	    this->blinkFont = font;
	    this->blinkBank = bank;
	    this->blinkPosition = position;
	}
	virtual inline void setDone(MugenFont *font, int bank, int position){
	    this->doneFont = font;
	    this->doneBank = bank;
	    this->donePosition = position;
	}
    private:
	//! Current state
	State state;
	//! Position
	Mugen::Point location;
	//! primary font
	MugenFont *font;
	int bank;
	int position;
	//! secondary font for blinking
	MugenFont *blinkFont;
	int blinkBank;
	int blinkPosition;
	//! third font for when completed
	MugenFont *doneFont;
	int doneBank;
	int donePosition;
	//! ticker for font switching when blinking
	int ticker;
	//! blink time defaults to 10
	int blinkTime;
	//! Blink setting
	State blinkState;
};

/* Forward declaration of Cell */
class Cell;

/*! Character Info handler, portrait name and etc */
class CharacterInfo {
    public:
        CharacterInfo(const std::string &definitionFile);
        virtual ~CharacterInfo();
	
	virtual inline bool operator==(CharacterInfo &character){
	    return (this->definitionFile.compare(character.definitionFile) == 0);
	}
	
	virtual inline const std::string &getDefinitionFile(){
	    return this->definitionFile;
	}

        virtual inline MugenSprite * getIcon() const{
            return icon;
        }

        virtual inline MugenSprite * getPortrait() const{
            return portrait;
        }

        virtual inline const std::string & getName() const{
            return name;
        }

        virtual inline const std::string & getDisplayName() const{
            return displayName;
        }

        virtual void setAct(int index);

        virtual inline void setRandomStage(bool r){
            randomStage = r;
        }

        virtual inline const bool hasRandomStage() const{
            return randomStage;
        }

        virtual inline void setStage(const std::string &str){
            stageFile = str;
        }

        virtual inline const std::string &getStage() const{
            return stageFile;
        }

        virtual inline void setMusic(const std::string &str){
            music = str;
        }

        virtual inline const std::string &getMusic() const{
            return music;
        }

        virtual inline void setOrder(int o){
            order = o;
        }

        virtual inline const int getOrder() const{
            return order;
        }
	
	virtual inline void setReferenceCell(Cell *cell){
	    this->referenceCell = cell;
	}
	
	virtual inline Cell * getReferenceCell() {
	    return this->referenceCell;
	}

    private:
        /* The characters definition File to pass on to stage or anything else */
        const std::string definitionFile;
        /* Character base directory */
        const std::string baseDirectory;
        /* Sprite File */
        std::string spriteFile;
        /* Characters Name */
        std::string name;
        /* Characters Display Name */
        std::string displayName;
        /* Current Act */
        int currentAct;
        /* Act Collection */
        std::vector< std::string > actCollection;
        /* image 9000 */
        MugenSprite *icon;
        /* image 9001 */
        MugenSprite *portrait;
        /* Random Stage */
        bool randomStage;
        /* Stage File */
        std::string stageFile;
        /* Music for stage */
        std::string music;
        /* Order in which to be set during Arcade mode */
        int order;
	
	//! Reference Cell mainly for random so that we can light it up when it selected
	Cell *referenceCell;
};

/*! Stage handler */
class StageHandler{
    public:
	StageHandler();
	virtual ~StageHandler();
	
	virtual void act();
	virtual void render(const Bitmap &);
	
	//! Get current selected stage
	virtual const std::string &getStage();
	
	//! Set Next Stage
	virtual void next();
	
	//! Set Prev Stage
	virtual void prev();
	
	//! Add stage to list
	virtual void addStage(const std::string &stage);
	
	//! Get font handler
	virtual inline FontHandler & getFontHandler() {
	    return this->font;
	}
	
	virtual inline void setDisplay(bool display){
	    this->display = display;
	}
	
	//! Is Selecting?
	virtual inline bool isSelecting(){
	    return selecting;
	}
	
	//! Set Selection
	virtual inline void toggleSelecting(){
	    selecting = !selecting;
	    if (!selecting) {
		font.setState(font.Done);
	    }
	}
	
    private:
	//! Font handler
	FontHandler font;
	
	//! Current stage
	unsigned int currentStage;
	
	//! Stage list First stage is reserved for random
	std::vector< std::string > stages;
	
	//! Actual Stage names first is reserved for random
	std::vector< std::string > stageNames;
	
	//! Display
	bool display;
	
	//! Selection still active?
	bool selecting;
};

/* Handle an individual cell which contains the data required to render itself */
class Cell{
    public:
        Cell(int x, int y);
        virtual ~Cell();

        virtual void act(std::vector<CharacterInfo *> &characters);
        virtual void render(const Bitmap &);
	
	virtual inline const bool operator==(const Cell &cell) const{
	    return (this->location == cell.location);
	}
	
	virtual inline const Mugen::Point &getLocation() const{
	    return this->location;
	}
	
	virtual inline void setBackground(MugenSprite *sprite){
	    this->background = sprite;
	}
	
	virtual inline void setCharacter(CharacterInfo *character){
	    this->empty = false;
	    this->character = character;
	}
	
	virtual inline CharacterInfo *getCharacter(){
	    return this->character;
	}
	
	virtual inline void setRandomSprite(MugenSprite *random){
	    randomSprite = random;
	}
        
        virtual inline void setPosition(int x, int y){
            position.x = x;
            position.y = y;
        }
	
	virtual inline void setDimensions(int x, int y){
	    dimensions.x = x;
	    dimensions.y = y;
	}
	
	virtual inline const Mugen::Point &getPosition() const {
	    return this->position;
	}
	
        virtual inline void setRandom(bool r){
	    empty = false;
            random = r;
        }
	virtual inline bool isRandom() const {
	    return random;
	}
        virtual inline void setEmpty(bool e){
            empty = e;
        }
	virtual inline bool isEmpty() const {
	    return empty;
	}
	virtual inline void setCharacterOffset(int x, int y){
	    this->characterOffset.x = x;
	    this->characterOffset.y = y;
	}
	virtual inline void setCharacterScale(double x, double y){
	    this->characterScaleX = x;
	    this->characterScaleY = y;
	}
	
	virtual inline void startFlash(){
	    flash = 10;
	}
	
	enum CursorState{
	    None,
	    One,
	    Two,
	};
	
	virtual inline void setCursorState(const CursorState &cursors){
	    this->cursors = cursors;
	}
	
	virtual inline void popCursor() {
	    switch (cursors){
		case One:
		    cursors = None;
		    break;
		case Two:
		    cursors = One;
		    break;
		case None:
		default:
		    break;
	    }
	}
	
	virtual inline void pushCursor() {
	     switch (cursors){
		case None:
		    cursors = One;
		    break;
		case One:
		    cursors = Two;
		    break;
		case Two:
		default:
		    break;
	    }
	}
	
	virtual inline const CursorState & getCursorState() const{
	    return cursors;
	}
	
    private:
	//! Location on grid
	const Mugen::Point location;
	
	//! Set sprite background
	MugenSprite *background;
	
	//! Set Character
        CharacterInfo *character;
	
	//! Set Random Sprite
	MugenSprite *randomSprite;
	
	//! Position of this cell
        Mugen::Point position;
	
	//! Width and height
	Mugen::Point dimensions;
	
	//! Is this a random select cell
        bool random;
	
	//! Is this cell empty
        bool empty;
	
	//! Offset in which the portrait is placed in the cell
	Mugen::Point characterOffset;
	
	//! Scale values of the portrait placed in the cell
	double characterScaleX;
	double characterScaleY;
	
	//! Flashes cell, when selected
	int flash;
	
	//! Cursor state
	CursorState cursors;

};

/* Cell map */
typedef std::vector< std::vector< Cell * > > CellMap;

/* forward declare Cursor */
class Cursor;

/* Handles the select screen grid */
class Grid{
    public:
	Grid();
	virtual ~Grid();
	
	virtual void initialize();

        virtual void act();
	
	virtual void render(const Bitmap &);
	
	virtual void addCharacter(CharacterInfo *character, bool isRandom = false);
	
	virtual void setCursorStart(Cursor &cursor);
	
	virtual void moveCursorLeft(Cursor & cursor);
	virtual void moveCursorRight(Cursor & cursor);
	virtual void moveCursorUp(Cursor & cursor);
	virtual void moveCursorDown(Cursor & cursor);
	virtual void selectCell(Cursor &cursor, const Mugen::Keys & key);
	virtual void selectStage();
        
        virtual inline void setRows(int r){
	    this->rows = r;
        }
        virtual inline void setColumns(int c){
	    this->columns = c;
        }
        virtual inline void setWrapping(bool w){
	    this->wrapping = w;
        }
        virtual inline void setPosition(int x, int y){
	    this->position.x = x;
	    this->position.y = y;
        }
        virtual inline void setShowEmptyBoxes(bool s){
	    this->showEmptyBoxes = s;
        }
        virtual inline void setMoveOverEmptyBoxes(bool m){
	    this->moveOverEmptyBoxes = m;
        }
        virtual inline void setCellSize(int x, int y){
	    this->cellSize.x = x;
	    this->cellSize.y = y;
        }
        virtual inline void setCellSpacing(int c){
            this->cellSpacing = c;
        }
        virtual inline void setCellBackgroundSprite(MugenSprite *s){
            this->cellBackgroundSprite = s;
        }
        virtual inline void setCellRandomSprite(MugenSprite *s){
            this->cellRandomSprite = s;
        }
        virtual inline void setCellRandomSwitchTime(int t){
            this->cellRandomSwitchTime = t;
        }
	
	virtual inline void setPortraitOffset(int x, int y){
	    portraitOffset.x = x;
	    portraitOffset.y = y;
	}
	
	virtual inline void setPortraitScale(double x, double y){
	    this->portraitScaleX = x;
	    this->portraitScaleY = y;
	}
	
	virtual inline StageHandler & getStageHandler() {
	    return stages;
	}
	
	virtual inline void setGameType(const GameType &type){
	    this->type = type;
	}
    
    private:
	
	Cell *getCell(int row, int column) throw (MugenException);
	
	//! Total rows
	int rows;
	
	//! Total columns
	int columns;
	
	//! Allow a cursor to wrap?
	bool wrapping;
	
	//! Starting position of the grid
	Mugen::Point position;
	
	//! Show empty boxes?
	bool showEmptyBoxes;
	
	//! Allow cursor to move over empty boxes?
	bool moveOverEmptyBoxes;
	
	//! Size of cell
	Mugen::Point cellSize;
	
	//! Spacing between cells
	int cellSpacing;
	
	//! The background of the cell
	MugenSprite *cellBackgroundSprite;
	
	//! Random sprite portrait
	MugenSprite *cellRandomSprite;
	
	//! Random sprite switch time
	int cellRandomSwitchTime;
	
	//! Portrait offset for placement of the picture
	Mugen::Point portraitOffset;
	
	//! portrait scale for resizing if need be
	double portraitScaleX;
	double portraitScaleY;

        //! Character list 
        std::vector< CharacterInfo * > characters;
	
        //! Cells of the grid
        CellMap cells;
	
	//! Stages
	StageHandler stages;
	
	//! Game Type
	GameType type;
};

/*! Handles Team Menu */
class TeamMenu{
    public:
	TeamMenu();
	virtual ~TeamMenu();
	
	void act();
	void render(const Bitmap &);
	
    private:
	/*! Allow cursor to wrap up and down the menu */
	bool wrapping;
	/*! Menu Position */
	Mugen::Point position;
	/*! Background Sprite */
	MugenSprite *backgroundSprite;
	/*! Self title Font */
	FontHandler selfTitleFont;
	/*! Self title text */
	std::string selfTitleText;
	/*! Enemy title Font */
	FontHandler enemyTitleFont;
	/*! Enemy title text */
	std::string enemyTitleText;
	/*! Move sound */
	MugenSound *moveSound;
	/*! Value Sound (left <-> right ?) */
	MugenSound *valueSound;
	/*! Done Sound */
	MugenSound *doneSound;
	/*! Item offset */
	Mugen::Point itemOffset;
	/*! Item spacing */
	Mugen::Point itemSpacing;
	/*! itemFont */
	FontHandler itemFont;
	/* *TODO figure this out
	p1.teammenu.item.cursor.offset = -10, 0
	p1.teammenu.item.cursor.anim = 180
	*/ 
	/*! value Icon offset */
	Mugen::Point valueIconOffset;
	/*! value Icon Sprite */
	MugenSprite *valueIconSprite;
	/*! value empty Icon offset */
	Mugen::Point valueEmptyIconOffset;
	/*! value empty Icon sprite */
	MugenSprite *valueEmptyIconSprite;
	/*! value Icon spacing */
	Mugen::Point valueIconSpacing;
};

/*! Handles player cursors */
class Cursor{
    public:
	Cursor();
	virtual ~Cursor();
	
	virtual void act(Grid &grid);
	
	virtual void render(Grid &grid, const Bitmap &);
	
	virtual inline void setInput(const InputMap<Mugen::Keys> & input){
	    this->input = input;
	}
	
	virtual inline void setStart(int x, int y){
	    this->start.x = x;
	    this->start.y = y;
	}
	
	virtual inline const Mugen::Point &getStart() const {
	    return this->start;
	}
	
	virtual inline void setCurrentCell(Cell *cell){
	    this->currentCell = cell;
	}
	
	virtual Cell *getCurrentCell(){
	    return this->currentCell;
	}
	
	virtual inline void setActiveSprite(MugenSprite *spr){
	    this->activeSprite = spr;
	}
	
	virtual inline void setDoneSprite(MugenSprite *spr){
	    this->doneSprite = spr;
	}
	
	virtual inline void setBlink(bool b){
	    this->blink = b;
	}
	
	virtual inline void setBlinkRate(int b){
	    this->blinkRate = b;
	}
	
	virtual inline void setFaceOffset(int x, int y){
	    this->faceOffset.x = x;
	    this->faceOffset.y = y;
	}
	
	virtual inline void setFaceScale(double x, double y){
	    this->faceScaleX = x;
	    this->faceScaleY = y;
	}
	
	virtual inline void setFacing(int f){
	    this->facing = f;
	}
	
	virtual inline FontHandler & getFontHandler(){
	    return this->font;
	}
	
	enum State{
	    NotActive,
	    TeamSelect,
	    CharacterSelect,
	    StageSelect,
	    Done
	};
	
	virtual inline void setState(const State &state){
	    this->state = state;
	}
	
	virtual inline const State & getState() const {
	    return state;
	}
	
    private:
	
	void renderPortrait(const Bitmap &);
	
	InputMap<Mugen::Keys> input;
	//! Starting cell position in terms of row and column
	Mugen::Point start;
	
	//! Cell
	Cell *currentCell;
	
	//! Sprite to display while selecting
	MugenSprite *activeSprite;
	
	//! Sprite to display when selected
	MugenSprite *doneSprite;
	
	//! Blink when overlapping player1 ?
	bool blink;
	
	//! Our blink rate at 10
	int blinkRate;
	
	//! Blink counter
	int blinkCounter;
	
	//! is it Time to show
	bool hideForBlink;
	
	//! the placement of the portrait
	Mugen::Point faceOffset;
	
	//! Scale data of the portrait
	double faceScaleX;
	double faceScaleY;
	
	//! Which direction facing
	int facing;
	
	//! Font
	FontHandler font;
	
	//! Current state for proper handling of selection
	State state;
};

class VersusScreen {
    public:
	VersusScreen();
	virtual ~VersusScreen();
	
	virtual void render(CharacterInfo & player1, CharacterInfo & player2, const Bitmap &);
	
	virtual inline void setBackground(Background * background){
	    this->background = background;
	}
	
	virtual inline void setTime(int time){
	    this->time = time;
	}
	
	virtual inline MugenFadeTool & getFadeTool(){
	    return this->fader;
	}
	
	virtual inline void setPlayer1Position(const Mugen::Point & point){
	    this->player1Position = point;
	}
	
	virtual inline void setPlayer2Position(const Mugen::Point & point){
	    this->player2Position = point;
	}
	
	virtual inline void setPlayer1Facing(int facing){
	    this->player1Effects.facing = facing;
	}
	
	virtual inline void setPlayer2Facing(int facing){
	    this->player2Effects.facing = facing;
	}
	
	virtual inline void setPlayer1Scale(double x, double y){
	    this->player1Effects.scalex = x;
	    this->player1Effects.scaley = y;
	}
	
	virtual inline void setPlayer2Scale(double x, double y){
	    this->player2Effects.scalex = x;
	    this->player2Effects.scaley = y;
	}
	
	virtual inline FontHandler & getPlayer1Font(){
	    return this->player1Font;
	}
	
	virtual inline FontHandler & getPlayer2Font(){
	    return this->player2Font;
	}
    private:
	//! Background
	Background *background;
	//! Time to display
	int time;
	//! Fade tool
	MugenFadeTool fader;
	//! Player 1 portrait
	Mugen::Point player1Position;
	//! Player 1 effects (facing, scale)
	Mugen::Effects player1Effects;
	//! Player 2 portrait
	Mugen::Point player2Position;
	//! Player 2 effects (facing, scale)
	Mugen::Effects player2Effects;
	//! Player 1 font
	FontHandler player1Font;
	//! Player 2 font
	FontHandler player2Font;
};

class CharacterSelect {
    public:
        CharacterSelect(const std::string & file, const GameType &type);
        virtual ~CharacterSelect();
	
	virtual void load() throw (MugenException);
	
	virtual void run(const std::string & title, const Bitmap &);
	
	virtual void parseSelect(const std::string &selectFile);
	
	virtual void renderVersusScreen(const Bitmap &);
	
	virtual inline void setPlayer1Keys(const InputMap<Mugen::Keys> &input){
	    player1.setInput(input);
	}
	
	virtual inline void setPlayer2Keys(const InputMap<Mugen::Keys> &input){
	    player2.setInput(input);
	}
	
	/*! **FIXME These are temporary until a method is 
	    figured to handling teams and multiple players elegantly */
	virtual inline const std::string &getPlayer1(){
	    return player1.getCurrentCell()->getCharacter()->getDefinitionFile();
	}
	virtual inline const std::string &getPlayer2(){
	    return player2.getCurrentCell()->getCharacter()->getDefinitionFile();
	}
	virtual inline const std::string &getStage(){
	    return grid.getStageHandler().getStage();
	}
	
    private:
	
	/*! Temporary to accomodate above above condition */
	bool checkPlayerData();
	
	//! Location of file
	const std::string systemFile;
	
	//! sprite file
	std::string sffFile;
	
	//! music
	std::string sndFile;
	
	//! Select file
	std::string selectFile;
	
	//! Game Type
	GameType type;
	
	//! Fonts
	std::vector<MugenFont *>fonts;
	
	//! Fade tool
	MugenFadeTool fader;
	
	//! Grid (Cell container)
	Grid grid;
	
	//!Player 1 Cursor
	Cursor player1;
	
	//!Player 2 Cursor
	Cursor player2;
	
	//! Title font handler
	FontHandler titleFont;
	
	//! Characters
	std::vector< CharacterInfo *> characters;
	
	//! Sprites
	Mugen::SpriteMap sprites;
	
	//! Select background
	Mugen::Background *background;
	
	//! Versus Screen
	VersusScreen versus;
};

}

#endif


