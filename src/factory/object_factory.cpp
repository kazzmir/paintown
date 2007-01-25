#include <iostream>
#include <map>

#include "level/blockobject.h"
#include "object/enemy.h"
#include "object/heart.h"
#include "util/load_exception.h"
#include "object/object.h"
#include "object_factory.h"

using namespace std;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif

ObjectFactory * ObjectFactory::factory = NULL;
Object * ObjectFactory::createObject( BlockObject * block ){
	if ( factory == NULL ){
		factory = new ObjectFactory();
	}

	return factory->makeObject( block );
}
	
void ObjectFactory::destroy(){
	if ( factory ){
		delete factory;
		factory = NULL;
	}
}

ObjectFactory::ObjectFactory(){
}

Object * ObjectFactory::makeEnemy( BlockObject * block ){

	Object * ret;
	/*
	string name = block->getName();
	for ( int q = 0; q < name.size(); q++ ){
		if ( name[q] >= 'A' && name[q] <= 'Z' ){
			name[q] = name[q] - 'A' + 'a';
		}
	}
	string filename( "data/chars/" );

	filename += name;
	filename += "/";
	filename += name;
	filename += ".txt";
	*/
	string filename = block->getPath();
	try{ 
		ret = new Enemy( filename );
	} catch( const LoadException & le ){
		cout<<__FILE__<<" : "<<le.getReason()<<endl;
		// delete ret;
		return NULL;
	}
	int x, z;
	block->getCoords( x, z );
	ret->setX( x );
	ret->setZ( z );
	// cout<<__FILE__<<":"<<((Enemy *)ret)->getName()<<" map = "<<block->getMap()<<endl;
	((Enemy *)ret)->setName( block->getAlias() );
	((Enemy *)ret)->setMap( block->getMap() );
	ret->setHealth( block->getHealth() );
	ret->setMaxHealth( block->getHealth() );

	hearts.push_back( ((Enemy *)ret)->getHeart() );

	return ret;
}

Object * ObjectFactory::makeObject( BlockObject * block ){
	Object * ret;
	
	switch( block->getType() ){
		case OBJECT_ENEMY : {
		
			if ( cached[ block->getName() ] != NULL ){
				ret = cached[ block->getName() ]->copy();
				int x, z;
				block->getCoords( x, z );
				ret->setX( x );
				ret->setZ( z );
				((Enemy *)ret)->setName( block->getAlias() );
				((Enemy *)ret)->setMap( block->getMap() );
				ret->setHealth( block->getHealth() );
				ret->setMaxHealth( block->getHealth() );
				return ret;
			}

			ret = makeEnemy( block );
			if ( ret == NULL ){
				cout<<"Could not instantiate enemy"<<endl;
				return NULL;
			}
			break;
		}
		default : {
			cout<<__FILE__<<": No type given for: "<<block->getName()<<endl;
			return NULL;

			break;
		}
	}

	/* we keep the original becuase only the original stores the animations
	 * and other special memory things 
	 */
	// cout<<"Adding cached object to factory"<<endl;
	cached[ block->getName() ] = ret;
	// cout<<"Object factory has "<<cached.size()<<" elements"<<endl;
	return ret->copy();
}

ObjectFactory::~ObjectFactory(){
	// cout<<"Object Factory erasing: "<<cached.size()<<" elements"<<endl;
	for ( map< string, Object * >::iterator it = cached.begin(); it != cached.end(); it++ ){
		// cout<<"Object factory deleting object: "<< (*it).second << endl;

		delete (*it).second;
	}

	/*
	for ( vector< Heart * >::iterator it = hearts.begin(); it != hearts.end(); it++ ){
		delete *it;
	}
	*/
}
