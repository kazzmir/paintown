#ifndef _paintown_gui_widget_h
#define _paintown_gui_widget_h

#include "gui/rectarea.h"
#include "gui/sigslot.h"

class Bitmap;

class Widget : public sigslot::has_slots<>
{
	public:
		Widget();
		Widget( const Widget & w );
		virtual ~Widget();
		
		// position info
		RectArea position;
		
		// Logic
		virtual void logic()=0;
		
		// Render
		virtual void render(Bitmap *work)=0;
	
	protected:
		void arc( Bitmap *work, int x, int y, double startAngle, int radius, int color );
		void roundRect( Bitmap *work, int radius, int x1, int y1, int x2, int y2, int color );
		void roundRectFill( Bitmap *work, int radius, int x1, int y1, int x2, int y2, int color );
		
		void checkWorkArea();
		
		Bitmap *workArea;
};

#endif
