#ifndef _paintown_gui_widget_h
#define _paintown_gui_widget_h

#include "gui/rectarea.h"

class Bitmap;

class Widget
{
	public:
		Widget();
		virtual ~Widget();
		
		// position info
		RectArea position;
		
		// Logic
		virtual void logic()=0;
		
		// Render
		virtual void render(Bitmap *work)=0;
};

#endif
