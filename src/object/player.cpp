#include <iostream>

#include "animation.h"
#include "util/bitmap.h"
#include "character.h" 
#include "util/font.h"
#include "util/funcs.h"
#include "factory/font_factory.h"
#include "factory/font_render.h"
#include "configuration.h"
#include "globals.h"
#include "util/keyboard.h"
#include "nameplacer.h"
#include "util/load_exception.h"
#include "object.h"
#include "player.h"

// how many ticks to wait before the key cache is cleared.
// this can probably be user defined in the future
static const int GLOBAL_KEY_DELAY = 15;
static const unsigned int KEY_CACHE_SIZE = 100;
static const char * PLAYER_FONT = "/fonts/arial.ttf";

using namespace std;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif

#define DEFAULT_LIVES 4

Player::Player( const char * filename ) throw( LoadException ): 
Character( filename, ALLIANCE_PLAYER ),
acts(0),
lives(DEFAULT_LIVES),
invincible( false ){
	
	if ( getMovement( "grab" ) == NULL ){
		throw LoadException("No 'grab' movement");
	}

	show_life = getHealth();

	int x, y;
	NamePlacer::getPlacement( x, y, name_id );
}
	
Player::Player( const string & filename ) throw( LoadException ):
Character( filename, ALLIANCE_PLAYER ),
acts(0),
lives(DEFAULT_LIVES),
invincible( false ){
	
	// if ( movements[ "grab" ] == NULL ){
	if ( getMovement( "grab" ) == NULL ){
		throw LoadException("No 'grab' movement");
	}

	show_life = getHealth();

	int x, y;
	NamePlacer::getPlacement( x, y, name_id );
}
	
Player::Player( const Character & chr ) throw( LoadException ):
Character( chr ),
acts(0),
lives(DEFAULT_LIVES),
invincible( false ){
	show_life = getHealth();
}

Player::Player( const Player & pl ) throw( LoadException ):
Character( pl ),
acts( 0 ),
lives( pl.lives ),
invincible( false ){
	show_life = getHealth();
}

void Player::loseLife( int l ){
	lives -= l;
}

void Player::gainLife( int l ){
	lives += l;
}

void Player::fillKeyCache(){

	keyboard.poll();

	if ( acts++ > GLOBAL_KEY_DELAY ){
		// key_cache.clear();
		/*
		if ( !key_cache.empty() )
			key_cache.pop_front();
		*/

		if ( !key_cache.empty() )
			key_cache.pop_front();

		acts = 0;
	}

	if ( keyboard.keypressed() ){
		// acts = 0;
		vector< int > all_keys;
		keyboard.readKeys( all_keys );
		map< int, bool > new_last;
		for ( vector<int>::iterator it = all_keys.begin(); it != all_keys.end(); it++ ){
			int n = *it;
			if ( ! last_key[ n ] ){
				// cout<<"Last key[ "<<n<<" ] = "<<last_key[n]<<endl;
				key_cache.push_back( keyState( n, getFacing() ) );
				acts = 0;
			}
			new_last[ n ] = true;		
			// cout<<"Read "<<n<<" Last = "<<last_key<<" Acts = "<<acts<<endl;
			/*
			if ( n != last_key ){
				key_cache.push_back( n );
				last_key = n;
			} else {
				if ( acts > 2 ){
					acts = 0;
					key_cache.clear();
				}
			}
			*/
		}
		last_key = new_last;
		// cout<<"Last key[ "<<83<<" ] = "<<last_key[83]<<endl;
		// cout<<endl;
	} else {
		last_key.clear();
		// last_key = -1;
	}

	while ( key_cache.size() > KEY_CACHE_SIZE ){
		key_cache.pop_front();
	}

	// cout << "Keys in cache: " << key_cache.size() << endl;

	// cout<<"Last key[ "<<83<<" ] = "<<last_key[83]<<endl;

	#if 0
	if ( keyboard.keypressed() ){

		vector< int > all_keys;
		keyboard.readKeys( all_keys );
		// cout<<"Add ";
		for ( vector<int>::iterator it = all_keys.begin(); it != all_keys.end(); it++ ){
			int n = *it;
			/*
			if ( key_cache.size() >= 16 )
				key_cache.pop_front();
			*/

			// cout<< n <<" ";
			if ( key_cache.back() != n )
				key_cache.push_back( n );
		}
		// cout<<endl;
	} else {
		/*
		if ( key_cache.size() >= 16 )
			key_cache.pop_front();
		*/
		if ( key_cache.back() != -99 )
			key_cache.push_back( -99 );

		/*
		if ( key_cache.size() >= 16 ){
			key_cache.pop_front();
			key_cache.pop_front();
		}

		// -2345 is a very important number to someone, somewhere
		key_cache.push_back( -2345 );
		key_cache.push_back( -2345 );
		key_cache.push_back( -2345 );
		*/

	}
	#endif
		

	/*
	if ( acts++ > 1 ){
		if ( !key_cache.empty() )
			key_cache.pop_front();
		acts = 0;
	}
	*/
}
	
void Player::drawLifeBar( int x, int y, Bitmap * work ){
	drawLifeBar( x, y, show_life, work );
}
	
void Player::draw( Bitmap * work, int rel_x ){

	Character::draw( work, rel_x );

	int x1, y1;
	NamePlacer::getPlacement( x1, y1, name_id );
	
	if ( icon )
		icon->draw( x1, y1, *work );

	int hasIcon = icon ? icon->getWidth() : 0;
	

	if ( show_life < 0 ){
		show_life = 0;
	}

	// Font * player_font = FontFactory::getFont( NAME_FONT );
	// const Font & player_font = Font::getFont( NAME_FONT );
	const Font & player_font = Font::getFont( Util::getDataPath() + PLAYER_FONT, 20, 20 );
	const string & name = getName();
	int nameHeight = player_font.getHeight( name ) / 2;
	nameHeight = 20 / 2;
	// cout << "Height: " << player_font.getHeight( name ) << endl;
	// Font * player_font = FontFactory::getFont( "bios" );

	// work->printf( ky + x1, y1, Bitmap::makeColor(255,255,255), player_font, getName() );
	FontRender * render = FontRender::getInstance();
	render->addMessage( player_font, (hasIcon + x1) * 2, y1 * 2, Bitmap::makeColor(255,255,255), -1, name );
	// cout << "Draw name at " << y1 * 2 << endl;
	// player_font.printf( (hasIcon + x1) * 2, y1, Bitmap::makeColor(255,255,255), *work, getName() );
	// drawLifeBar( hasIcon + x1, y1 + player_font.getHeight() / 2, show_life, work );
	drawLifeBar( hasIcon + x1, y1 + nameHeight, work );
	// cout << "Y1: " << y1 << " Height: " << player_font.getHeight() << " new y1: " << (y1 + player_font.getHeight() / 2) << endl;
	// work->printf( hasIcon + x1 + getMaxHealth() + 5, y1 + player_font->getHeight(), Bitmap::makeColor(255,255,255), player_font, "x %d", 3 );
	int max = getMaxHealth() < 100 ? getMaxHealth() : 100;
	render->addMessage( player_font, (x1 + hasIcon + max + 5) * 2, y1 + nameHeight, Bitmap::makeColor(255,255,255), -1, "x %d", getLives() );

	// work->rectangle( x1, y1, x1 + 100, y1 + nameHeight + 1, Bitmap::makeColor( 255, 255, 255 ) );
}

bool Player::combo( Animation * ani, deque< keyState >::iterator cache_cur_key, deque< keyState >::iterator end ){
	int startFacing = (*cache_cur_key).facing;
			
	// cout << "Testing " << ani->getName() << " facing = " << startFacing << ". current facing = " << getFacing() << endl;
	const vector< KeyPress > & keys = ani->getKeys();
	if ( keys.empty() ){
		return false;
	}
	for ( vector<KeyPress>::const_iterator k = keys.begin(); k != keys.end(); k++ ){
		if ( cache_cur_key == end ){
			return false;
		}

		const KeyPress & kp = *k;
		bool all_pressed = false;
		for ( vector<int>::const_iterator cur_key = kp.combo.begin(); cur_key != kp.combo.end(); cur_key++ ){
			int find_key = getKey( *cur_key, startFacing );
			int key = (*cache_cur_key).key;
			// if ( find_key == (*cache_cur_key) ){
			if ( find_key == key ){
				all_pressed = true;
			}
		}
		if ( !all_pressed ){
			return false;
		}

		cache_cur_key++;
	}
	return true;

}

bool Player::combo( Animation * ani ){

	deque< keyState >::iterator cur = key_cache.begin();
	for ( cur = key_cache.begin(); cur != key_cache.end(); cur++ ){
		if ( combo( ani, cur, key_cache.end() ) ){
			return true;
		}
	}
	return false;

	/*
	deque< keyState >::iterator cache_cur_key = key_cache.begin();
	if ( cache_cur_key == key_cache.end() ){
		return false;
	}
	int startFacing = (*cache_cur_key).facing;
			
	// cout << "Testing " << ani->getName() << " facing = " << startFacing << ". current facing = " << getFacing() << endl;
	const vector< KeyPress > & keys = ani->getKeys();
	if ( keys.empty() )
		return false;
	for ( vector<KeyPress>::const_iterator k = keys.begin(); k != keys.end(); k++ ){
		if ( cache_cur_key == key_cache.end() ){
			return false;
		}

		const KeyPress & kp = *k;
		bool all_pressed = false;
		for ( vector<int>::const_iterator cur_key = kp.combo.begin(); cur_key != kp.combo.end(); cur_key++ ){
			int find_key = getKey( *cur_key, startFacing );
			int key = (*cache_cur_key).key;
			// if ( find_key == (*cache_cur_key) ){
			if ( find_key == key ){
				all_pressed = true;
			}
		}
		if ( !all_pressed ){
			return false;
		}

		cache_cur_key++;
	}
	return true;
	*/
}

/*
bool Player::combo( Animation * ani ){
	deque< keyState >::reverse_iterator cache_cur_key = key_cache.rbegin();
	if ( cache_cur_key == key_cache.rend() ){
		return false;
	}
	int startFacing = (*cache_cur_key).facing;
			
	cout << "Testing " << ani->getName() << " facing = " << startFacing << ". current facing = " << getFacing() << endl;
	const vector< KeyPress > & keys = ani->getKeys();
	if ( keys.empty() )
		return false;
	for ( vector<KeyPress>::const_reverse_iterator k = keys.rbegin(); k != keys.rend(); k++ ){
		if ( cache_cur_key == key_cache.rend() ){
			return false;
		}

		const KeyPress & kp = *k;
		bool all_pressed = false;
		for ( vector<int>::const_iterator cur_key = kp.combo.begin(); cur_key != kp.combo.end(); cur_key++ ){
			int find_key = getKey( *cur_key, startFacing );
			int key = (*cache_cur_key).key;
			// if ( find_key == (*cache_cur_key) ){
			if ( find_key == key ){
				all_pressed = true;
			}
		}
		if ( !all_pressed ){
			return false;
		}

		cache_cur_key++;
	}
	return true;
	
}
*/

int Player::getKey( int motion, int facing ){
	return Configuration::config( 0 ).getKey( motion, facing );
}
	
int Player::getKey( int x ){
	return this->getKey( x, getFacing() );
}
	
Object * Player::copy(){
	return new Player( *this );
}
	
void Player::hurt( int x ){
	if ( ! isInvincible() ){
		Character::hurt( x );
	}
}

void Player::takeDamage( ObjectAttack * obj, int x ){
	if ( getLink() != NULL ){
		getLink()->unGrab();
		unGrab();
	}

	Character::takeDamage( obj, x );

	// cout<<"Player taking "<<x<<" damage"<<endl;
}

/* check to see if the player is close enough to an enemy */
bool Player::canGrab( Object * enemy ){
	return false;
	if ( !enemy->isCollidable( this ) )
		return false;
	/*
	bool d = ZDistance( enemy ) < MIN_RELATIVE_DISTANCE;
	bool c = realCollision( enemy );
	cout<<enemy<<": distance = "<<d<<" collision = "<<c<<endl;
	*/
	// if ( ZDistance( enemy ) < MIN_RELATIVE_DISTANCE && enemy->collision( this ) ){

	// animation_current = movements[ "grab" ];
	animation_current = getMovement( "grab" );

	// if ( ZDistance( enemy ) < MIN_RELATIVE_DISTANCE && realCollision( enemy ) ){
	if ( ZDistance( enemy ) < MIN_RELATIVE_DISTANCE && XDistance( enemy ) < 30 ){
		return true;
	}
	return false;
}

void Player::grabEnemy( Object * enemy ){
	// enemy->setStatus( Status_Grabbed );
	enemy->grabbed( this );
	setLink( enemy );
}
			
static Animation * hasGetAnimation( const map< Animation *, int > & animations ){
	for ( map< Animation *, int >::const_iterator it = animations.begin(); it != animations.end(); it++ ){
		Animation * a = (*it).first;
		if ( a->getName() == "get" ){
			return a;
		}
	}

	return NULL;
}

/* things to do when the player dies */
void Player::deathReset(){
	setY( 200 );
	setMoving( true );
	setStatus( Status_Falling );
	setHealth( getMaxHealth() );
	setInvincibility( 400 );
	setDeath( 0 );
	animation_current = getMovement( "idle" );
	loseLife();
}

void Player::act( vector< Object * > * others, World * world, vector< Object * > * add ){

	if ( show_life < getHealth() ){
		show_life++;
	} else if ( show_life > getHealth() ){
		show_life--;
	}
	
	/*
	if ( getInvincibility() < 500 ){
		setInvincibility( 1000 );
	}
	*/
	if ( isInvincible() && getInvincibility() < 1 ){
		setInvincibility( 100 );
	}

	/* Character handles jumping and possibly other things */
	Character::act( others, world, add );

	fillKeyCache();

	/* special cases... */
	if ( getStatus() == Status_Hurt || getStatus() == Status_Fell || getStatus() == Status_Rise || getStatus() == Status_Get || getStatus() == Status_Falling )
		return;

	/*
	if ( getStatus() == Status_Grab ){
		return;
	}
	*/
	
	/* Now the real meat */

	/*
	if ( !key_cache.empty() ){
		for ( deque<int>::iterator dit = key_cache.begin(); dit != key_cache.end(); dit++ ){
			cout<< *dit <<" ";
		}
		cout<<endl;
	}
	*/
	
	bool reset = animation_current->Act();

	/* cant interrupt an animation unless its walking or idling */
	// if ( animation_current != movements[ "walk" ] && animation_current != movements[ "idle" ]  && animation_current != movements[ "jump" ] ){
	if ( animation_current != getMovement( "walk" ) && animation_current != getMovement( "idle" ) && animation_current != getMovement( "jump" ) ){
	// if ( animation_current != movements[ "walk" ] && animation_current != movements[ "idle" ] ){
		// if ( !animation_current->Act() ) return;
		// animation_current = movements[ "idle" ];
		if ( !reset ) return;
	} else {
		/*
		if ( animation_current->Act() )
			animation_current->reset();
		*/
	}
	string current_name = animation_current->getName();

	// bool status = animation_current->getStatus();

	if ( reset ){
		animation_current->reset();

		const vector<string> & allow = animation_current->getCommisions();
		for ( vector<string>::const_iterator it = allow.begin(); it != allow.end(); it++ ){
			// cout<<"Commision: "<<*it<<endl;
			// Animation * ok = movements[ *it ];
			Animation * ok = getMovement( *it );
			if ( ok != NULL ){
				ok->setCommision( true );
			}
		}
		
		const vector<string> & disallow = animation_current->getDecommisions();
		for ( vector<string>::const_iterator it = disallow.begin(); it != disallow.end(); it++ ){
			// cout<<"Decommision: "<<*it<<endl;
			// Animation * ok = movements[ *it ];
			Animation * ok = getMovement( *it );
			if ( ok != NULL ){
				ok->setCommision( false );
			}
		}
		
		// leave this line here, ill figure out why its important later
		// if you need the animation_current->getName(), use current_name
		// cout << "Set current animation to null" << endl;
		animation_current = NULL;
	}
	// animation_current = NULL;

	if ( true ){
		Animation * final = NULL;
		// unsigned int num_keys = 0;
		map<Animation *, int > possible_animations;


		/*
		vector< Animation * > xv;
		for ( map<string,Animation*>::const_iterator it = movements.begin(); it != movements.end(); it++ ){
			xv.push_back( (*it).second );
		}
		for ( vector< Animation * >::iterator it = xv.begin(); it != xv.end(); ){
			Animation * b = *it;
			if ( combo( b ) ){
				it++;
				if ( b->getStatus() == getStatus() && (b->getPreviousSequence() == "none" || current_name == b->getPreviousSequence() ) ){
					possible_animations[ b ] = b->getKeys().size();
				}
			} else it = xv.erase( it );
		}
		*/

		for ( map<string,Animation *>::const_iterator it = getMovements().begin(); it != getMovements().end(); it++ ){
			if ( (*it).second == NULL ) continue;
			Animation * b = (*it).second;
			if ( b->isCommisioned() && combo(b) && b->getStatus() == getStatus() ){
				// cout<<b->getName() << " has sequences"<<endl;
				/*
				for ( vector<string>::const_iterator it = b->getSequences().begin(); it != b->getSequences().end(); it++ ){
					cout<< *it << endl;
				}
				*/
				// cout<<"Testing proper sequence with "<<b->getName()<<endl;
				if ( b->properSequence( current_name ) ){
				// if ( b->getPreviousSequence() == "none" || current_name == b->getPreviousSequence() ){
					// cout<<"Possible sequence " <<b->getName()<<endl;
					possible_animations[ b ] = b->getKeys().size();
				}
			} 
		}

		/*
		for ( map<string,Animation*>::const_iterator it = movements.begin(); it != movements.end(); it++ ){

			// word up
			debug
			Animation *const & ani = (*it).second;
		
			if ( animation_current != movements[ "walk" ] && animation_current != movements[ "idle" ] ){
				debug
				if ( current_name != ani->getPreviousSequence() )
					continue;
			}

			const vector< KeyPress > & keys = ani->getKeys();
			for ( vector<KeyPress>::const_iterator k = keys.begin(); k != keys.end(); k++ ){
				const KeyPress & kp = *k;
				bool all_pressed = false;
				debug
				for ( vector<int>::const_iterator cur_key = kp.combo.begin(); cur_key != kp.combo.end(); cur_key++ ){
		
					int find_key = getKey(*cur_key);

					/ *
					if ( key[ *cur_key ] )
						all_pressed = true;
					else {
						all_pressed = false;
						break;
					}
					* /
					bool found_it = false;
					for ( deque<int>::iterator dit = key_cache.begin(); dit != key_cache.end(); dit++ ){
						if ( find_key == *dit ) 
							found_it = true;
					}

					if ( found_it ){
						all_pressed = true;
					} else {
						all_pressed = false;
						break;
					}
				}

				debug
				// if ( all_pressed && num_keys < keys.size() ){
				if ( all_pressed ){
					debug
					if ( getStatus() == ani->getStatus() ){
						debug
						// cout<<"Valid movement "<<(*it).first<<" Current = "<<animation_current->getName()<<". Previous = "<<ani->getPreviousSequence()<<endl;
						if ( ani->getPreviousSequence() == "none" || current_name == ani->getPreviousSequence() ){
							debug
							final = ani;
							num_keys = keys.size();
							// cout<< "map: " << possible_animations[ ani ] << " " << kp.combo.size() << endl;
							if ( possible_animations[ ani ] < kp.combo.size() )
								possible_animations[ ani ] = kp.combo.size();
							// cout<<"Chose "<<(*it).first<<endl;
						}
					}
					// cout<<endl;
				}
			}
			debug
		}
		*/

		int max = -1;
		if ( ! possible_animations.empty() ){

			Animation * get = hasGetAnimation( possible_animations );
			if ( get != NULL ){
				for ( vector< Object * >::iterator it = others->begin(); it != others->end(); it++ ){
					Object * o = *it;
					if ( o->isGettable() && fabs((double)(o->getRX() - getRX())) < 25 && ZDistance( o ) <= get->getMinZDistance() ){
						final = get;
						setStatus( Status_Get );
					}
				}
				possible_animations.erase( get );
			}

			if ( getStatus() != Status_Get ){

				for ( map<Animation *, int>::iterator mit = possible_animations.begin(); mit != possible_animations.end(); mit++ ){
					int & cur = (*mit).second;
					Animation * blah = (*mit).first;
					if ( Global::globalDebug() ){
						cout<< blah->getName() << "? ";
					}
					// if ( cur > max || blah->getPreviousSequence() == current_name ){
					// cout<<"Testing "<<blah->getName()<<endl;
					if ( blah->hasSequence( current_name ) ){
						// cout<<blah->getName() << " has "<< current_name << endl;
					}
					if ( cur > max || blah->hasSequence( current_name ) ){
						// cout<<"Current choice = "<<blah->getName() <<endl;
						final = blah;
						max = cur;
					}
				}
				if ( Global::globalDebug() ){
					cout<<endl;
				}
			}
		}
		// cout<< final->getName() << endl;
		
		if ( final == NULL /* && animation_current == NULL ){ */ && getStatus() != Status_Grab ){
			// cout<<"Going to walk or idle"<<endl;

			// cout<<"Grabbing,walking,idling"<<endl;

			bool moving = keyboard[ getKey( PAIN_KEY_FORWARD ) ] || keyboard[ getKey( PAIN_KEY_UP ) ] || keyboard[ getKey( PAIN_KEY_DOWN ) ];
			// if ( animation_current != movements[ "jump" ] ){
			if ( getMovement( "jump" ) == NULL || animation_current != getMovement( "jump" ) ){
				if ( !moving ){
					// animation_current = movements[ "idle" ];
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
						// animation_current = movements[ "walk" ];
						animation_current = getMovement( "walk" );
					}
				}
			}
			
			// cout<<"Animation is "<<animation_current<<endl;

			/*
			if ( keyboard[ getKey( PAIN_KEY_FORWARD ) ] || keyboard[ getKey( PAIN_KEY_UP ) ] || keyboard[ getKey( PAIN_KEY_DOWN ) ] ){
				animation_current = movements[ "walk" ];
				// animation_current->reset();
			} else if ( animation_current != movements[ "jump" ] ){
				// nextTicket();
				animation_current = movements[ "idle" ];
				animation_current->reset();
			}
			*/
		} else if ( final != NULL && animation_current != final ){
			// cout<<"Final animation = "<<final->getName()<<endl;
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
			// const vector< KeyPress > & keys = animation_current->getKeys();
			/*
			for ( unsigned int i = 0; i < keys.size(); i++ ){
				key_cache.pop_front();
			}
			*/
			key_cache.clear();
			
			// if ( animation_current == movements["jump"] ) {
			if ( animation_current == getMovement("jump") ) {
				double x = 0;
				double y = 0;
				if ( keyboard[ getKey( PAIN_KEY_FORWARD ) ] ){
					x = getSpeed() * 1.2;
				}
				if ( keyboard[ getKey( PAIN_KEY_DOWN ) ] ){
					y = getSpeed() * 1.2;	
				}
				if ( keyboard[ getKey( PAIN_KEY_UP ) ] ){
					y = -getSpeed() * 1.2;	
				}

				doJump( x, y );
				/*
				int x = 0;
				if ( keyboard[ getKey( PAIN_KEY_FORWARD ) ] ){
					if ( keyboard[ KEY_RIGHT ] ){
						x = 1;
					} else	x = -1;
				}
				doJump( x );
				*/
			}
		}
		
		// key_cache.clear();
	}

	if ( getStatus() == Status_Grab && animation_current == NULL ){
		// animation_current = movements[ "grab" ];
		animation_current = getMovement( "grab" );
	}

	

	// cout<<"Animation ticket = "<<getTicket()<<endl;

	/*
	key_cache.clear();
	*/
	/*
	if ( animation_current != movements[ "walk" ] )
		key_cache.clear();
	*/

	if ( getStatus() == Status_Ground && animation_current == getMovement( "walk" ) || animation_current == getMovement( "idle" ) ){
		/*
		if ( keyboard[ KEY_RIGHT ] ){
			setFacing( Object::FACING_RIGHT );
		} else if ( keyboard[ KEY_LEFT ] ) {
			setFacing( Object::FACING_LEFT ); 
		}
		*/


		// if ( keyboard[ KEY_RIGHT ] || keyboard[ KEY_LEFT ] ){
		if ( keyboard[ getKey( PAIN_KEY_FORWARD ) ] ){
			moveX( getSpeed() );
		} else if ( keyboard[ getKey( PAIN_KEY_BACK ) ] ){
			// cout << "Reverse facing" << endl;
			setFacing( getOppositeFacing() );
		}
		/*
		if ( keyboard[ KEY_RIGHT ] ){
			// setX( getX() + 1 );
			moveX( 1.2 );
		} else if ( keyboard[ KEY_LEFT ] ){
			// setX( getX() - 1 );
			moveX( -1.2 );
		}
		*/

		if ( keyboard[ getKey( PAIN_KEY_UP ) ] ){
			// setZ( getZ() - 1);
			moveZ( -getSpeed() );
		} else if ( keyboard[ getKey( PAIN_KEY_DOWN ) ] ){
			// setZ( getZ() + 1 );
			moveZ( getSpeed() );
		}
	} else {
	
		// if ( animation_current == movements[ "throw" ] ){
		if ( getMovement( "throw" ) != NULL && animation_current == getMovement( "throw" ) ){
			if ( getLink() == NULL ){
				cout<<"Link is null. This cant happen."<<endl;
				exit( 1 );
			}
			getLink()->thrown();
			getLink()->fall( 3.2, 5.0 );
		}
	}

	// cout << "Final animation = " << animation_current << endl;

	// cout<<"Ultimate animation current = "<< animation_current->getName() <<endl;
	// animation_current->Act();

}
