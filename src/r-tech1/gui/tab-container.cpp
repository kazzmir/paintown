#include "r-tech1/gui/tab-container.h"

#include "r-tech1/font.h"
#include "r-tech1/debug.h"

#include <stdexcept>
#include <sstream>

namespace Gui{

TabItem::TabItem():
active(false){
}

TabItem::TabItem(const std::string & name):
active(false),
name(name){
}

TabItem::~TabItem(){
}

void TabItem::inspectBody(const Graphics::Bitmap &){
}

DummyTab::DummyTab(const std::string & name):
TabItem(name){
}

DummyTab::~DummyTab(){
}

void DummyTab::act(const Font &){
}

void DummyTab::draw(const Font &, const Graphics::Bitmap & work){
    work.fill(Graphics::makeColor(220, 220, 220));
}

TabContainer::TabContainer():
current(0),
body(640,480){
}

TabContainer::TabContainer(const TabContainer & copy):
Widget(),
tabs(copy.tabs),
current(copy.current),
body(copy.body){
}

TabContainer::~TabContainer(){
}

TabContainer & TabContainer::operator=(const TabContainer & copy){
    tabs = copy.tabs;
    current = copy.current;
    body = copy.body;
    return *this;
}

void TabContainer::act(const Font & font){
    for (std::vector< Util::ReferenceCount<TabItem> >::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Util::ReferenceCount<TabItem> tab = *i;
        tab->inspectBody(body);
        tab->act(font);
    }
}

void TabContainer::render(const Graphics::Bitmap &){
}

void TabContainer::draw(const Font & font, const Graphics::Bitmap & work){
    const int tabHeight = font.getHeight();
    const int height = location.getHeight() - tabHeight+1;
    
    // Draw tabs
    drawTabs(font, Graphics::Bitmap(work, location.getX(), location.getY(), location.getWidth(), tabHeight+1));
    
    // Draw body
    Graphics::Bitmap area(work, location.getX(), location.getY() + tabHeight+1, location.getWidth(), height);
    drawBox(transforms.getRadius(), 0, -(tabHeight+1), location.getWidth(), height, colors, area);
    
    // Draw body content
    const int modifier = (area.getWidth() * (transforms.getRadius()*.001)) == 0 ? 2 : area.getWidth() * (transforms.getRadius()*.001);
    body.drawStretched(modifier, modifier, area.getWidth() - modifier*2, area.getHeight() - modifier*2, area);
}

void TabContainer::add(Util::ReferenceCount<TabItem> tab){
    tabs.push_back(tab);
    if (tabs.size() == 1){
        tab->toggleActive();
    }
}

Util::ReferenceCount<TabItem> TabContainer::remove(unsigned int index){
    try {
        // Check
        Util::ReferenceCount<TabItem> tab = tabs.at(index);
        tabs.erase(tabs.begin() + index);
        return tab;
    } catch (const std::out_of_range & ex){
        throw new TabContainer::NoSuchTab(index); 
    }
}

void TabContainer::removeCurrent(){
    tabs.erase(tabs.begin() + current);
    if (current > 0){
        current--;
    }
}

void TabContainer::setBodySize(int width, int height){
    if (body.getWidth() == width && body.getHeight() == height){
        return;
    }
    
    body = Graphics::Bitmap(width, height);
}

void TabContainer::next(){
    tabs[current]->toggleActive();
    current = (current + 1) % tabs.size();
    tabs[current]->toggleActive();
}

void TabContainer::previous(){
    tabs[current]->toggleActive();
    if (current == 0){
        current = tabs.size()-1;
    } else {
        current--;
    }
    tabs[current]->toggleActive();
}

void TabContainer::gotoTab(unsigned int index){
    try {
        // Check
        Util::ReferenceCount<TabItem> tab = tabs.at(index);
        tabs[current]->toggleActive();
        current = index;
        tabs[current]->toggleActive();
    } catch (const std::out_of_range & ex){
        throw new TabContainer::NoSuchTab(index); 
    }
}

void TabContainer::gotoTabByName(const std::string & name){
    try {
        gotoTab(findTab(name));
    } catch (const TabContainer::NoSuchTab & ex){
        throw ex;
    }
}

TabContainer::NoSuchTab::NoSuchTab(const std::string & name) throw():
name(name){
}

TabContainer::NoSuchTab::NoSuchTab(unsigned int index) throw(){
    std::ostringstream os;
    os << index;
    name = os.str();
}

TabContainer::NoSuchTab::~NoSuchTab() throw() {
}

const char* TabContainer::NoSuchTab::what() const throw() {
    return name.c_str();
}

Util::ReferenceCount<TabItem> TabContainer::getTab(unsigned int index){
    try {
        return tabs.at(index);
    } catch (const std::out_of_range & ex){
        throw new TabContainer::NoSuchTab(index); 
    }
}

unsigned int TabContainer::findTab(const std::string & name){
    for (unsigned int i = 0; i < tabs.size(); i++){
        Util::ReferenceCount<TabItem> tab = tabs[i];
        if (name == tab->getName()){
            return i;
        }
    }
    throw TabContainer::NoSuchTab(name);
}

Util::ReferenceCount<TabItem> TabContainer::getByName(const std::string & name){
    return tabs[findTab(name)];
}

void TabContainer::drawTabs(const Font & font, const Graphics::Bitmap & work){
    if (tabs.empty()){
        drawBox(transforms.getRadius(), 0, 0, work.getWidth(), work.getHeight()*2, colors, work);
        font.printf((work.getWidth()/2) - (font.textLength("Empty")/2), 0, Graphics::makeColor(255,255,255), work, "Empty", 0);
        return;
    }
    const int width = work.getWidth() / tabs.size();
    const int inactiveY = work.getHeight() * .25;
    const int modifier = (width * (transforms.getRadius()*.005)) == 0 ? 2 : (width * (transforms.getRadius()*.005));
    int currentX = 0;
    for (std::vector< Util::ReferenceCount<TabItem> >::iterator i = tabs.begin(); i != tabs.end(); ++i){
        Util::ReferenceCount<TabItem> tab = *i;
        if (tab->isActive()){
            drawBox(transforms.getRadius(), currentX, 0, currentX + width, work.getHeight()*2, colors, work);
            Graphics::Bitmap fontArea(work, currentX + modifier, 0, width - (modifier*2), work.getHeight());
            font.printf((fontArea.getWidth()/2) - (font.textLength(tab->getName().c_str())/2), 0, Graphics::makeColor(255,255,255), fontArea, tab->getName(), 0);
            tab->draw(font, body);
        } else {
            drawBox(transforms.getRadius(), currentX, inactiveY, currentX + width, work.getHeight()*2, colors, work);
            if (colors.bodyAlpha < 255){
                // Graphics::Bitmap::transBlender(0,0,0,colors.borderAlpha);
                work.translucent(colors.borderAlpha).hLine(currentX,work.getHeight()-1,currentX + width,colors.border);
            } else {
                work.hLine(currentX,work.getHeight()-1,currentX + width,colors.border);
            }
            Graphics::Bitmap fontArea(work, currentX + modifier, inactiveY, width - (modifier*2), work.getHeight());
            font.printf((fontArea.getWidth()/2) - (font.textLength(tab->getName().c_str())/2), 0, Graphics::makeColor(255,255,255), fontArea, tab->getName(), 0);
        }
        currentX += width;
    }
}

}
