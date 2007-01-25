#include "collector.h"
#include "heart_factory.h"
#include "nameplacer.h"
#include "font_factory.h"
#include "font_render.h"
#include "object_factory.h"
#include "shadow.h"
#include <iostream>

using namespace std;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

Collector::Collector(){
}

Collector::~Collector(){
	Shadow::destroy();
	ObjectFactory::destroy();
	NamePlacer::destroy();
	HeartFactory::destroy();
	FontFactory::destroy();
	FontRender::destroy();
}
