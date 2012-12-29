#include <string>
#include <iostream>

#include "globals.h"
#include "animation.h"
#include "util/graphics/bitmap.h"
#include "character.h"
#include "enemy.h"
#include "util/funcs.h"
#include "util/font.h"
#include "factory/font_render.h"
#include "../level/scene.h"
#include "../game/nameplacer.h"
#include "alliance.h"
#include "util/debug.h"
#include "util/file-system.h"
#include "object.h"
#include "../game/world.h"
#include <math.h>

using namespace std;

namespace Paintown{

const int NORMAL_AGRESSION = 97;

Enemy::Enemy( ):
Character( ALLIANCE_ENEMY ),
aggression( NORMAL_AGRESSION ){

	constructSelf();
}

Enemy::Enemy( const char * filename ):
Character( filename, ALLIANCE_ENEMY  ),
aggression( NORMAL_AGRESSION ){
	constructSelf();
}

Enemy::Enemy( const Filesystem::AbsolutePath & filename ):
Character( filename, ALLIANCE_ENEMY  ),
aggression( NORMAL_AGRESSION ){
	constructSelf();
}

Enemy::Enemy( const Enemy & chr ):
Character( chr ),
aggression( chr.getAggression() ){
	constructSelf();
}

Enemy::Enemy( const Character & chr ):
Character( chr ),
aggression( NORMAL_AGRESSION ){
	constructSelf();
}

void Enemy::constructSelf(){
    want_path = false;
    show_name_time = 0;
    show_life = getHealth();
    id = -1;
}

Enemy::~Enemy(){
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
	
void Enemy::drawLifeBar( int x, int y, Graphics::Bitmap * work ){
	drawLifeBar( x, y, getDeath() == 0 ? show_life : 0, work );
}

void Enemy::drawFront( Graphics::Bitmap * work, int rel_x ){
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
		const Font & my_font = Font::getDefaultFont(20, 20 );
		const string & name = getName();
		// const int height = my_font.getHeight( name ) / 2;
		const int height = 20 / 2;
		fac->addMessage( my_font, (icon_x+x+1)*2, y*2, Graphics::makeColor(255,255,255), Graphics::MaskColor(), name );


		drawLifeBar( icon_x + x + 1, y + height + 1, work );
	}
}
        
void Enemy::died(const Util::ReferenceCount<Scene> & scene, std::vector< Object * > & objects){
    scene->removeEnemy(this);
    Character::died(scene, objects);
}

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

static bool closeFloat(double a, double b){
    const double epsilon = 0.0001;
    return fabs(a-b) < epsilon;
}
        
void Enemy::created(Scene & scene){
    scene.addEnemy(this);
}

/* returns the closest object in the X plane */
const Object * Enemy::findClosest( const vector< Object * > & enemies ){
	Object * e = NULL;
	double max = 0;
	for ( vector< Object * >::const_iterator it = enemies.begin(); it != enemies.end(); it++ ){
		Object * current = *it;
		/* should probably see if current is a character.. */
		double distance = fabs( current->getX() - getX() );
		if ( e == NULL || distance < max ){
			e = current;
			max = distance;
		}
	}

	return e;
}

/* TODO: cleanup */
void Enemy::act( vector< Object * > * others, World * world, vector< Object * > * add ){

	Global::debug( 4 ) << this << " " << getName() << " (x,y,z) = ( " << getX() << ", " << getY() << ", " << getZ() << " ) " << " status = " << getStatus() << endl;

	Character::act( others, world, add );

	if ( show_name_time % 10 == 0 ){
		world->addMessage( showNameMessage( show_name_time ) );
	}

	if ( show_life > getHealth() ){
		show_life--;
	}

	if ( show_life < getHealth() ){
		show_life++;
	}

	vector< Object * > enemies;
	
	if ( getStatus() != Status_Ground && getStatus() != Status_Jumping )
		return;

	filterEnemies( enemies, others );
		
	if (animation_current != NULL && animation_current->Act()){
		animation_current->reset();
		// nextTicket();
		// animation_current = movements[ "idle" ];
		animation_current = getMovement( "idle" );
		animation_current->reset();
	}
	
	if ( ! enemies.empty() ){
		/* guaranteed to get something back ... */
		const Object * main_enemy = findClosest( enemies );

		// if ( animation_current == movements["idle"] || animation_current == movements["walk"] ){
		if ( animation_current == getMovement( "idle" ) || animation_current == getMovement( "walk" ) ){
			faceObject( main_enemy );

			/*
			if ( main_enemy->getX() < getX() ){
				setFacing( Object::FACING_LEFT );
			} else	setFacing( Object::FACING_RIGHT );
			*/

			if ( ! want_path && Util::rnd( 50 ) > 30 ) {
				if ( Util::rnd( 15 ) < (double)getAggression() / 20.0 ){
					want_x = Util::rnd( (int) main_enemy->getX() - 200, (int) main_enemy->getX() + 200 );
					// want_z = rnd( main_enemy->getZ(), 10, 10 );
					want_z = Util::rnd( world->getMinimumZ(), world->getMaximumZ() );
					
				} else {
					want_x = (int)(main_enemy->getX() + (Util::rnd( 20 ) + 35) * (Util::rnd( 2 ) * 2 - 1));
					want_z = Util::rnd( (int) main_enemy->getZ() - 1, (int) main_enemy->getZ() + 1 );
				}

				if ( want_z < world->getMinimumZ() ){
					want_z = world->getMinimumZ();
				}
				if ( want_z > world->getMaximumZ() ){
					want_z = world->getMaximumZ();
				}

				want_path = true;
			}

			/* See if we can attack:
			 * If we are in range of the Z coordinate, relativeDistance(), then find an attack 
			 * with a suitable X range.  
			 */
			if ( Util::rnd( 100 ) >= getAggression() ){
				// cout<<getName()<<":In range"<<endl;
				vector<Util::ReferenceCount<Animation> > attacks;
				for ( map<string, Util::ReferenceCount<Animation> >::const_iterator it = getMovements().begin(); it != getMovements().end(); it++ ){
                                    Util::ReferenceCount<Animation> maybe = (*it).second;
					if ( maybe->isAttack() ){
						attacks.push_back( maybe );
                                        }
				}

				double attack_range = fabs( getX() - main_enemy->getX() );
				double zdistance = ZDistance( main_enemy );
				// cout<<getName()<<": Range = "<<attack_range<<endl;
				for ( vector<Util::ReferenceCount<Animation> >::iterator it = attacks.begin(); it != attacks.end(); ){
                                    Util::ReferenceCount<Animation> maybe = *it;
					// cout<<getName()<<":"<<maybe->getName()<<" range = "<<maybe->getRange()<<endl;
					if ( attack_range > maybe->getRange() || zdistance > maybe->getMinZDistance() ){
						it = attacks.erase( it );
					} else {
						it++;
					}
				}

				if ( !attacks.empty() ){
					// Animation * choose = attacks[ rnd( attacks.size() ) ];
					animation_current = attacks[ Util::rnd( attacks.size() ) ];
					world->addMessage( animationMessage() );
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
			if ( closeFloat(want_x, getX()) && closeFloat(want_z, getZ()) ){
				want_path = false;
			}

			bool moved = false;

			// animation_current = movements[ "walk" ];
			animation_current = getMovement( "walk" );
			world->addMessage( animationMessage() );
			if ( !closeFloat(want_x, getX()) ){
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
				moved = true;
			}

			if ( !closeFloat(want_z, getZ()) ){
				/*
				if ( Global::globalDebug() ){
					cout << "Z = " << getZ() << " velocity = " << velocity( getZ(), want_z, getSpeed() ) << endl;
				}
				*/
				moveZ( velocity( getZ(), want_z, getSpeed() ) );
				moved = true;
			}

			if ( moved ){
				world->addMessage( movedMessage() );
			}

		}
		
	// }
}

}
