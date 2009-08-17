#include "util/bitmap.h"
#include "resource.h"
#include "factory/collector.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/load_exception.h"
#include "util/file-system.h"
#include <string>
#include <vector>

using namespace std;

Resource * Resource::resource = NULL;
Sound * Resource::getSound(const std::string & path) throw (LoadException){
    return resource->_getSound(Filesystem::find(path));
}

Bitmap * Resource::getBitmap(const std::string & path) throw (LoadException){
    return resource->_getBitmap(Filesystem::find(path));
}

/* the resource is created in the Collector */
Resource::Resource(){
    resource = this;
}

Resource::~Resource(){
    for (std::map<std::string, Sound*>::iterator it = sounds.begin(); it != sounds.end(); it++){
        delete (*it).second;
    }
    
    for (std::map<std::string, Bitmap*>::iterator it = bitmaps.begin(); it != bitmaps.end(); it++){
        delete (*it).second;
    }
}

Sound * Resource::_getSound(const std::string & path) throw (LoadException){
    if (sounds[path] == NULL){
        sounds[path] = new Sound(path);
    }
    return sounds[path];
}
    
Bitmap * Resource::_getBitmap(const std::string & path) throw (LoadException){
    if (bitmaps[path] == NULL){
        bitmaps[path] = new Bitmap(path);
    }

    return bitmaps[path];
}
