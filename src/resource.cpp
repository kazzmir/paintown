#include "resource.h"
#include "factory/collector.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/load_exception.h"
#include <string>
#include <vector>

using namespace std;

Resource * Resource::resource = NULL;
Sound * Resource::getSound(const std::string & path) throw (LoadException){
    return resource->_getSound(Util::getDataPath() + path);
}

/* the resource is created in the Collector */
Resource::Resource(){
    resource = this;
}

Resource::~Resource(){
    for (std::map<std::string, Sound*>::iterator it = sounds.begin(); it != sounds.end(); it++){
        delete (*it).second;
    }
}

Sound * Resource::_getSound(const std::string & path) throw (LoadException){
    if (sounds[path] == NULL){
        sounds[path] = new Sound(path);
    }
    return sounds[path];
}
