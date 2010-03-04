#ifndef _paintown_gui_container_h
#define _paintown_gui_container_h

#include <list>
#include "gui/rectarea.h"

class Bitmap;

namespace Gui{
    
class Widget;

class Container
{
	public:
		Container();
		virtual ~Container();
		
		// position info
		RectArea position;
		
		// Logic
		void act();
		
		// Render
		void render(const Bitmap &);
		
		// Add widget
		void add(Widget *widget);
		
		// Remove widget
		void remove(Widget *widget);
		
	private:
		std::list<Widget *>widgets;
};

}

#endif
