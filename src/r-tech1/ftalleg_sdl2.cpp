#ifdef USE_SDL2

#include "ftalleg.h"

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/utf.h"
#include <iostream>
#include <sstream>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <stdarg.h>

using namespace std;

namespace ftalleg{

freetype::freetype(const Filesystem::AbsolutePath & str, const int x, const int y ):
_height(y){
    font = TTF_OpenFont(str.path().c_str(), y);
    if (font == nullptr){
        DebugLog << "Could not load font " << str.path() << ": " << TTF_GetError() << std::endl;
    }
}

freetype::~freetype(){
    if (font != nullptr){
        TTF_CloseFont(font);
    }
}

void freetype::destroyGlyphIndex(){
}

character * freetype::extractGlyph(signed long unicode){
    return nullptr;
}

void freetype::createIndex(){
}

void freetype::drawCharacter(signed long unicode, int &x1, int &y1, const Graphics::Bitmap & bitmap, const Graphics::Color &color){
}

bool freetype::load(const unsigned char *memoryFont, unsigned int length, int index, unsigned int width, unsigned int height) {
    return false;
}

bool freetype::load(const Filesystem::AbsolutePath & filename, int index, unsigned int width, unsigned int height){
    return false;
}

int freetype::getLength(const std::string & text) {
    int width, height;
    TTF_SizeUTF8(font, text.c_str(), &width, &height);
    return width;
}

void freetype::render(int x, int y, const Graphics::Color & color, const Graphics::Bitmap & bmp, ftAlign alignment, const std::string & text, int marker ...) {
    ostringstream str;
    /* use vsnprintf/Util::limitPrintf here? */

    // Get extra arguments
    va_list ap;
    va_start(ap, marker);
    for(unsigned int i = 0; i<text.length();++i) {
        if (text[i] == '%') {
            if(text[i+1]=='s') {
                str << va_arg(ap, char *);
                ++i;
            } else if(text[i+1]=='d'||text[i+1]=='i') {
                str << va_arg(ap, signed int);
                ++i;
            } else if(text[i+1]=='c') {
                str << (char)va_arg(ap, int);
                ++i;
            } else {
                str << text[i];
            }
        } else {
            str << text[i];
        }
    }
    va_end(ap);

    string data = str.str();

    if (data == ""){
        return;
    }

    /*
    SDL_Color xcolor;
    xcolor.r = 255;
    xcolor.g = 0;
    xcolor.b = 0;
    xcolor.a = 255;
    */
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, data.c_str(), color.getInternalColor());
    if (text_surface == nullptr){
        /* FIXME: log an error? */
        return;
    }
    Graphics::Bitmap copy(text_surface);
    // SDL_FreeSurface(text_surface);
    switch (alignment){
        case ftLeft: {
            /* nothing to do */
            break;
        }
        case ftCenter: {
            x -= copy.getWidth()/2;
            break;
        }
        case ftRight: {
            x -= copy.getWidth();
            break;
        }
    }
    copy.translucent(Graphics::getAlpha(color)).draw(x, y, bmp);
}

int freetype::calculateMaximumHeight(){
    return TTF_FontHeight(font);
}

int freetype::height(long code) const {
    return 0;
}

int freetype::calculateHeight(const std::string & str) const {
    int width, height;
    TTF_SizeUTF8(font, str.c_str(), &width, &height);
    return height;
}

void freetype::setSize( unsigned int w, unsigned int h){
    TTF_SetFontSize(font, h);
}

void freetype::setItalics(int i){
    TTF_SetFontStyle(font, TTF_STYLE_ITALIC);
}

int freetype::getWidth() const {
    return _height;
}

int freetype::getHeight(const std::string & str) const {
   int width, height;
   TTF_SizeUTF8(font, str.c_str(), &width, &height);
   return height;
}

void freetype::getSize(int * w, int * h) const {
    //TTF_SizeUTF8(font, text.c_str(), w, h);
    *w = *h = _height;
}

int freetype::getItalics(){
    // We can't set the slant size so just send 1 if set
    return TTF_GetFontStyle(font) == TTF_STYLE_ITALIC;
}

}

#endif
