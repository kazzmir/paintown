#ifndef _paintown_gui_rectarea_h
#define _paintown_gui_rectarea_h

class Bitmap;

struct RectArea
{
	RectArea();
	RectArea(int x, int y, int w, int h);
	bool empty();
	int x;
	int y;
	int width; 
	int height;
	int body;
	int bodyAlpha;
	int border;
	int borderAlpha;
	int radius;
        inline int getX1() const { return x; }
        inline int getY1() const { return y; }
	inline int getX2() const { return x + width; }
	inline int getY2() const { return y + height; }
	bool operator==( const RectArea &);
	bool operator!=( const RectArea &);
	bool operator==( const Bitmap &);
	bool operator!=( const Bitmap &);
};

#endif
