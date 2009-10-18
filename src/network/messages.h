#ifndef _paintown_messages_h
#define _paintown_messages_h

#include <string>
#include <vector>

class Bitmap;
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

	virtual void draw( int x, int y, const Bitmap & work, const Font & font );

	virtual ~Messages();

protected:
	int width;
	int height;
    int opaque;
    int borderColor;
	std::vector< std::string > messages;
};

#endif
