#include "util/bitmap.h"
#include <map>
#include "object_nonattack.h"
#include "util/funcs.h"
#include "globals.h"
#include "animation.h"
#include "../game/world.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/file-system.h"
#include <math.h>
#include "cat.h"

using namespace std;

namespace Paintown{

Cat::Cat(const Filesystem::AbsolutePath & filename):
ObjectNonAttack( 0, 0 ),
state( IDLE1 ){

	path = filename;

	setMaxHealth( 1 );
	setHealth( 1 );
	
	TokenReader tr;
	try{
		Token * head;
		head = tr.readTokenFromFile(path.path());
		if ( *head != "cat" ){
			throw LoadException(__FILE__, __LINE__, "File does not begin with 'Cat'" );
		}

		while ( head->hasTokens() ){
			Token * next = head->readToken();
			if ( *next == "animation" ){
				Animation * animation = new Animation( next, 0 );
				animations[ animation->getName() ] = animation;
			}
		}
		if ( animations.size() == 0 ){
			throw LoadException(__FILE__, __LINE__, "No animation given" );
		}
        } catch (const TokenException & ex){
            // Global::debug(0) << "Could not read "<< filename.path() <<": "<< ex.getReason()<<endl;
            throw LoadException(__FILE__, __LINE__, ex, "Could not open file " + filename.path());
        }
	current_animation = animations[ "idle1" ];

	meow = Sound(Storage::instance().find(Filesystem::RelativePath("misc/cat/meow.wav")).path());
}

Cat::Cat( const Cat & cat ):
ObjectNonAttack( cat ),
state( IDLE1 ){
	setMaxHealth( cat.getMaxHealth() );
	setHealth( cat.getHealth() );
	path = cat.path;
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

int Cat::getRX() const {
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

void Cat::draw( Graphics::Bitmap * work, int rel_x, int rel_y ){
    if ( getFacing() == Object::FACING_RIGHT ){
        current_animation->Draw(getRX() - rel_x, getRY(), NULL, work);
    } else {
        current_animation->DrawFlipped(getRX() - rel_x, getRY(), NULL, work); 
    }

    if (Util::rnd(2000) == 0){
        int distance = 1000 - fabs((double)getRX() - rel_x) / 2;
        double volume = distance < 0 ? 0.1 : (double) distance / 1000.0;
        int pan = 128 + (getRX() - rel_x) / 50;
        meow.play(volume, pan);
    }
}
	
Network::Message Cat::getCreateMessage(){
	Network::Message message;
	
	message.id = 0;
	message << World::CREATE_CAT;
	message << getId();

        Filesystem::RelativePath mypath = Storage::instance().cleanse(path);
        message << mypath.path();
        /*
	mypath.erase( 0, Util::getDataPath().length() );
	message << mypath;
        */

	return message;
}

bool Cat::isCollidable( Object * obj ){
	return false;
}

bool Cat::isGettable(){
	return false;
}

int Cat::getWidth() const {
	return 0;
}

int Cat::getHeight() const {
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

}
