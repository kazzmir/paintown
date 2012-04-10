#ifndef _mugen_menu_h
#define _mugen_menu_h

#include <vector>
#include <map>

#include "exception.h"
#include "util/gui/fadetool.h"
#include "util/gui/scroll-list.h"
#include "util/language-string.h"
#include "search.h"

/*
#include "menu/menu.h"
#include "menu/menu_option.h"
*/

#include "util.h"

/*
 * We already have a setup for menus, might as well as utilize it to accomodate mugen
 */

class MugenAnimation;
class MugenSection;

namespace Mugen{
    class Font;
}

namespace Ast{
    class Section;
}

struct MugenMenuArea{
    int x1;
    int y1;
    int x2;
    int y2;
    int alpha;
    int alphaMove;
    bool visible;
};

class CursorHandler{
    public:
	CursorHandler();
	virtual ~CursorHandler();
	
	// Handles cursor box blinking
	virtual void act();
	virtual void renderText(int x, int y, bool active, const std::string & text, std::vector<Mugen::Font *> &, const Graphics::Bitmap &);
	int getFontHeight(std::vector<Mugen::Font *> & fonts);
	virtual inline void setCursor(int x1, int y1, int x2, int y2){
	    this->cursor.x1 = x1;
	    this->cursor.y1 = y1;
	    this->cursor.x2 = x2;
	    this->cursor.y2 = y2;
	}
	
	virtual int getCursorHeight() const;
	
	virtual inline void setCursorVisible(bool visible){
	    this->cursor.visible = visible;
	}
	
	virtual inline void setItemFont(int index, int bank, int position){
	    this->itemFont.index = index;
	    this->itemFont.position = position;
	    this->itemFont.bank = bank;
	}
	
	virtual inline void setActiveFont(int index, int bank, int position){
	    this->activeFont.index = index;
	    this->activeFont.position = position;
	    this->activeFont.bank = bank;
	}
	
    private:
	virtual void renderCursor(int x, int y, const Graphics::Bitmap &);
	MugenMenuArea cursor;
	Mugen::FontInfo itemFont;
	Mugen::FontInfo activeFont;
};

namespace NewMenu{
    class Context;
}

namespace Gui{
    class Context;
}

namespace Mugen{
    class Font;
    class Sprite;
    class Background;
    class Character;
    class CharacterSelect;
    class Storyboard;
    
class ItemOption{
public:
	ItemOption();
	virtual ~ItemOption();
	
	virtual void logic();
	
	virtual void render(int x, int y, CursorHandler &, std::vector<Font *> &, const Graphics::Bitmap &);
	
	virtual void executeOption(const Mugen::PlayerType &, bool & endGame)=0;
 
    enum state{
        Selected = 0,
        Deselected,
        Run
    };
    
    inline void setText(const LanguageString & t) { text = t; }
    virtual inline std::string getText() const { return text.get(); }
    
    inline void setState(const state s) { currentState = s; }
    inline state getState() const { return currentState; }

    inline void setRunnable(const bool r) { runnable = r; }
    inline bool isRunnable() const { return runnable; }

protected:

    state currentState;
    LanguageString text;
    bool runnable;
};

}

class MugenStage;

class MugenMenu {
    public:
	/*! ctor dtor */
        explicit MugenMenu(const MugenMenu & menu);
	MugenMenu(const Filesystem::RelativePath &filename);
	virtual ~MugenMenu();
	
	/*! load */
	virtual void loadData();
	
	/*! do logic, draw whatever */
	virtual void run();
	
        virtual inline Filesystem::AbsolutePath getSelectInfoFile() const {
            return selectInfoFile;
        }
        
        inline void setName(const std::string & name){
            this->name = name;
        }
        
        inline const std::string & getName() const {
            return this->name;
        }
	
    private:
        enum SoundType{
            Move,
            Cancel,
            Done
        };

        void playSound(SoundType sound);
        void playSound(int group, int item);
        
        std::string name;
        
        // Add option
	void addMenuOption(Mugen::ItemOption * option);

	// Option offset
	int optionLocation;
	
        Filesystem::RelativePath location;
	
	//spr = system.sff          ;Filename of sprite data
	std::string spriteFile;
	// snd = system.snd          ;Filename of sound data
	std::string soundFile;
	//logo.storyboard =         ;Logo storyboard definition (optional)
	std::string logoFile;
	//intro.storyboard =        ;Intro storyboard definition (optional)
	std::string introFile;
        std::string introMusic;
	//select = select.def       ;Character and stage selection list
	std::string selectFile;
	//fight = fight.def         ;Fight definition filename
	std::string fightFile;
	
	/*! Fonts */
	std::vector<Mugen::Font *>fonts;
	
	/*! Music files */
	std::string titleMusic;
	// Loop?
	std::string selectMusic;
	std::string versusMusic;
	
	/*! Fade tool */
	Gui::FadeTool fader;
	
	/*! Menu Position */
	Mugen::Point position;
        
        /*! Current menu position for smooth scrolling */
        int currentMenuPosition;

        /*! Y Menu range positioning x = top y = bottom */
        Mugen::Point menuRange;
	
	/*! Limit speed on scroll */
	int menuScrollWait;
	
	/*! Box cursor and fonts */
	CursorHandler fontCursor;
	
	/*! Fonts spacing */
	Mugen::Point fontSpacing;
	
	/*
	; These parameters define the window in which the items are visible
	; in.*/
	//menu.window.margins.y = 12, 8 I can only assume there is an X version of this as well
	Mugen::Point windowMargin;
	//menu.window.visibleitems = 5
	int windowVisibleItems;
	//; These are the sounds for cursor movement (will be implemented later)
	//cursor.move.snd = 100,0
	//cursor.done.snd = 100,1
	//cancel.snd = 100,2
	
	// Our ticker/frames during duration of title
	int ticker;
	
	/* Sprites */
	Mugen::SpriteMap sprites;

        /*! Sounds */
        Mugen::SoundMap sounds;

        Mugen::Point moveSound;
        Mugen::Point doneSound;
        Mugen::Point cancelSound;
	
	// Title background
	Mugen::Background *background;
	
	// Logo
	Mugen::Storyboard *logo;
	
	// Intro
	Mugen::Storyboard *intro;
	
	// Select Info
	Mugen::CharacterSelect *characterSelect;
	
	// Cleanup
	void cleanup();
	
	// Cleanup sprites
	void cleanupSprites();

        // Move menu up
        void moveMenuUp();

        // Move menu down
        void moveMenuDown();

        // Handle movement
        void doMenuMovement();
	
	// Draw text
	void renderText(Graphics::Bitmap *bmp);
	
	// Move text (for opening sequence where text scrolls into place from the sides)
	bool moveText;

        // bool doInput(InputMap<Mugen::Keys> & input, Mugen::PlayerType & chosenPlayer, Mugen::PlayerType type);
	
        /* FIXME: don't use Point, make up a new object so we can call the
         * data 'left' and 'right' instead of 'x' and 'y'
         */
	// side position x left y right
	Mugen::Point movePosition;

        Filesystem::AbsolutePath selectInfoFile;
	
	// Options
	std::vector<Mugen::ItemOption *> options;
	
	std::vector<Mugen::ItemOption *>::iterator currentOption;
};

namespace Mugen{
    /*! ScrollAction options.h */
    class ScrollAction;
    
    /*! Mugen Menu */
    class Menu{
    public:
        Menu(const Filesystem::RelativePath &);
        //! For Sub menus
        Menu(const Menu &, const std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > &);
        
        virtual ~Menu();
        
        virtual void act();
        virtual void draw(const Graphics::Bitmap &);
    
        virtual void up();
        virtual void down();
        virtual void enter();
        virtual void cancel();
        virtual bool isDone();
        
    protected:
        //! Name of menu
        std::string name;
        
        //! List position
        int x;
        int y;
        
        //! List
        PaintownUtil::ReferenceCount<ScrollAction> list;
        
        //! List items
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > listItems;
        
        //! Background
        PaintownUtil::ReferenceCount<Background> background;
        
        //! Sound types
        enum Sounds{
            Move,
            Done,
            Cancel,
        };
        
        /*! Sprites */
        Mugen::SpriteMap sprites;
        
        //! Sound system
        Mugen::SoundSystem<Sounds> sounds;
        
        /*! Music files */
        std::string introMusic;
        std::string titleMusic;
        std::string selectMusic;
        std::string versusMusic;
        
        //! Fonts
        std::vector< PaintownUtil::ReferenceCount<Font> > fonts;
        
        //! Logo storyboard
        PaintownUtil::ReferenceCount<Mugen::Storyboard> logo;
        
        //! Intro storyboard
        PaintownUtil::ReferenceCount<Mugen::Storyboard> intro;
        
        //! Fade tool
        Gui::FadeTool fader;
        
        //! Fade enabled
        bool fadeEnabled;
        
        //! Menu state
        enum State{
            Intros,
            MainMenu,
            DemoMode,
        };
        
        //! Current menu state
        State state;
        
        //! Ticks (movement or action will reset ticks to 0)
        int ticks;
        
        //! Demo mode
        bool demoEnabled;
        
        //! Time till demo
        int startDemoTime;
        
        //! Demo cycles
        int demoCycles;
        
        //! Max cycles of demo before displaying next intro
        int nextIntroCycle;
    };

    /*! run the mugen menu */
    void run();
}

#endif
