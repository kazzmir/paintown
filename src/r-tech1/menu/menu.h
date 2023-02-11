#ifndef _rtech1_Menu_h
#define _rtech1_Menu_h

#include <vector>
#include <string>
#include <map>

#include "r-tech1/file-system.h"
#include "r-tech1/pointer.h"
#include "r-tech1/parameter.h"
#include "r-tech1/input/input-map.h"
#include "r-tech1/graphics/gradient.h"
#include "r-tech1/menu/font-info.h"

#include "r-tech1/gui/animation.h"
#include "r-tech1/gui/box.h"
#include "r-tech1/gui/context-box.h"
#include "r-tech1/gui/fadetool.h"
#include "r-tech1/gui/popup-box.h"
#include "r-tech1/gui/tabbed-box.h"
#include "r-tech1/gui/widget.h"
#include "r-tech1/gui/coordinate.h"

#ifdef _MSC_VER
#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif
#endif

class Font;

namespace Graphics{
class Bitmap;
}

class MenuOption;
class Token;
class TokenView;

namespace Menu{

extern Util::Parameter<Util::ReferenceCount<FontInfo> > menuFontParameter;

Effects::Gradient standardGradient();

class OptionFactory;
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
        
        void act(const Font &);
        virtual void render(const Graphics::Bitmap &);
        void render(const Graphics::Bitmap &, const Font & font);
        void open();
        void close();
        void setText(const std::string &);
        
        inline bool isActive(){
            return (this->state != NotActive);
        }
        
        /*
        inline void setFont(const Util::ReferenceCount<FontInfo> & font){
            this->font = font;
        }
        */
        virtual void initialize(const Font & font);
        
    private:
        enum State{
            NotActive,
            Opening,
            Active,
            Closing,
        };
        
        State state;
        Gui::PopupBox popup;
        
        // Util::ReferenceCount<FontInfo> font;
        
        int fadeAlpha;
        
        std::vector<std::string> text;
        
        std::vector<int> padding;
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

/* reload the current menu, usually thrown by some option */
class Reload: public MenuException {
public:
    Reload(const std::string & file, int line, const std::string reason = "");
    virtual ~Reload() throw();
protected:
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
        virtual ValueHolder & operator<<(TokenView &); 
        
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
    PlayAsteroids /* Hack for now, remove later */
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

        enum Type{
            Default,
            Tabbed
        };
        
        //! Reader
        virtual bool readToken(const Token *, const OptionFactory &)=0;

        virtual Type getType() const = 0;
        
        virtual void initialize(Context &)=0;
        virtual void finish()=0;
        virtual bool active()=0;
        
        virtual void act(const Context &)=0;
        virtual void render(const Graphics::Bitmap &, const Font & font)=0;
        
        // Set font if applicable
        // virtual void setFont(const Util::ReferenceCount<FontInfo> &);
        
        //! Compatibility for now, remove later
        virtual void addOption(MenuOption *)=0;
        virtual std::vector<Util::ReferenceCount<MenuOption> > getOptions() const = 0;
        
        /*! Handle action, with access to context
        */
        virtual void doAction(const Actions &, Context &)=0;
        
        /*! Invoke override */
        virtual void invokeOverride(const Context &)=0;

        virtual void setPosition(const Gui::Coordinate & coordinate) = 0;
        
    protected:
        /*! Info boxes */
        std::vector <InfoBox *> info;
        
        /*! Menu info box */
        InfoBox menuInfo;
        
        /*! Add info box */
        virtual void addInfo(const std::string &, const Gui::Widget &, Context &, const Font &);
        
        /*! act info box */
        virtual void actInfo(const Font &);
        
        /*! render info box */
        virtual void renderInfo(const Graphics::Bitmap &, const Font & font);
};

/*! Regular Menu */
class DefaultRenderer: public Renderer {
    public:
        DefaultRenderer();
        virtual ~DefaultRenderer();
        
        // virtual void setFont(const Util::ReferenceCount<FontInfo> &);
        virtual bool readToken(const Token *, const OptionFactory &);
        virtual void initialize(Context &);
        virtual void finish();
        virtual bool active();
        virtual void act(const Context &);
        virtual void render(const Graphics::Bitmap &, const Font &); 
        virtual void addOption(MenuOption *);
        virtual void doAction(const Actions &, Context &);
        virtual std::vector<Util::ReferenceCount<MenuOption> > getOptions() const;
        virtual void invokeOverride(const Context &);
        virtual void setPosition(const Gui::Coordinate & coordinate);

        virtual Type getType() const;

        virtual const Gui::ContextBox & getBox() const {
            return menu;
        }
        
        virtual Gui::ContextBox & getBox(){
            return menu;
        }
        
    private:

        /*! Options */
        std::vector<Util::ReferenceCount<MenuOption> > options;
        
        /*! Context Box */
        Gui::ContextBox menu;
        
        /*! Override */
        bool hasOverride;
        
        /*! Override index */
        unsigned int overrideIndex;
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
    std::vector<Util::ReferenceCount<MenuOption> > options;
};

class TabRenderer: public Renderer {
    public:
        TabRenderer();
        virtual ~TabRenderer();
        
        // virtual void setFont(const Util::ReferenceCount<FontInfo> &);
        virtual bool readToken(const Token *, const OptionFactory &);
        virtual void initialize(Context &);
        virtual void finish();
        virtual bool active();
        virtual void act(const Context &);
        virtual void render(const Graphics::Bitmap &, const Font &);
        virtual void addOption(MenuOption *);
        virtual void doAction(const Actions &, Context &);
        virtual std::vector<Util::ReferenceCount<MenuOption> > getOptions() const;
        virtual void invokeOverride(const Context &);
        virtual void setPosition(const Gui::Coordinate & coordinate);
        virtual Type getType() const;
        
        virtual Gui::TabbedBox & getBox(){
            return menu;
        }
        
    private:

        /*! Tabs */
        std::vector<TabInfo *> tabs;
        
        /*! Tabbed Box */
        Gui::TabbedBox menu;
        
        /*! Override */
        bool hasOverride;
        
        /*! Override index */
        unsigned int overrideIndex;
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
        virtual void render(const Util::ReferenceCount<Renderer> &, const Graphics::Bitmap &) const;
        
        virtual void renderBackground(const Graphics::Bitmap & bmp) const;
        virtual void renderForeground(const Graphics::Bitmap & bmp) const;
        
        /*! Parse data */
        virtual void parseToken(const Token *);

        //! Compatibility stuff
        virtual void addBackground(const Token *);
        virtual void addBackground(const std::string &);
        virtual void addBackground(const Graphics::Bitmap & image);
        virtual void addBackground(const Util::ReferenceCount<Gui::Animation> & animation);
        
        /*! Initializes things like faders */
        virtual void initialize();
        
        /*! Closes things out like faders */
        virtual void finish();
        
        /*! Play sound */
        virtual void playSound(const Actions &) const;
        
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
/*
        virtual void setBackground(Util::ReferenceCount<Gui::AnimationManager> background);

        virtual inline Util::ReferenceCount<Gui::AnimationManager> getBackground(){
            return this->background;
        }
  */      
        virtual inline void setFont(const Util::ReferenceCount<FontInfo> & font){
            this->font = font;
        }

        virtual bool hasFont() const;

        virtual inline const Util::ReferenceCount<FontInfo> & getFont() const {
            return this->font;
        }
        
        virtual inline const Util::ReferenceCount<FontInfo> & getFontInfo() const {
            return this->font;
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
        
        virtual inline const Gui::RelativePoint & getMenuInfoLocation() const {
            return this->menuInfoLocation;
        }
        
        virtual inline void setMenuInfoText(const std::string & text){
            this->menuInfo = text;
        }
        
        virtual inline const std::string & getMenuInfoText() const {
            return this->menuInfo;
        }
        
        virtual std::vector<std::string> getLanguages() const;
        virtual void setLanguages(const std::vector<std::string> & languages);
        
    private:
        /*! Require cleanup *default constructor only* */
        bool cleanup;

        /*! Current state */
        State state;

        /*! Fade Tool */
        Gui::FadeTool * fades;

        /*! Backgrounds */
        Gui::AnimationManager background;

        /*! Sounds */
        std::map<Actions, Filesystem::RelativePath> sounds;
        
        /*! Music */
        Filesystem::RelativePath music;
        
        /*! Font */
        Util::ReferenceCount<FontInfo> font;

        /*! Info Placement */
        Gui::RelativePoint infoLocation;
        
        /*! Menu Info Placement */
        Gui::RelativePoint menuInfoLocation;
        
        /*! Menu Info Text */
        std::string menuInfo;

        std::vector<std::string> languages;
};

/*! New Menu class */
class Menu{
    public:
        
        /* These two are basically the same but the Type version will create the renderer for you */
        Menu(const Renderer::Type & type = Renderer::Default);
        Menu(const Util::ReferenceCount<Renderer> & renderer);

        Menu(const Filesystem::AbsolutePath &, const Renderer::Type & type = Renderer::Default);
        Menu(const Filesystem::AbsolutePath &, const OptionFactory & factory, const Renderer::Type & type = Renderer::Default);
        Menu(const Token *, const Renderer::Type & type = Renderer::Default);
        Menu(const Token * token, const OptionFactory & factory, const Renderer::Type & type = Renderer::Default);
        virtual ~Menu();

        /*! Run Menu pass parent context */
        virtual void run(const Context &);

        /* Set position of the menu */
        virtual void setPosition(const Gui::Coordinate & coordinate);

        /*! Logic pass local context */
        virtual void act(Context &);

        /*! render pass local context and work */
        virtual void render(const Context &, const Graphics::Bitmap &) const;

        virtual void setRenderer(const Renderer::Type &);
        virtual void setRenderer(const Util::ReferenceCount<Renderer> & renderer);

        /* a list of languages (translations) supported by this menu */
        virtual std::vector<std::string> getLanguages() const;
        
        /*! Get Name */
        virtual std::string getName();
        virtual std::string getInfo();

        virtual void setFont(const Util::ReferenceCount<FontInfo> &);
        
        /*! Add option */
        virtual void addOption(MenuOption * opt);

        virtual inline Util::ReferenceCount<Renderer> getRenderer() const {
            return renderer;
        }
        
        static const int Width = 640;
        static const int Height = 480;
        
    protected:
        void setupDefaultLanguage(const Context & context, const Menu & parent);
        virtual void openOptions();
        virtual void closeOptions();
        
        /*! Context */
        Context context;
        
        /*! Data holder */
        std::map<std::string, ValueHolder *> data;

        /*! Renderer */
        Util::ReferenceCount<Renderer> renderer;
        
        /*! load token */
        void load(const Token * token, const OptionFactory & factory);
#if 0
        /*! Handle Override 
         *  There can only be one decleration of an override per file/token 
         *  The first one found will be the one executed.
         *  If found it will return true so that the menu can handle it and avoid going to everything else
         */
        virtual bool handleOverride(const Token *);
#endif 
        /*! Do current version */
        virtual void handleCurrentVersion(const Token *);
        
        /*! Prior token compatibility based on version Global::getVersion() */
        virtual void handleCompatibility(const Token *, int version, const OptionFactory & factory);
        
        /*! Add Data */
        void addData(ValueHolder *);
        
        /*! Keys */
        InputMap<Actions> input;
        
        /*! Type */
        Renderer::Type type;
        
        /*! Check type */
        virtual Util::ReferenceCount<Renderer> rendererType(const Renderer::Type &);
        std::vector<std::string> languages;
};

}
#endif
