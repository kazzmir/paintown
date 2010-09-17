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
	enum Type{
	    Default,
	    Tabbed,
	};
        Menu(const Type & type = Default);
        Menu(const Filesystem::AbsolutePath &, const Type & type = Default);
        Menu(Token *, const Type & type = Default);
        virtual ~Menu();

        /*! Run Menu pass parent context */
        virtual void run(const Context &);

        /*! Logic pass local context */
        virtual void act(Context &);

        /*! render pass local context and work */
        virtual void render(Context &, const Bitmap &);

        virtual void setRenderer(const Type &);
        
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
        void setupDefaultLanguage(const Context & context);
        
        /*! Context */
        Context context;
        
        /*! Data holder */
        std::map<std::string, ValueHolder *> data;

        /*! Renderer */
        Renderer * renderer;
        
        /*! load token */
        void load(Token * token);
	
	/*! Do current version */
	virtual void handleCurrentVersion(Token *);
	
        /*! Prior token compatibility based on version Global::getVersion() */
        virtual void handleCompatibility(Token *, int version);
        
        /*! Add Data */
        void addData(ValueHolder *);
        
        /*! Keys */
        InputMap<Actions> input;
	
	/*! Type */
	Type type;
	
	/*! Check type */
	virtual Renderer * rendererType(const Type &);
};

}
#endif
