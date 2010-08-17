#ifndef _paintown_options_h
#define _paintown_options_h

#include "menu_option.h"

#include "input/input-map.h"

class Token;

class OptionAdventure : public MenuOption{
    public:
        // Do logic before run part
        virtual void logic();

        // Finally it has been selected, this is what shall run 
        // endGame will be set true if it is a terminating option
        virtual void run(const Menu::Context &);

        OptionAdventure(Token *token);

        virtual ~OptionAdventure();
};

class OptionAdventureCpu : public MenuOption{
public:
	OptionAdventureCpu(Token *token);

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
    OptionChangeMod(Token *token);
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

    OptionCredits(Token *token);

    virtual ~OptionCredits();
private:
    Bitmap *background;
    std::vector<std::string> credits;
    std::string music;
    int color, title;
    InputMap<CreditKey> input;
};

/*! Dummy option, to allow place fillers in menus */
class OptionDummy : public MenuOption{
public:
	OptionDummy( Token *token );
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

    OptionFullscreen(Token *token);

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
		
		OptionInvincible(Token *token);
	
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

    OptionJoystick(Token *token);

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

        OptionKey(Token *token);

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
	OptionLevel(Token *token, int * set, int value);

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

    OptionLives(Token *token);

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

        OptionMenu(Token *token);

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

    OptionMugenMenu(Token *token);

    virtual ~OptionMugenMenu();

private:
    // MugenMenu *_menu;
};

#ifdef HAVE_NETWORKING
/*! Handles key reconfiguration */
class OptionNetworkHost : public MenuOption{
public:
	OptionNetworkHost(Token *token);

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
	OptionNetworkJoin(Token *token);

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
	OptionNpcBuddies( Token *token );

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
    OptionPlayMode(Token *token);

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
	OptionQuit( Token *token );
	OptionQuit( const std::string &name );

	// Do logic before run part
	virtual void logic();

	// Finally it has been selected, this is what shall run 
	// endGame will be set true if it is a terminating option
	virtual void run(const Menu::Context &);

	virtual ~OptionQuit();
};

struct ScreenSize{
    ScreenSize(int w, int h):w(w), h(h){}
    int w, h;
};

/*! Handles key reconfiguration */
class OptionScreenSize : public MenuOption {
public:
    OptionScreenSize(Token *token);

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
    OptionSelectFont(Token *token);

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
    
    // fonts
    /* FIXME: change this to Filesystem::RelativePath */
    std::vector<std::string> fonts;
    
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
		
		OptionSpeed(Token *token);
	
		virtual ~OptionSpeed();
	private:
		// name
		std::string name;
		
		int lblue, lgreen;
		int rblue, rgreen;
};

/*! Handles sub menus */
class OptionTabMenu : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(const Menu::Context &);
		
		OptionTabMenu(Token *token);
	
		virtual ~OptionTabMenu();
		
	private:
		Menu::Menu *menu;
};

/** Handles key reconfiguration */
class OptionVersus : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(const Menu::Context &);
		
		OptionVersus(Token *token);
	
		virtual ~OptionVersus();
		
	private:
		bool human;
};

class OptionSound: public MenuOption {
public:
    OptionSound(Token *token);

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
    OptionMusic(Token *token);

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

#endif
