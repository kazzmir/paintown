#include "player.h"
#include "versus_player.h"
#include "util/load_exception.h"
#include "object/animation.h"
#include "util/funcs.h"
#include "globals.h"
#include <string>
#include <iostream>

using namespace std;

VersusPlayer::VersusPlayer( const std::string & str ) throw( LoadException ):
Player( str ){
}
	
VersusPlayer::VersusPlayer( const Player & player ) throw( LoadException ):
Player( player ){
}

void VersusPlayer::draw( Bitmap * work, int rel_x ){

	Character::draw( work, rel_x );
}
	
void VersusPlayer::act( vector< Object * > * others, World * world, vector< Object * > * add ){

	if ( show_life < getHealth() ){
		show_life++;
	} else if ( show_life > getHealth() ){
		show_life--;
	}

	if ( show_life < 0 ){
		show_life = 0;
	}

	if ( isInvincible() && getInvincibility() < 1 ){
		setInvincibility( 100 );
	}

	/* Character handles jumping and possibly other things */
	Character::act( others, world, add );

	fillKeyCache();

	/* special cases... */
	if ( getStatus() == Status_Hurt || getStatus() == Status_Fell || getStatus() == Status_Rise || getStatus() == Status_Get || getStatus() == Status_Falling )
		return;

	bool reset = animation_current->Act();

	/* cant interrupt an animation unless its walking or idling */
	if ( animation_current != getMovement( "walk" ) && animation_current != getMovement( "idle" ) && animation_current != getMovement( "jump" ) ){
		if ( !reset ) return;
	}
	string current_name = animation_current->getName();

	// bool status = animation_current->getStatus();

	if ( reset ){
		animation_current->reset();

		// leave this line here, ill figure out why its important later
		// if you need the animation_current->getName(), use current_name
		// cout << "Set current animation to null" << endl;
		animation_current = NULL;
	}

	if ( true ){
		Animation * final = NULL;
		// unsigned int num_keys = 0;
		map<Animation *, int > possible_animations;

		for ( map<string,Animation *>::const_iterator it = getMovements().begin(); it != getMovements().end(); it++ ){
			if ( (*it).second == NULL ) continue;
			Animation * b = (*it).second;
			if ( b->isCommisioned() && combo(b) && b->getStatus() == getStatus() ){
				if ( b->properSequence( current_name ) ){
					possible_animations[ b ] = b->getKeys().size();
				}
			} 
		}

		int max = -1;
		if ( ! possible_animations.empty() ){

			for ( map<Animation *, int>::iterator ani = possible_animations.begin(); ani != possible_animations.end(); ani++ ){
				if ( ani->first->getName() == "get" ){
					possible_animations.erase( ani );
				}
			}

			if ( getStatus() != Status_Get ){

				for ( map<Animation *, int>::iterator mit = possible_animations.begin(); mit != possible_animations.end(); mit++ ){
					int & cur = (*mit).second;
					Animation * blah = (*mit).first;
					if ( Global::globalDebug() ){
						cout<< blah->getName() << "? ";
					}
					if ( blah->hasSequence( current_name ) ){
					}
					if ( cur > max || blah->hasSequence( current_name ) ){
						final = blah;
						max = cur;
					}
				}
				if ( Global::globalDebug() ){
					cout<<endl;
				}
			}
		}
		
		if ( final == NULL  && getStatus() != Status_Grab ){
			bool moving = keyboard[ getKey( PAIN_KEY_FORWARD ) ] || keyboard[ getKey( PAIN_KEY_UP ) ] || keyboard[ getKey( PAIN_KEY_DOWN ) ] || keyboard[ getKey( PAIN_KEY_BACK ) ];
			if ( getMovement( "jump" ) == NULL || animation_current != getMovement( "jump" ) ){
				if ( !moving ){
					animation_current = getMovement( "idle" );
				} else	{
					vector< Object * > my_enemies;
					filterEnemies( my_enemies, others );
					bool cy = false;
					for ( vector< Object * >:: iterator it = my_enemies.begin(); it != my_enemies.end(); it++ ){
						Object * guy = *it;

						if ( canGrab( guy ) ){
							cy = true;
							grabEnemy( guy );
							setZ( guy->getZ()+1 );
							// animation_current = movements[ "grab" ];
							animation_current = getMovement( "grab" );
							setStatus( Status_Grab );
							// cout<<"Grabbed"<<endl;
						}
						if ( cy )
							break;
					}
					if ( !cy ){
						animation_current = getMovement( "walk" );
					}
				}
			}
		} else if ( final != NULL && animation_current != final ){
			if ( final->getName() == "special" ){
				if ( getHealth() <= 10 ){
					animation_current = getMovement( "idle" );
					return;
				} else {
					hurt( 10 );
				}
			}
			nextTicket();
			animation_current = final;
			animation_current->reset();

			/* remove the used keys from the key cache */
			key_cache.clear();
			
			if ( animation_current == getMovement("jump") ) {
				double x = 0;
				double y = 0;
				if ( keyboard[ getKey( PAIN_KEY_FORWARD ) ] ){
					x = getSpeed() * 1.2;
				}
				if ( keyboard[ getKey( PAIN_KEY_BACK ) ] ){
					x = -(getSpeed() * 1.2);
				}
				if ( keyboard[ getKey( PAIN_KEY_DOWN ) ] ){
					y = getSpeed() * 1.2;	
				}
				if ( keyboard[ getKey( PAIN_KEY_UP ) ] ){
					y = -getSpeed() * 1.2;	
				}

				doJump( x, y );
			}
		}
	}

	if ( getStatus() == Status_Grab && animation_current == NULL ){
		animation_current = getMovement( "grab" );
	}

	if ( getStatus() == Status_Ground && animation_current == getMovement( "walk" ) || animation_current == getMovement( "idle" ) ){

		// if ( keyboard[ KEY_RIGHT ] || keyboard[ KEY_LEFT ] ){
		if ( keyboard[ getKey( PAIN_KEY_FORWARD ) ] ){
			moveX( getSpeed() );
		} else if ( keyboard[ getKey( PAIN_KEY_BACK ) ] ){
			moveX( -getSpeed() );
		}

		if ( keyboard[ getKey( PAIN_KEY_UP ) ] ){
			moveZ( -getSpeed() );
		} else if ( keyboard[ getKey( PAIN_KEY_DOWN ) ] ){
			moveZ( getSpeed() );
		}
	} else {
	
		if ( getMovement( "throw" ) != NULL && animation_current == getMovement( "throw" ) ){
			if ( getLink() == NULL ){
				cout<<"Link is null. This cant happen."<<endl;
				exit( 1 );
			}
			getLink()->thrown();
			getLink()->fall( 3.2, 5.0 );
		}
	}
}
	
VersusPlayer::~VersusPlayer(){
}
