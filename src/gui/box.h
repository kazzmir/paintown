#ifndef _paintown_gui_box_h
#define _paintown_gui_box_h

#include "gui/widget.h"

class Box : public Widget
{
	public:
		Box();
		Box( const Box & b );
		virtual ~Box();
		
		// copy
		Box &operator=( const Box &);
		
		// Logic
		virtual void logic();
		
		// Render
		virtual void render(const Bitmap *work);
	
	protected:
};

#endif
