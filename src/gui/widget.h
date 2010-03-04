#ifndef _paintown_gui_widget_h
#define _paintown_gui_widget_h

#include "gui/rectarea.h"

class Bitmap;

namespace Gui{

class Widget{
	public:
		Widget();
		Widget( const Widget & w );
		virtual ~Widget();
		
		// copy
		Widget &operator=( const Widget &);
		
		// position info
		RectArea position;
		
		// Logic
		virtual void logic()=0;
		
		// Render
		virtual void render(const Bitmap *work)=0;
	
	protected:
		void arc( Bitmap *work, int x, int y, double startAngle, int radius, int color );
		void roundRect( Bitmap *work, int radius, int x1, int y1, int x2, int y2, int color );
		void roundRectFill( Bitmap *work, int radius, int x1, int y1, int x2, int y2, int color );
		
		void checkWorkArea();
		
		Bitmap *workArea;
};

}

#endif
