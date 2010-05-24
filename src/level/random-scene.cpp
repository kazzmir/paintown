#include "random-scene.h"

using namespace std;
    
RandomScene::RandomScene(const char * filename, const Level::Cacher & cacher):
Scene(filename, cacher){
    objects = collectObjects();
}
    
void RandomScene::advanceBlocks(int n){
}

vector<BlockObject*> RandomScene::collectObjects(){
    vector<BlockObject*> all;

    return all;
}

RandomScene::~RandomScene(){
}
