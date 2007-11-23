#include <map>
#include "object_nonattack.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "globals.h"
#include "animation.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include <math.h>
#include "cat.h"

using namespace std;

Cat::Cat( const string & filename ) throw( LoadException ):
ObjectNonAttack( 0, 0 ),
state( IDLE1 ){

	setMaxHealth( 1 );
	setHealth( 1 );
	
	TokenReader tr( filename );
	try{
		Token * head;
		head = tr.readToken();
		if ( *head != "cat" ){
			throw LoadException( "File does not begin with 'Cat'" );
		}

		while ( head->hasTokens() ){
			Token * next = head->readToken();
			if ( *next == "animation" ){
				Animation * animation = new Animation( next, 0 );
				animations[ animation->getName() ] = animation;
			}
		}
		if ( animations.size() == 0 ){
			throw LoadException( "No animation given" );
		}
	} catch( const TokenException & ex ){
		cerr<< "Could not read "<<filename<<" : "<<ex.getReason()<<endl;
		throw LoadException( "Could not open file" );
	}
	current_animation = animations[ "idle1" ];

	meow = Sound( Util::getDataPath() + "/misc/cat/meow.wav" );
}

Cat::Cat( const Cat & cat ):
ObjectNonAttack( cat ),
state( IDLE1 ){
	setMaxHealth( cat.getMaxHealth() );
	setHealth( cat.getHealth() );
	/*
	animation = new Animation( *Cat.animation, 0 );
	animation->reset();
	*/
	for ( map< string, Animation * >::const_iterator it = cat.animations.begin(); it != cat.animations.end(); it++ ){
		// animations.push_back( new Animation( **it, 0 ) );
		animations[ it->first ] = new Animation( *(it->second), 0 );

	}
	current_animation = animations[ "idle1" ];
	meow = cat.meow;
}

const int Cat::getRX() const {
	if ( current_animation ){
		if ( getFacing() == FACING_LEFT ){
			return Object::getRX() - current_animation->getOffsetX();
		} else {
			return Object::getRX() + current_animation->getOffsetX();
		}
	}
	return Object::getRX();
}
	
void Cat::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	switch ( state ){
		case WALK : {
			moveX( -0.8 );
			if ( Util::rnd( 5 ) == 0 ){
				moveZ( -0.2 );
			}
			if ( Util::rnd( 5 ) == 0 ){
				moveZ( 0.2 );
			}
			break;
		}
		case RUN : {
			moveX( -2.2 );
			if ( Util::rnd( 5 ) == 0 ){
				moveZ( -0.2 );
			}
			if ( Util::rnd( 5 ) == 0 ){
				moveZ( 0.2 );
			}
			break;
		}
		default : {
			break;
		}
	}

	if ( current_animation->Act() ){
		switch ( state ){
			case IDLE1 :
			case IDLE2 : {
				switch ( Util::rnd( 5 ) ){
					case 0 : {
						state = IDLE1;
						current_animation = animations[ "idle1" ];
						break;
					}
					case 1 : {
						state = IDLE2;
						current_animation = animations[ "idle2" ];
						break;
					}
					case 2 :
					case 3 :
					case 4 : {
						state = YAWN;
						current_animation = animations[ "yawn" ];
						if ( Util::rnd( 2 ) == 0 ){
							setFacing( getOppositeFacing() );
						}
						break;
					}
					default : {
						break;
					}
				}
				break;
			}
			case YAWN : {
				switch ( Util::rnd( 6 ) ){
					case 0 :
					case 1 : {
						state = SIT;
						current_animation = animations[ "sit" ];
						break;
					}
					case 2 :
					case 3 :
					case 4 :
					case 5 : {
						state = WALK;
						current_animation = animations[ "walk" ];
						break;
					}
				}
				break;
			}
			case WALK : {
				switch ( Util::rnd( 12 ) ){
					case 0 : {
						state = SIT;
						current_animation = animations[ "sit" ];
						break;
					}
					case 1 :
					case 2 :
					case 3 : {
						state = TURN;
						current_animation = animations[ "turn" ];
						break;
					}
					case 4 :
					case 5 : {
						state = RUN;
						current_animation = animations[ "run" ];
					}
					default : {
					}
				}
				break;
			}
			case SIT : {
				switch ( Util::rnd( 2 ) ){
					case 0 : {
						state = IDLE1;
						current_animation = animations[ "idle1" ];
						break;
					}
					case 1 : {
						state = IDLE2;
						current_animation = animations[ "idle2" ];
						break;
					}
				}
				break;
			}
			case TURN : {
				setFacing( getOppositeFacing() );
				state = WALK;
				current_animation = animations[ "walk" ];
				break;
			}
			case RUN : {
				switch ( Util::rnd( 4 ) ){
					case 0 : {
						state = WALK;
						current_animation = animations[ "walk" ];
						break;
					}
					default : {
					}
				}
				break;
			}
		}
		current_animation->reset();
	}
}

void Cat::draw( Bitmap * work, int rel_x ){
	if ( getFacing() == Object::FACING_RIGHT ){
		current_animation->Draw( getRX() - rel_x, getRY(), work );
	} else {
		current_animation->DrawFlipped( getRX() - rel_x, getRY(), work ); 
	}

	if ( Util::rnd( 2000 ) == 0 ){
		meow.play( (int)(255 - fabs(getRX() - rel_x) / 50), 128 + (getRX() - rel_x) / 50 );
	}
}
	
Network::Message Cat::getCreateMessage(){
	Network::Message m;

	return m;
}

bool Cat::isCollidable( Object * obj ){
	return false;
}

bool Cat::isGettable(){
	return false;
}

const int Cat::getWidth() const {
	return 0;
}

const int Cat::getHeight() const {
	return 0;
}

Object * Cat::copy(){
	return new Cat( *this );
}

Cat::~Cat(){
	for ( map< string, Animation * >::iterator it = animations.begin(); it != animations.end(); it++ ){
		delete it->second;
	}
}

