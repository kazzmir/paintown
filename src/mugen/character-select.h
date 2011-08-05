#ifndef _mugen_character_select_h
#define _mugen_character_select_h

#include <vector>
#include <deque>
#include <map>
#include <queue>

#include "exception.h"

#include "menu/menu.h"

#include "util/gui/fadetool.h"
#include "util/input/input-map.h"
#include "util/thread.h"

#include "util.h"

#include "ast/all.h"

namespace PaintownUtil = ::Util;

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

/* Encapsulate in Mugen namespace */
namespace Mugen{
    
class Background;
class Character;
class Stage;

/*! Font Handling */
class FontHandler{
    public:
	FontHandler();
	virtual ~FontHandler();
	
	void act();
	void render(const std::string &text, const Graphics::Bitmap &);
	
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

/* Character Info handler, portrait name and etc */
class CharacterInfo {
    public:
        CharacterInfo(const Filesystem::AbsolutePath &definitionFile);
        virtual ~CharacterInfo();
	
	virtual void loadPlayer1();
	virtual void loadPlayer2();
	
	virtual inline bool operator==(CharacterInfo &character){
	    return (this->definitionFile.path().compare(character.definitionFile.path()) == 0);
	}
	
	virtual inline const Filesystem::AbsolutePath & getDefinitionFile(){
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

        virtual void setPlayer1Act(int index);

        virtual inline int getPlayer1Act() const {
            return this->currentPlayer1Act;
        }

        virtual void setPlayer2Act(int index);

        virtual inline int getPlayer2Act() const {
            return this->currentPlayer2Act;
        }

        virtual inline void setRandomStage(bool r){
            randomStage = r;
        }

        virtual inline bool hasRandomStage() const{
            return randomStage;
        }

        virtual inline void setStage(const Filesystem::RelativePath &str){
            stageFile = str;
        }

        virtual inline const Filesystem::RelativePath &getStage() const{
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

        virtual inline int getOrder() const{
            return order;
        }
	
	virtual inline void setReferenceCell(Cell *cell){
	    this->referenceCell = cell;
	}
	
	virtual inline Cell * getReferenceCell() {
	    return this->referenceCell;
	}
	
	virtual inline Character *getPlayer1() {
	    return this->character1;
	}
	
	virtual inline Character *getPlayer2() {
	    return this->character2;
	}

    private:
        void cleanup();

    private:
        /* The characters definition File to pass on to stage or anything else */
        const Filesystem::AbsolutePath definitionFile;
        /* Character base directory */
        const Filesystem::AbsolutePath baseDirectory;
        /* Sprite File */
        Filesystem::RelativePath spriteFile;
        /* Characters Name */
        std::string name;
        /* Characters Display Name */
        std::string displayName;
        /* Current Act */
        int currentPlayer1Act;
        int currentPlayer2Act;
        /* Act Collection */
        std::vector<Filesystem::RelativePath> actCollection;
        /* image 9000 */
        MugenSprite *icon;
        /* image 9001 */
        MugenSprite *portrait;
        /* Random Stage */
        bool randomStage;
        /* Stage File */
        Filesystem::RelativePath stageFile;
        /* Music for stage */
        std::string music;
        /* Order in which to be set during Arcade mode */
        int order;
	
	//! Reference Cell mainly for random so that we can light it up when it selected
	Cell *referenceCell;
	
	//! Actual character for player 1
	Character *character1;
	//! Actual character for player 2
	Character *character2;
};

/*! Stage handler */
class StageHandler{
    public:
	StageHandler();
	virtual ~StageHandler();
	
	virtual void act();
	virtual void render(const Graphics::Bitmap &);
	
	//! Get current selected stage
	virtual const Filesystem::RelativePath &getStage();
	
	//! Get random stage
	virtual const Filesystem::RelativePath &getRandomStage();
	
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
	virtual void toggleSelecting();

        //! Set move sound
        virtual inline void setMoveSound(MugenSound * sound){
            this->moveSound = sound;
        }

        //! Set select sound
        virtual inline void setSelectSound(MugenSound * sound){
            this->selectSound = sound;
        }
	
    private:
	//! Font handler
	FontHandler font;
	
	//! Current stage
	unsigned int currentStage;
	
	//! Stage list First stage is reserved for random
	std::vector<Filesystem::RelativePath> stages;
	
	//! Actual Stage names first is reserved for random
	std::vector< std::string > stageNames;
	
	//! Display
	bool display;
	
	//! Selection still active?
	bool selecting;

        //! Move Sound
        MugenSound *moveSound;

        //! Select Sound
        MugenSound *selectSound;
};

/* Handle an individual cell which contains the data required to render itself */
class Cell{
    public:
        Cell(int x, int y);
        virtual ~Cell();

        virtual void act();
        virtual void randomize(std::vector<CharacterInfo *> &characters);
        virtual void render(const Graphics::Bitmap &);
	
	virtual inline bool operator==(const Cell &cell) const{
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

        virtual bool isBlank() const {
            return blank;
        }

        virtual void setBlank(bool what){
            empty = false;
            blank = what;
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

        bool blank;
	
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

        virtual void act(Cursor & player1, Cursor & player2);
	
	virtual void render(const Graphics::Bitmap &);
	
        virtual void addBlank();
	virtual void addCharacter(CharacterInfo *character, bool isRandom = false);
	
	virtual void setCursorPlayer1Start(Cursor &cursor);
	
	virtual void setCursorPlayer2Start(Cursor &cursor);
	
	virtual void setCursorStageSelect(Cursor &cursor);
	
        virtual void moveCursor(Cursor & cursor, int Point::* field, int wrap, int direction);
	virtual void moveCursorLeft(Cursor & cursor);
	virtual void moveCursorRight(Cursor & cursor);
	virtual void moveCursorUp(Cursor & cursor);
	virtual void moveCursorDown(Cursor & cursor);
	virtual void selectCell(Cursor &cursor, const Mugen::Keys & key, bool modifier);
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
	
	virtual inline void setPlayer1Start(int x, int y){
	    this->player1Start.set(x, y);
	}
	
	virtual inline void setPlayer2Start(int x, int y){
	    this->player2Start.set(x, y);
	}

        /* lock for safety. grab this if any changes will be made to the grid
         * itself or any cells.
         */
        virtual void lock();
        virtual void unlock();

        /* add the characterinfo to the grid, can be called from another thread */
        bool addInfo(CharacterInfo * character);
        /* true if no grid cell has the same characterinfo */
        bool isUniqueCharacter(CharacterInfo * character);
    
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

        //! Random switch time ticker
        int randomSwitchTimeTicker;
	
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
	
	//! Player1 starting position
	Mugen::Point player1Start;
	
	//! Player2 starting position
	Mugen::Point player2Start;

        /* traffic jam! */
        PaintownUtil::Thread::Lock gridLock;
};

/*! Handles Team Menu */
class TeamMenu{
    public:
	TeamMenu();
	virtual ~TeamMenu();
	
	void act();
	void render(const Graphics::Bitmap &);
	
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
	
	virtual void render(Grid &grid, const Graphics::Bitmap &);
	
	virtual inline void setInput(const InputMap<Mugen::Keys> & input){
	    this->input = input;
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

        virtual inline void setMoveSound(MugenSound * sound){
            this->moveSound = sound;
        }

        virtual void playMoveSound();

        virtual inline void setSelectSound(MugenSound * sound){
            this->selectSound = sound;
        }

        virtual void playSelectSound();

        virtual inline void setRandomSound(MugenSound * sound){
            this->randomSound = sound;
        }

        virtual void playRandomSound();
        
        virtual inline void setRandomCancel(bool cancel){
            this->cancelRandom = cancel;
        }

        virtual inline void setActSelection(const Mugen::Keys & key, bool modifier = false){
            this->actSelection = key;
            this->actModifier = modifier;
        }

        virtual int getActSelection();
	
    private:
	
	void renderPortrait(const Graphics::Bitmap &);
	
	InputMap<Mugen::Keys> input;
	
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

        //! Move Sound
        MugenSound *moveSound;

        //! Select Sound
        MugenSound *selectSound;

        //! Random Sound
        MugenSound *randomSound;

        //! Cancel Random
        bool cancelRandom;

        //! Character Key selection for determing act
        Mugen::Keys actSelection;

        //! Use modifier for selection
        bool actModifier;
};

/* Displays the two selected characters with some overlayed text right before
 * the battle begins. Players are loaded in the background while the animation
 * is playing.
 */
class VersusScreen {
    public:
	VersusScreen();
	virtual ~VersusScreen();
	
	//! Renders the versus screen as well as loads the characters and stage
	virtual void render(CharacterInfo & player1, CharacterInfo & player2, Mugen::Stage * stage);
	
	virtual inline void setBackground(Background * background){
	    this->background = background;
	}
	
	virtual inline void setTime(int time){
	    this->time = time;
	}
	
	virtual inline Gui::FadeTool & getFadeTool(){
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
        Gui::FadeTool fader;
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

/* runs the whole character selection process */
class CharacterSelect {
    public:
        CharacterSelect(const Filesystem::AbsolutePath & file, const PlayerType &, const GameType &);
        virtual ~CharacterSelect();
	
	virtual void load();
	
	virtual void run(const std::string & title);

        virtual void reset();
	
	//! This will load the character and stage so that you can retrieve them when setting up
	virtual void renderVersusScreen();
	
	//! Get next arcade match character returns false if there are no more characters
	virtual bool setNextArcadeMatch();
	
	//! Get next team match character returns false if there are no more characters
	virtual bool setNextTeamMatch();
	
	virtual inline void setPlayer1Keys(const InputMap<Mugen::Keys> &input){
	    player1Cursor.setInput(input);
	}
	
	virtual inline void setPlayer2Keys(const InputMap<Mugen::Keys> &input){
	    player2Cursor.setInput(input);
	}
	
	virtual inline Character * getPlayer1(){
	    return this->currentPlayer1->getPlayer1();
	}
	
	virtual inline const std::string & getPlayer1Music(){
	    return this->currentPlayer1->getMusic();
	}
	
	virtual inline Character * getPlayer2(){
	    return this->currentPlayer2->getPlayer2();
	}
	
	virtual inline const std::string & getPlayer2Music(){
	    return this->currentPlayer1->getMusic();
	}
	
	virtual inline Mugen::Stage * getStage(){
	    return this->currentStage;
	}
	
	virtual inline const Filesystem::AbsolutePath & getPlayer1Def(){
	    return currentPlayer1->getDefinitionFile();
	}

	virtual inline const Filesystem::AbsolutePath & getPlayer2Def(){
	    return currentPlayer2->getDefinitionFile();
	}
	
	/*! **FIXME These are temporary until a method is 
	    figured to handling teams and multiple players elegantly */
	virtual inline const Filesystem::RelativePath &getStageOld(){
	    return grid.getStageHandler().getStage();
	}

        virtual MugenFont * getFont(int index) const;
	
    private:
        /* searches the filesystem for more characters: arg is this */
        static void * searchForCharacters(void * arg);
        /* loads character info data and adds them to the grid: arg is this */
        static void * doAddCharacters(void * arg);
        /* returns false if no more characters can be added */
        bool maybeAddCharacter(const Filesystem::AbsolutePath & path);

        /* add all the files to the character add list */
        void addFiles(const std::vector<Filesystem::AbsolutePath> & files);

        bool addInfo(CharacterInfo * info);
        bool isUniqueCharacter(CharacterInfo * character);
	
	/*! Temporary to accomodate above above condition */
	bool checkPlayerData();
	
	//! Get group of characters by order number
	std::vector<CharacterInfo *> getCharacterGroup(int orderNumber);
	
	//! Parse select file to get characters and stages
	void parseSelect(const Filesystem::AbsolutePath &selectFile);
	
	//! Location of file
	const Filesystem::AbsolutePath systemFile;
	
	//! sprite file
	std::string sffFile;
	
	//! music
	std::string sndFile;
	
	//! Select file
	std::string selectFile;
	
	//! Game Type
	GameType gameType;
	
	//! Fonts
	std::vector<MugenFont *>fonts;
	
	//! Fade tool
        Gui::FadeTool fader;
	
	//! Grid (Cell container)
	Grid grid;
	
	//!Player 1 Cursor
	Cursor player1Cursor;
	
	//!Player 2 Cursor
	Cursor player2Cursor;
	
	//! Title font handler
	FontHandler titleFont;
	
	//! Characters
	std::vector< CharacterInfo *> characters;
	
	//! Sprites
	Mugen::SpriteMap sprites;

        //! Sounds
        Mugen::SoundMap sounds;

        //! Cancel sound
        MugenSound *cancelSound;
	
	//! Select background
	Mugen::Background *background;
	
	//! Versus Screen
	VersusScreen versus;
	
	//! Arcade matches
	std::queue< std::queue<CharacterInfo *> > arcadeMatches;
	
	//! Team matches
	std::queue< std::queue<CharacterInfo *> > teamMatches;
	
	//! Current set Player 1
	CharacterInfo *currentPlayer1;
	//! Current set Player 2
	CharacterInfo *currentPlayer2;
	//! Current set Stage
	Mugen::Stage *currentStage;
	
	//! PlayerType
	Mugen::PlayerType playerType;

        PaintownUtil::Thread::Id characterSearchThread;
        PaintownUtil::Thread::LockObject searchingLock;
        volatile bool quitSearching;
        PaintownUtil::ThreadBoolean searchingCheck;

        PaintownUtil::Thread::Id characterAddThread;
        PaintownUtil::Thread::LockObject addCharacterLock;
        // volatile bool addCharacterSignal;
        std::deque<Filesystem::AbsolutePath> addCharacters;
};

}

#endif
