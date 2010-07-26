#ifndef _paintown_gui_tabbed_box_h
#define _paintown_gui_tabbed_box_h

#include <string>
#include <vector>

#include "gui/widget.h"
#include "util/file-system.h"

namespace Gui{

class TabbedBox : public Widget{
    public:
        TabbedBox();
        TabbedBox( const TabbedBox & );
        virtual ~TabbedBox();
        
        // copy
        TabbedBox &operator=( const TabbedBox &);
        
        // Logic
        virtual void act();
        
        // Render
        virtual void render(const Bitmap &);
        
        //! Set current font
        virtual inline void setFont(const Filesystem::RelativePath & font, int width, int height){
            this->font = font;
            this->fontWidth = width;
            this->fontHeight = height;
        }
    
    protected:
        class Tab;
        
        std::vector<Tab *> tabs;
        
        unsigned int current;
        
        Filesystem::RelativePath font;
        int fontWidth;
        int fontHeight;
};

}

#endif
