#ifndef _font_factory_h
#define _font_factory_h

#include <string>
#include <map>

struct DATAFILE;
class Font;
class Collector;

class FontFactory{
private:
    friend class Collector;
    friend class Font;
    static Font * getFont( const std::string & str, const int x, const int y );

private:
    FontFactory();
    ~FontFactory();

    static void destroy();

    Font * getRealFont( const std::string & str, const int x, const int y );

private:
    static FontFactory * my_factory;

    DATAFILE * my_data;
    std::map<std::string, Font *> font_mapper;
};

#endif
