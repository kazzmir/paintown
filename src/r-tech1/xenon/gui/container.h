#ifndef _rtech1_gui_container_h
#define _rtech1_gui_container_h

#include <list>
#include "rectarea.h"

namespace Graphics{
class Bitmap;
}
class Font;

namespace Gui{
    
class Widget;

class Container{
public:
    Container();
    virtual ~Container();

    // position info
    RectArea position;

    // Logic
    void act(const Font & font);

    // Render
    void render(const Graphics::Bitmap &);

    // Add widget
    void add(Widget *widget);

    // Remove widget
    void remove(Widget *widget);

private:
    std::list<Widget *>widgets;
};

}

#endif
