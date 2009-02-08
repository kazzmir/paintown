#include "world.h"

using namespace std;

World::World():
quake_time( 0 ){
}

World::~World(){
}

void World::Quake( int q ){
    quake_time += q;
}
