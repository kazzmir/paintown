#ifndef _paintown_mugen_options_h
#define _paintown_mugen_options_h

#include "menu.h"
#include "util/menu/optionfactory.h"
#include "util/gui/scroll-list.h"
#include "util/pointer.h"

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
    
    virtual void draw(int x, int y, const std::string &, const Graphics::Bitmap &);
    virtual void draw(int x, int y, int position, const std::string &, const Graphics::Bitmap &);
    
    virtual int getHeight();
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
    void draw(int x, int y, const Graphics::Bitmap &, const ::Font & font, int distance) const;
    int size(const ::Font & font) const;
    
    virtual void run() = 0;
    virtual void draw(int x, int y, const Graphics::Bitmap &, const ListFont &) const = 0;
    
protected:
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
    virtual void addItem(const PaintownUtil::ReferenceCount<Gui::ScrollItem> &);

    //! Add vector of text
    virtual void addItems(const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > &);
    
    //! Get vector of text
    virtual const std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > & getItems() const;
    
    //! Clear list
    virtual void clearItems();
    
    //! Get current Index
    virtual unsigned int getCurrentIndex() const;

    //! Next
    virtual bool next();

    //! Previous
    virtual bool previous() ;
    
    enum ExpandState{
        Disabled,
        Retract,
        Expand,
    };
    
    //! Toggle expanding in and out of view
    virtual void setExpandState(const ExpandState &);
    
    //! List Font
    virtual void setListFont(const ListFont &);
    
    //! Active Font
    virtual void setActiveFont(const ListFont &);
    
    //! Set visible items (defaults to 5)
    virtual inline void setVisibleItems(int items){
        this->visibleItems = items;
    }
    
    //! Set autospacing (ignores set spacing)
    virtual inline void setAutoSpacing(bool spacing){
        this->autoSpacing = spacing;
    }
    
    //! Set Spacing
    virtual inline void setSpacing(int x, int y){
        this->spacingX = x;
        this->spacingY = y;
    }
    
protected:
    //! Check the y offset
    virtual void checkOffset();
    
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
    int visibleItems;
    
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
    
    //! Item list
    std::vector<PaintownUtil::ReferenceCount<Gui::ScrollItem> > text;
    
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

/*! Handles Mugen Options */
class OptionOptions: public ItemOption {
public:
    OptionOptions(const std::string &name);
    
    virtual ~OptionOptions();
    void executeOption(const Mugen::PlayerType &, bool & endGame);

private:

    std::vector<class Option *> options;
    std::vector<class Option *>::const_iterator selectedOption;
    
    // void doOptions(Font & font, int x, int y, const Graphics::Bitmap &);
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
