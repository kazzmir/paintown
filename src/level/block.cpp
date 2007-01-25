#include <iostream>

#include "block.h"
#include "blockobject.h"
#include "object/enemy.h"
#include "globals.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "factory/object_factory.h"

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif

	
Block::Block( Token * tok ) throw ( LoadException ){

	if ( *tok != "block" ){
		throw LoadException("Not a scene block");
	}

	while ( tok->hasTokens() ){

		try{
			Token * current;
			*tok >> current;
			if ( *current == "length" ){
				int l;
				*current >> l;
				setLength( l );
			} else if ( *current == "wait" ){
				*current >> wait;
			} else if ( *current == "object" ){
				BlockObject * so;
				try{ 
					so = new BlockObject( current );
				} catch ( const LoadException & le ){
					throw le;
				}

				// cout<<"Caching object. Type: "<<so->getType()<<endl;
				// current->print(" ");

				/* cache the object in the factory */
				Object * tmp = ObjectFactory::createObject( so );
				if ( tmp == NULL ){
					current->print(" ");
					delete so;
					throw LoadException( "Could not cache object" );
				} else {
					/* clean up! */

					/*
					Heart * old_heart = NULL;

					if ( so->getType() == OBJECT_ENEMY )
						Heart * old_heart = ((Enemy *)tmp)->getHeart();
					*/
					delete tmp;
					/*
					if ( old_heart )
						delete old_heart;
					*/

					objects.push_back( so );
				}
			}
		} catch( const TokenException & te ){
			throw LoadException("Block parse exception");
		} catch( const LoadException & le ){
			cout<<"Ignoring error: "<<le.getReason()<<endl;
			// throw le;
		}
	}

}
	
bool Block::empty(){
	return objects.empty();
}

vector< Heart * > Block::createObjects( int total_length, int min_x, int max_x, vector< Object * > * list ){
	vector< Heart * > hearts;

	// cout<<"Total length = "<<total_length<<" Min_x = "<<min_x<<" Max_x = " << max_x << endl;
	// cout<<"Current block has "<<objects.size()<<" total = "<<total_length<<" min = "<<min_x<<" max_x = "<<max_x<<endl;
	for ( vector< BlockObject * >::iterator it = objects.begin(); it != objects.end(); ){
		BlockObject * obj = *it;

		int x, z;
		obj->getCoords( x, z );
		x += total_length;
		// if ( x < min_x || x >= max_x + 10 ){
		/* dont care about distance.. */
		if ( true ){

			Object * newobj = ObjectFactory::createObject( obj );
			if ( newobj == NULL ){
				delete obj;
				it = objects.erase( it );
				continue;
			}

			/* does this violate some OOP principle? oh wel */
			if ( obj->getType() == OBJECT_ENEMY ){
				Heart * h = ((Enemy *)newobj)->getHeart();
				hearts.push_back( h );
			}
			newobj->moveX( total_length );
			newobj->moveZ( MIN_WORLD_Z );
			if ( newobj->getZ() < MIN_WORLD_Z ){
				newobj->setZ( MIN_WORLD_Z );
			}
			if ( newobj->getZ() > MAX_WORLD_Z ){
				newobj->setZ( MAX_WORLD_Z );
			}
			list->push_back( newobj );
			// cout<<"Created new object at "<<newobj->getX()<<" " <<newobj->getZ()<<". Min_x = "<<min_x<<". block = "<<total_length<<endl;

			delete obj;
			it = objects.erase( it );
		} else {
			// cout<<"Coords not in range "<<x<<endl;
			it++;
		}
	}

	return hearts;
}
	
Block::~Block(){
	for ( vector<BlockObject *>::iterator it = objects.begin(); it != objects.end(); it++ ){
		delete *it;
	}
}
