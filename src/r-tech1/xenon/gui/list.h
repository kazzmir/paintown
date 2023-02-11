#ifndef _gui_list_h
#define _gui_list_h

#include "widget.h"
#include "r-tech1/pointer.h"
#include "r-tech1/font.h"

namespace Gui{
class ListItem{
public:
    ListItem();
    virtual ~ListItem();
    
    virtual bool operator==(const ListItem &) = 0;
    virtual bool equals(Util::ReferenceCount<ListItem>) = 0;
    virtual int compareTo(Util::ReferenceCount<ListItem>) = 0;
    virtual void act() = 0;
    virtual void draw(const Font &, const Graphics::Bitmap &) = 0;
};

class ListInterface : public Gui::Widget{
public:
    ListInterface();
    virtual ~ListInterface();
    
    virtual void act(const Font &) = 0;
    virtual void draw(const Font &, const Graphics::Bitmap &) = 0;
    
    virtual void add(const Util::ReferenceCount<ListItem>) = 0;
    virtual void add(const std::vector< Util::ReferenceCount<ListItem> > &) = 0;
    virtual void remove(const Util::ReferenceCount<ListItem>) = 0;
    virtual void replace(const std::vector< Util::ReferenceCount<ListItem> > &) = 0;
    virtual void clear() = 0;
    virtual const std::vector< Util::ReferenceCount<ListItem> > & getList() const = 0;
private:
    void render(const Graphics::Bitmap &);
    void render(const Graphics::Bitmap &, const Font &);
};

class SimpleList : public ListInterface{
public:
    SimpleList();
    virtual ~SimpleList();
    
    void act(const Font &);
    void draw(const Font &, const Graphics::Bitmap &);
    
    void add(const Util::ReferenceCount<ListItem>);
    void add(const std::vector< Util::ReferenceCount<ListItem> > &);
    void remove(const Util::ReferenceCount<ListItem>);
    void replace(const std::vector< Util::ReferenceCount<ListItem> > &);
    void clear();
    const std::vector< Util::ReferenceCount<ListItem> > & getList() const;
protected:
    std::vector< Util::ReferenceCount<ListItem> > list;
};
}

#endif
