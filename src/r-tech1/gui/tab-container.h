#ifndef _gui_tab_container_h
#define _gui_tab_container_h

#include <string>
#include <vector>
#include <exception>

#include "widget.h"
#include "r-tech1/file-system.h"
#include "r-tech1/pointer.h"
#include "r-tech1/graphics/gradient.h"

namespace Gui{

class TabItem{
public:
    TabItem();
    TabItem(const std::string &);
    virtual ~TabItem();
    virtual void inspectBody(const Graphics::Bitmap &);
    virtual void act(const Font &) = 0;
    virtual void draw(const Font &, const Graphics::Bitmap &) = 0;
    virtual inline void setName(const std::string & name) {
        this->name = name;
    } 
    virtual inline const std::string & getName() const {
        return this->name;
    } 
    virtual inline bool isActive() const {
        return this->active;
    }
    virtual inline void toggleActive() {
        this->active = !this->active;
    }
protected:
    bool active;
    std::string name;    
};

class DummyTab: public TabItem{
public:
    DummyTab(const std::string &);
    virtual ~DummyTab();
    virtual void act(const Font &);
    virtual void draw(const Font &, const Graphics::Bitmap &);
};
    

class TabContainer: public Widget {
public:
    TabContainer();
    TabContainer(const TabContainer &);
    virtual ~TabContainer();
        
    // copy
    TabContainer & operator=(const TabContainer &);
        
    // Logic
    virtual void act(const Font &);
    
    // Render
    using Widget::render;
    virtual void render(const Graphics::Bitmap &);
    virtual void draw(const Font &, const Graphics::Bitmap &);
    
    // Add
    virtual void add(Util::ReferenceCount<TabItem> tab);
    
    // Remove
    virtual Util::ReferenceCount<TabItem> remove(unsigned int tab);
    void removeCurrent();
    
    // Set size of Content Body Area
    virtual void setBodySize(int width, int height);
    
    // Next
    virtual void next();
    
    // Previous
    virtual void previous();
    
    // Go to specific tab
    virtual void gotoTab(unsigned int index);
    
    // Go to specific tab by name
    virtual void gotoTabByName(const std::string &);
    
    // Get the Content Body Area
    virtual inline const Graphics::Bitmap & getBody() const {
        return this->body;
    }
        
    // Empty
    virtual inline bool empty() const {
        return this->tabs.empty();
    }
    
    // Get current tab
    virtual inline Util::ReferenceCount<TabItem> getCurrent() {
        return this->tabs[current];
    }
    
    class NoSuchTab: public std::exception{
    public:
        NoSuchTab(const std::string &) throw();
        NoSuchTab(unsigned int index) throw();
        virtual ~NoSuchTab() throw();
        virtual const char* what() const throw();
    private:
        std::string name;
    };
    
    virtual unsigned int findTab(const std::string &);
    virtual Util::ReferenceCount<TabItem> getTab(unsigned int index);
    virtual Util::ReferenceCount<TabItem> getByName(const std::string &);
    
protected: 

    virtual void drawTabs(const Font &, const Graphics::Bitmap &);

    std::vector< Util::ReferenceCount<TabItem> > tabs;
    
    unsigned int current;
    
    // Body defaults to 640x480
    Graphics::Bitmap body;
    
    /*! Gradient for active selection */
    Effects::Gradient  * activeColor;
};

}

#endif
