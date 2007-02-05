#include "object/item.h"
#include <string>

using namespace std;

Item::Item( const string & filename ) throw( LoadException ){
}

Item::Item( const Item & item ){
}
	
Object * Item::copy(){
	return new Item( *this );
}

void Item::act( vector< Object * > * others, World * world ){
}

void Item::draw( Bitmap * work, int rel_x ){
}

bool Item::isCollidable( Object * obj ){
	return false;
}

const int Item::getWidth() const {
	return 0;
}

const int Item::getHeight() const {
	return 0;
}
	
Item::~Item(){
}
