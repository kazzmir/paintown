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
        
        //! Set current font
        virtual inline void setFont(const Filesystem::RelativePath &, int width, int height){
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
    
    protected:
        std::vector<Gui::Tab *> tabs;
        
        unsigned int current;
        
        Filesystem::RelativePath font;
        int fontWidth;
        int fontHeight;
        
        /*! Is in tab */
        bool inTab;
        
        /*! Render tab */
        void renderTabs(const Bitmap &);
};

}

#endif
