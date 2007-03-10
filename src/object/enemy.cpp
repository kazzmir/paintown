#include <string>
#include <iostream>

#include "animation.h"
#include "util/bitmap.h"
#include "character.h"
#include "enemy.h"
#include "util/funcs.h"
#include "util/font.h"
#include "factory/font_render.h"
#include "factory/font_factory.h"
#include "nameplacer.h"
#include "globals.h"
#include "heart.h"
#include "factory/heart_factory.h"
#include "util/load_exception.h"
#include "object.h"
#include "world.h"

using namespace std;

static const char * ENEMY_FONT = "data/fonts/arial.ttf";

Enemy::Enemy( ):
Character( ALLIANCE_ENEMY ),
aggression( 97 ){

	constructSelf();
}

Enemy::Enemy( const char * filename ) throw( LoadException ):
Character( filename, ALLIANCE_ENEMY  ),
aggression( 97 ){
	constructSelf();
}

Enemy::Enemy( const string & filename ) throw( LoadException ):
Character( filename, ALLIANCE_ENEMY  ),
aggression( 97 ){
	constructSelf();
}

Enemy::Enemy( const Enemy & chr ) throw( LoadException ):
Character( chr ),
aggression( chr.getAggression() ){
	constructSelf();
}

void Enemy::constructSelf(){
	// heart = new Heart();
	heart = HeartFactory::createHeart();
	want_path = false;
	show_name_time = 0;
	show_life = getHealth();
	id = -1;
}

Enemy::~Enemy(){
	heart->setAlive( false );
}
	
Object * Enemy::copy(){
	return new Enemy( *this );
}
	
/*
void Enemy::takeDamage( ObjectAttack * obj, int x ){
	Character::takeDamage( obj, x );
	show_name_time += 90;
}
*/

void Enemy::hurt( int x ){
	Character::hurt( x );
	show_name_time += 90;
}

void Enemy::draw( Bitmap * work, int rel_x ){
	Character::draw( work, rel_x );

	if ( show_name_time > 0 ){
		show_name_time--;

		int x, y;
		NamePlacer::getPlacement( x, y, id );

		if ( icon ){
			icon->draw( x, y, *work );
		}
		int icon_x = icon ? icon->getWidth() : 0;

		// work->printfNormal( icon_x + x + 1, y, Bitmap::makeColor(255,255,255), getName() );
		FontRender * fac = FontRender::getInstance();
		// Font * my_font = FontFactory::getFont( NAME_FONT );
		// const Font & my_font = Font::getFont( NAME_FONT );
		const Font & my_font = Font::getFont( ENEMY_FONT, 20, 20 );
		const string & name = getName();
		// const int height = my_font.getHeight( name ) / 2;
		const int height = 20 / 2;
		fac->addMessage( my_font, (icon_x+x+1)*2, y*2, Bitmap::makeColor(255,255,255), -1, name );

		if ( show_life > getHealth() ){
			show_life--;
		}

		if ( show_life < getHealth() ){
			show_life++;
		}

		drawLifeBar( icon_x + x + 1, y + height, show_life, work );
	}
}

/*
void Enemy::filterEnemies( vector< Object * > * mine, vector< Object * > * all ){
	for ( vector< Object * >::iterator it = all->begin(); it != all->end(); it++ ){
		Object * n = *it;
		int q = n->getAlliance();
		if ( q != ALLIANCE_NONE && q != getAlliance() )
			mine->push_back( n );

			/ *
		if ( n->getAlliance() == ALLIANCE_PLAYER ){
			mine->push_back( n );
		}
		* /
	}
}
*/

static inline double min( double a, double b ){
	return a < b ? a : b;
}

static inline double max( double a, double b ){
	return a > b ? a : b;
}

/* if x1 < x2, return the minimum of speed and x2 - x1 */
static double velocity( double x1, double x2, double speed ){
	if ( x2 > x1 ){
		return min( speed, x2 - x1 );
	} else {
		return -min( speed, fabs( x1 - x2 ) );
	}
	// moveX( want_x > getX() ? getSpeed() : -getSpeed() );
}

void Enemy::act( vector< Object * > * others, World * world ){

	Character::act( others, world );

	vector< Object * > enemies;
	
	if ( getStatus() != Status_Ground )
		return;

	filterEnemies( enemies, others );
		
	if ( animation_current->Act() ){
		animation_current->reset();
		// nextTicket();
		// animation_current = movements[ "idle" ];
		animation_current = getMovement( "idle" );
		animation_current->reset();
	}
	
	if ( !enemies.empty() ){
		Object * main_enemy = enemies[ 0 ];

		// if ( animation_current == movements["idle"] || animation_current == movements["walk"] ){
		if ( animation_current == getMovement( "idle" ) || animation_current == getMovement( "walk" ) ){
			if ( main_enemy->getX() < getX() ){
				setFacing( Object::FACING_LEFT );
			} else	setFacing( Object::FACING_RIGHT );

			if ( !want_path && Util::rnd( 30 ) > 10 ) {
				/*want_x == getX() && want_z == getZ() ){ */
				want_x = Util::rnd( (int) main_enemy->getX() - 200, (int) main_enemy->getX() + 200 );
				// want_z = rnd( main_enemy->getZ(), 10, 10 );
				want_z = Util::rnd( world->getMinimumZ(), world->getMaximumZ() );

				if ( want_z < world->getMinimumZ() ){
					want_z = world->getMinimumZ();
				}
				if ( want_z > world->getMaximumZ() ){
					want_z = world->getMaximumZ();
				}
				// cout<<"Min: "<<MIN_WORLD_Z<<" Max: "<<MAX_WORLD_Z<<" Want: "<<want_z <<endl;

				want_path = true;
			}

			/* See if we can attack:
			 * If we are in range of the Z coordinate, relativeDistance(), then find an attack 
			 * with a suitable X range.  
			 */
			if ( ZDistance( main_enemy ) < MIN_RELATIVE_DISTANCE && Util::rnd( 100 ) >= getAggression() ){
				// cout<<getName()<<":In range"<<endl;
				vector< Animation * > attacks;
				for ( map<string,Animation *>::const_iterator it = getMovements().begin(); it != getMovements().end(); it++ ){
					Animation * maybe = (*it).second;
					if ( maybe->isAttack() )
						attacks.push_back( maybe );
				}

				double attack_range = fabs( getX() - main_enemy->getX() );
				// cout<<getName()<<": Range = "<<attack_range<<endl;
				for ( vector< Animation * >::iterator it = attacks.begin(); it != attacks.end(); ){
					Animation * maybe = *it;
					// cout<<getName()<<":"<<maybe->getName()<<" range = "<<maybe->getRange()<<endl;
					if ( attack_range > maybe->getRange() ){
						it = attacks.erase( it );
					} else {
						it++;
					}
				}

				if ( !attacks.empty() ){
					// Animation * choose = attacks[ rnd( attacks.size() ) ];
					animation_current = attacks[ Util::rnd( attacks.size() ) ];
					// cout<<getName()<<" chose "<<animation_current->getName()<<endl;
					nextTicket();
					animation_current->reset();
					want_path = false;
				} else {
					// cout<<getName()<<": no attacks?"<<endl;
				}
			}

		}
	}

	// if ( getStatus() == Status_Ground ){
		// cout<<"WantX: "<<want_x<<" WantZ: "<<want_z<<" getX: "<<getX()<<" getZ: "<<getZ()<<endl;
		// cout << "Speed: " << getSpeed() << endl;
		if ( want_path ){
		// if ( want_x != getX() || want_z != getZ() ){
			if ( want_x == getX() && want_z == getZ() ){
				want_path = false;
			}

			// animation_current = movements[ "walk" ];
			animation_current = getMovement( "walk" );
			if ( want_x != getX() ){
				int dir = 1;
				if ( getFacing() == Object::FACING_LEFT ){
					dir = -1;
					// moveX( velocity( want_x, getX(), getSpeed() ) );
					// want_x > getX() ? -getSpeed() : getSpeed() );
				} else {
					// moveX( - velocity( want_x, getX(), getSpeed() ) );
					// moveX( want_x > getX() ? getSpeed() : -getSpeed() );
				}
				moveX( dir * velocity( getX(), want_x, getSpeed() ) );
			}

			if ( want_z != getZ() ){
				moveZ( velocity( getZ(), want_z, getSpeed() ) );
			}

		}
		
	// }

	/*
	if ( animation_current ){
		if ( animation_current->Act() ){
			// animation_current->reset();
			nextTicket();
			if ( getStatus() == Status_Fell ){
				animation_current = movements[ "rise" ];
				setStatus( Status_Ground );
			} else	animation_current = movements[ "idle" ];

			animation_current->reset();
		}
	}
	*/
}
