#include "font-info.h"
#include "util/file-system.h"
#include "util/font.h"
#include "configuration.h"

namespace Menu{

FontInfo::FontInfo(){
}

FontInfo::FontInfo(const Filesystem::RelativePath & font, int width, int height):
font(font),
width(width),
height(height){
}

FontInfo::FontInfo(const FontInfo & copy):
font(copy.font),
width(copy.width),
height(copy.height){
}

FontInfo::~FontInfo(){
}

FontInfo & FontInfo::operator=(const FontInfo & copy){
    this->font = copy.font;
    this->width = copy.width;
    this->height = copy.height;
    
    return *this;
}

static bool menuFontAvailable(){
    return Configuration::getMenuFont() != NULL;
}

const Font & FontInfo::get(const FontInfo & next) const {
    // Check for override from system and return that instead otherwise continue
    /*
    if (menuFontAvailable()){
        return Configuration::getMenuFont()->get();
    }
    */
    return Font::getFont(font, width, height);
}

const Filesystem::RelativePath FontInfo::getFont(const FontInfo & next) const {
    return this->font;
}

const int FontInfo::getWidth(const FontInfo & next) const {
    return this->width;
}

const int FontInfo::getHeight(const FontInfo & next) const{
    return this->height;
}
    
const Font & DefaultFontInfo::get(const FontInfo & next) const {
    return next.get(next);
}

const Filesystem::RelativePath DefaultFontInfo::getFont(const FontInfo & next) const {
    return next.getFont(next);
}

}
