#include <iostream>

#include "block.h"
#include "blockobject.h"
#include "object/enemy.h"
#include "globals.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "util/token_exception.h"
#include "factory/object_factory.h"

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif

using namespace std;

/*
static const string dataPath( const string & str ){
	return Util::getDataPath() + str;
}
*/
	
Block::Block( Token * tok ) throw ( LoadException ):
id(-1),
finished( -1 ),
continuous( false ){

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
			} else if ( *current == "id" ){
                            int id;
                            *current >> id;
                            setId(id);
			} else if ( *current == "continuous" ){
				setContinuous( true );
			} else if ( *current == "finish" ){
				int f;
				*current >> f;
				setFinished( f );
			} else if ( *current == "object" ){
				try{ 
					BlockObject * so = new BlockObject( current );

					/* cache the object in the factory */
					Object * tmp = ObjectFactory::createObject(so);
					if ( tmp == NULL ){
						current->print(" ");
						delete so;
						throw LoadException( "Could not cache object" );
					} else {
						/* clean up! */

						delete tmp;
						objects.push_back( so );
					}

				} catch ( const LoadException & le ){
					throw le;
				}
			}
		} catch( const TokenException & te ){
			throw LoadException("Block parse exception: " + te.getReason());
		} catch( const LoadException & le ){
			cout<<"Ignoring error: "<<le.getReason()<<endl;
			// throw le;
		}
	}

}
	
bool Block::empty(){
	return objects.empty();
}

vector< Heart * > Block::createObjects( int total_length, int min_x, int max_x, int min_z, int max_z, vector< Object * > * list ){
	vector< Heart * > hearts;

    /* to silence compiler warnings */
    min_x = min_x;
    max_x = max_x;

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
			if ( obj->getType() == ObjectFactory::EnemyType ){
				Heart * h = ((Enemy *)newobj)->getHeart();
				hearts.push_back( h );
			}
			// newobj->moveX( total_length );
			newobj->moveRight( total_length );
			newobj->moveZ( min_z );
			if ( newobj->getZ() < min_z ){
				newobj->setZ( min_z );
			}
			if ( newobj->getZ() > max_z ){
				newobj->setZ( max_z );
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
