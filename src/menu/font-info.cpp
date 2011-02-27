#include "font-info.h"
#include "util/file-system.h"
#include "util/font.h"
#include "configuration.h"
#include "exceptions/exception.h"

namespace Menu{

FontInfo::FontInfo(){
}

FontInfo::~FontInfo(){
}

static bool menuFontAvailable(){
    return Configuration::getMenuFont() != NULL;
}

bool RelativeFontInfo::operator==(const FontInfo & who) const {
    return who == *this;
}

bool RelativeFontInfo::operator==(const AbsoluteFontInfo & who) const {
    return getName() == who.getName();
}

bool RelativeFontInfo::operator==(const RelativeFontInfo & who) const {
    return path == who.path;
}

bool AbsoluteFontInfo::operator==(const FontInfo & who) const {
    return who == *this;
}

bool AbsoluteFontInfo::operator==(const AbsoluteFontInfo & who) const {
    return path == who.path;
}

bool AbsoluteFontInfo::operator==(const RelativeFontInfo & who) const {
    return getName() == who.getName();
}

}
