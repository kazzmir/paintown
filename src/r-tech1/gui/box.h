#ifndef _rtech1_gui_box_h
#define _rtech1_gui_box_h

#include <string>

#include "widget.h"

namespace Gui{

class Box : public Widget {
public:
    Box();
    Box( const Box & b );
    virtual ~Box();

    // copy
    Box &operator=( const Box &);

    // Logic
    virtual void act(const Font &);

    // Render
    using Widget::render;
    virtual void render(const Graphics::Bitmap &);

    // Do a message Dialog centered with respect to the given width/height
    static void messageDialog(int width, int height, const std::string & message, int radius = 0);

protected:
};

}

#endif
