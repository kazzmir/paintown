#include "r-tech1/gui/container.h"

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/gui/widget.h"

using namespace Gui;

Container::Container()
{
    // Nothing yet
}

Container::~Container()
{
    // Nothing yet
}

// Add widget
void Container::add(Widget *widget)
{
    widgets.push_back(widget);
}

// Remove widget
void Container::remove(Widget *widget)
{
    widgets.remove(widget);
}

// Logic
void Container::act(const Font & font){
    /*
    std::list<Widget *>::iterator i = widgets.begin(), end = widgets.end();
    while(i!=end){
        (*i)->act(font);
        i++;
    }
    */
    for (Widget* widget: widgets){
        widget->act(font);
    }

}

// Render
void Container::render(const Graphics::Bitmap & work)
{
    // Graphics::Bitmap sub(work, position.x, position.y, position.getX2(), position.getY2());
    work.setClipRect(position.x,position.y,position.getX2(),position.getY2());
    // std::list<Widget *>::iterator i = widgets.begin(), end = widgets.end();
    for (Widget* widget: widgets){
        widget->render(work);
    }
    work.setClipRect(0,0,work.getWidth(),work.getHeight());
}
