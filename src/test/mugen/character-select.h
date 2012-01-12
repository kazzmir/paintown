#include <vector>

#include "mugen/background.h"
#include "mugen/util.h"
#include "mugen/sprite.h"
#include "mugen/font.h"

#include "util/pointer.h"
#include "util/gui/select-list.h"
#include "util/gui/fadetool.h"

namespace Mugen{
    
/*! Font */
class SelectFont{
public:
    SelectFont();
    SelectFont(PaintownUtil::ReferenceCount<MugenFont> font, int bank, int position);
    SelectFont(const SelectFont &);
    ~SelectFont();
    const SelectFont & operator=(const SelectFont &);
    void render(int x, int y, const std::string &, const Graphics::Bitmap &);
private:
    PaintownUtil::ReferenceCount<MugenFont> font;
    int bank;
    int position;
};
    
/*! Font Handling */
class FontHandler{
    public:
    FontHandler();
    virtual ~FontHandler();
    
    void act();
    //! Render with set text
    void render(const Graphics::Bitmap &);
    //! Override set text (For players names)
    void render(const std::string &, const Graphics::Bitmap &);
    
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
    
    virtual inline void setActive(const SelectFont & font){
        this->active = font;
    }
    virtual inline void setActive2(const SelectFont & font){
        this->active2 = font;
    }
    virtual inline void setDone(const SelectFont & font){
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
    SelectFont active;
    //! active font 2 for blinking
    SelectFont active2;
    //! done font for when completed
    SelectFont done;
    //! ticker for font switching when blinking
    int ticker;
    //! blink time defaults to 10
    int blinkTime;
};
    
class Cell : public Gui::SelectItem {
public:
    Cell(unsigned int index, const Gui::SelectListInterface *);
    virtual ~Cell();
    
    virtual void act();
    virtual void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const;
    virtual bool isEmpty() const;
    
    virtual void setRandom(bool r);
    
    virtual inline unsigned int getIndex() const {
        return index;
    }
    
    static void setBackground(PaintownUtil::ReferenceCount<MugenSprite> background);
    static void setRandomIcon(PaintownUtil::ReferenceCount<MugenSprite> randomIcon);
    static void setPlayer1ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor);
    static void setPlayer1DoneCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor);
    static void setPlayer2ActiveCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor);
    static void setPlayer2DoneCursor(PaintownUtil::ReferenceCount<MugenSprite> cursor);
    static inline void setBlinkCursor(bool blink){
        Cell::blinkCursor = blink;
    }
    
protected:
    static PaintownUtil::ReferenceCount<MugenSprite> background;
    static PaintownUtil::ReferenceCount<MugenSprite> randomIcon;
    static PaintownUtil::ReferenceCount<MugenSprite> player1ActiveCursor;
    static PaintownUtil::ReferenceCount<MugenSprite> player1DoneCursor;
    static PaintownUtil::ReferenceCount<MugenSprite> player2ActiveCursor;
    static PaintownUtil::ReferenceCount<MugenSprite> player2DoneCursor;
    
    static bool blinkCursor;
    static int blinkTime;
    
    void drawPlayer1Cursor(int x, int y, const Graphics::Bitmap &) const;
    void drawPlayer2Cursor(int x, int y, const Graphics::Bitmap &, bool blink=false) const;
    
    unsigned int index;
    const Gui::SelectListInterface * parent;
    bool empty;
    bool isRandom;
};

/*! Team Menu */
class TeamMenu{
public:
    //! Side 
    enum Side{
        Left,
        Right
    };
    TeamMenu(const Side &);
    virtual ~TeamMenu();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &, bool enemy = false);
    
    //! Up
    void up();
    //! Down
    void down();
    //! Left
    void left();
    //! Right
    void right();
    //! Fight types
    enum FightType{
        Single,
        Simultaneous,
        Turns2,
        Turns3,
        Turns4,
    };
    //! Get a return value for selected
    const FightType & select();
    
    //! Fonts
    FontHandler titleFont;
    FontHandler enemyTitleFont;
    FontHandler itemFont;
    FontHandler itemCurrentFont;
    
    virtual inline void setPosition(int x, int y){
        this->x = x;
        this->y = y;
    }
    virtual inline void setEnemyPosition(int x, int y){
        this->enemyX = x;
        this->enemyY = y;
    }
protected:
    //! Side this is on
    Side side;
    //! Current selection
    FightType current;
    int x, y;
    int enemyX, enemyY;
};

struct IndexValue{
    int group;
    int index;
};

class CharacterSelect{
public:
    CharacterSelect(const Filesystem::AbsolutePath &);
    virtual ~CharacterSelect();
    
    virtual void init();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    
    virtual void up(unsigned int cursor);
    virtual void down(unsigned int cursor);
    virtual void left(unsigned int cursor);
    virtual void right(unsigned int cursor);
    
    //! Sound types
    enum SoundType{
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
    virtual void setSound(const SoundType &, int group, int sound);
    
    //! Fonts
    FontHandler titleFont;
    FontHandler player1Font;
    FontHandler player2Font;
    FontHandler stageFont;
    
    //! Team menu data
    TeamMenu player1TeamMenu;
    TeamMenu player2TeamMenu;
    
protected:
    //! Get font
    PaintownUtil::ReferenceCount<MugenFont> getFont(int index) const;
    //! Path
    const Filesystem::AbsolutePath & file;
    //! Grid
    Gui::GridSelect grid;
    //! Grid dimensions
    int gridX, gridY;
    //! Grid draw point
    int gridPositionX, gridPositionY;
    //! Starting positions
    int player1Start, player2Start;
    //! Cells
    std::vector< PaintownUtil::ReferenceCount<Cell> > cells;
    //! Sprites
    Mugen::SpriteMap sprites;
    //! Sounds
    Mugen::SoundMap sounds;
    //! Sound lookup
    std::map<SoundType, IndexValue> soundLookup;
    //! Select file
    std::string selectFile;
    //! Fonts
    std::vector< PaintownUtil::ReferenceCount<MugenFont> > fonts;
    //! Fade tool
    Gui::FadeTool fader;
    //! Select background
    PaintownUtil::ReferenceCount<Mugen::Background> background;
};

}
