#include "cacher.h"
#include "blockobject.h"
#include "../factory/object_factory.h"

namespace Level{

Cacher::Cacher(){
}

Cacher::~Cacher(){
}

DefaultCacher::DefaultCacher():
Cacher(){
}

Paintown::Object * DefaultCacher::cache(const Util::ReferenceCount<BlockObject> & block) const {
    return ObjectFactory::createObject(block);
}

DefaultCacher::~DefaultCacher(){
}

}
