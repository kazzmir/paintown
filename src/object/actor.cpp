#include "object_nonattack.h"
#include "util/bitmap.h"
#include "globals.h"
#include "animation.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "actor.h"

Actor::Actor( const string & filename ) throw( LoadException ):
ObjectNonAttack( 0, 0 ),
animation( 0 ){

	setMaxHealth( 1 );
	setHealth( 1 );
	
	TokenReader tr( filename );
	try{
		Token * head;
		head = tr.readToken();
		if ( *head != "actor" ){
			throw new LoadException( "File does not begin with 'actor'" );
		}

		while ( head->hasTokens() ){
			Token * next = head->readToken();
			if ( *next == "animation" ){
				animation = new Animation( next, 0 );
				animation->reset();
				Global::debug( 1 ) << "Created animation " << animation << " bitmap = " << animation->getCurrentFrame()->getBitmap() << endl;
			}
		}
		if ( animation == NULL ){
			throw new LoadException( "No animation given" );
		}
	} catch( const TokenException & ex ){
		cerr<< "Could not read "<<filename<<" : "<<ex.getReason()<<endl;
		throw LoadException( "Could not open file" );
	}
}

Actor::Actor( const Actor & actor ):
ObjectNonAttack( actor ),
animation( 0 ){
	setMaxHealth( actor.getMaxHealth() );
	setHealth( actor.getHealth() );
	animation = new Animation( *actor.animation, 0 );
	animation->reset();
}
	
void Actor::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	if ( animation->Act() ){
		animation->reset();
	}
}

void Actor::draw( Bitmap * work, int rel_x ){
	animation->Draw( getRX() - rel_x, getRY(), work );
}

bool Actor::isCollidable( Object * obj ){
	return false;
}

bool Actor::isGettable(){
	return false;
}

const int Actor::getWidth() const {
	return 0;
}

const int Actor::getHeight() const {
	return 0;
}

Object * Actor::copy(){
	return new Actor( *this );
}

Actor::~Actor(){
	delete animation;
}

