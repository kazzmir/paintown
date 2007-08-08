#include "util/funcs.h"
#include "object/object.h"
#include "object/object_attack.h"
#include "object/character.h"
#include "util/load_exception.h"
#include "globals.h"
#include "object/effect.h"
#include "level/scene.h"
#include "world.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif

World::World():
player( NULL ),
quake_time( 0 ),
min_x( 0 ){
	scene = NULL;
	bang = NULL;
}

World::World( Object * _player, const string & path, int _screen_size ) throw( LoadException ):
player( _player ),
quake_time( 0 ),
min_x( 0 ),
path( path ){ 
	scene = NULL;
	bang = NULL;
	screen_size = _screen_size;

	loadLevel( path );

	/*
	if ( player != NULL ){
		objects.push_back( player );
	}
	*/
}

World::~World(){

	if ( bang )
		delete bang;

	if ( scene )
		delete scene;
	
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		if ( *it != player )
			delete *it;
	}

}

void World::reloadLevel() throw( LoadException ){
	min_x = 0;
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
		
	if ( player != NULL ){
		player->setX( 140 );
		player->setZ( (getMinimumZ() + getMaximumZ()) / 2 );
		player->setY( 0 );
	}

	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		if ( *it != player )
			delete *it;
	}
	objects.clear();
	if ( player != NULL ){
		objects.push_back( player );
	}

}

void World::Quake( int q ){
	quake_time += q;
}

/* true if the player has crossed the finish line */
const bool World::finished() const {
	if ( player != NULL ){
		int f = scene->getFinished();
		if ( f != -1 ){
			return player->getX() >= f;
		} else {
			return false;
		}
	} else {
		return true;
	}
}

void World::doLogic(){

	vector< Object * > added_effects;
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
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
					// cout << o_good << " is attacking " << *fight << " with " << o_good->getAttackName() << endl;

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
							}

							o_good->attacked( *fight, added_effects );
							(*fight)->collided( o_good, added_effects );
							(*fight)->takeDamage( o_good, o_good->getDamage() );
						// }
					}
				}
			}
		}

		if ( (*it)->getHealth() <= 0 ){
			(*it)->died( added_effects );
			if ( *it != player ){
				delete *it;
			}
			it = objects.erase( it );
		} else ++it;
		// it++;
	}

	/* special case for getting items */
	if ( player != NULL ){
		Character * const cplayer = (Character *) player; 
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
	
	if ( player != NULL ){
		double mx = player->getX() - screen_size / 2;
		// double mx = ch->getX() - work.getWidth() / 2;
		if ( min_x < mx ){
			min_x++;
			// min_x_virtual += 1.2;
		}
		if ( min_x + screen_size >= scene->getLimit() ){
			min_x = scene->getLimit() - screen_size;
		}
		/*
		if ( min_x >= scene->getLimit() ){
			min_x = scene->getLimit();
		}
		*/
		if ( player->getX() < min_x ){
			player->setX( min_x );
		}
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

	scene->act( min_x, min_x + screen_size, &objects );

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
