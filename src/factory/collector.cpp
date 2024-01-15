#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/resource.h"
#include "r-tech1/font_factory.h"
#include "r-tech1/sound/sound.h"
#include "r-tech1/init.h"
#include "r-tech1/configuration.h"
#include "collector.h"
#include "paintown-engine/game/nameplacer.h"
#include "font_render.h"
#include "paintown-engine/factory/object_factory.h"
#include "mugen/config.h"
#include <iostream>

using namespace std;

#ifndef debugx
#define debugx cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

Collector::Collector(){
}

/* cleans up global state */
Collector::~Collector(){
    ObjectFactory::destroy();
    NamePlacer::destroy();
    FontFactory::destroy();
    FontRender::destroy();
    Graphics::Bitmap::cleanupTemporaryBitmaps();
    Graphics::Bitmap::shutdown();
    Mugen::Data::destroy();
    Sound::uninitialize();
    Resource::destroy();
    Configuration::cleanup();
    Global::close();
}
