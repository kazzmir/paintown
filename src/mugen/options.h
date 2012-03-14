#ifndef _paintown_mugen_options_h
#define _paintown_mugen_options_h

#include "exception.h"
#include "menu.h"
#include "util/menu/optionfactory.h"
#include "util/gui/scroll-list.h"
#include "util/gui/fadetool.h"
#include "util/pointer.h"
#include "util.h"

#include <vector>
#include <string>

class Token;

namespace PaintownUtil = ::Util;

namespace Mugen {

class Font;

class Character;

class ListFont{
public:
    ListFont();
    ListFont(PaintownUtil::ReferenceCount<Mugen::Font> font, int bank, int position);
    ListFont(const ListFont &);
    virtual ~ListFont();
    
    virtual const ListFont & operator=(const ListFont &);
    
    virtual void draw(int x, int y, const std::string &, const Graphics::Bitmap &) const;
    virtual void draw(int x, int y, int positionOverride, const std::string &, const Graphics::Bitmap &) const;
    
    virtual int getHeight() const;
    virtual int getWidth(const std::string &) const;
protected:
    PaintownUtil::ReferenceCount<Mugen::Font> font;
    int bank;
    int position;
};

class ListItem : public Gui::ScrollItem{
public:
    ListItem();
    virtual ~ListItem();
    //Ignore end of the line
    void draw(int x, int y, const Graphics::Bitmap &, const ::Font &, int distance) const;
    int size(const ::Font & font) const;
    virtual bool isRunnable() const;
    virtual void run();
    virtual const std::string & getInfo() const;
    virtual void render(int x, int y, const Graphics::Bitmap &, const ListFont &, int left = 0, int right = 0) const = 0;
    virtual int getWidth(const ListFont &) const = 0;
    virtual bool next() = 0;
    virtual bool previous() = 0;
    
protected:
};

class BaseMenuItem : public ListItem {
public:
    BaseMenuItem();
    virtual ~BaseMenuItem();
    virtual bool next() = 0;
    virtual bool previous() = 0;
    
    virtual void render(int x, int y, const Graphics::Bitmap & work, const ListFont & font, int left, int right) const;
    
    virtual int getWidth(const ListFont & font) const;
    
    virtual const std::string & getInfo() const;
    
    static bool displayInfo;
protected:
    std::string optionName;
    std::string currentValue;
};

class ScrollAction : public Gui::ScrollListInterface{
public:
    ScrollAction();
    virtual ~ScrollAction();
    
    //! Act
    virtual void act();
    
    //! Render (Ignores font from paintown) pass Mugen::Font for list and active wrapped in ListFont
    virtual void render(const Graphics::Bitmap &, const ::Font &) const;

    //! Add item
    virtual void addItem(const PaintownUtil::ReferenceCount<Gui::ScrollItem> item);

    //! Add vector of text
    virtual void addItems(const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > &);
    
    //! Get vector of text
    virtual const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > & getItems() const;
    
    //! Update item
    virtual bool updateItem(unsigned int index, const PaintownUtil::ReferenceCount<Gui::ScrollItem> item);
    
    //! Clear list
    virtual void clearItems();
    
    //! Get current Index
    virtual unsigned int getCurrentIndex() const;
    
    //! Set current index
    virtual void setCurrentIndex(unsigned int index);
    
    //! Get current item
    virtual inline const PaintownUtil::ReferenceCount<ListItem> getCurrent() const{
        return this->text[current].convert<ListItem>();
    }

    //! Next
    virtual bool next();

    //! Previous
    virtual bool previous();
    
    //! Value next
    virtual bool valueNext();
    
    //! Value previous
    virtual bool valuePrevious();
    
    //! Get max width of largest option
    virtual int getMaxWidth();
    
    //! Get max height of the option area
    virtual int getMaxHeight();
    
    //! Recalculate visibleItems given a specific height
    virtual void recalculateVisibleItems(int height);
    
    enum ExpandState{
        Disabled,
        Retract,
        Expand,
    };
    
    //! Toggle expanding in and out of view
    virtual void setExpandState(const ExpandState &);
    
    //! List Font
    virtual void setListFont(const ListFont &);
    
    //! Get Font
    virtual inline const ListFont & getFont() const {
        return this->font;
    }
    
    //! Active Font
    virtual void setActiveFont(const ListFont &);
    
    //! Set visible items (defaults to 5)
    virtual inline void setVisibleItems(unsigned int items){
        this->visibleItems = items;
        this->itemBottom = items-1;
    }
    
    //! Get visible items
    virtual unsigned int getVisibleItems();
    
    //! Set autospacing (ignores set spacing)
    virtual inline void setAutoSpacing(bool spacing){
        this->autoSpacing = spacing;
        this->spacingY = this->font.getHeight() + this->font.getHeight()/2;
        setMargins(this->spacingY, this->spacingY);
    }
    
    //! Set Spacing
    virtual inline void setSpacing(int x, int y){
        this->spacingX = x;
        this->spacingY = y;
    }
    
    virtual inline void setLocation(int x, int y){
        this->startX = x;
        this->startY = y;
    }
    
    virtual inline void setMargins(int top, int bottom){
        this->marginTop = top;
        this->marginBottom = bottom;
    }
    
    virtual inline int getTopMargin() const {
        return this->marginTop;
    }
    
    virtual inline int getBottomMargin() const {
        return this->marginBottom;
    }
    
    virtual inline void setBoundaries(int left, int right){
        this->leftBoundary = left;
        this->rightBoundary = right;
    }
    
    virtual inline void setShowCursor(bool cursor){
        this->showCursor = cursor;
    }
    
    virtual inline bool getShowCursor() const {
        return this->showCursor;
    }
    
    virtual inline void setAutoCursor(bool cursor){
        this->autoCursor = cursor;
    }
    
    virtual inline void setCursorCoords(int x1, int y1, int x2, int y2){
        this->cursorX1 = x1;
        this->cursorY1 = y1;
        this->cursorX2 = x2;
        this->cursorY2 = y2;
    }
    
protected:
    
    //! Is expanding enabled
    ExpandState expandState;
    
    //! Font
    ListFont font;
    
    //! Active font
    ListFont activeFont;
    
    //! Current item
    unsigned int current;
    
    //! Item offset
    unsigned int itemTop;
    unsigned int itemBottom;
    
    //! Set visible items
    unsigned int visibleItems;
    
    //! Auto spacing
    bool autoSpacing;
    
    //! location to render
    int startX;
    int startY;
    
    //! Spacing
    int spacingX;
    int spacingY;
    
    //! Offsets
    int offsetX;
    int currentOffsetX;
    int offsetY;
    int currentOffsetY;
    int scrollWaitX;
    int scrollWaitY;
    
    //! Margins
    int marginTop;
    int marginBottom;
    
    //! Boundaries
    int leftBoundary;
    int rightBoundary;
    
    //! Item list
    std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > text;
    
    //! Box cursor
    bool showCursor;
    //! Create coordinates for box cursor depending on the text
    bool autoCursor;
    //! Predefined cursor coordinates, ignored if autoCursor is enabled
    int cursorX1;
    int cursorX2;
    int cursorY1;
    int cursorY2;
    int cursorAlpha;
    int cursorAlphaMod;
    
};

class OptionArcade: public ItemOption {
public:
    OptionArcade(const std::string &name);
    virtual ~OptionArcade();
    
    void executeOption(const Mugen::PlayerType &, bool & endGame);
};

/*! Handles key reconfiguration */
class OptionVersus: public Mugen::ItemOption {
public:
    OptionVersus(const std::string &name);
    virtual ~OptionVersus();
    void executeOption(const Mugen::PlayerType &, bool & endGame);
};

class Option{
public:
    Option();
    virtual ~Option();
    
    virtual void next()=0;
    virtual void prev()=0;
    virtual void enter();
    virtual void render(Font &, int x1, int x2, int y, const Graphics::Bitmap &);
    virtual int getWidth(Font &);
    virtual inline const Mugen::Point & getPosition() const {
        return this->position;
    }
    virtual inline void toggleSelected(){
        this->selected = !this->selected;
    }
protected:
    Mugen::Point position;
    std::string optionName;
    std::string currentValue;
    
private:
    bool selected;
    // box
    int alpha,alphaMod;
};

/*! The option menu */
class OptionMenu{
public:
    OptionMenu(const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > &);
    OptionMenu(const OptionMenu &);
    virtual ~OptionMenu();
    
    virtual void act();
    virtual void draw(const Graphics::Bitmap &);
    virtual void drawList(const Graphics::Bitmap &);
    virtual void drawInfo(int x, int y, const std::string &, const Graphics::Bitmap &);
    /*! Draws the background and some info
     *  - 1st string - title
     *  - x, y - location of info
     *  - 2nd string - info text
     */ 
    virtual void drawInfoWithBackground(const std::string &, int x, int y, const std::string &, const Graphics::Bitmap &);
    
    virtual void updateList(const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > &);
    virtual void addItem(PaintownUtil::ReferenceCount<Gui::ScrollItem> item);
    virtual void updateItem(unsigned int index, PaintownUtil::ReferenceCount<Gui::ScrollItem> item);
    
    virtual void up();
    virtual void down();
    virtual void left();
    virtual void right();
    virtual void enter();
    virtual void cancel();
    
    virtual bool isDone();
    
    virtual void reset();
    
    virtual void setFirst();
    
    virtual void setLast();
    
    virtual inline unsigned int getSelected() const {
        return this->list.getCurrentIndex();
    }
    
    virtual inline void toggleCursor(){
        this->list.setShowCursor(!this->list.getShowCursor());
    }
    
    //! Sound types
    enum Sounds{
        Move,
        Done,
        Cancel,
    };
    
    virtual inline void setName(const std::string & name){
        this->name = name;
    }
    
    virtual inline void setRecalculateHeight(int height){
        this->recalculateHeight = height;
    }
    
    virtual inline void setRenderBackground(bool render){
        this->renderBackground = render;
    }
    
    virtual inline int getRenderBackground() const {
        return this->renderBackground;
    }
    
    virtual inline void setClearColor(const Graphics::Color & color){
        this->clearColor = color;
    }
    
    virtual inline const Graphics::Color & getClearColor() const {
        return this->clearColor;
    }
     
    virtual inline void setClearAlpha(int alpha){
        this->clearAlpha = alpha;
    }
    
    virtual inline int getClearAlpha() const {
        return this->clearAlpha;
    }
    
    virtual inline void updateScreenCapture(PaintownUtil::ReferenceCount<Graphics::Bitmap> bitmap){
        this->screenCapture = bitmap;
    }
    
    virtual inline void setFadeEnabled(bool fade){
        this->fadeEnabled = false;        
    }
    
    class KeysChangedException : public MugenException{
    public:
        KeysChangedException(const Mugen::PlayerType &);
        virtual ~KeysChangedException() throw();
        
        inline const Mugen::PlayerType & getType() const{
            return this->type;
        }
    private:
        Mugen::PlayerType type;
    };
    
    static PaintownUtil::ReferenceCount<Gui::ScrollItem> getPlayerKeys(int player, const std::string &, PaintownUtil::ReferenceCount<OptionMenu> menu);

private:
    //! Name
    std::string name;
    
    //! Background
    PaintownUtil::ReferenceCount<Background> background;
    
    //! Render Background
    bool renderBackground;
    
    //! Clear color
    Graphics::Color clearColor;
    
    //! Clear Alpha
    int clearAlpha;
    
    //! List
    ScrollAction list;
    
    //! Sound system
    Mugen::SoundSystem<Sounds> sounds;
    
    //! 1st or 2nd font from system.def
    PaintownUtil::ReferenceCount<Font> font;
    
    //! Fade tool
    Gui::FadeTool fader;
    
    //! Disabled/Enabled fader
    bool fadeEnabled;
    
    //! Recalculate height
    int recalculateHeight;
    
    //! Screen capture (can be updated and will render when Mugen::Background is disabled)
    PaintownUtil::ReferenceCount<Graphics::Bitmap> screenCapture;
    
};

/*! Handles Mugen Options */
class OptionOptions: public ItemOption {
public:
    OptionOptions(const std::string &name);
    
    virtual ~OptionOptions();
    void executeOption(const Mugen::PlayerType &, bool & endGame);
    
private:
    //! Menu
    PaintownUtil::ReferenceCount<OptionMenu> optionMenu;
};

/* For the top level paintown menu */
class OptionFactory: public Menu::OptionFactory {
public:
    OptionFactory();
    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const;
    virtual ~OptionFactory();
};

}

#endif
