#ifndef _rtech1_gui_rectarea_h
#define _rtech1_gui_rectarea_h

namespace Graphics{
class Bitmap;
}

namespace Gui{

struct RectArea{
    RectArea();
	RectArea(int x, int y, int w, int h);
    inline int getX1() const { return x; }
    inline int getY1() const { return y; }
	inline int getX2() const { return x + width; }
	inline int getY2() const { return y + height; }
	bool operator==( const RectArea &);
    bool operator!=( const RectArea &);
	bool operator==( const Graphics::Bitmap &);
	bool operator!=( const Graphics::Bitmap &);
	bool empty();
	int x;
	int y;
	int width; 
	int height;
	int radius;
};

}

#endif
