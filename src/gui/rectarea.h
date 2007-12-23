#ifndef _paintown_gui_rectarea_h
#define _paintown_gui_rectarea_h

class RectArea
{
	public:
		RectArea();
		RectArea(int x, int y, int w, int h); 
		bool empty();
		int x;
		int y;
		int width; // Really not necessary but is there in case of later need
		int height;
		int body;
		int bodyAlpha;
		int border;
		int borderAlpha;
		inline int getX2() { return x + width; }
		inline int getY2() { return y + height; }
};

#endif
