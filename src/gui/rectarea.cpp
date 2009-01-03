#include "gui/rectarea.h"

RectArea::RectArea() : x(0), y(0), width(0), height(0), body(0), bodyAlpha(255), border(0), borderAlpha(255), radius(0)
{
}

RectArea::RectArea(int x, int y, int w, int h){
    this->x  = x;
    this->y  = y;
    this->width  = width;
    this->height  = height;
}

RectArea::RectArea( const RectArea &r ){
    this->x = r.x;
    this->y = r.y;
    this->width = r.width;
    this->height = r.height;
}

bool RectArea::empty(){
    return (x==0 && y==0 && width==0 && height==0);
}

bool RectArea::operator==( const RectArea &rect){
    return ( (x == rect.x) &&
            (y == rect.y) &&
            (width == rect.width) &&
            (height == rect.height));
}

RectArea &RectArea::operator=( const RectArea &copy){
    x =  copy.x;
    y = copy.y;
    width = copy.width; 
    height =  copy.height;
    body =  copy.body;
    bodyAlpha =  copy.bodyAlpha;
    border = copy.border;
    borderAlpha = copy.borderAlpha;
    radius = copy.radius;

    return *this;
}
