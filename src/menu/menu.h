#ifndef _paintown_menu_h
#define _paintown_menu_h

#include <vector>
#include <string>

#include "util/load_exception.h"
#include "util/gradient.h"
#include "util/file-system.h"
#include "input/input-map.h"
#include "return_exception.h"
#include "gui/box.h"
#include "gui/context-box.h"

#ifdef _MSC_VER
#ifndef uint32_t
	typedef unsigned __int32 uint32_t;
#endif
#endif

class Font;
class Bitmap;
class MenuOption;
class Token;
class MenuAnimation;



class Point{
    public:
    int x;
    int y;
    Point();
    Point(int x, int y);
    ~Point();
};

class Menu{
public:
    /*! ctor dtor */
    Menu();
    Menu(const Filesystem::AbsolutePath & str) throw (LoadException);
    Menu(Token * token) throw (LoadException);
    virtual ~Menu();

    /*! load */
    virtual void load(const Filesystem::AbsolutePath & filename) throw (LoadException);

    /*! load */
    virtual void load(Token *token) throw (LoadException);

    /*! Logic */
    virtual void act(bool &endGame, bool reset = true);

    /*! Draw */
    virtual void draw(const Gui::RectArea &, Bitmap *bmp);

    /*! run as it's own menu */
    virtual void run();
    
    /*! run option */
    virtual void runOption(bool &endGame);
    
    /*! run option */
    virtual void runOption(unsigned int index);

    /*! Parent */
    virtual void setParent(Menu *menu);

    /*! get parent */
    inline Menu *getParent() { return parent; }

    /*! get background */
    virtual Bitmap *getBackground();

    /*! get menu name */
    inline const std::string &getName() const { return _name; };

    /*! set menu name */
    inline void setName(const std::string &n){ _name = n; };

    /*! Add options to menu */
    void addOption(MenuOption *opt);

    /*! get working bitmap */
    Bitmap *getWork();

    //! get font
    static std::string &getFont();

    //! get font width
    static int getFontWidth();

    //! get font height
    static int getFontHeight();

    //! set new font menu wide
    static void setFont(const std::string &font, int w, int h);

    static void setFontName(const std::string & str);
    static void setFontWidth(int w);
    static void setFontHeight(int h);

    inline bool checkRemoval() const { return removeOption; }

    inline void setAsOption( const bool o ) { option = o; }

    inline bool isOption() const { return option; }

    inline void setInfoText(const std::string &text) { menuInfo = text; }

    inline bool hasSomeOptions() const {
        return hasOptions;
    }

    /* finish initializing stuff for options */
    void setupOptions();

    //! Set height of context menu
    virtual inline void setMenuHeight(int height){
        this->contextMenu.position.height = height;
    }
    
    //! Get context menu
    virtual std::vector<Gui::ContextItem *> getContextList();

protected:
    //! Current music
    std::string music;
    //! Current select sound, beep or whatever
    std::string selectSound;
    std::string backSound;
    std::string okSound;
    //! longest menu text
    int longestTextLength;

    //! Context box
    Gui::ContextBox contextMenu;

    std::vector <MenuOption *> menuOptions;
public:
    /* hack to make tab menu work */
    MenuOption * selectedOption;
protected:
    //! Animations
    std::vector <MenuAnimation *>backgroundAnimations;
    std::vector <MenuAnimation *>foregroundAnimations;

    //! Draw states
    enum menuDrawState
    {
        FadeIn = 0,
        FadeInText,
        NoFade
    };

    //! Enum containing the draw state so we can do some effects
    menuDrawState currentDrawState;

    //! Our current bitmap to draw to
    /* can't this be passed in instead of created for every menu? */
    Bitmap *work;

    //! Draw background
    void drawBackground(Bitmap *work);

    //! Draw info box
    void drawInfoBox (const std::string &info, const Point &location, Bitmap *bmp );

    int getSelectedColor(bool selected);


    //! General text about the menu, similar to info about options if empty it draws nothing it also borrows the colors of the main menu box
    std::string menuInfo;

    //! menu info location
    Point menuInfoLocation;

    //! parent menu
    Menu *parent;

private:
    std::string _name;

    // Error checker if no options do not add to menu and remove from list
    bool hasOptions;

    // this is for menu removal if an option
    bool removeOption;

    //! Set longest length
    void checkTextLength(MenuOption *opt);

    //! Cleanup options to remove omited items
    void optionCleanup();

    //! Set background for this menu
    Bitmap *background;

    //! Clear color only used if no background is set this is for every existant menu
    int clearColor;

    //! Is this an option?
    bool option;

    //! This is the location of the option info text
    Point optionInfoTextLocation;

    Effects::Gradient selectedGradient;

    enum MenuInput{
        Up,
        Down,
        Left,
        Right,
        Select,
        /* return is a safe menu return */
        Return,
        /* exit is usually done by ESC */
        Exit,
    };

    InputMap<MenuInput> input;

};
#endif
