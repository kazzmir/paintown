#include "util/bitmap.h"
#include "gui/rectarea.h"

using namespace Gui;

RectArea::RectArea(): 
x(0),
y(0),
width(0),
height(0),
radius(0){
}

RectArea::RectArea(int x, int y, int w, int h):
x(x),
y(y),
width(w),
height(h),
radius(0){
}

bool RectArea::empty(){
    // return (x==0 && y==0 && width==0 && height==0);
    return width == 0 && height == 0;
}

bool RectArea::operator==( const RectArea &rect){
    return ( (x == rect.x) &&
            (y == rect.y) &&
            (width == rect.width) &&
            (height == rect.height));
}

bool RectArea::operator!=( const RectArea &rect){
    return ( (x != rect.x) ||
            (y != rect.y) ||
            (width != rect.width) ||
            (height != rect.height));
}

bool RectArea::operator==( const Bitmap &bmp){
    return ( (width == bmp.getWidth()) &&
            (height == bmp.getHeight()));
}

bool RectArea::operator!=( const Bitmap &bmp){
    return ( (width != bmp.getWidth()) ||
            (height != bmp.getHeight()));
}


