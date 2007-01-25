#ifndef _font_render_h
#define _font_render_h

#include <string>
#include <vector>

using namespace std;

class Font;
class Bitmap;

struct render_message{
	render_message( Font * f, int x, int y, int fg, int bg, const string & str ){
		r_font = f;
		this->x = x;
		this->y = y;
		this->fg = fg;
		this->bg = bg;
		this->str = str;
	}

	render_message( const render_message & c ){
		r_font = c.r_font;
		x = c.x;
		y = c.y;
		fg = c.fg;
		bg = c.bg;
		str = c.str;
	}

	Font * r_font;
	int x;
	int y;
	int fg;
	int bg;
	string str;
};

/* factory class to render strings later on */
class FontRender{
public:

	static FontRender * getInstance();
	static void destroy();

	void addMessage( Font * f, int x, int y, int fg, int bg, const string & str );
	void addMessage( Font * f, int x, int y, int fg, int bg, const char * str, ... );
	void addMessage( const char * font_name, int x, int y, int fg, int bg, const string & str );
	void render( Bitmap * work );

private:

	static FontRender * my_render;

	vector< render_message > messages;

};

#endif
