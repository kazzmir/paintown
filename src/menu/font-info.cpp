#include "font-info.h"
#include "util/file-system.h"
#include "util/font.h"
#include "configuration.h"
#include "exceptions/exception.h"

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
    
const Font & FontInfo::get() const {
    return Font::getFont(font, width, height);
}

std::string FontInfo::getName() const {
    return font.path();
}

const Font & FontInfo::get(const Font & next) const {
    // Check for override from system and return that instead otherwise continue
    /*
    if (menuFontAvailable()){
        return Configuration::getMenuFont()->get();
    }
    */
    return Font::getFont(font, width, height);
}
        
const FontInfo & FontInfo::get(const FontInfo & next) const {
    return *this;
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
    
const Font & DefaultFontInfo::get(const Font & next) const {
    return next;
}

const Filesystem::RelativePath DefaultFontInfo::getFont(const FontInfo & next) const {
    return next.getFont(next);
}

const FontInfo & DefaultFontInfo::get(const FontInfo & next) const {
    return next;
}

const Font & DefaultFontInfo::get() const {
    throw Exception::Base(__FILE__, __LINE__);
}
    
std::string DefaultFontInfo::getName() const {
    return "Default";
}

RelativeFontInfo::RelativeFontInfo(){
}

RelativeFontInfo::RelativeFontInfo(const Filesystem::RelativePath & font, int width, int height):
FontInfo(font, width, height){
}

AbsoluteFontInfo::AbsoluteFontInfo(const Filesystem::AbsolutePath & font, int width, int height):
myfont(font),
mwidth(width),
mheight(height){
}

const Font & AbsoluteFontInfo::get() const {
    return Font::getFont(myfont, mwidth, mheight);
}

const Font & AbsoluteFontInfo::get(const Font & next) const {
    return get();
}
    
std::string AbsoluteFontInfo::getName() const {
    return myfont.getFilename().path();
}

}
