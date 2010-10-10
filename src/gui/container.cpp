#include "gui/container.h"

#include "util/bitmap.h"
#include "gui/widget.h"

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
    std::list<Widget *>::iterator i = widgets.begin(), end = widgets.end();
    while(i!=end){
        (*i)->act(font);
        i++;
    }
}

// Render
void Container::render(const Bitmap & work)
{
	work.setClipRect(position.x,position.y,position.getX2(),position.getY2());
	std::list<Widget *>::iterator i = widgets.begin(), end = widgets.end();
	while(i!=end)
	{
		(*i)->render(work);
		i++;
	}
	work.setClipRect(0,0,work.getWidth(),work.getHeight());
}
