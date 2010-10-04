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
    return (Configuration::getMenuFont() != "" && Filesystem::exists(Filesystem::RelativePath(Configuration::getMenuFont())));
}

const Font & FontInfo::get() const {
    // Check for override from system and return that instead otherwise continue
    if (menuFontAvailable()){
	return Font::getFont(Filesystem::RelativePath(Configuration::getMenuFont()), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight());
    }
    return Font::getFont(font, width, height);
}

const Filesystem::RelativePath FontInfo::getFont() const {
    if (menuFontAvailable()){
	return Filesystem::RelativePath(Configuration::getMenuFont());
    }
    return this->font;
}

const int FontInfo::getWidth() const {
    if (menuFontAvailable()){
	return Configuration::getMenuFontWidth();
    }
    return this->width;
}

const int FontInfo::getHeight() const{
    if (menuFontAvailable()){
	return Configuration::getMenuFontHeight();
    }
    return this->height;
}

}
