#ifndef _paintown_messages_h
#define _paintown_messages_h

class Bitmap;

class Messages{
public:
	Messages( int width, int height );

	virtual void draw( int x, int y, const Bitmap & work );

	virtual ~Messages();

protected:
	int width;
	int height;
};

#endif
