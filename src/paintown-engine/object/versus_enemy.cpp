#include "versus_enemy.h"
#include "character.h"
#include "util/exceptions/load_exception.h"
#include "util/funcs.h"
#include "animation.h"
#include "object.h"
#include <math.h>

using namespace std;

namespace Paintown{

static const int DO_NOTHING = 0;
static const int DO_WALK_FORWARD = 1;
static const int DO_WALK_BACKWARD = 2;

VersusEnemy::VersusEnemy( const Character & chr ) throw( LoadException ):
Enemy( chr ),
what_to_do( DO_NOTHING ){
}
	
VersusEnemy::~VersusEnemy(){
}
	
void VersusEnemy::hurt( int x ){
	Character::hurt( x );
}
	
void VersusEnemy::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	Character::act( others, world, add );

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
		
	if ( animation_current->Act() ){
		animation_current->reset();
		// nextTicket();
		// animation_current = movements[ "idle" ];
		animation_current = getMovement( "idle" );
		animation_current->reset();
	} else if ( animation_current != getMovement( "walk" ) && animation_current != getMovement( "idle" ) ){
		return;
	}

	switch( what_to_do ){
		case DO_NOTHING : {
			/* well that was easy enough */
			break;
		}
		case DO_WALK_BACKWARD : {
			animation_current = getMovement( "walk" );
			moveX( -getSpeed() );
			break;
		}
		case DO_WALK_FORWARD : {
			animation_current = getMovement( "walk" );
			moveX( getSpeed() );
			break;
		}
	}

	if ( Util::rnd( 30 ) == 0 ){
		switch( Util::rnd( 3 ) ){
			case 0 : {
				what_to_do = DO_NOTHING;
				break;
			}
			case 1 : {
				what_to_do = DO_WALK_FORWARD;
				break;
			}
			case 2 : {
				what_to_do = DO_WALK_BACKWARD;
				break;
			}
			default : {
				what_to_do = DO_NOTHING;
				break;
			}
		}
	}

	if ( ! enemies.empty() ){
		/* guaranteed to get something back ... */
		const Object * main_enemy = findClosest( enemies );

		if ( animation_current == getMovement( "idle" ) || animation_current == getMovement( "walk" ) ){
			/* See if we can attack:
			 * If we are in range of the Z coordinate, relativeDistance(), then find an attack 
			 * with a suitable X range.  
			 */
			if ( Util::rnd( 100 ) >= getAggression() ){
				vector<Util::ReferenceCount<Animation> > attacks;
				for ( map<string, Util::ReferenceCount<Animation> >::const_iterator it = getMovements().begin(); it != getMovements().end(); it++ ){
                                    Util::ReferenceCount<Animation>  maybe = (*it).second;
					if ( maybe->isAttack() )
						attacks.push_back( maybe );
				}

				double attack_range = fabs( getX() - main_enemy->getX() );
				double zdistance = ZDistance( main_enemy );
				// cout<<getName()<<": Range = "<<attack_range<<endl;
				for ( vector< Util::ReferenceCount<Animation>  >::iterator it = attacks.begin(); it != attacks.end(); ){
                                    Util::ReferenceCount<Animation> maybe = *it;
					if ( attack_range > maybe->getRange() || zdistance > maybe->getMinZDistance() ){
						it = attacks.erase( it );
					} else {
						it++;
					}
				}

				if ( !attacks.empty() ){
					animation_current = attacks[ Util::rnd( attacks.size() ) ];
					nextTicket();
					animation_current->reset();
					want_path = false;
				}
			}
		}
	}
}

}
