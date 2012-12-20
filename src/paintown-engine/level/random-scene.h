#ifndef _paintown_random_scene_h
#define _paintown_random_scene_h

#include <vector>
#include "scene.h"

class BlockObject;

class RandomScene: public Scene {
public:
    RandomScene(const Filesystem::AbsolutePath & filename, const Level::Cacher & cacher);
	
    virtual void advanceBlocks( int n );

    virtual ~RandomScene();

protected:
    std::vector<Util::ReferenceCount<BlockObject> > collectObjects();

    std::vector<Util::ReferenceCount<BlockObject> > objects;
};

#endif
