#ifndef _paintown_menu_h
#define _paintown_menu_h

#include <vector>
#include <string>
#include <map>

#include "util/load_exception.h"
#include "util/file-system.h"
#include "input/input-map.h"
#include "gui/box.h"
#include "gui/context-box.h"
#include "gui/popup-box.h"
#include "gui/widget.h"

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
namespace _Menu{
class Point{
    public:
    int x;
    int y;
    Point();
    Point(int x, int y);
    ~Point();
};

class InfoBox: public Gui::Widget {
    public:
        InfoBox();
        ~InfoBox();
        
        void act();
        void render(const Bitmap &);
        void open();
        void close();
        void setText(const std::string &);
        
        inline bool isActive(){
            return (this->state != NotActive);
        }
        
        inline void setFont(const Filesystem::RelativePath & font, int width, int height){
            this->font = font;
            this->fontWidth = width;
            this->fontHeight = height;
        }
        
    private:
        enum State{
            NotActive,
            Opening,
            Active,
            Closing,
        };
        
        State state;
        Gui::PopupBox popup;
        
        Filesystem::RelativePath font;
        int fontWidth;
        int fontHeight;
        
        int fadeAlpha;
        
        std::vector<std::string> text;
};

/*! New Menu class */

class Menu{
    public:
        Menu();
        Menu(const Filesystem::AbsolutePath & filename);
        Menu(Token * token);
        virtual ~Menu();

        /*! Run Menu */
        virtual void run();

        /*! Logic */
        virtual void act();

        /*! render - renders only actual context menu excluding other elements (animations etc) */
        virtual void render(int x, int y, const Bitmap &);

    protected:
        /*! load token */
        void load(Token * token);

        /*! Option List */
        std::vector <MenuOption *> options;

        /*! Context Menu */
        Gui::ContextBox contextMenu;

        /*! Need to come up with a method of storing data without having 10,000 variables for different aspects of the menu */

    private:
};

}

/* Keep old menu class for backwards compatibility until everything has been moved over */
class Menu{
public:
    /* TODO: make the fadein stuff part of the configuration file for
     * the menu (menu.txt)
     */
    Menu(bool fadein = false, int fadeColor = 0);
    Menu(const Filesystem::AbsolutePath & str);
    Menu(Token * token);
    virtual ~Menu();

    /*! load */
    virtual void load(const Filesystem::AbsolutePath & filename);

    /*! load */
    virtual void load(Token *token);

    /*! Logic */
    virtual void act(bool &endGame, bool reset = true);

    /*! run as it's own menu */
    virtual void run();
    
    /*! run option */
    virtual void runOption(bool &endGame);
    
    /*! run option */
    virtual void runOption(unsigned int index);
    
    /*! get option */
    virtual MenuOption * getOption(unsigned int index);

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
    static const Filesystem::RelativePath getFont();

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

    /* wait for the select key to be released */
    virtual void waitForSelect();

    /* finish initializing stuff for options */
    void setupOptions();

    //! Set height of context menu
    virtual inline void setMenuHeight(int height){
        this->contextMenu.location.setPosition2(Gui::AbsolutePoint(this->contextMenu.location.getX2(), this->contextMenu.location.getY() + height));
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
    std::map<std::string, Gui::Space::Space *> coordinateSpaces;

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
    
    void setMenuInfo(const std::string &);

    bool isFading();
    void doFade(Bitmap * work);
    
    //! Menu info box
    _Menu::InfoBox menuInfoBox;
    
    //! Add Info box
    void addInfoBox (const std::string &);
    
    //! Update info boxes
    void actInfoBoxes();
    
    //! Render info boxes
    void renderInfoBoxes(const Bitmap &);
    
    //! Get rid of info boxes
    void closeInfoBoxes();
    
    //! General text about the menu, similar to info about options if empty it draws nothing it also borrows the colors of the main menu box
    std::string menuInfo;

    //! menu info location
    _Menu::Point menuInfoLocation;

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

    struct Fade {
        Fade():
            fade(0),
            color(0){
        }

        int fade;
        int color;
    } fade;

    //! Clear color only used if no background is set this is for every existant menu
    int clearColor;

    //! Is this an option?
    bool option;

    //! This is the location of the option info text
    _Menu::Point optionInfoTextLocation;
    
    //! Info boxes
    std::vector<_Menu::InfoBox *> optionInfoBoxes;

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
