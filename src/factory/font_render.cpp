#include <string>
#include <vector>
#include <stdarg.h>
#include <allegro.h>
#include "util/font.h"
#include "util/bitmap.h"
#include "font_render.h"
#include "font_factory.h"

using namespace std;

/*
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
*/

FontRender * FontRender::my_render = NULL;
FontRender * FontRender::getInstance(){
	if ( my_render == NULL )
		my_render = new FontRender();
	return my_render;
}

void FontRender::destroy(){
	if ( my_render != NULL )
		delete my_render;
}

void FontRender::render(const Bitmap * work ){
	for ( vector<render_message>::iterator it = messages.begin(); it != messages.end(); it++ ){
		const render_message & r = *it;

                if (r.translucency != -1){
                    Bitmap::transBlender(0, 0, 0, r.translucency);
                    work->drawingMode( Bitmap::MODE_TRANS );
                }

		// work->printf( r.x, r.y, r.fg, r.r_font, r.str );
		r.r_font.printf( r.x, r.y, r.sizeX, r.sizeY, r.fg, *work, r.str, 0 );
		// work->printf( ky + x1, y1, Bitmap::makeColor(255,255,255), player_font, getName() );
                if (r.translucency != -1){
                    work->drawingMode( Bitmap::MODE_SOLID );
                }
	}
	messages.clear();
}

void FontRender::addMessage( const Font & f, int x, int y, int fg, int bg, int translucency, const string & str ){
    render_message r( f, x, y, f.getSizeX(), f.getSizeY(), fg, bg, translucency, str );
    messages.push_back( r );
}

void FontRender::addMessage( const Font & f, int x, int y, int fg, int bg, const string & str ){
    addMessage(f, x, y, fg, bg, -1, str);
}

void FontRender::addMessage( const Font & f, int x, int y, int fg, int bg, const char * str, ... ){
	char buf[1024];
	va_list ap;

	va_start(ap, str);
	uvszprintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	string mm( buf );
	addMessage( f, x, y, fg, bg, -1, mm );
}
	
void FontRender::addMessage( const char * font_name, int x, int y, int fg, int bg, const string & str ){
	addMessage( Font::getFont( font_name ), x, y, fg, bg, -1, str );
}
