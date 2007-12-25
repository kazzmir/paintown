#ifndef _paintown_gui_container_h
#define _paintown_gui_container_h

#include <list>
#include "gui/rectarea.h"

class Bitmap;
class Widget;

class Container
{
	public:
		Container();
		virtual ~Container();
		
		// position info
		RectArea position;
		
		// Logic
		void logic();
		
		// Render
		void render(Bitmap *work);
		
		// Add widget
		void add(Widget *widget);
		
		// Remove widget
		void remove(Widget *widget);
		
	private:
		std::list<Widget *>widgets;
};

#endif
