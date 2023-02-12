#include "r-tech1/menu/font-info.h"
#include "r-tech1/file-system.h"
#include "r-tech1/font.h"
#include "r-tech1/configuration.h"
#include "r-tech1/exceptions/exception.h"

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
