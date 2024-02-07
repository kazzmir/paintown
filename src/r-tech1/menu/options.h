#ifndef _rtech1_options_h
#define _rtech1_options_h

#include "r-tech1/menu/menu_option.h"
#include "r-tech1/gui/animation.h"
#include "r-tech1/input/input-map.h"
#include "r-tech1/file-system.h"
#include "r-tech1/pointer.h"
#include "r-tech1/menu/font-info.h"

class Token;

namespace Menu{
    class OptionFactory;
}

/*! Handles key reconfiguration */
class OptionCredits: public MenuOption {
public:
    enum CreditKey{
        Exit
    };
    
    class Block{
    public:
        Block(const std::string &);
        Block(const Token *);
        Block(const Block &);
        ~Block();
        
        const Block & operator=(const Block &);
        void addCredit(const std::string &);
        
        void act();
        
        enum Justification{
            Left,
            Center,
            Right,
        };
        
        int print(int x, int y, Graphics::Color defaultTitleColor, Graphics::Color defaultColor, const Font &, const Graphics::Bitmap &, const Justification &) const;
        
        int size(const Font &) const;
        
        inline bool empty() const {
            return title.empty() && credits.empty();
        }
        
    protected:
        std::string title;
        std::vector<std::string> credits;
        bool titleColorOverride;
        Graphics::Color titleColor;
        bool colorOverride;
        Graphics::Color color;
        // Before title
        Util::ReferenceCount<Gui::Animation> topAnimation;
        int topWidth, topHeight;
        // After last credit
        Util::ReferenceCount<Gui::Animation> bottomAnimation;
        int bottomWidth, bottomHeight;
        // Spacing at bottom
        int spacing;
    };
    
    class Sequence {
    public:
        Sequence(const Token *);
        Sequence(const Sequence &);
        ~Sequence();
        
        const Sequence & operator=(const Sequence &);
        
        void act();
        void draw(Graphics::Color title, Graphics::Color color, const Graphics::Bitmap &);
        
        void reset();
        
        enum Type{
            Primary,
            Roll,
        };
        
        inline bool isDone() const {
            return this->done;
        }
        
    protected:
        // Next for primary
        void next();
        
        Type type;
        double x;
        double y;
        int startx;
        int endx;
        int starty;
        int endy;
        // Only used if both x's or y's are == to each other
        int ticks;
        int duration;
        double speed;
        int alpha;
        double alphaMultiplier;
        Block::Justification justification;
        std::vector<Block> credits;
        unsigned int current;
        bool done;
        int creditLength;
    };

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    OptionCredits(const Gui::ContextBox & parent, const Token *token);

    virtual ~OptionCredits();
private:
    Util::ReferenceCount<Menu::Context> creditsContext;
    
    std::vector<Sequence> sequences;
    std::string music;
    Graphics::Color color, title;
    InputMap<CreditKey> input;
    // Clear background (default black)
    Graphics::Color clearColor;
};

/*! Dummy option, to allow place fillers in menus */
class OptionDummy: public MenuOption{
public:
	OptionDummy(const Gui::ContextBox & parent, const Token *token);
	OptionDummy(const Gui::ContextBox & parent, const std::string &name);

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);

	virtual ~OptionDummy();
};

/*! Handles key reconfiguration */
class OptionFullscreen: public MenuOption{
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);
    
    virtual std::string getText() const;

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    OptionFullscreen(const Gui::ContextBox & parent, const Token *token);

    virtual ~OptionFullscreen();
private:

    /*
    int lblue, lgreen;
    int rblue, rgreen;
    */
};

/*! Handles joystick reconfiguration */
/*
class OptionJoystick: public MenuOption {
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    OptionJoystick(const Gui::ContextBox & parent, const Token *token);

    virtual ~OptionJoystick();

    //! keys
    enum JoystickType{
        Up = 0,
        Down,
        Left,
        Right,
        Jump,
        Attack1,
        Attack2,
        Attack3,
        Attack4,
        Attack5,
        Attack6,
        Invalidkey
    };

private:
    //! name
    std::string name;
    int player;

    JoystickType type;
    int keyCode;
};
*/

/*! Handles key reconfiguration */
class OptionKey: public MenuOption{
    public:
        // Do logic before run part
        virtual void logic();

        // Finally it has been selected, this is what shall run 
        // endGame will be set true if it is a terminating option
        virtual void run(const Menu::Context &);

        OptionKey(const Gui::ContextBox & parent, const Token *token);

        virtual ~OptionKey();

        //! keys
        enum keyType
        {
            up=0,
            down,
            left,
            right,
            jump,
            attack1,
            attack2,
            attack3,
            attack4,
            attack5,
            attack6,
            invalidkey
        };

    private:
        //! name
        std::string name;
        int player;

        keyType type;
        int keyCode;
};

/*! Handles key reconfiguration */
class OptionLevel: public MenuOption {
public:
	OptionLevel(const Gui::ContextBox & parent, const Token *token, int * set, int value);

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);

	virtual ~OptionLevel();

protected:
    /* integer to set if this option is chosen */
    int * set;
    int value;
};

namespace Menu {
    class Menu;
    class Context;
}

/*! Handles sub menus */
class OptionMenu: public MenuOption {
    public:
        // Do logic before run part
        virtual void logic();

        // Finally it has been selected, this is what shall run 
        // endGame will be set true if it is a terminating option
        virtual void run(const Menu::Context &);

        OptionMenu(const Gui::ContextBox & parent, const Token *token, const Menu::OptionFactory & factory);

        virtual ~OptionMenu();

    private:
        //Menu *menu;
        Menu::Menu *menu;
};

class OptionQuit : public MenuOption {
public:
	OptionQuit(const Gui::ContextBox & parent, const Token *token);
	OptionQuit(const Gui::ContextBox & parent, const std::string &name);

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);

	virtual ~OptionQuit();
};

/* return to previous menu */
class OptionReturn: public MenuOption {
public:
    OptionReturn(const Gui::ContextBox & parent, const Token * token);
    virtual void logic();
    virtual void run(const Menu::Context &);
    virtual ~OptionReturn();
};

/* Sets the quality filter used to stretch the screen (xbr/hqx) */
class OptionQualityFilter: public MenuOption {
public:
    OptionQualityFilter(const Gui::ContextBox & parent, const Token * token);
    virtual void logic();
    virtual void run(const Menu::Context &);
    std::string getText() const;
    virtual bool leftKey();
    virtual bool rightKey();
    virtual ~OptionQualityFilter();

protected:
    std::string filter;
};

/* Change the fps */
class OptionFps: public MenuOption {
public:
    OptionFps(const Gui::ContextBox & parent, const Token * token);
    virtual void logic();
    virtual void run(const Menu::Context &);
    std::string getText() const;
    virtual bool leftKey();
    virtual bool rightKey();
    virtual ~OptionFps();
};

/* continue the game */
class OptionContinue: public MenuOption {
public:
    OptionContinue(const Gui::ContextBox & parent, const Token * token);
    virtual void logic();
    virtual void run(const Menu::Context &);
    virtual ~OptionContinue();
};

struct ScreenSize{
    ScreenSize(int w, int h):w(w), h(h){}
    int w, h;
};

/*! Handles key reconfiguration */
class OptionScreenSize : public MenuOption {
public:
    OptionScreenSize(const Gui::ContextBox & parent, const Token *token);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionScreenSize();

protected:

    void setMode(int width, int height);
    int findMode(int width, int height);

private:
    // name
    std::string name;
    std::vector<ScreenSize> modes;

    int lblue, lgreen;
    int rblue, rgreen;
};

/*! Handles font selection */
class OptionSelectFont: public MenuOption {
public:
    OptionSelectFont(const Gui::ContextBox & parent, const Token *token);

    virtual void open();
    virtual void close();

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionSelectFont();

protected:

private:
    // Type of selector
    enum Adjust{
      fontName=0,
      fontHeight,
      fontWidth
    };
    // Current type of menu to adjust
    Adjust typeAdjust;
    
    int lblue, lgreen;
    int rblue, rgreen;
    
    std::vector<Util::ReferenceCount<Menu::FontInfo> > fonts;
    
    void nextIndex(bool forward);

};

/*! Handles key reconfiguration */
class OptionSpeed: public MenuOption {
	public:
		// Do logic before run part
		virtual void logic();
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(const Menu::Context &);
		
		// This is to pass paramaters to an option ie a bar or something
		virtual bool leftKey();
		virtual bool rightKey();

        virtual std::string getText() const;
		
		OptionSpeed(const Gui::ContextBox & parent, const Token *token);
	
		virtual ~OptionSpeed();
	private:
		// name
		std::string name;
		
        /*
		int lblue, lgreen;
		int rblue, rgreen;
        */
};

/*! Handles sub menus */
class OptionTabMenu: public MenuOption {
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    OptionTabMenu(const Gui::ContextBox & parent, const Token *token, const Menu::OptionFactory & factory);

    virtual ~OptionTabMenu();

private:
    Menu::Menu *menu;
};

class OptionSound: public MenuOption {
public:
    OptionSound(const Gui::ContextBox & parent, const Token *token);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    // This is to pass parameters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionSound();

protected:
    void changeSound(int much);

private:
    int lblue, lgreen;
    int rblue, rgreen;
    std::string originalName;
};

class OptionMusic: public MenuOption {
public:
    OptionMusic(const Gui::ContextBox & parent, const Token *token);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    // This is to pass parameters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionMusic();

protected:
    void changeMusic(int much);

private:
    /*
    int lblue, lgreen;
    int rblue, rgreen;
    */
    std::string originalName;
};

class OptionLanguage: public MenuOption {
public:
    OptionLanguage(const Gui::ContextBox & parent, const Token * token);
    virtual void logic();
    virtual void run(const Menu::Context &);
};

class OptionJoystick: public MenuOption {
public:
    OptionJoystick(const Gui::ContextBox & parent, const Token *token);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    virtual ~OptionJoystick();
};

#endif
