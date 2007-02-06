#include "object/item.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/bitmap.h"
#include <iostream>
#include <string>

using namespace std;

Item::Item( const string & filename ) throw( LoadException ){
	TokenReader tr( filename );

	setHealth( 1 );

	Token * head;
	try{
		head = tr.readToken();
	} catch( const TokenException & ex ){
		cerr<< "Could not read "<<filename<<" : "<<ex.getReason()<<endl;
		// delete head;
		throw LoadException("Could not open character file");
	}

	if ( *head != "item" ){
		throw new LoadException( "Item does not begin with 'item'" );
	}

	while ( head->hasTokens() ){
		Token * next = head->readToken();
		if ( *next == "frame" ){
			string file;
			*next >> file;
			picture.load( file );
		}
	}
}

Item::Item( const Item & item ){
	this->picture = item.picture;
	setHealth( item.getHealth() );
	setX( item.getX() );
	setY( item.getY() );
	setZ( item.getZ() );
}
	
Object * Item::copy(){
	return new Item( *this );
}

void Item::act( vector< Object * > * others, World * world ){
	// cout << "tough actin tinactin: " << this << endl;
}

void Item::draw( Bitmap * work, int rel_x ){
	// cout << "draw item at " << getRX() - rel_x << " " << getRY() << endl;
	picture.draw( getRX() - rel_x, getRY(), *work );
}

bool Item::isCollidable( Object * obj ){
	return false;
}

const int Item::getWidth() const {
	return picture.getWidth();
}

const int Item::getHeight() const {
	return picture.getHeight();
}
	
Item::~Item(){
}
