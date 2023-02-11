#ifndef _rtech1_gui_tabbed_box_h
#define _rtech1_gui_tabbed_box_h

#include <string>
#include <vector>

#include "widget.h"
#include "../file-system.h"
#include "../pointer.h"
#include "../graphics/gradient.h"
#include "context-box.h"

class Token;

namespace Gui{

class ContextBox;

class Tab{
public:
    Tab();
    virtual ~Tab();

    void addOption(const Util::ReferenceCount<ContextItem> & item);

    virtual void render(const Graphics::Bitmap & work, const Font & font);
    virtual void act(const Font & font);
    virtual void setList(const std::vector<Util::ReferenceCount<ContextItem> > & list);

    virtual const Gui::ContextBox & getContext() const {
        return context;
    }

    virtual Gui::ContextBox & getContext(){
        return context;
    }

    virtual void close();
    virtual void open();
    virtual void previous(const Font & font);
    virtual void next(const Font & font);
    virtual void adjustLeft();
    virtual void adjustRight();
    virtual int getCurrentIndex();

    virtual void setName(const std::string & name);

    std::string name;
    bool active;
protected:
    Gui::ContextBox context;
};

class TabbedBox: public Widget {
    public:
        TabbedBox();
        TabbedBox( const TabbedBox & );
        virtual ~TabbedBox();
        
        //! copy
        TabbedBox &operator=( const TabbedBox &);
        
        //! Logic
        virtual void act(const Font &);
        
        //! Render
        using Widget::render;
        virtual void render(const Graphics::Bitmap &);
        virtual void render(const Graphics::Bitmap &, const Font &);
        
        //! Add tab
        virtual void addTab(const std::string &, const std::vector<Util::ReferenceCount<ContextItem> > & list);
        virtual void addTab(Tab * tab);
        
        //! Up
        virtual void up(const Font &);
        
        //! Down
        virtual void down(const Font &);
        
        //! Left 
        virtual void left(const Font &);
        
        //! Right
        virtual void right(const Font &);
	
	//! Virtual
	virtual void toggleTabSelect();
        
        //! Set current font
        virtual inline void setFont(const Filesystem::RelativePath & font, int width, int height){
            this->font = font;
            this->fontWidth = width;
            this->fontHeight = height;
        }
        
        //! Get current tab
        virtual inline unsigned int getCurrentTab() const {
            return this->current;
        }
        
        //! Get current index on selected tab
        virtual unsigned int getCurrentIndex() const;
	
	//! Set tab font colors
	virtual void setTabFontColor(Graphics::Color color);
	
	//! Set selected tab font color
	virtual void setSelectedTabFontColor(Graphics::Color color);
        
        //! Empty
        virtual inline bool empty() const {
            return this->tabs.empty();
        }
        
        virtual inline bool isInTab() const {
	    return this->inTab;
	}
        
        /*! Tab Colors */
        ColorInfo tabColors;
        
        /*! Selected Tab Colors */
        ColorInfo selectedTabColors;
	
        /*! Selected Tab Colors */
        ColorInfo runningTabColors;
	
    protected:
        void moveTab(int direction);

        std::vector<Gui::Tab *> tabs;
        
        unsigned int current;
        
        Filesystem::RelativePath font;
        int fontWidth;
        int fontHeight;
        
        /*! Is in tab */
        bool inTab;
        
        /*! Max Width of tabs */
        int tabWidthMax;
        
        /*! Render tab */
        void renderTabs(const Graphics::Bitmap &, const Font &);
	
	/*! Font Color */
        Graphics::Color tabFontColor;
	
	/*! Current Tab Font Color */
        Graphics::Color currentTabFontColor;
	
	/*! Gradient for selected Font */
	Effects::Gradient  * activeTabFontColor;
};

}

#endif
