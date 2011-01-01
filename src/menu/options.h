#ifndef _paintown_options_h
#define _paintown_options_h

#include "menu_option.h"
#include "util/input/input-map.h"
#include "util/file-system.h"
#include "paintown-engine/level/utils.h"
#include "util/pointer.h"
#include "font-info.h"

class Token;

/* FIXME figure out what to do with this, since network depends on it." */
Level::LevelInfo doLevelMenu(const std::string dir, const Menu::Context & context);

class OptionAdventure : public MenuOption{
    public:
        // Do logic before run part
        virtual void logic();

        // Finally it has been selected, this is what shall run 
        // endGame will be set true if it is a terminating option
        virtual void run(const Menu::Context &);

        OptionAdventure(const Token *token);

        virtual ~OptionAdventure();
};

class OptionAdventureCpu : public MenuOption{
public:
	OptionAdventureCpu(const Token *token);

	// Do logic before run part
	virtual void logic();
	
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);
	
	virtual ~OptionAdventureCpu();
};

/* Change game mod */
class OptionChangeMod: public MenuOption {
public:
    OptionChangeMod(const Token *token);
    virtual ~OptionChangeMod();
    virtual void logic();
    virtual void run(const Menu::Context &);
};

/*! Handles key reconfiguration */
class OptionCredits : public MenuOption {
public:
    enum CreditKey{
        Exit
    };

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    OptionCredits(const Token *token);

    virtual ~OptionCredits();
private:
    Filesystem::RelativePath background;
    std::vector<std::string> credits;
    std::string music;
    int color, title;
    InputMap<CreditKey> input;
};

/*! Dummy option, to allow place fillers in menus */
class OptionDummy : public MenuOption{
public:
	OptionDummy( const Token *token );
	OptionDummy( const std::string &name );

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);

	virtual ~OptionDummy();
};

/*! Handles key reconfiguration */
class OptionFullscreen : public MenuOption{
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);
    
    virtual std::string getText();

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    OptionFullscreen(const Token *token);

    virtual ~OptionFullscreen();
private:

    int lblue, lgreen;
    int rblue, rgreen;
};

/*! Handles key reconfiguration */
class OptionInvincible : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(const Menu::Context &);
		
		// This is to pass paramaters to an option ie a bar or something
		virtual bool leftKey();
		virtual bool rightKey();
    
                virtual std::string getText();
		
		OptionInvincible(const Token *token);
	
		virtual ~OptionInvincible();
	private:
		int lblue, lgreen;
		int rblue, rgreen;
};

/*! Handles joystick reconfiguration */
class OptionJoystick : public MenuOption {
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    OptionJoystick(const Token *token);

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

/*! Handles key reconfiguration */
class OptionKey : public MenuOption{
    public:
        // Do logic before run part
        virtual void logic();

        // Finally it has been selected, this is what shall run 
        // endGame will be set true if it is a terminating option
        virtual void run(const Menu::Context &);

        OptionKey(const Token *token);

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
class OptionLevel : public MenuOption {
public:
	OptionLevel(const Token *token, int * set, int value);

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


/*! Handles key reconfiguration */
class OptionLives : public MenuOption{
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();
    
    virtual std::string getText();

    OptionLives(const Token *token);

    virtual ~OptionLives();
private:
    int lblue, lgreen;
    int rblue, rgreen;
};

namespace Menu {
    class Menu;
    class Context;
}

/*! Handles sub menus */
class OptionMenu : public MenuOption {
    public:
        // Do logic before run part
        virtual void logic();

        // Finally it has been selected, this is what shall run 
        // endGame will be set true if it is a terminating option
        virtual void run(const Menu::Context &);

        OptionMenu(const Token *token);

        virtual ~OptionMenu();

    private:
        //Menu *menu;
        Menu::Menu *menu;
};

/*! Handles sub menus */
class OptionMugenMenu : public MenuOption {
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    OptionMugenMenu(const Token *token);

    virtual ~OptionMugenMenu();

private:
    // MugenMenu *_menu;
};

#ifdef HAVE_NETWORKING
/*! Handles key reconfiguration */
class OptionNetworkHost : public MenuOption{
public:
	OptionNetworkHost(const Token *token);

	// Do logic before run part
	virtual void logic();
	
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);
	
	virtual ~OptionNetworkHost();
};

/*! Handles key reconfiguration */
class OptionNetworkJoin : public MenuOption{
public:
	OptionNetworkJoin(const Token *token);

	// Do logic before run part
	virtual void logic();
	
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);
	
	virtual ~OptionNetworkJoin();
};
#endif

/*! Handles key reconfiguration */
class OptionNpcBuddies : public MenuOption {
public:
	OptionNpcBuddies( const Token *token );

	// Do logic before run part
	virtual void logic();
	
	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);
    
        virtual std::string getText();
	
	// This is to pass paramaters to an option ie a bar or something
	virtual bool leftKey();
	virtual bool rightKey();
	
	virtual ~OptionNpcBuddies();
private:
	int lblue, lgreen;
	int rblue, rgreen;
};

/*! Handles key reconfiguration */
class OptionPlayMode : public MenuOption {
public:
    OptionPlayMode(const Token *token);

    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);
    
    virtual std::string getText();

    // This is to pass paramaters to an option ie a bar or something
    virtual bool leftKey();
    virtual bool rightKey();

    virtual ~OptionPlayMode();

protected:
    virtual void changeMode();

private:

    int lblue, lgreen;
    int rblue, rgreen;
};

/*! Handles key reconfiguration */
class OptionQuit : public MenuOption {
public:
	OptionQuit( const Token *token );
	OptionQuit( const std::string &name );

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
    OptionReturn(const Token * token);
    virtual void logic();
    virtual void run(const Menu::Context &);
    virtual ~OptionReturn();
};

/* continue the game */
class OptionContinue: public MenuOption {
public:
    OptionContinue(const Token * token);
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
    OptionScreenSize(const Token *token);

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
class OptionSelectFont : public MenuOption {
public:
    OptionSelectFont(const Token *token);

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
class OptionSpeed : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(const Menu::Context &);
		
		// This is to pass paramaters to an option ie a bar or something
		virtual bool leftKey();
		virtual bool rightKey();

                virtual std::string getText();
		
		OptionSpeed(const Token *token);
	
		virtual ~OptionSpeed();
	private:
		// name
		std::string name;
		
		int lblue, lgreen;
		int rblue, rgreen;
};

/*! Handles sub menus */
class OptionTabMenu : public MenuOption {
public:
    // Do logic before run part
    virtual void logic();

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &);

    OptionTabMenu(const Token *token);

    virtual ~OptionTabMenu();

private:
    Menu::Menu *menu;
};

class OptionSound: public MenuOption {
public:
    OptionSound(const Token *token);

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
};

class OptionMusic: public MenuOption {
public:
    OptionMusic(const Token *token);

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
    int lblue, lgreen;
    int rblue, rgreen;
};

class OptionLanguage: public MenuOption {
public:
    OptionLanguage(const Token * token);
    virtual void logic();
    virtual void run(const Menu::Context &);
protected:
    std::vector<std::string> languages;
};

#endif
