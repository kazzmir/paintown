#include "util/bitmap.h"
#include "tabbed-box.h"

#include "menu/menu.h"

#include "util/font.h"

#include "gui/context-box.h"

using namespace Gui;

Tab::Tab():
context(new ContextBox()),
active(false){
    // Set alpha to 0 as we are not interested in the box
    context->colors.borderAlpha = 0;
    context->colors.bodyAlpha = 0;
}
Tab::~Tab(){
    delete context;
}

TabbedBox::TabbedBox():
current(0),
fontWidth(24),
fontHeight(24),
inTab(false){
}

TabbedBox::TabbedBox( const TabbedBox & b ){
    this->location = b.location;
    this->workArea = b.workArea;
}

TabbedBox::~TabbedBox(){
    for (std::vector<Gui::Tab *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Gui::Tab * tab = *i;
        if (tab){
            delete tab;
        }
    }
}

TabbedBox &TabbedBox::operator=( const TabbedBox &copy){
    location = copy.location;
    workArea = copy.workArea;

    return *this;
}

// Logic
void TabbedBox::act(){
}

// Render
void TabbedBox::render(const Bitmap & work){
    const int tabHeight = fontHeight + 5;
    checkWorkArea();
    // Check if we are using a rounded box
    if (location.getRadius() > 0){
        //roundRectFill( *workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.body );
        //roundRect( *workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.border );
    } else {
        workArea->rectangleFill( 0, tabHeight, location.getWidth()-1, location.getHeight()-1, colors.body );
        workArea->rectangle( 0, tabHeight, location.getWidth()-1, location.getHeight()-1, colors.border );
    }
    Bitmap::transBlender( 0, 0, 0, colors.bodyAlpha );
    
    workArea->drawTrans(location.getX(), location.getY(), work);
}


// Add tab
void TabbedBox::addTab(const std::string & name, const std::vector<ContextItem *> & list){
    Tab * tab = new Tab();
    tab->name = name;
    tab->context->setList(list);
}

unsigned int TabbedBox::getCurrentIndex() const {
    return this->tabs[current]->context->getCurrentIndex();
}

void TabbedBox::renderTabs(const Bitmap & bmp){
    const int tabHeight = fontHeight + 5;
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    
    int x = 0;
    /* FIXME calculate in act */
    int maxWidth = location.getWidth()/4;
    
    for (std::vector<Gui::Tab *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Gui::Tab * tab = *i;
        const int textWidth = vFont.textLength(tab->name.c_str()) + 5;
        const int width = (textWidth) > maxWidth ? maxWidth : textWidth;
    
        if (tab->context->location.getRadius() > 0){
        } else {
            bmp.rectangleFill( x, 0, location.getWidth()-1, location.getHeight()-1, colors.body );
            bmp.vLine(0, x, tabHeight, colors.border);
            bmp.hLine(x, 0, width, colors.border); 
            bmp.vLine(0, x + width, tabHeight, colors.border);
        }
        
        x+=width;
    }
}
