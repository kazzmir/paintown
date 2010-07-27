#include "util/bitmap.h"
#include "tabbed-box.h"

#include "menu/menu.h"

#include "util/font.h"

#include "gui/context-box.h"

using namespace Gui;

Tab::Tab():
context(new ContextBox()){
    // Set alpha to 0 as we are not interested in the box
    context->colors.borderAlpha = 0;
    context->colors.bodyAlpha = 0;
}
Tab::~Tab(){
    delete context;
}

TabbedBox::TabbedBox():
fontWidth(24),
fontHeight(24){
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
    checkWorkArea();
    // Check if we are using a rounded box
    if (location.getRadius() > 0){
        //roundRectFill( *workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.body );
        //roundRect( *workArea, (int)location.getRadius(), 0, 0, location.getWidth()-1, location.getHeight()-1, colors.border );
    } else {
        workArea->rectangleFill( 0, fontHeight, location.getWidth()-1, location.getHeight()-1, colors.body );
        workArea->rectangle( 0, fontHeight, location.getWidth()-1, location.getHeight()-1, colors.border );
    }
    Bitmap::transBlender( 0, 0, 0, colors.bodyAlpha );
    
    workArea->drawTrans(location.getX(), location.getY(), work);
}


// Add tab
void TabbedBox::addTab(const std::vector<ContextItem *> & list){
}

unsigned int TabbedBox::getCurrentIndex() const {
    return this->tabs[current]->context->getCurrentIndex();
}
