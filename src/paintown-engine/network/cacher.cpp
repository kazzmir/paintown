#include "cacher.h"
#include "../factory/object_factory.h"
#include "../level/blockobject.h"
#include "../object/object.h"

NetworkCacher::NetworkCacher():
Cacher(){
}
    
Paintown::Object * NetworkCacher::cache(const Util::ReferenceCount<BlockObject> & block) const {
    if (block->getType() == ObjectFactory::EnemyType){
        Util::ReferenceCount<BlockObject> networkEnemy(new BlockObject(*block));
        networkEnemy->setType(ObjectFactory::NetworkCharacterType);
        return ObjectFactory::createObject(networkEnemy);
    } else {
        return ObjectFactory::createObject(block);
    }
}

NetworkCacher::~NetworkCacher(){
}
