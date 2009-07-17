#ifndef _paintown_gui_rectarea_h
#define _paintown_gui_rectarea_h

class Bitmap;

struct RectArea
{
	RectArea();
	RectArea(int x, int y, int w, int h); 
	RectArea( const RectArea &r );
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
	inline const int getX2() const { return x + width; }
	inline const int getY2() const { return y + height; }
	bool operator==( const RectArea &);
	bool operator!=( const RectArea &);
	bool operator==( const Bitmap &);
	bool operator!=( const Bitmap &);
	RectArea &operator=( const RectArea &);
};

#endif
