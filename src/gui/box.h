#ifndef _paintown_gui_box_h
#define _paintown_gui_box_h

#include "gui/widget.h"

class Box : public Widget
{
	public:
		Box();
		virtual ~Box();
		
		// Logic
		virtual void logic();
		
		// Render
		virtual void render(Bitmap *work);
	
	protected:
};

#endif
