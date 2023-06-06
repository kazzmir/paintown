#ifndef _gui_select_list_h
#define _gui_select_list_h

#include <string>
#include <vector>

#include "coordinate.h"
#include "r-tech1/pointer.h"

class Font;

namespace Gui{

/*! Select Item pure virtual interface */
class SelectItem{
public:
    SelectItem();
    virtual ~SelectItem();
    
    virtual void draw(int x, int y, int width, int height, const Graphics::Bitmap &, const Font &) const = 0;
    virtual bool isEmpty() const = 0;
};

/*! Select List Interface */
class SelectListInterface{
public:
    SelectListInterface();
    virtual ~SelectListInterface();

    //! Logic
    virtual void act() = 0;

    //! Render
    virtual void render(const Graphics::Bitmap &, const Font &) const = 0;

    //! Add item
    virtual void addItem(const Util::ReferenceCount<SelectItem> &) = 0;

    //! Add vector of items
    virtual void addItems(const std::vector<Util::ReferenceCount<SelectItem> > &) = 0;
    
    //! Get vector of items
    virtual const std::vector<Util::ReferenceCount<SelectItem> > & getItems() const = 0;
    
    //! Get specific item
    virtual const Util::ReferenceCount<SelectItem> getItem(unsigned int index) const = 0;
    
    //! Get specific item
    virtual const Util::ReferenceCount<SelectItem> getItemByCursor(unsigned int index) const = 0;
    
    virtual void clearItems() = 0;
    
    virtual void setCellDimensions(int width, int height) = 0;
    virtual void setCellSpacing(int x, int y) = 0;
    virtual void setCellMargins(int x, int y) = 0;
    virtual void setStartingOffset(int x, int y) = 0;
    
    virtual void setCursors(int total) = 0;
    virtual int totalCursors() const = 0;
    
    enum CursorState{
        Disabled,
        Active,
        Done,
        Invalid,
    };

    /* Get the location in pixels of an item where it would be drawn */
    virtual void getDrawLocation(const Util::ReferenceCount<SelectItem> & item, int * x, int * y) const = 0;
    
    virtual void setCurrentIndex(unsigned int cursor, unsigned int location) = 0;
    virtual unsigned int getCurrentIndex(unsigned int cursor) const = 0;
    virtual void setCurrentState(unsigned int cursor, const CursorState &) = 0;
    virtual CursorState getCurrentState(unsigned int cursor) const = 0;
    virtual bool up(unsigned int cursor) = 0;
    virtual bool down(unsigned int cursor) = 0;
    virtual bool left(unsigned int cursor) = 0;
    virtual bool right(unsigned int cursor) = 0;
    
    //! Has more low/high
    virtual bool hasMoreLow() const = 0;
    virtual bool hasMoreHigh() const = 0;
    
    //! Get dimensions
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    
    //! Access Empty
    virtual inline void setAccessEmpty(bool access){
        this->accessEmpty = access;
    }
    //! Get Occupy empty
    virtual inline bool getAccessEmpty() const {
        return this->accessEmpty;
    }
    
    //! Draw Empty
    virtual inline void setDrawEmpty(bool draw){
        this->drawEmpty = draw;
    }
    //! Get Draw empty
    virtual inline bool getDrawEmpty() const {
        return this->drawEmpty;
    }
    
    //! Set wrap
    virtual inline void setWrap(bool wrap){
        this->allowWrap = wrap;
    }
    //! Get wrap
    virtual inline bool getWrap() const {
        return this->allowWrap;
    }
    
protected:
    
    //! Can occupy empty cells/spaces
    bool accessEmpty;
    
    //! Can draw empty cells
    bool drawEmpty;
    
    //! Is wrappable
    bool allowWrap;
};

/*! Cursor handler */
class Cursor{
public:
    Cursor(unsigned int index, const SelectListInterface::CursorState &);
    ~Cursor();
    Cursor(const Cursor &);
    const Cursor & operator=(const Cursor &);
    void setIndex(unsigned int index);
    unsigned int getIndex() const;
    void increment();
    void decrement();
    void setState(const SelectListInterface::CursorState &);
    const SelectListInterface::CursorState & getState() const;
private:
    unsigned int index;
    SelectListInterface::CursorState state;
};

/*! Simple list */
class SimpleSelect: public SelectListInterface {
public:
    SimpleSelect();
    virtual ~SimpleSelect();
    
    virtual void act();
    virtual void render(const Graphics::Bitmap &, const Font &) const;
    virtual void addItem(const Util::ReferenceCount<SelectItem> &);
    virtual void addItems(const std::vector<Util::ReferenceCount<SelectItem> > &);
    virtual const std::vector<Util::ReferenceCount<SelectItem> > & getItems() const;
    virtual const Util::ReferenceCount<SelectItem> getItem(unsigned int index) const;
    virtual const Util::ReferenceCount<SelectItem> getItemByCursor(unsigned int cursor) const;
    virtual void clearItems();
    virtual void setCellDimensions(int width, int height);
    virtual void setCellSpacing(int x, int y);
    virtual void setCellMargins(int x, int y);
    virtual void setStartingOffset(int x, int y);
    virtual void setCursors(int total);
    virtual int totalCursors() const;
    virtual void setCurrentIndex(unsigned int cursor, unsigned int location);
    virtual unsigned int getCurrentIndex(unsigned int cursor) const;
    virtual void setCurrentState(unsigned int cursor, const SelectListInterface::CursorState &);
    virtual SelectListInterface::CursorState getCurrentState(unsigned int cursor) const;
    virtual void getDrawLocation(const Util::ReferenceCount<SelectItem> & item, int * x, int * y) const;
    virtual bool up(unsigned int cursor);
    virtual bool down(unsigned int cursor);
    virtual bool left(unsigned int cursor);
    virtual bool right(unsigned int cursor);
    
    virtual bool hasMoreLow() const;
    virtual bool hasMoreHigh() const;
    
    virtual int getWidth();
    virtual int getHeight();
    
    virtual void setViewable(unsigned int viewable){
        this->viewable = viewable;
    }
    virtual unsigned int getViewable() const {
        return this->viewable;
    }
    
    enum Layout {
        Vertical,
        Horizontal,
    };
    
    virtual void setLayout(const Layout & layout){
        this->layout = layout;
    }
    virtual const Layout & getLayout() const {
        return this->layout;
    }
    
    
    virtual void setScrollOffset(int offset){
        this->scrollOffset = offset;
    }
    virtual int getScrollOffset() const {
        return this->scrollOffset;
    }
    
protected:
    bool checkCursor(unsigned int cursor) const;
    void calculateLeft(unsigned int cursor);
    void calculateRight(unsigned int cursor);
    Layout layout;
    unsigned int viewable;
    unsigned int currentTop;
    int scrollOffset;
    int cellWidth, cellHeight;
    int cellSpacingX, cellSpacingY;
    int cellMarginX, cellMarginY;
    int startOffsetX, startOffsetY;
    std::vector<Cursor> cursors;
    std::vector<Util::ReferenceCount<SelectItem> > items;
};

/*! Select Grid */
class GridSelect: public SelectListInterface {
public:
    GridSelect();
    virtual ~GridSelect();
    
    virtual void act();
    virtual void render(const Graphics::Bitmap &, const Font &) const;
    virtual void addItem(const Util::ReferenceCount<SelectItem> &);
    virtual void addItems(const std::vector<Util::ReferenceCount<SelectItem> > &);
    virtual const std::vector<Util::ReferenceCount<SelectItem> > & getItems() const;
    virtual const Util::ReferenceCount<SelectItem> getItem(unsigned int index) const;
    virtual const Util::ReferenceCount<SelectItem> getItemByCursor(unsigned int cursor) const;
    virtual void clearItems();
    virtual void setCellDimensions(int width, int height);
    virtual void setCellSpacing(int x, int y);
    virtual void setCellMargins(int x, int y);
    virtual void setStartingOffset(int x, int y);
    virtual void setCursors(int total);
    virtual int totalCursors() const;
    virtual void setCurrentIndex(unsigned int cursor, unsigned int location);
    virtual unsigned int getCurrentIndex(unsigned int cursor) const;
    virtual void setCurrentState(unsigned int cursor, const SelectListInterface::CursorState &);
    virtual SelectListInterface::CursorState getCurrentState(unsigned int cursor) const;
    virtual void getDrawLocation(const Util::ReferenceCount<SelectItem> & item, int * x, int * y) const;
    virtual bool up(unsigned int cursor);
    virtual bool down(unsigned int cursor);
    virtual bool left(unsigned int cursor);
    virtual bool right(unsigned int cursor);
    
    virtual bool hasMoreLow() const;
    virtual bool hasMoreHigh() const;
    
    virtual int getWidth();
    virtual int getHeight();
    
    enum Layout {
        Static,
        InfiniteHorizontal,
        InfiniteVertical,
    };
    
    virtual void setLayout(const Layout & layout){
        this->layout = layout;
    }
    virtual const Layout & getLayout() const {
        return this->layout;
    }
    
    virtual void setGridSize(int x, int y){
        this->gridX = x;
        this->gridY = y;
    }
    virtual int getGridX() const {
        return this->gridX;
    }
    virtual int getGridY() const {
        return this->gridY;
    }
    
protected:
    bool moveUp(unsigned int cursor);
    bool moveDown(unsigned int cursor);
    bool moveLeft(unsigned int cursor);
    bool moveRight(unsigned int cursor);

    bool checkCursor(unsigned int cursor) const;
    
    Layout layout;
    int gridX, gridY;
    int cellWidth, cellHeight;
    int cellSpacingX, cellSpacingY;
    int cellMarginX, cellMarginY;
    int startOffsetX, startOffsetY;
    unsigned int offset;
    std::vector<Cursor> cursors;
    std::vector<Util::ReferenceCount<SelectItem> > items;
};

}

#endif
