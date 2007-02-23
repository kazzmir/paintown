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

World::World( Object * _player, const string & path, int _screen_size ):
player( _player ),
quake_time( 0 ),
min_x( 0 ){
	scene = NULL;
	bang = NULL;
	screen_size = _screen_size;

	loadLevel( path );

	if ( player != NULL ){
		objects.push_back( player );
	}
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
	
void World::loadLevel( const string & path ){
	if ( scene )
		delete scene;
	if ( bang ){
		delete bang;
	}
		
	string bang_path( "data/misc/flash/flash.txt" );
	try{
		bang = new Effect( bang_path.c_str() );
	} catch ( const LoadException & ex ){
		cout<<"Could not load effect: "<< ex.getReason() << endl;
	}
	
	try{
		scene = new Scene( path.c_str() );
	} catch ( const LoadException & e ){
		cout<<"Error loading scene: "<<e.getReason()<<endl;
	}

	if ( player != NULL ){
		player->setX( 140 );
		player->setZ( 100 );
	}
}

void World::Quake( int q ){
	quake_time += q;
}

void World::doLogic(){

	vector< Object * > added_effects;
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
		Object * good = *it;
		good->act( &objects, this );

		/* Check for collisions */
		if ( good->isAttacking() ){
			// ObjectAttack * o_good = dynamic_cast<ObjectAttack*>( good );
			ObjectAttack * o_good = (ObjectAttack *)good;
			for ( vector<Object*>::iterator fight = objects.begin(); fight != objects.end(); fight++){
				if ( (*fight)->isCollidable( good ) && good->isCollidable( *fight ) ){

					// cout<<"Attacking"<<endl;
					if ( good->ZDistance( *fight ) < MIN_RELATIVE_DISTANCE && (*fight)->collision( o_good ) ){ 

						// cout<<"Attacked " << *fight << " with animation "<< good->getAttackName() << " ticket " << o_good->getTicket() << endl;

						// if ( good->isAttacking() ){
							int x = 0, y = 0;
							// o_good->getAttackCoords(x,y);
							x = (*fight)->getX();
							y = (*fight)->getRY() - (*fight)->getHeight() + (*fight)->getHeight() / 3;
							
							/*
							Object * addx = bang->copy();
							addx->setX( x );
							addx->setY( 0 );
							addx->setZ( y+addx->getHeight()/2 );
							addx->setHealth( 1 );
							added_effects.push_back( addx );
							*/

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
	Character * const cplayer = (Character *) player; 
	if ( cplayer->getStatus() == Status_Get ){
		for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
			Object * const o = *it;
			if ( o->isGettable() && o->collision( cplayer ) ){
				o->touch( cplayer );
				delete o;
				it = objects.erase( it );
			} else ++it;
		}
	}

	objects.insert( objects.end(), added_effects.begin(), added_effects.end() );
}

void World::act(){
	if ( quake_time > 0 )
		quake_time--;
		
	doLogic();
	
	if ( player != NULL ){
		int mx = player->getX() - screen_size / 2;
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
		if ( player->getZ() < MIN_WORLD_Z ){
			player->setZ( MIN_WORLD_Z );
		}
		if ( player->getZ() > MAX_WORLD_Z ){
			player->setZ( MAX_WORLD_Z );
		}

	}

	scene->act( min_x, min_x + screen_size, &objects );

}

void World::draw( Bitmap * work ){

	map< int, vector<Object*> > object_z;

	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * n = *it;
		object_z[ n->getZ() ].push_back( n );
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
