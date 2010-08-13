#include "util/bitmap.h"
#include "tabbed-box.h"

#include "menu/menu.h"

#include "util/font.h"

#include "gui/context-box.h"

using namespace Gui;

#if 0
/* FIXME add rounded tabs */
static void roundTab( const Bitmap & work, int radius, int x1, int y1, int x2, int y2, int color, bool bottom = true ){
    const int width = x2 - x1;
    const int height = y2 - y1;
    radius = Mid(0, radius, Min((x1+width - x1)/2, (y1+height - y1)/2));
    
    work.circleFill(x1+radius, y1+radius, radius, color);
    work.circleFill((x1+width)-radius, y1+radius, radius, color);
    work.circleFill(x1+radius, (y1+height)-radius, radius, color);
    work.circleFill((x1+width)-radius, (y1+height)-radius, radius, color);
    work.rectangleFill( x1+radius, y1, x2-radius, y1+radius, color);
    work.rectangleFill( x1, y1+radius, x2, y2-radius, color);
    work.rectangleFill( x1+radius, y2-radius, x2-radius, y2, color);
    
    work.line(x1+radius, y1, x1+width-radius, y1, color);
    work.line(x1+radius, y1+height, x1+width-radius,y1+height, color);
    work.line(x1, y1+radius,x1, y1+height-radius, color);
    work.line(x1+width, y1+radius,x1+width, y1+height-radius, color);

    arc(work, x1+radius, y1+radius, S_PI-1.115, radius, color);
    arc(work, x1+radius + (width - radius *2), y1+radius, -S_PI/2 +0.116, radius, color);
    arc(work, x1+width-radius, y1+height-radius, -0.110, radius ,color);
    arc(work, x1+radius, y1+height-radius, S_PI/2-0.119, radius, color);
}
#endif

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
tabWidthMax(0),
tabFontColor(Bitmap::makeColor(255,255,255)),
currentTabFontColor(Bitmap::makeColor(0,0,255)){
    activeTabFontColor = new Effects::Gradient(50, tabFontColor, currentTabFontColor);
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
    
    if (activeTabFontColor){
	delete activeTabFontColor;
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
	const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
	//tabWidthMax = location.getWidth()/tabs.size();
	const int width = vFont.textLength(tabs[current]->name.c_str()) + 5;
	tabWidthMax = (location.getWidth() - width) / (tabs.size() - 1);
    } else {
	return;
    }
    if (!tabs[current]->active){
	tabs[current]->active = true;
    }
    tabs[current]->context->act();
    if (inTab){
	if (activeTabFontColor){
	    activeTabFontColor->update();
	}
    }
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
    tab->context->location.setPosition(Gui::AbsolutePoint(0, fontHeight+5));
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
	tabs[current]->active = false;
        if (current == 0){
            current = tabs.size()-1;
        } else {
            current--;
        }
        tabs[current]->context->open();
	tabs[current]->active = true;
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
	tabs[current]->active = false;
        if (current == tabs.size()-1){
            current = 0;
        } else {
            current++;
        }
        tabs[current]->context->open();
	tabs[current]->active = true;
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
	tabs[current]->active = false;
        if (current == 0){
            current = tabs.size()-1;
        } else {
            current--;
        }
        tabs[current]->context->open();
	tabs[current]->active = true;
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
	tabs[current]->active = false;
        if (current == tabs.size()-1){
            current = 0;
        } else {
            current++;
        }
        tabs[current]->context->open();
	tabs[current]->active = true;
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

void TabbedBox::setTabFontColor(int color){
    tabFontColor = color;
    if (activeTabFontColor){
	delete activeTabFontColor;
    }
    activeTabFontColor = new Effects::Gradient(50, tabFontColor, currentTabFontColor);
}

void TabbedBox::setSelectedTabFontColor(int color){
    currentTabFontColor = color;
    if (activeTabFontColor){
	delete activeTabFontColor;
    }
    activeTabFontColor = new Effects::Gradient(50, tabFontColor, currentTabFontColor);
}

void TabbedBox::renderTabs(const Bitmap & bmp){
    const int tabHeight = fontHeight + 5;
    const Font & vFont = Font::getFont(font, fontWidth, fontHeight);
    
    int x = 0;
    
    for (std::vector<Gui::Tab *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Gui::Tab * tab = *i;
        const int textWidth = vFont.textLength(tab->name.c_str()) + 5;
	// for last tab
	int modifier = 0;
	// Check last tab so we can ensure proper sizing
	if ( i == (tabs.begin() + tabs.size() -1)){
	    if ( ( (tabWidthMax * (tabs.size() - 1) ) + textWidth ) != (unsigned int)location.getWidth() ){
		modifier = location.getWidth() - x - (tab->active ? textWidth : tabWidthMax);
	    }
	}
	
	if (tab->context->location.getRadius() > 0){
        } else {
            if (tab->active){
		if (!inTab){
		    bmp.rectangle(x, 0, x+textWidth + modifier - 1, tabHeight, colors.border);
		    bmp.rectangleFill( x+1, 1, x+textWidth + modifier - 2, tabHeight, colors.body );
		    
		    bmp.setClipRect(x, 0, x+textWidth + modifier, tabHeight-1);
		    vFont.printf(x + (((textWidth + modifier)/2)-(((textWidth + modifier) - 5)/2)), 0, currentTabFontColor, bmp, tab->name, 0 );
		} else {
		    bmp.rectangle(x, 0, x+textWidth + modifier -1, tabHeight, colors.border);
		    bmp.rectangleFill( x+1, 1, x+textWidth-2 + modifier, tabHeight, colors.body );
		    
		    bmp.setClipRect(x, 0, x+textWidth + modifier, tabHeight-1);
		    vFont.printf(x + (((textWidth + modifier)/2)-(((textWidth + modifier) - 5)/2)), 0, activeTabFontColor->current(), bmp, tab->name, 0 );
		}
		x+=textWidth + modifier;
            } else {
		bmp.rectangle(x, 0, x+tabWidthMax + modifier -1, tabHeight, tabColors.border);
                bmp.rectangleFill( x+1, 1, x+tabWidthMax + modifier -2, tabHeight-2, tabColors.body );
		
		bmp.setClipRect(x+2, 1, x+tabWidthMax + modifier -3, tabHeight-1);
		vFont.printf(x + (((tabWidthMax + modifier)/2)-((textWidth + modifier)/2)), 0, tabFontColor, bmp, tab->name, 0 );
		x+=tabWidthMax + modifier;
            }
	    bmp.setClipRect(0, 0, bmp.getWidth(), bmp.getHeight());
        }
    }
}
