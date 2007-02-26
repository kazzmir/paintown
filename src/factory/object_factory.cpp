#include <iostream>
#include <map>

#include "object/health_stimulation.h"
#include "object/stimulation.h"
#include "level/blockobject.h"
#include "object/enemy.h"
#include "object/heart.h"
#include "object/item.h"
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

static Stimulation * makeStimulation( const string & str, int value ){
	if ( str == "health" ){
		return new HealthStimulation( value );
	}
	return new Stimulation();
}

Object * ObjectFactory::makeItem( BlockObject * block ){
	
	try{
		Item * item;
		item = new Item( block->getPath(), makeStimulation( block->getStimulationType(), block->getStimulationValue() ) );
		int x, z;
		block->getCoords( x, z );
		item->setX( x );
		item->setZ( z );
		return item;
	} catch ( LoadException le ){
		cout << __FILE__ << " : " << le.getReason() << endl;
		// delete ret;
		return NULL;
	}
}

Object * ObjectFactory::makeEnemy( BlockObject * block ){

	Enemy * ret;
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
	
	
	/*
	((Enemy *)ret)->setName( block->getAlias() );
	((Enemy *)ret)->setMap( block->getMap() );
	*/
	
	ret->setName( block->getAlias() );
	ret->setMap( block->getMap() );
	ret->setHealth( block->getHealth() );
	ret->setMaxHealth( block->getHealth() );

	hearts.push_back( ret->getHeart() );

	return ret;
}

Object * ObjectFactory::makeObject( BlockObject * block ){
	Object * ret;
	
	switch( block->getType() ){
		case OBJECT_ITEM : {
			if ( cached[ block->getName() ] != NULL ){
				return cached[ block->getName() ]->copy();
			}
			
			cout << "ObjectFactory creating " << block->getPath() << endl;
			ret = makeItem( block );
			break;
		}
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

			cout << "ObjectFactory creating " << block->getPath() << endl;
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
