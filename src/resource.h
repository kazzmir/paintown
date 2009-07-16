#ifndef _paintown_resource_manager_h
#define _paintown_resource_manager_h

#include "factory/collector.h"
#include "util/load_exception.h"
#include <map>
#include <string>
#include <vector>

class Sound;
class Bitmap;

class Resource{
public:
    /* do not prepend Util::getDataPath() to paths. Resource will do it for you.
     */
    static Sound * getSound(const std::string & path) throw (LoadException);
    static Bitmap * getBitmap(const std::string & path) throw (LoadException);
private:
    friend class Collector;
    Resource();
    virtual ~Resource();

    Sound * _getSound(const std::string & path) throw (LoadException);
    Bitmap * _getBitmap(const std::string & path) throw (LoadException);

private:
    static Resource * resource;
    std::map<std::string, Sound*> sounds;
    std::map<std::string, Bitmap*> bitmaps;
};

#endif
