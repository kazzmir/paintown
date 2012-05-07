#ifndef mugen_character_select_h
#define mugen_character_select_h

#include <vector>

#include "background.h"
#include "util.h"
#include "sprite.h"
#include "font.h"
#include "search.h"

#include "util/events.h"
#include "util/pointer.h"
#include "util/gui/select-list.h"
#include "util/gui/fadetool.h"

#include "util/input/input-map.h"

namespace Mugen{
    
/*! Font Handling */
class FontHandler{
    public:
    FontHandler();
    virtual ~FontHandler();
    
    void act();
    //! Render with set text
    void draw(const Graphics::Bitmap &, int offset = 0);
    //! Override set text (For players names)
    void draw(const std::string &, const Graphics::Bitmap &, int offset = 0);
    
    enum State{
        Normal=0,
        Blink,
        Done
    };
    
    virtual inline void setText(const std::string & text){
        this->text = text;
    }
    
    virtual inline const std::string & getText() const {
        return this->text;
    }
    
    virtual inline void setLocation(int x, int y){
        this->x = x;
        this->y = y;
    }
    
    virtual inline void setState(const State & state){
        this->state = state;
    }
    
    virtual inline const State & getState() const {
        return state;
    }
    
    virtual inline void setBlinkTime(int time){
        this->blinkTime = time;
    }
    
    virtual inline void setActive(const FontSystem::Font & font){
        this->active = font;
    }
    virtual inline void setActive2(const FontSystem::Font & font){
        this->active2 = font;
    }
    virtual inline void setDone(const FontSystem::Font & font){
        this->done = font;
    }
private:
    //! Text
    std::string text;
    //! Current state
    State state;
    //! Position
    int x, y;
    //! active font
    FontSystem::Font active;
    //! active font 2 for blinking
    FontSystem::Font active2;
    //! done font for when completed
    FontSystem::Font done;
    //! ticker for font switching when blinking
    int ticker;
    //! blink time defaults to 10
    int blinkTime;
};
    
class Cell: public Gui::SelectItem {
public:
    Cell(unsigned int index, const Gui::SelectListInterface *);
    virtual ~Cell();
    
    virtual void act();
    virtual void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const ::Font &) const;
    virtual bool isEmpty() const;
    
    virtual void setCharacter(const Mugen::ArcadeData::CharacterInfo &);
    
    virtual void setRandom(bool r);
    
    virtual void select();
    
    virtual inline unsigned int getIndex() const {
        return index;
    }
    virtual inline bool getRandom() const {
        return this->isRandom;
    }
    virtual inline void setEmpty(bool empty){
        this->empty = empty;
    }
    virtual inline const Mugen::ArcadeData::CharacterInfo & getCharacter() const {
        return this->character;
    }
    
    static void setBackground(PaintownUtil::ReferenceCount<Sprite> background);
    static void setRandomIcon(PaintownUtil::ReferenceCount<Sprite> randomIcon);
    static void setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<Sprite> cursor);
    static void setPlayer1DoneCursor(PaintownUtil::ReferenceCount<Sprite> cursor);
    static void setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<Sprite> cursor);
    static void setPlayer2DoneCursor(PaintownUtil::ReferenceCount<Sprite> cursor);
    static inline void setBlinkCursor(bool blink){
        Cell::blinkCursor = blink;
    }
    static inline void setEffects(const Mugen::Effects & effects){
        Cell::effects = effects;
    }
    static inline void setOffset(int x, int y){
        Cell::offsetX = x;
        Cell::offsetY = y;
    }
    
protected:
    static PaintownUtil::ReferenceCount<Sprite> background;
    static PaintownUtil::ReferenceCount<Sprite> randomIcon;
    static PaintownUtil::ReferenceCount<Sprite> player1ActiveCursor;
    static PaintownUtil::ReferenceCount<Sprite> player1DoneCursor;
    static PaintownUtil::ReferenceCount<Sprite> player2ActiveCursor;
    static PaintownUtil::ReferenceCount<Sprite> player2DoneCursor;
    
    static bool blinkCursor;
    static int blinkTime;
    
    static Mugen::Effects effects;
    static int offsetX, offsetY;
    
    void drawPlayer1Cursor(int x, int y, const Graphics::Bitmap &) const;
    void drawPlayer2Cursor(int x, int y, const Graphics::Bitmap &, bool blink=false) const;
    
    unsigned int index;
    const Gui::SelectListInterface * parent;
    bool empty;
    bool isRandom;
    int flash;
    
    //! Character
    Mugen::ArcadeData::CharacterInfo character;
};

/*! Team Menu */
class TeamMenu{
public:
    TeamMenu();
    virtual ~TeamMenu();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &, bool enemy = false);
    
    //! Up
    bool up();
    //! Down
    bool down();
    //! Left
    bool left();
    //! Right
    bool right();
    //! Get a return value for selected
    const Mugen::ArcadeData::CharacterCollection::Type & select();
    
    //! Reset
    void reset();
    
    //! Fonts
    FontHandler titleFont;
    FontHandler enemyTitleFont;
    FontHandler itemFont;
    FontHandler itemCurrentFont;
    
    static inline void setWrapping(bool wrap){
        TeamMenu::wrapping = wrap;
    }
    virtual inline void setPosition(int x, int y){
        this->x = x;
        this->y = y;
    }
    virtual inline void setBackgroundSprite(PaintownUtil::ReferenceCount<Sprite> sprite){
        this->background = sprite;
    }
    virtual inline void setItemOffset(int x, int y){
        this->itemOffsetX = x;
        this->itemOffsetY = y;
    }
    virtual inline void setItemSpacing(int x, int y){
        this->itemSpacingX = x;
        this->itemSpacingY = y;
    }
    virtual inline void setValueIconOffset(int x, int y){
        this->valueIconOffsetX = x;
        this->valueIconOffsetY = y;
    }
    virtual inline void setValueIconSprite(PaintownUtil::ReferenceCount<Sprite> icon){
        this->icon = icon;
    }
    virtual inline void setEmptyValueIconOffset(int x, int y){
        this->emptyValueIconOffsetX = x;
        this->emptyValueIconOffsetY = y;
    }
    virtual inline void setEmptyValueIconSprite(PaintownUtil::ReferenceCount<Sprite> icon){
        this->emptyIcon = icon;
    }
    virtual inline void setValueSpacing(int x, int y){
        this->valueSpacingX = x;
        this->valueSpacingY = y;
    }
    virtual inline void setEnabled(bool enabled){
        this->enabled = enabled;
    }
    virtual inline bool getEnabled() const {
        return this->enabled;
    }
protected:
    //! Handle values (depending on value spacing)
    bool valueLess();
    bool valueMore();
    //! Current selection
    Mugen::ArcadeData::CharacterCollection::Type current;
    //! Current turns
    Mugen::ArcadeData::CharacterCollection::Type turns;
    //! Wrapping
    static bool wrapping;
    //! Position of menu
    int x, y;
    //! Background sprite
    PaintownUtil::ReferenceCount<Sprite> background;
    //! Item starting offset
    int itemOffsetX, itemOffsetY;
    //! Item spacing offset
    int itemSpacingX, itemSpacingY;
    //! Value icon offset
    int valueIconOffsetX, valueIconOffsetY;
    //! Value icon sprite
    PaintownUtil::ReferenceCount<Sprite> icon;
    //! Empty Value icon offset
    int emptyValueIconOffsetX, emptyValueIconOffsetY;
    //! Empty Value icon sprite
    PaintownUtil::ReferenceCount<Sprite> emptyIcon;
    //! Value spacing
    int valueSpacingX, valueSpacingY;
    //! Menu enabled?
    bool enabled;
};

class StageMenu{
public:
    StageMenu();
    virtual ~StageMenu();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    //! Add stage
    virtual void add(const Filesystem::AbsolutePath &);
    
    //! Up
    bool up();
    //! Down
    bool down();
    //! Left
    bool left();
    //! Right
    bool right();
    //! Get a return value for selected
    const Filesystem::AbsolutePath & select();
    //! Reset
    void reset();
    //! Get stage collection
    const std::vector<Filesystem::AbsolutePath> & getStages() const {
        return this->stages;
    }
    //! Set enabled
    virtual inline void setEnabled(bool enabled){
        this->enabled = enabled;
    }
    //! Get enabled
    virtual inline bool getEnabled() const {
        return this->enabled;
    }
    //! Finished
    virtual inline void finish(){
        this->finished = true;
        this->font.setState(FontHandler::Done);
    }
    virtual inline bool getFinished() const {
        return this->finished;
    }
    //! Font
    FontHandler font;
protected:
    //! Stages
    std::vector<Filesystem::AbsolutePath> stages;
    //! Stage Names
    std::vector<std::string> names;
    //! Random Stage ?
    bool random;
    //! Current Selected Stage
    unsigned int current;
    //! Is enabled
    bool enabled;
    //! Is finished
    bool finished;
    //! Thread lock
    PaintownUtil::Thread::LockObject lock;
};

//! Sound types
enum SelectSoundType{
    Player1Move=0,
    Player1Done,
    Player1Random,
    Player2Move,
    Player2Done,
    Player2Random,
    Player1TeamMove,
    Player1TeamValue,
    Player1TeamDone,
    Player2TeamMove,
    Player2TeamValue,
    Player2TeamDone,
    StageMove,
    StageDone,
    Cancel,
};

class Player{
public:
    Player(unsigned int cursor, Gui::GridSelect &, std::vector< PaintownUtil::ReferenceCount<Cell> > &, std::vector<Mugen::ArcadeData::CharacterInfo> &, TeamMenu &, TeamMenu &, StageMenu &, FontHandler &, FontHandler &, Mugen::SoundSystem<SelectSoundType> &);
    virtual ~Player();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    virtual void up();
    virtual void down();
    virtual void left();
    virtual void right();
    virtual void select(int act = 0);
    
    virtual void setPortraitEffects(const Mugen::Effects &, const Mugen::Effects &);
    
    void setCurrentGameType(const Mugen::GameType &);
    
    //! Rest game state
    void reset();
    
    //! Set cooperative positioning and information
    void setCooperativeData(const Player &);
    
    //! Get collection
    const Mugen::ArcadeData::CharacterCollection & getCollection() const {
        return this->collection;
    }
    
    //! Get opponent collection
    const Mugen::ArcadeData::CharacterCollection & getOpponentCollection() const {
        return this->opponentCollection;
    }
    
    static inline void setRandomSwitchTime(int time){
        Player::randomSwitchTime = time;
    }
    
    inline bool isFinished() const {
        return (selectState == Finished);
    }
    
    inline void setCursorPosition(int x){
        this->cursorPosition = x;
    }
    inline void setOpponentCursorPosition(int x){
        this->opponentCursorPosition = x;
    }
    inline void setPortraitOffset(int x, int y){
        this->portraitX = x;
        this->portraitY = y;
    }
    inline void setOpponentPortraitOffset(int x, int y){
        this->opponentPortraitX = x;
        this->opponentPortraitY = y;
    }
    
protected:
    const unsigned int cursor;
    Gui::GridSelect & grid;
    std::vector< PaintownUtil::ReferenceCount<Cell> > & cells;
    std::vector<Mugen::ArcadeData::CharacterInfo> & characters;
    TeamMenu & teamMenu;
    TeamMenu & opponentTeamMenu;
    StageMenu & stageMenu;
    FontHandler & font;
    FontHandler & opponentFont;
    Mugen::SoundSystem<SelectSoundType> & sounds;
    
    SelectSoundType moveSound, doneSound, randomSound, teamMoveSound, teamValueSound, teamDoneSound;
    
    //! Current GameType
    Mugen::GameType currentGameType;
    
    //! Get current cell
    const Mugen::ArcadeData::CharacterInfo & getCurrentCell();
    
    //! Next selection
    void next(int act = 0);
    
    //! Draw profile info
    void drawPortrait(const Mugen::ArcadeData::CharacterCollection &, const Mugen::Effects &, int x, int y, FontHandler &, const Graphics::Bitmap &);
    
    //! Start position
    int cursorPosition, opponentCursorPosition;
    
    //! Portrait offset
    int portraitX, portraitY, opponentPortraitX, opponentPortraitY;
    
    //! Portrait effects
    Mugen::Effects portraitEffects, opponentPortraitEffects;
    
    //! Random switch
    static int randomSwitchTime;
    
    //! switch time
    int switchTime;
    
    //! Current random
    unsigned int currentRandom;
    
    //! Player Collections
    Mugen::ArcadeData::CharacterCollection collection;
    Mugen::ArcadeData::CharacterCollection opponentCollection;
    
    enum SelectState{
        NotStarted,
        Team,
        OpponentTeam,
        Character,
        Opponent,
        Stage,
        WaitFinished,
        Finished,
    };
    //! Current State
    SelectState selectState;
    
};

class CharacterSelect{
public:
    CharacterSelect(const Filesystem::AbsolutePath &);
    virtual ~CharacterSelect();
    
    //! Initialize all data from select.def
    virtual void init();
    
    //! Cancel and fade out
    virtual void cancel();
    
    //! Is done
    virtual bool isDone();
    
    //! Act
    virtual void act();
    //! Draw
    virtual void draw(const Graphics::Bitmap &);
    
    enum PlayerType{
        Player1,
        Player2,
        Both,
        Demo,
    };
    //! Set Mode
    virtual void setMode(const Mugen::GameType &, const PlayerType & player = Player1);
    //! Move up
    virtual void up(unsigned int cursor);
    //! Move down
    virtual void down(unsigned int cursor);
    //! Move left
    virtual void left(unsigned int cursor);
    //! Move right
    virtual void right(unsigned int cursor);
    //! Make current selection
    virtual void select(unsigned int cursor, int act = 0);
    //! Add Character
    virtual bool addCharacter(const Mugen::ArcadeData::CharacterInfo &);
    //! Add Empty slot
    virtual void addEmpty();
    //! Make slot a random selector
    virtual void addRandom();
    //! Add stage
    virtual void addStage(const Filesystem::AbsolutePath &);
    //! Get Arcade Match
    virtual Mugen::ArcadeData::MatchPath getArcadePath();
    //! Get Team Arcade Match
    virtual Mugen::ArcadeData::MatchPath getTeamArcadePath();
    //! Get character list
    virtual const std::vector<Mugen::ArcadeData::CharacterInfo> & getCharacters() const;
    //! Get stage list
    const std::vector<Filesystem::AbsolutePath> & getStages() const;
    
    //! Get Logic class
    PaintownUtil::ReferenceCount<PaintownUtil::Logic> getLogic(InputMap<Mugen::Keys> &, InputMap<Mugen::Keys> &, Searcher &);
    
    //! Get Draw class
    PaintownUtil::ReferenceCount<PaintownUtil::Draw> getDraw();
    
    //! Get Player1
    inline const Player & getPlayer1() const {
        return this->player1;
    }
    
    //! Get Player2
    inline const Player & getPlayer2() const {
        return this->player2;
    }
    
    //! Get Selected stage
    inline const Filesystem::AbsolutePath & getStage() {
        return this->stages.select();
    }
    
    //! Was canceled
    inline bool wasCanceled() const {
        return this->canceled;
    }
    
    //! Current Player
    inline const PlayerType & getCurrentPlayer() const {
        return this->currentPlayer;
    }
    
    //! Override title
    inline void overrideTitle(const std::string & title){
        this->titleOverride = title;
    }
    
protected:
    /* Adds a stage without taking the lock */
    void doAddStage(const Filesystem::AbsolutePath & stage);

    //! Parse select file
    void parseSelect();
    //! Path
    const Filesystem::AbsolutePath file;
    //! Grid
    Gui::GridSelect grid;
    //! Grid dimensions
    int gridX, gridY;
    //! Grid draw point
    int gridPositionX, gridPositionY;
    //! Cells
    std::vector< PaintownUtil::ReferenceCount<Cell> > cells;
    //! Sprites
    Mugen::SpriteMap sprites;
    //! Sounds
    Mugen::SoundSystem<SelectSoundType> sounds;
    //! Select file
    Filesystem::AbsolutePath selectFile;
    //! Title override
    std::string titleOverride;
    //! Fonts
    //std::vector< PaintownUtil::ReferenceCount<Font> > fonts;
    Mugen::FontSystem fonts;
    
    //! Font Handlers
    FontHandler titleFont;
    FontHandler player1Font;
    FontHandler player2Font;
    
    //! Font Handlers for Team menu data
    TeamMenu player1TeamMenu;
    TeamMenu player2TeamMenu;
    
    //! Fade tool
    Gui::FadeTool fader;
    //! Select background
    PaintownUtil::ReferenceCount<Mugen::Background> background;
    //! Characters
    std::vector<Mugen::ArcadeData::CharacterInfo> characters;
    //! Current add Cell
    unsigned int nextCell;
    //! Stage menu
    StageMenu stages;
    //! Arcade Matches
    std::vector<int> arcadeOrder;
    std::vector<int> teamArcadeOrder;
    //! Current GameType
    Mugen::GameType currentGameType;
    //! Current players
    PlayerType currentPlayer;
    //! Players
    Player player1, player2;
    //! Canceled?
    bool canceled;
    //! Thread Lock
    PaintownUtil::Thread::LockObject lock;
    //! Demo moves left
    int demoLeftTime, demoRightTime;
    int demoLeftRemaining, demoRightRemaining;
};

}
#endif
