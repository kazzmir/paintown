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
        
double World::ticks(const double in) const {
    return in;
}

void World::dyingObject(const Paintown::Object & obj){
}

void World::dyingObject(const Paintown::Player & obj){
}
