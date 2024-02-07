#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/gui/tabbed-box.h"

#include "r-tech1/font.h"

#include "r-tech1/gui/context-box.h"

using namespace Gui;

Tab::Tab():
active(false){
    // Set alpha to 0 as we are not interested in the box
    context.setRenderOnlyText(true);
}

Tab::~Tab(){
}
    
void Tab::addOption(const Util::ReferenceCount<ContextItem> & item){
    context.addItem(item);
}

void Tab::render(const Graphics::Bitmap & area, const Font & font){
    context.render(area, font);
}
    
void Tab::act(const Font & font){
    context.act(font);
}
    
void Tab::setList(const std::vector<Util::ReferenceCount<ContextItem> > & list){
    context.setList(list);
}

void Tab::setName(const std::string & name){
    this->name = name;
}

void Tab::close(){
    context.close();
}

void Tab::open(){
    context.open();
}

void Tab::previous(const Font & font){
    context.previous(font);
}

void Tab::next(const Font & font){
    context.next(font);
}

void Tab::adjustLeft(){
    context.adjustLeft();
}

void Tab::adjustRight(){
    context.adjustRight();
}

int Tab::getCurrentIndex(){
    return context.getCurrentIndex();
}

TabbedBox::TabbedBox():
current(0),
fontWidth(24),
fontHeight(24),
inTab(false),
tabWidthMax(0),
tabFontColor(Graphics::makeColor(255,255,255)),
currentTabFontColor(Graphics::makeColor(0,0,255)){
    activeTabFontColor = new Effects::Gradient(50, tabFontColor, currentTabFontColor);
}

TabbedBox::TabbedBox(const TabbedBox & b):
Widget(),
activeTabFontColor(NULL){
    this->location = b.location;
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

    return *this;
}

// Logic
void TabbedBox::act(const Font & font){
    if (!tabs.empty()){
        const int width = font.textLength(tabs[current]->name.c_str()) + 5;
        if (tabs.size() > 1){
            tabWidthMax = (location.getWidth() - width) / (tabs.size() - 1);
        } else {
            tabWidthMax = location.getWidth() - width;
        }
    } else {
        return;
    }
    if (!tabs[current]->active){
        tabs[current]->active = true;
    }
    tabs[current]->act(font);
    if (inTab){
        if (activeTabFontColor){
            activeTabFontColor->update();
        }
    }
}
        
void TabbedBox::render(const Graphics::Bitmap & work){
    /* nothing */
}

// Render
void TabbedBox::render(const Graphics::Bitmap & work, const Font & font){
    const int tabHeight = fontHeight + 5;
    // checkWorkArea();
    Graphics::Bitmap area(work, location.getX(), location.getY(), location.getWidth(), location.getHeight());
    // Check if we are using a rounded box
    if (transforms.getRadius() > 0){
        const int offset = transforms.getRadius() * 4;

        /* FIXME: what should translucency be here? */
        Graphics::TranslucentBitmap temp(Graphics::Bitmap(area, 0, location.getHeight() - offset/2, location.getWidth(), offset/2), 255);
        temp.roundRectFill((int)transforms.getRadius(), 0, offset/2 * -1, location.getWidth()-1, (offset/2)-1, colors.body);
        temp.roundRect((int)transforms.getRadius(), 0, offset/2 * -1, location.getWidth()-1, (offset/2)-1, colors.border);

        /*
        Graphics::Bitmap temp(location.getWidth(), offset/2);
        temp.clearToMask();
        temp.roundRectFill((int)transforms.getRadius(), 0, offset/2 * -1, location.getWidth()-1, (offset/2)-1, colors.body);
        temp.roundRect((int)transforms.getRadius(), 0, offset/2 * -1, location.getWidth()-1, (offset/2)-1, colors.border);
        temp.translucent().draw(0,location.getHeight() - offset/2, area);
        */
        
        area.translucent(255).rectangleFill(0, tabHeight+1, location.getWidth()-1, location.getHeight()-1 - offset/2, colors.body );
        area.translucent(255).vLine(tabHeight,0,location.getHeight()-1 - offset/2,colors.border);
        area.translucent(255).vLine(tabHeight,location.getWidth()-1,location.getHeight()-1 - offset/2,colors.border);
    } else {
        area.translucent(255).rectangleFill(0, tabHeight+1, location.getWidth()-1, location.getHeight()-1, colors.body );
        area.translucent(255).vLine(tabHeight,0,location.getHeight()-1,colors.border);
        area.translucent(255).hLine(0,location.getHeight()-1,location.getWidth()-1,colors.border);
        area.translucent(255).vLine(tabHeight,location.getWidth()-1,location.getHeight()-1,colors.border);
    }
    
    tabs[current]->render(area, font);
    
    renderTabs(area, font);
    
    /* FIXME: only render the background in translucent mode, the text should
     * not be translucent
     */
    // workArea->draw(location.getX(), location.getY(), work);
}

// Add tab
void TabbedBox::addTab(const std::string & name, const std::vector<Util::ReferenceCount<ContextItem> > & list){
    for (std::vector<Tab *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Tab * tab = *i;
        if (tab->name == name){
            return;
        }
    }
    Tab * tab = new Tab();
    tab->name = name;
    tab->setList(list);
    
    addTab(tab);
}

void TabbedBox::addTab(Tab * tab){
    const int modifier = fontHeight * .35;
    tab->getContext().location.setPosition(Gui::AbsolutePoint(0, fontHeight + modifier));
    tab->getContext().location.setPosition2(Gui::AbsolutePoint(location.getWidth(), location.getHeight()- modifier));
    tab->open();
    tabs.push_back(tab);
}

void TabbedBox::moveTab(int direction){
    tabs[current]->close();
    tabs[current]->active = false;
    current = (unsigned int) ((int)current + direction + (int) tabs.size()) % tabs.size();
    tabs[current]->open();
    tabs[current]->active = true;
}

void TabbedBox::up(const Font & font){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        moveTab(-1);
    } else {
        tabs[current]->previous(font);
    }
}

void TabbedBox::down(const Font & font){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        moveTab(1);
    } else {
        tabs[current]->next(font);
    }
}

void TabbedBox::left(const Font & font){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        moveTab(-1);
    } else {
        tabs[current]->adjustLeft();
    }
}

void TabbedBox::right(const Font & font){
    if (tabs.size() == 0){
        return;
    }
    if (!inTab){
        moveTab(1);
    } else {
        tabs[current]->adjustRight();
    }
}

void TabbedBox::toggleTabSelect(){
    inTab = !inTab;
}

unsigned int TabbedBox::getCurrentIndex() const {
    if (tabs.size() == 0){
        return 0;
    }
    return this->tabs[current]->getCurrentIndex();
}

void TabbedBox::setTabFontColor(Graphics::Color color){
    tabFontColor = color;
    if (activeTabFontColor){
        delete activeTabFontColor;
    }
    activeTabFontColor = new Effects::Gradient(50, tabFontColor, currentTabFontColor);
}

void TabbedBox::setSelectedTabFontColor(Graphics::Color color){
    currentTabFontColor = color;
    if (activeTabFontColor){
        delete activeTabFontColor;
    }
    activeTabFontColor = new Effects::Gradient(50, tabFontColor, currentTabFontColor);
}

void TabbedBox::renderTabs(const Graphics::Bitmap & bmp, const Font & vFont){
    const int tabHeight = fontHeight + 5;
    
    int x = 0;
    // Graphics::Bitmap::transBlender(0, 0, 0, colors.bodyAlpha);
    
    for (std::vector<Gui::Tab *>::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Gui::Tab * tab = *i;
        const int textWidth = vFont.textLength(tab->name.c_str()) + 5;
        // for last tab
        int modifier = 0;
        // Check last tab so we can ensure proper sizing
        if (i == (tabs.begin() + tabs.size() -1)){
            if (((tabWidthMax * (tabs.size() - 1)) + textWidth) != (unsigned int) location.getWidth()){
                modifier = location.getWidth() - x - (tab->active ? textWidth : tabWidthMax);
            }
        }
        
        if (transforms.getRadius() > 0){
            const int offset = transforms.getRadius() * 4;
            if (tab->active){
                Graphics::Bitmap area(bmp, x, 0, x+textWidth+modifier-1, tabHeight+1);
                if (!inTab){
                    area.translucent(colors.bodyAlpha).roundRectFill((int)transforms.getRadius(), 0, 0, textWidth+modifier-1, tabHeight*2, colors.body);
                    area.translucent(colors.bodyAlpha).roundRect((int)transforms.getRadius(), 0, 0, textWidth+modifier-1, tabHeight*2, colors.border);
                    vFont.printf((((textWidth + modifier)/2)-(((textWidth + modifier) - 5)/2)), 0, currentTabFontColor, area, tab->name, 0 );
                } else {
                    area.translucent(colors.bodyAlpha).roundRectFill((int)transforms.getRadius(), 0, 0, textWidth+modifier-1, tabHeight*2, colors.body);
                    area.translucent(colors.bodyAlpha).roundRect((int)transforms.getRadius(), 0, 0, textWidth+modifier-1, tabHeight*2, colors.border);
                    vFont.printf((((textWidth + modifier)/2)-(((textWidth + modifier) - 5)/2)), 0, activeTabFontColor->current(), area, tab->name, 0 );
                }
                x += textWidth + modifier;
            } else {
                const int heightMod = tabHeight * .15;

                int width = x+tabWidthMax+modifier-1;
                int height = tabHeight-heightMod;

                if (width > 1 && height > 1){
                    Graphics::Bitmap area(bmp, x, 1 + heightMod, width, height);
                    area.translucent(colors.bodyAlpha).roundRectFill((int)transforms.getRadius(),0,0,tabWidthMax+modifier-1,tabHeight*2,Graphics::makeColor(105, 105, 105));
                    area.translucent(colors.bodyAlpha).roundRect((int)transforms.getRadius(),0,0,tabWidthMax+modifier-1,tabHeight*2,Graphics::makeColor(58, 58, 58));
                    area.translucent(colors.bodyAlpha).hLine(0,tabHeight,tabWidthMax+modifier-1,colors.border);
                    vFont.printf((((tabWidthMax + modifier)/2)-((textWidth + modifier)/2)), 0, tabFontColor, area, tab->name, 0 );
                }
                x += tabWidthMax + modifier;
            }
        } else {
            if (tab->active){
                Graphics::Bitmap area(bmp, x, 0, x+textWidth+modifier-1, tabHeight+1);
                if (!inTab){
                    area.translucent(colors.bodyAlpha).rectangleFill(0, 0, textWidth+modifier-1, tabHeight*2, colors.body);
                    area.translucent(colors.bodyAlpha).rectangle(0, 0, textWidth+modifier-1, tabHeight*2, colors.border);
                    vFont.printf((((textWidth + modifier)/2)-(((textWidth + modifier) - 5)/2)), 0, currentTabFontColor, area, tab->name, 0 );
                } else {
                    area.translucent(colors.bodyAlpha).rectangleFill(0, 0, textWidth+modifier-1, tabHeight*2, colors.body);
                    area.translucent(colors.bodyAlpha).rectangle(0, 0, textWidth+modifier-1, tabHeight*2, colors.border);
                    vFont.printf((((textWidth + modifier)/2)-(((textWidth + modifier) - 5)/2)), 0, activeTabFontColor->current(), area, tab->name, 0 );
                }
                x += textWidth + modifier;
            } else {
                const int heightMod = tabHeight * .15;
                Graphics::Bitmap area(bmp, x, 1 + heightMod, x+tabWidthMax+modifier-1, tabHeight-heightMod);
                area.translucent(colors.bodyAlpha).rectangleFill(0,0,tabWidthMax+modifier-1,tabHeight*2,Graphics::makeColor(105, 105, 105));
                area.translucent(colors.bodyAlpha).rectangle(0,0,tabWidthMax+modifier-1,tabHeight*2,Graphics::makeColor(58, 58, 58));
                area.translucent(colors.bodyAlpha).hLine(0,tabHeight,tabWidthMax+modifier-1,colors.border);
                vFont.printf((((tabWidthMax + modifier)/2)-((textWidth + modifier)/2)), 0, tabFontColor, area, tab->name, 0 );
                x += tabWidthMax + modifier;
            }
        }
    }
}
