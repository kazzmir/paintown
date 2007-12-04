#include "util/funcs.h"
#include "util/bitmap.h"
#include "object/object.h"
#include "object/object_attack.h"
#include "object/character.h"
#include "util/load_exception.h"
#include "globals.h"
#include "object/effect.h"
#include "level/scene.h"
#include "world.h"
#include "network.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

World::World():
quake_time( 0 ){
	scene = NULL;
	bang = NULL;
}

World::World( const vector< Object * > & players, const string & path, int _screen_size ) throw( LoadException ):
quake_time( 0 ),
path( path ){
	scene = NULL;
	bang = NULL;
	screen_size = _screen_size;

	for ( vector< Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
		PlayerTracker t;
		t.min_x = 0;
		t.player = *it;
		// this->players.push_back( (PlayerTracker) { .min_x = 0, .player = *it } );
		this->players.push_back( t );
	}

	loadLevel( path );

	/*
	if ( player != NULL ){
		objects.push_back( player );
	}
	*/
}

World::~World(){

	if ( bang ){
		delete bang;
	}

	if ( scene ){
		delete scene;
	}

	deleteObjects( &objects );
}

void World::reloadLevel() throw( LoadException ){
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		it->min_x = 0;
	}
	loadLevel( path );
}
	
void World::loadLevel( const string & path ) throw( LoadException ){
	/*
	if ( scene ){
		delete scene;
		scene = NULL;
	}
	if ( bang ){
		delete bang;
		bang = NULL;
	}
	*/

	Scene * s = new Scene( path.c_str() );
	if ( scene != NULL ){
		delete scene;
	}
	scene = s;
	
	string bang_path( Util::getDataPath() + "/misc/flash/flash.txt" );
	Object * effect = new Effect( bang_path.c_str() );
	if ( bang != NULL ){
		delete bang;
	}
	bang = effect;
		
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		Object * const player = it->player;	
		player->setX( 100 + Util::rnd( 50 ) );
		player->setZ( getMinimumZ() + Util::rnd( (getMaximumZ() - getMinimumZ()) / 2 ) );
		player->setY( 0 );
	}

	deleteObjects( &objects );
	
	objects.clear();
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		objects.push_back( it->player );
	}

}
	
void World::deleteObjects( vector< Object * > * objects ){

	for ( vector< Object * >::iterator it = objects->begin(); it != objects->end(); it++ ){
		if ( ! isPlayer( *it ) ){
			delete *it;
		}
	}

}

void World::Quake( int q ){
	quake_time += q;
}

/* true if the player has crossed the finish line */
const bool World::finished() const {
	int f = scene->getFinished();
	if ( players.size() > 0 ){
		for ( vector< PlayerTracker >::const_iterator it = players.begin(); it != players.end(); it++ ){
			Object * const player = it->player;
			if ( f != -1 && player->getX() >= f ){
				return player->getX() >= f;
			}
		}
		return false;
	} else {
		return true;
	}
}
	
const bool World::isPlayer( Object * o ) const {
		for ( vector< PlayerTracker >::const_iterator it = players.begin(); it != players.end(); it++ ){
			if ( it->player == o ){
				return true;
			}
		}
		return false;
}

void World::addMessage( Network::Message m ){
	/* nothing */
}
	
Network::Message World::createBangMessage( int x, int y, int z ){
	Network::Message message;

	message.id = 0;
	message << CREATE_BANG;
	message << x << y << z;

	return message;
}

void World::doLogic(){

	vector< Object * > added_effects;
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * good = *it;
		good->act( &objects, this, &added_effects );

		if ( good->getZ() < getMinimumZ() ){
			good->setZ( getMinimumZ() );
		}
		if ( good->getZ() > getMaximumZ() ){
			good->setZ( getMaximumZ() );
		}

		/* Check for collisions */
		if ( good->isAttacking() ){
			// ObjectAttack * o_good = dynamic_cast<ObjectAttack*>( good );
			ObjectAttack * o_good = (ObjectAttack *)good;
			for ( vector<Object*>::iterator fight = objects.begin(); fight != objects.end(); fight++){
				if ( fight != it && (*fight)->isCollidable( good ) && good->isCollidable( *fight ) ){
					Global::debug( 2 ) << o_good << " is attacking " << *fight << " with " << o_good->getAttackName() << endl;

					// cout << "Zdistance: " << good->ZDistance( *fight ) << " = " << (good->ZDistance( *fight ) < o_good->minZDistance()) << endl;
					// cout << "Collision: " << (*fight)->collision( o_good ) << endl;
					if ( good->ZDistance( *fight ) <= o_good->minZDistance() && (*fight)->collision( o_good ) ){ 

						// cout << "There was a collision" << endl;
						// cout<<"Attacked " << *fight << " with animation "<< good->getAttackName() << " ticket " << o_good->getTicket() << endl;

						// if ( good->isAttacking() ){
							double x = 0, y = 0;
							// o_good->getAttackCoords(x,y);
							x = (*fight)->getX();
							y = (*fight)->getRY() - (*fight)->getHeight() + (*fight)->getHeight() / 3;
							
							if ( bang != NULL ){
								Object * addx = bang->copy();
								addx->setX( x );
								addx->setY( 0 );
								addx->setZ( y+addx->getHeight()/2 );
								addx->setHealth( 1 );
								added_effects.push_back( addx );
								addMessage( createBangMessage( (int) x, 0, (int) y + addx->getHeight() / 2 ) );
							}

							o_good->attacked( *fight, added_effects );
							(*fight)->collided( o_good, added_effects );
							(*fight)->takeDamage( this, o_good, o_good->getDamage() );
						// }
					}
				}
			}
		}
	}

	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
		if ( (*it)->getHealth() <= 0 ){
			(*it)->died( added_effects );
			if ( ! isPlayer( *it ) ){
				delete *it;
			}
			it = objects.erase( it );
		} else ++it;
	}

	/* special case for getting items */
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		Character * const cplayer = (Character *) it->player; 
		if ( cplayer->getStatus() == Status_Get ){
			for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
				Object * const o = *it;
				if ( o->isGettable() && o->ZDistance( cplayer ) < 10 && o->collision( cplayer ) ){
					o->touch( cplayer );
					delete o;
					it = objects.erase( it );
				} else ++it;
			}
		}
	}

	objects.insert( objects.end(), added_effects.begin(), added_effects.end() );
}

void World::killAllHumans( Object * player ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		if ( o != player ){
			// o->takeDamage( o->getMaxHealth() * 2 );
			o->takeDamage( this, NULL, 999999 );
		}
	}
}

int World::getMinimumZ(){
	return scene->getMinimumZ();
	/*
	const int MIN_WORLD_Z = 160;
	return MIN_WORLD_Z;
	*/
}

int World::getMaximumZ(){
	return scene->getMaximumZ();
	/*
	const int MAX_WORLD_Z = 232;
	return MAX_WORLD_Z;
	*/
}

void World::act(){
	if ( quake_time > 0 )
		quake_time--;
		
	doLogic();
	
	double lowest = 9999999;
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		Object * player = it->player;
		if ( player->getHealth() > 0 ){
			double mx = player->getX() - screen_size / 2;
			if ( it->min_x < mx ){
				it->min_x++;
			}

			if ( it->min_x > mx ){
				it->min_x = mx;
			}

			if ( it->min_x < 0 ){
				it->min_x = 0;
			}
		
			if ( it->min_x + screen_size >= scene->getLimit() ){
				it->min_x = scene->getLimit() - screen_size;
			}

			if ( it->min_x < lowest ){
				lowest = it->min_x;
			}
			
			/*
			if ( player->getX() < it->min_x ){
				player->setX( it->min_x );
			}
			*/

			if ( player->getX() > scene->getLimit() ){
				player->setX( scene->getLimit() );
			}
			if ( player->getZ() < getMinimumZ() ){
				player->setZ( getMinimumZ() );
			}
			if ( player->getZ() > getMaximumZ() ){
				player->setZ( getMaximumZ() );
			}
		}
	}

	doScene( (int) lowest, (int)(lowest + screen_size) );
	// scene->act( min_x, min_x + screen_size, &objects );
}
	
void World::doScene( int min_x, int max_x ){
	scene->act( min_x, max_x, &objects );
}

void World::addObject( Object * o ){
	objects.push_back( o );
}

void World::draw( Bitmap * work ){

	map< int, vector<Object*> > object_z;

	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * n = *it;
		object_z[ n->getRZ() ].push_back( n );
	}
	
	// min_x = (int)min_x_virtual;
	
	int min_x = 0;
	if ( players.size() > 0 ){
		min_x = (int) players[ 0 ].min_x;

		int max_x = (int)(players[ 0 ].player->getX() + screen_size / 2 > scene->getLimit() ? scene->getLimit() : players[ 0 ].player->getX() + screen_size / 2);
		min_x = (int)(max_x - screen_size);
		if ( min_x < 0 ){
			min_x = 0;
		}

		if ( min_x > players[ 0 ].min_x ){
			min_x = (int) players[ 0 ].min_x;
		}
	}

	scene->drawBack( min_x, work );
	for ( map<int,vector<Object *> >::iterator it = object_z.begin(); it != object_z.end(); it++ ){
		vector<Object *> & xx = (*it).second;
		for ( vector<Object *>::iterator mm = xx.begin(); mm != xx.end(); mm++ ){

			(*mm)->draw( work, min_x );
		}
	}
	scene->drawFront( min_x, work );
}
	
int World::getX(){
	if ( quake_time )
		return Util::rnd( 9 ) - 4;
	return 0;
}

int World::getY(){
	if ( quake_time )
		return Util::rnd( 9 ) - 4;
	return 0;
}
