#include "object/item.h"
#include "object/object_attack.h"
#include "stimulation.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/bitmap.h"
#include "util/ebox.h"
#include "util/funcs.h"
#include <iostream>
#include <string>

using namespace std;

static const string dataPath( const string & str ){
	return Util::getDataPath() + str;
}

Item::Item( const string & filename, Stimulation * const stimulation ) throw( LoadException ):
collide( 0 ),
stimulation( stimulation ){
	TokenReader tr( filename );

	setMaxHealth( 1 );
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
			picture.load( dataPath( file ) );
		} else if ( *next == "sound" ){
			string path;
			*next >> path;
			sound = Sound( dataPath( path ) );
		}
	}

	collide = new ECollide( picture );
}

Item::Item( const Item & item ):
ObjectNonAttack( item ),
collide( 0 ),
stimulation( item.copyStimulation() ){
	this->picture = item.picture;
	collide = new ECollide( this->picture );
	setHealth( item.getHealth() );
	sound = item.sound;
	setX( item.getX() );
	setY( item.getY() );
	setZ( item.getZ() );
}
	
Stimulation * const Item::copyStimulation() const {
	return getStimulation()->copy();
}
	
Object * Item::copy(){
	return new Item( *this );
}
	
bool Item::isGettable(){
	return true;
}
	
void Item::touch( Object * obj ){
	obj->stimulate( *getStimulation() );
	sound.play();
}

ECollide * Item::getCollide() const {
	return collide;
}
	
bool Item::collision( ObjectAttack * obj ){
	if ( getAlliance() == obj->getAlliance() ){
		return false;
	}

	if ( this->getCollide() != 0 && obj->getCollide() != 0 ){
		bool my_xflip = false;
		bool his_xflip = false;
		if ( getFacing() == FACING_LEFT )
			my_xflip = true;
		if ( obj->getFacing() == FACING_LEFT )
			his_xflip = true;

		int mx, my;
		int ax, ay;

		ECollide * me = getCollide();
		ECollide * him = obj->getCollide();

		mx = this->getRX() - getWidth() / 2;
		my = this->getRY() - getHeight();
		ax = obj->getRX() - obj->getWidth() / 2;
		ay = obj->getRY() - obj->getHeight();

		return ( me->Collision( him, mx, my, ax, ay, my_xflip, false, his_xflip, false ) );

	}
	return false;
}

void Item::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	// cout << "tough actin tinactin: " << this << endl;
}

void Item::draw( Bitmap * work, int rel_x ){
	// cout << "draw item at " << getRX() - rel_x << " " << getRY() << endl;
	picture.draw( getRX() - rel_x - picture.getWidth() / 2, getRY() - picture.getHeight(), *work );

	if ( Global::globalDebug() ){
		work->circleFill( getRX() - rel_x, (int) getZ(), 5, Bitmap::makeColor( 255, 255, 255 ) );
	}
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
	delete collide;
	delete stimulation;
}
