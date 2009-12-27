#ifndef _font_render_h
#define _font_render_h

#include <string>
#include <vector>

#include "util/font.h"

class Bitmap;

struct render_message{
	render_message( const Font & f, int x, int y, int sizeX, int sizeY, int fg, int bg, int translucency, const std::string & str ):r_font( f ){
            this->sizeX = sizeX;
            this->sizeY = sizeY;
            this->x = x;
            this->y = y;
            this->fg = fg;
            this->bg = bg;
            this->str = str;
            this->translucency = translucency;
	}

	render_message( const render_message & c ):
		r_font( c.r_font ){
                sizeX = c.sizeX;
                sizeY = c.sizeY;
		x = c.x;
		y = c.y;
		fg = c.fg;
		bg = c.bg;
		str = c.str;
                translucency = c.translucency;
	}

	render_message & operator=( const render_message & rhs ){
		if ( this == &rhs ) return *this;

		/*
		this->r_font = rhs.r_font;
		this->x = rhs.x;
		this->y = rhs.y;
		this->fg = rhs.fg;
		this->bg = rhs.bg;
		this->str = rhs.str;
		*/
		return *this;
	}

	const Font & r_font;
        int sizeX;
        int sizeY;
	int x;
	int y;
	int fg;
	int bg;
        std::string str;
        int translucency;
};

/* factory class to render strings later on */
class FontRender{
public:

	static FontRender * getInstance();
	static void destroy();

	void addMessage( const Font & f, int x, int y, int fg, int bg, const std::string & str );
	void addMessage( const Font & f, int x, int y, int fg, int bg, int translucency, const std::string & str );
	void addMessage( const Font & f, int x, int y, int fg, int bg, const char * str, ... );
	void addMessage( const char * font_name, int x, int y, int fg, int bg, const std::string & str );
	void render( Bitmap * work );

private:

	static FontRender * my_render;

        std::vector<render_message> messages;

};

#endif
