#include "cacher.h"
#include "factory/object_factory.h"
#include "paintown-engine/level/blockobject.h"
#include "paintown-engine/object/object.h"

NetworkCacher::NetworkCacher():
Cacher(){
}
    
Paintown::Object * NetworkCacher::cache(const BlockObject & block) const {
    if (block.getType() == ObjectFactory::EnemyType){
        BlockObject networkEnemy(block);
        networkEnemy.setType(ObjectFactory::NetworkCharacterType);
        return ObjectFactory::createObject(&networkEnemy);
    } else {
        return ObjectFactory::createObject(&block);
    }
}

NetworkCacher::~NetworkCacher(){
}
