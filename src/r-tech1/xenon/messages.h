#ifndef _rtech1_messages_h
#define _rtech1_messages_h

#include <string>
#include <vector>
#include "graphics/color.h"

namespace Graphics{
class Bitmap;
}
class Font;

class Messages{
public:
	Messages(int width, int height, int opaque = 128);

	virtual inline int getHeight() const {
		return height;
	}
	
	virtual inline int getWidth() const {
		return width;
	}

	virtual void addMessage( const std::string & s );

	virtual void draw( int x, int y, const Graphics::Bitmap & work, const Font & font );

	virtual ~Messages();

protected:
	int width;
	int height;
    int opaque;
    Graphics::Color borderColor;
	std::vector< std::string > messages;
};

#endif
