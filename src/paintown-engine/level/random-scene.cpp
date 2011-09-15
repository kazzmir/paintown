#include "random-scene.h"
#include "block.h"
#include "blockobject.h"
#include "util/funcs.h"

using namespace std;
    
RandomScene::RandomScene(const char * filename, const Level::Cacher & cacher):
Scene(filename, cacher){
    objects = collectObjects();
}

static Block * createRandomBlock(int minZ, int maxZ, const vector<BlockObject*> & objects){
    Block * block = new Block();
    int limit = 2 + Util::rnd(10);
    for (int i = 0; i < limit; i++){
        int x = 0;
        if (Util::rnd(2) == 0){
            x = 400 + Util::rnd(1000);
        } else {
            x = -320 - Util::rnd(1000);
        }
        int z = minZ + Util::rnd(maxZ - minZ);
        BlockObject * use = new BlockObject(*objects[Util::rnd(objects.size())]);
        use->setCoords(x, z);
        block->addBlockObject(use);
    }
    block->setLength(320 + Util::rnd(1000));
    return block;
}

/* I don't think I really care about `blocks' here */
void RandomScene::advanceBlocks(int blocks){
    current_block = createRandomBlock(getMinimumZ(), getMaximumZ(), objects);
}

static vector<BlockObject*> collect(const Util::ReferenceCount<Block> & block){
    vector<BlockObject*> objects;
    for (vector<BlockObject*>::const_iterator it = block->getObjects().begin(); it != block->getObjects().end(); it++){
        BlockObject * object = *it;
        if (object->getType() == ObjectFactory::EnemyType){
            objects.push_back(object);
        }
    }
    return objects;
}

/* get all the different objects that could be created. try to just get enemies */
vector<BlockObject*> RandomScene::collectObjects(){
    vector<BlockObject*> all;
    for (deque<Block *>::iterator it = level_blocks.begin(); it != level_blocks.end(); it++){
        vector<BlockObject*> collected = collect(*it);
        all.insert(all.end(), collected.begin(), collected.end());
    }
    {
        vector<BlockObject*> collected = collect(current_block);
        all.insert(all.end(), collected.begin(), collected.end());
    }
    return all;
}

RandomScene::~RandomScene(){
}
