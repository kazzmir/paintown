#include "util/graphics/bitmap.h"
#include "util/graphics/trans-bitmap.h"
#include <string>
#include <vector>
#include <stdarg.h>
#include "util/font.h"
#include "util/funcs.h"
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

void FontRender::render(const Graphics::Bitmap * work, double scaleWidth, double scaleHeight){
    for ( vector<render_message>::iterator it = messages.begin(); it != messages.end(); it++ ){
        const render_message & r = *it;

        /* FIXME: replace with work->translucent() */
        /*
        if (r.translucency != -1){
            Graphics::Bitmap::transBlender(0, 0, 0, r.translucency);
            work->drawingMode( Graphics::Bitmap::MODE_TRANS );
        }
        */

        if (r.translucency != -1){
            Graphics::Bitmap::transBlender(0, 0, 0, r.translucency);
            r.r_font.printf((int)(r.x * scaleWidth), (int)(r.y * scaleHeight), (int)(r.sizeX * scaleWidth), (int)(r.sizeY * scaleHeight), r.fg, work->translucent(), r.str, 0);
        } else {
            r.r_font.printf((int)(r.x * scaleWidth), (int)(r.y * scaleHeight), (int)(r.sizeX * scaleWidth), (int)(r.sizeY * scaleHeight), r.fg, *work, r.str, 0);
        }

        // work->printf( r.x, r.y, r.fg, r.r_font, r.str );
        // work->printf( ky + x1, y1, Bitmap::makeColor(255,255,255), player_font, getName() );
        /*
        if (r.translucency != -1){
            work->drawingMode( Graphics::Bitmap::MODE_SOLID );
        }
        */
    }
    messages.clear();
}

void FontRender::addMessage( const Font & f, int x, int y, Graphics::Color fg, Graphics::Color bg, int translucency, const string & str ){
    render_message r( f, x, y, f.getSizeX(), f.getSizeY(), fg, bg, translucency, str );
    messages.push_back( r );
}

void FontRender::addMessage( const Font & f, int x, int y, Graphics::Color fg, Graphics::Color bg, const string & str ){
    addMessage(f, x, y, fg, bg, -1, str);
}

void FontRender::addMessage( const Font & f, int x, int y, Graphics::Color fg, Graphics::Color bg, const char * str, ... ){
	char buf[1024];
	va_list ap;

	va_start(ap, str);
        Util::limitPrintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	string mm( buf );
	addMessage( f, x, y, fg, bg, -1, mm );
}
	
void FontRender::addMessage(const Filesystem::RelativePath & font_name, int x, int y, Graphics::Color fg, Graphics::Color bg, const string & str ){
    addMessage(Font::getFont(font_name), x, y, fg, bg, -1, str );
}
