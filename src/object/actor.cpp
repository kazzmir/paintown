#include "object_nonattack.h"
#include "util/bitmap.h"
#include "globals.h"
#include "animation.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "actor.h"

using namespace std;

Actor::Actor( const string & filename ) throw( LoadException ):
ObjectNonAttack( 0, 0 ){

	setMaxHealth( 1 );
	setHealth( 1 );
	
	TokenReader tr( filename );
	try{
		Token * head;
		head = tr.readToken();
		if ( *head != "actor" ){
			throw LoadException( "File does not begin with 'actor'" );
		}

		while ( head->hasTokens() ){
			Token * next = head->readToken();
			if ( *next == "animation" ){
				Animation * animation = new Animation( next, 0 );
				animations.push_back( animation );
			}
		}
		if ( animations.size() == 0 ){
			throw LoadException( "No animation given" );
		}
	} catch( const TokenException & ex ){
		cerr<< "Could not read "<<filename<<" : "<<ex.getReason()<<endl;
		throw LoadException( "Could not open file" );
	}
	current_animation = animations[ 0 ];
}

Actor::Actor( const Actor & actor ):
ObjectNonAttack( actor ){
	setMaxHealth( actor.getMaxHealth() );
	setHealth( actor.getHealth() );
	/*
	animation = new Animation( *actor.animation, 0 );
	animation->reset();
	*/
	for ( vector< Animation * >::const_iterator it = actor.animations.begin(); it != actor.animations.end(); it++ ){
		animations.push_back( new Animation( **it, 0 ) );
	}
	current_animation = animations[ 0 ];
}
	
void Actor::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	if ( current_animation->Act() ){
		current_animation->reset();
	}
}

void Actor::draw( Bitmap * work, int rel_x ){
	current_animation->Draw( getRX() - rel_x, getRY(), work );
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
	
Network::Message Actor::getCreateMessage(){
	Network::Message m;

	return m;
}

Object * Actor::copy(){
	return new Actor( *this );
}

Actor::~Actor(){
	for ( vector< Animation * >::iterator it = animations.begin(); it != animations.end(); it++ ){
		delete *it;
	}
}

