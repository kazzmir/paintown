#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/resource.h"
// #include "factory/collector.h"
#include "r-tech1/funcs.h"
#include "r-tech1/sound/sound.h"
#include "r-tech1/file-system.h"
#include "r-tech1/thread.h"
#include <string>
#include <vector>

using namespace std;

Resource * Resource::resource = NULL;
Util::ReferenceCount<Sound> Resource::getSound(const Filesystem::RelativePath & path){
    return getResource()->_getSound(Storage::instance().find(path));
}

Util::ReferenceCount<Graphics::Bitmap> Resource::getBitmap(const Filesystem::RelativePath & path){
    return getResource()->_getBitmap(Storage::instance().find(path));
}

static Util::Thread::Lock resourceLock;

void Resource::initialize(){
    Util::Thread::initializeLock(&resourceLock);
}

void Resource::destroy(){
    Util::Thread::acquireLock(&resourceLock);
    delete resource;
    resource = NULL;
    Util::Thread::releaseLock(&resourceLock);
}

Resource * Resource::getResource(){
    Util::Thread::acquireLock(&resourceLock);
    if (resource == NULL){
        resource = new Resource();
    }
    Resource * out = resource;
    Util::Thread::releaseLock(&resourceLock);
    return out;
}

/* the resource is created in the Collector */
Resource::Resource(){
    resource = this;
}

Resource::~Resource(){
}

Util::ReferenceCount<Sound> Resource::_getSound(const Filesystem::AbsolutePath & path){
    Util::Thread::ScopedLock locked(lock);
    if (sounds[path.path()] == NULL){
        sounds[path.path()] = Util::ReferenceCount<Sound>(new Sound(path.path()));
    }
    return sounds[path.path()];
}
    
Util::ReferenceCount<Graphics::Bitmap> Resource::_getBitmap(const Filesystem::AbsolutePath & path){
    Util::Thread::ScopedLock locked(lock);
    if (bitmaps[path.path()] == NULL){
        bitmaps[path.path()] = Util::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(path.path()));
    }

    return bitmaps[path.path()];
}
