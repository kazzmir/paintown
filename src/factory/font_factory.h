#ifndef _paintown_font_factory_h
#define _paintown_font_factory_h

#include <string>
#include <map>
#include "util/file-system.h"
#include "util/font.h"

struct DATAFILE;
class Font;
class Collector;

class FontFactory{
private:
    friend class Collector;
    friend class Font;
    static Font * getFont(const Filesystem::RelativePath & path, const int x, const int y );
    static Font * getFont(const Filesystem::AbsolutePath & path, const int x, const int y );

private:
    FontFactory();
    virtual ~FontFactory();

    static void destroy();

    Font * getRealFont(const Filesystem::RelativePath & str, const int x, const int y );
    Font * getRealFont(const Filesystem::AbsolutePath & str, int x, int y );

private:
    static FontFactory * my_factory;

    // DATAFILE * my_data;
    std::map<std::string, Font *> font_mapper;
    NullFont nullFont;
};

#endif
