#ifndef _paintown_gui_tabbed_box_h
#define _paintown_gui_tabbed_box_h

#include <string>
#include <vector>

#include "gui/widget.h"
#include "util/file-system.h"

class Token;

namespace Gui{

class ContextBox;
class ContextItem;
    
class Tab{
    public:
        Tab();
        ~Tab();
        std::string name;
        Gui::ContextBox * context;
        bool active;
};

class TabbedBox : public Widget{
    public:
        TabbedBox();
        TabbedBox( const TabbedBox & );
        virtual ~TabbedBox();
        
        //! copy
        TabbedBox &operator=( const TabbedBox &);
        
        //! Logic
        virtual void act();
        
        //! Render
        virtual void render(const Bitmap &);
        
        //! Add tab
        virtual void addTab(const std::string &, const std::vector<ContextItem *> & list);
        
        //! Up
        virtual void up();
        
        //! Down
        virtual void down();
        
        //! Left 
        virtual void left();
        
        //! Right
        virtual void right();
	
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
        void renderTabs(const Bitmap &);
};

}

#endif
