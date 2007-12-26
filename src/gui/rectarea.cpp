#include "gui/rectarea.h"

RectArea::RectArea() : x(0), y(0), width(0), height(0), body(0), bodyAlpha(0), border(0), borderAlpha(0), radius(0)
{
}

RectArea::RectArea(int x, int y, int w, int h)
{
	this->x  = x;
	this->y  = y;
	this->width  = width;
	this->height  = height;
}

bool RectArea::empty()
{
	return (x==0 && y==0 && width==0 && height==0);
}

