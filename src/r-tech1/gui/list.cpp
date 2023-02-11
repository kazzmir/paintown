#include "r-tech1/gui/list.h"

namespace Gui{
    
ListItem::ListItem(){
}

ListItem::~ListItem(){
}

ListInterface::ListInterface(){
}

ListInterface::~ListInterface(){
}

void ListInterface::render(const Graphics::Bitmap &){
}

void ListInterface::render(const Graphics::Bitmap &, const Font &){
}

SimpleList::SimpleList(){
}

SimpleList::~SimpleList(){
}

void SimpleList::act(const Font &){
    
}

void SimpleList::draw(const Font & font, const Graphics::Bitmap & work){
    
    Graphics::Bitmap temp = Graphics::Bitmap(work, location.getX(), location.getY(), location.getWidth(), location.getHeight());
    
    drawBox(transforms.getRadius(), 0, 0, location.getWidth(), location.getHeight(), colors, temp);
    
    const int modifier = (temp.getWidth() * (transforms.getRadius()*.001)) == 0 ? 2 : (temp.getWidth() * (transforms.getRadius()*.001));

    int y = 2;
    for (std::vector< Util::ReferenceCount<ListItem> >::iterator i = list.begin(); i != list.end(); ++i){
        Graphics::Bitmap listTemp = Graphics::Bitmap(temp, modifier, y, temp.getWidth()-(modifier*2), y);
        Util::ReferenceCount<ListItem> item = *i;
        item->draw(font, listTemp);
        y += font.getHeight()+2;
    }
}

void SimpleList::add(const Util::ReferenceCount<ListItem> item){
    //list.push_back(item);
    // Keep list sorted
    std::vector< Util::ReferenceCount<ListItem> >::iterator position = list.begin();
    while (position != list.end() && (*position)->compareTo(item) < 0){
        position++;
    }
    list.insert(position, item);
}

void SimpleList::add(const std::vector< Util::ReferenceCount<ListItem> > & more){
    /*list.insert(list.end(),more.begin(),more.end());*/
    for (std::vector< Util::ReferenceCount<ListItem> >::const_iterator i = more.begin(); i != more.end(); i++){
        Util::ReferenceCount<ListItem> item = *i;
        add(item);
    }
}

void SimpleList::remove(const Util::ReferenceCount<ListItem> removable){
    for (std::vector< Util::ReferenceCount<ListItem> >::iterator i = list.begin(); i != list.end(); i++){
        Util::ReferenceCount<ListItem> item = *i;
        if (removable->equals(item)){
            list.erase(i);
            return;
        }
    }
}

void SimpleList::replace(const std::vector< Util::ReferenceCount<ListItem> > & newList){
    list = newList;
}

void SimpleList::clear(){
    list.clear();
}

const std::vector< Util::ReferenceCount<ListItem> > & SimpleList::getList() const{
    return list;
}

}
