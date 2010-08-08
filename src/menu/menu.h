#ifndef _paintown_Menu_h
#define _paintown_Menu_h

#include <vector>
#include <string>
#include <map>

#include "util/load_exception.h"
#include "util/file-system.h"
#include "input/input-map.h"

#include "gui/animation.h"
#include "gui/box.h"
#include "gui/context-box.h"
#include "gui/fadetool.h"
#include "gui/popup-box.h"
#include "gui/tabbed-box.h"
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

namespace Menu{
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

class MenuException : public Exception::Base{
    public:
        MenuException(const std::string & file, int line, const std::string reason = "");
        MenuException(const MenuException & copy);
        MenuException(const Exception::Base & copy);
        virtual ~MenuException() throw();
    protected:
        
        std::string reason;
        
        virtual inline const std::string getReason() const {
            return reason;
        }
        
        virtual Exception::Base * copy() const;
};

class ValueHolder{
    public:
        ValueHolder(const std::string &);
        ValueHolder(const ValueHolder &);
        virtual ~ValueHolder();

        virtual ValueHolder & operator=(const ValueHolder &);
        
        virtual ValueHolder & operator<<(const std::string &);
        virtual ValueHolder & operator<<(bool val);
        virtual ValueHolder & operator<<(int val);
        virtual ValueHolder & operator<<(double val);  
        virtual ValueHolder & operator<<(Token *);
        
        virtual ValueHolder & operator>>(std::string &);
        virtual ValueHolder & operator>>(bool &);
        virtual ValueHolder & operator>>(int &);
        virtual ValueHolder & operator>>(double &);

        virtual inline const std::string & getName() const {
            return this->name;
        }

        virtual const std::string getValues();

    private:
        std::string name;
        std::vector<std::string> values;
        unsigned int location;
        
        void next();
};

/*! Menu Backgrounds */
class Background{
    public:
        Background();
        ~Background();

        /*! Logic - Change over to Gui::Space later */
        void act(const Gui::Coordinate &);

        void render(const Gui::Animation::Depth &, const Bitmap &);

        void add(Gui::Animation *);
    private:
        std::map<Gui::Animation::Depth, std::vector<Gui::Animation *> > backgrounds;

        void drawBackgrounds(std::vector<Gui::Animation *> &, const Bitmap &);
};

/*! Menu actions */
enum Actions{
    Up,
    Down,
    Left,
    Right,
    Select,
    Back,
    Cancel,
    Modify,
    Pause,
};

class Context;

/*! Menu Renderer
 * Allows menu to modify the way it displays it's contents
 * ie regular menus, tab menus, specialized menu etc
 */
class Renderer{
    public:
        Renderer();
        virtual ~Renderer();
        
        //! Reader
        virtual bool readToken(Token *)=0;
        
        virtual void initialize(Context &)=0;
        virtual void finish()=0;
        virtual bool active()=0;
        
        virtual void act()=0;
        virtual void render(const Bitmap &)=0;
        
        //! Compatibility for now, remove later
        virtual void addOption(MenuOption *)=0;
        
        /*! Handle action, with access to context
        */
        virtual void doAction(const Actions &, Context &)=0;
        
    protected:
        /*! Info boxes */
        std::vector <InfoBox *> info;
        
        /*! Menu info box */
        InfoBox menuInfo;
        
        /*! Add info box */
        virtual void addInfo(const std::string &, const Gui::Widget &, Context &);
        
        /*! act info box */
        virtual void actInfo();
        
        /*! render info box */
        virtual void renderInfo(const Bitmap &);
};

/*! Regular Menu */
class DefaultRenderer : public Renderer {
    public:
        DefaultRenderer();
        virtual ~DefaultRenderer();
        
        virtual bool readToken(Token *);
        virtual void initialize(Context &);
        virtual void finish();
        virtual bool active();
        virtual void act();
        virtual void render(const Bitmap &);
        virtual void addOption(MenuOption *);
        virtual void doAction(const Actions &, Context &);
        
    private:

        /*! Options */
        std::vector <MenuOption *> options;
        
        /*! Context Box */
        Gui::ContextBox menu;
};

/*! Tabbed Menu */
class TabInfo {
    public:
        TabInfo();
        ~TabInfo();
        std::string name;
        std::string info;
        std::string menuInfo;
        
        void act();
        
        /*! Options */
        std::vector <MenuOption *> options;
};
class TabRenderer : public Renderer {
    public:
        TabRenderer();
        virtual ~TabRenderer();
        
        virtual bool readToken(Token *);
        virtual void initialize(Context &);
        virtual void finish();
        virtual bool active();
        virtual void act();
        virtual void render(const Bitmap &);
        virtual void addOption(MenuOption *);
        virtual void doAction(const Actions &, Context &);
        
    private:

        /*! Tabs */
        std::vector <TabInfo *> tabs;
        
        /*! Tabbed Box */
        Gui::TabbedBox menu;
        
};

/*! Menu contexts
    - Each menu has a context which it defaults to
        - Sub menus will be passed the parents context when run
    - Fader
    - Backgrounds (No background will fall back onto a fill screen)
    - Menu (options, ContextBox, etc)
    - Contexts are settable to omit drawing certain items (usefull for things like in-game menus)
 */
class Context{
    public:
        Context();
        Context(const Context &, const Context &);
        virtual ~Context();
        
        virtual void act();
        /*! Pass the widget (Menu ContextBox in this case) to be drawn
         * Allows for custom widget menus to be draw in place (ie for tabs or something)
        */
        virtual void render(Renderer *, const Bitmap &);
        
        /*! Parse data */
        virtual void parseToken(Token *);

        //! Compatibility stuff
        virtual void addBackground(Token *);
        virtual void addBackground(const std::string &);
        
        /*! Initializes things like faders */
        virtual void initialize();
        
        /*! Closes things out like faders */
        virtual void finish();
        
        /*! Play sound */
        virtual void playSound(const Actions &);
        
        /*! Add sound */
        virtual void addSound(const Actions &, const Filesystem::RelativePath &);
        
        /*! Play music */
        virtual void playMusic();
        
        /*! set music */
        virtual inline void setMusic(const Filesystem::RelativePath & music){
            this->music = music;
        }

        /*! Current state */
        enum State{
            NotStarted,
            Initializing,
            Running,
            Finishing,
            Completed,
        };
        
        inline const State & getState() const {
            return this->state;
        }
        
        virtual void setFadeTool(Gui::FadeTool *);
        virtual inline Gui::FadeTool * getFadeTool(){
            return this->fades;
        }
        virtual void setBackground(Background *);
        virtual inline Background * getBackground(){
            return this->background;
        }
        
        virtual inline void setFont(const Filesystem::RelativePath & font){
            this->font = font;
        }
        virtual inline const Filesystem::RelativePath & getFont() const {
            return this->font;
        }
        virtual inline void setFontWidth(int width){
            this->fontWidth = width;
        }
        virtual inline int getFontWidth() const {
            return this->fontWidth;
        }
        virtual inline void setFontHeight(int height){
            this->fontHeight = height;
        }
        virtual inline int getFontHeight() const {
            return this->fontHeight;
        }
        
        virtual inline void setInfoLocation(double x, double y){
            this->infoLocation.set(x,y);
        }
        
        virtual inline const Gui::RelativePoint & getInfoLocation() const {
            return this->infoLocation;
        }
        
        virtual inline void setMenuInfoLocation(double x, double y){
            this->menuInfoLocation.set(x,y);
        }
        
        virtual inline const Gui::RelativePoint & getMenuInfoLocation()const {
            return this->menuInfoLocation;
        }
        
        virtual inline void setMenuInfoText(const std::string & text){
            this->menuInfo = text;
        }
        
        virtual inline const std::string & getMenuInfoText() const {
            return this->menuInfo;
        }
        
    private:
        /*! Require cleanup *default constructor only* */
        bool cleanup;

        /*! Current state */
        State state;

        /*! Fade Tool */
        Gui::FadeTool * fades;

        /*! Backgrounds */
        Background * background;

        /*! Sounds */
        std::map<Actions, Filesystem::RelativePath> sounds;
        
        /*! Music */
        Filesystem::RelativePath music;
        
        /*! Font */
        Filesystem::RelativePath font;
        
        /*! Font Width */
        int fontWidth;
        
        /*! Font Height */
        int fontHeight;
        
        /*! Info Placement */
        Gui::RelativePoint infoLocation;
        
        /*! Menu Info Placement */
        Gui::RelativePoint menuInfoLocation;
        
        /*! Menu Info Text */
        std::string menuInfo;
};

/*! New Menu class */
class Menu{
    public:
        Menu();
        Menu(const Filesystem::AbsolutePath &);
        Menu(Token *);
        virtual ~Menu();

        /*! Run Menu pass parent context */
        virtual void run(const Context &);

        /*! Logic pass local context */
        virtual void act(Context &);

        /*! render pass local context and work */
        virtual void render(Context &, const Bitmap &);
        
        /*! Get Name */
        virtual std::string getName();
        
        /*! Add option */
        virtual inline void addOption(MenuOption * opt){
            if (renderer){
                this->renderer->addOption(opt);
            }
        }
    
    static const int Width = 640;
    static const int Height = 480;

    protected:
        
        /*! Context */
        Context context;
        
        /*! Data holder */
        std::map<std::string, ValueHolder *> data;

        /*! Renderer */
        Renderer * renderer;
        
        /*! load token */
        void load(Token * token);
        
        /*! Add Data */
        void addData(ValueHolder *);
        
        /*! Prior token compatibility based on version Global::getVersion() */
        virtual void handleCompatibility(Token * token, int version);

        /*! Keys */
        InputMap<Actions> input;
    
};

}

/* Keep old menu class for backwards compatibility until everything has been moved over */
namespace OldMenu{
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
    /*
    Bitmap *getWork();
    void setWork(Bitmap * work);
    */

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

    static const int Width = 640;
    static const int Height = 480;

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
    std::vector <Gui::Animation *>backgroundAnimations;
    std::vector <Gui::Animation *>foregroundAnimations;

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
    Bitmap * currentWork;

    //! Draw background
    void drawBackground(Bitmap *work);
    
    void setMenuInfo(const std::string &);

    bool isFading();
    void doFade(Bitmap * work);
    
    //! Menu info box
    ::Menu::InfoBox menuInfoBox;
    
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
    ::Menu::Point menuInfoLocation;

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
    ::Menu::Point optionInfoTextLocation;
    
    //! Info boxes
    std::vector< ::Menu::InfoBox *> optionInfoBoxes;

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
}
#endif
