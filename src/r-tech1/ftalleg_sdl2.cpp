#ifdef USE_SDL2

#include "ftalleg.h"

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/utf.h"
#include <iostream>
#include <sstream>
#include <cassert>
#include <exception>
#include <SDL2/SDL_ttf.h>

namespace ftalleg{

freetype::freetype(const Filesystem::AbsolutePath & str, const int x, const int y ){
    font = TTF_OpenFont(str.path().c_str(), 12);
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
    return 0;
}

void freetype::render(int x, int y, const Graphics::Color & color, const Graphics::Bitmap & bmp, ftAlign alignment, const std::string & text, int marker ...) {
}

int freetype::calculateMaximumHeight(){
    return 0;
}

int freetype::height(long code) const {
    return 0;
}

int freetype::calculateHeight(const std::string & str) const {
    return 0;
}

void freetype::setSize( unsigned int w, unsigned int h){
}

void freetype::setItalics(int i){
}

int freetype::getWidth() const {
    return 0;
}

int freetype::getHeight(const std::string & str) const {
    return 0;
}

void freetype::getSize(int * w, int * h) const {
}

int freetype::getItalics(){
    return 0;
}

}

#endif
