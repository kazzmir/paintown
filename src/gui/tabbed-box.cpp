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
inTab(false),
tabWidthMax(0){
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
    if (!tabs.empty()){
	tabWidthMax = location.getWidth()/tabs.size();
    }
    tabs[current]->context->act();
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
    
    tabs[current]->context->render(*workArea);
    
    renderTabs(*workArea);
    
    Bitmap::transBlender( 0, 0, 0, colors.bodyAlpha );
    
    workArea->drawTrans(location.getX(), location.getY(), work);
}


// Add tab
void TabbedBox::addTab(const std::string & name, const std::vector<ContextItem *> & list){
    for (std::vector<Tab *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
	Tab * tab = *i;
	if (tab->name == name){
	    return;
	}
    }
    Tab * tab = new Tab();
    tab->name = name;
    tab->context->setList(list);
    tab->context->setFont(font, fontWidth, fontHeight);
    tab->context->location.setPosition(Gui::AbsolutePoint(0, fontHeight));
    tab->context->location.setPosition2(Gui::AbsolutePoint(location.getWidth(), location.getHeight()));
    tab->context->open();
    tabs.push_back(tab);
}

void TabbedBox::up(){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        tabs[current]->context->close();
        if (current == 0){
            current = tabs.size()-1;
        } else {
            current--;
        }
        tabs[current]->context->open();
    } else {
        tabs[current]->context->previous();
    }
}

void TabbedBox::down(){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        tabs[current]->context->close();
        if (current == tabs.size()-1){
            current = 0;
        } else {
            current++;
        }
        tabs[current]->context->open();
    } else {
        tabs[current]->context->next();
    }
}

void TabbedBox::left(){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        tabs[current]->context->close();
        if (current == 0){
            current = tabs.size()-1;
        } else {
            current--;
        }
        tabs[current]->context->open();
    } else {
        tabs[current]->context->adjustLeft();
    }
}

void TabbedBox::right(){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        tabs[current]->context->close();
        if (current == tabs.size()-1){
            current = 0;
        } else {
            current++;
        }
        tabs[current]->context->open();
    } else {
        tabs[current]->context->adjustRight();
    }
}

void TabbedBox::toggleTabSelect(){
    inTab = !inTab;
}

unsigned int TabbedBox::getCurrentIndex() const {
    if (tabs.size() == 0){
        return 0;
    }
    return this->tabs[current]->context->getCurrentIndex();
}

void TabbedBox::renderTabs(const Bitmap & bmp){
    const int tabHeight = fontHeight + 5;
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    
    int x = 0;
    
    for (std::vector<Gui::Tab *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Gui::Tab * tab = *i;
        const int textWidth = vFont.textLength(tab->name.c_str()) + 5;
        const int width = (textWidth) > tabWidthMax ? tabWidthMax : textWidth;
        
        if (tab->context->location.getRadius() > 0){
        } else {
            if (tab->active){
                bmp.rectangleFill( x, 0, location.getWidth()-1, tabHeight, selectedTabColors.body );
                bmp.vLine(0, x, tabHeight, selectedTabColors.border);
                bmp.hLine(x, 0, width, selectedTabColors.border); 
                bmp.vLine(0, x + width, tabHeight, selectedTabColors.border);
            } else {
                bmp.rectangleFill( x, 0, location.getWidth()-1, tabHeight-1, tabColors.body );
                bmp.vLine(0, x, tabHeight, tabColors.border);
                bmp.hLine(x, 0, width, tabColors.border); 
                bmp.vLine(0, x + width, tabHeight, tabColors.border);
            }
            /* FIXME font color */
            vFont.printf(x + ((width/2)-(textWidth/2)), 0, Bitmap::makeColor(255,255,255), bmp, tab->name, 0 );
        }
        
        x+=width;
    }
}
