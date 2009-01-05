#include <iostream>
#include <map>

#include "object/health_stimulation.h"
#include "object/stimulation.h"
#include "level/blockobject.h"
#include "object/enemy.h"
#include "object/network_character.h"
#include "object/network_player.h"
#include "object/actor.h"
#include "object/cat.h"
#include "object/heart.h"
#include "object/item.h"
#include "util/load_exception.h"
#include "object/object.h"
#include "object_factory.h"
#include "script/character.h"

using namespace std;

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

Object * ObjectFactory::makeItem( Item * item, BlockObject * block ){
	
	int x, z;
	block->getCoords( x, z );
	item->setX( x );
	item->setZ( z );
        item->setObjectId(block->getId());
	return item;

}

Object * ObjectFactory::makeActor( Actor * ret, BlockObject * block ){
	int x, z;
	block->getCoords( x, z );
	ret->setX( x );
	ret->setZ( z );
        ret->setObjectId(block->getId());

	return ret;
}

Object * ObjectFactory::makeCat( Cat * ret, BlockObject * block ){
	int x, z;
	block->getCoords( x, z );
	ret->setX( x );
	ret->setZ( z );

	return ret;
}

Object * ObjectFactory::makeNetworkCharacter( NetworkCharacter * guy, BlockObject * block ){

	guy->setMap( block->getMap() );
        guy->setObjectId(block->getId());

	return guy;
}

Object * ObjectFactory::makeNetworkPlayer( NetworkPlayer * guy, BlockObject * block ){
	guy->setMap( block->getMap() );
        guy->setObjectId(block->getId());
	return guy;
}

Object * ObjectFactory::makeEnemy( Enemy * ret, BlockObject * block ){

	int x, z;
	block->getCoords( x, z );
	ret->setX( x );
	ret->setZ( z );
	if ( block->getAggression() > 0 ){
		ret->setAggression( block->getAggression() );
	}
	
	ret->setName( block->getAlias() );
	ret->setMap( block->getMap() );
	ret->setMaxHealth( block->getHealth() );
	ret->setHealth( block->getHealth() );
        ret->setObjectId(block->getId());
        ret->setScriptObject(new Script::Character(ret));

	hearts.push_back( ret->getHeart() );

	return ret;
}

Object * ObjectFactory::makeObject( BlockObject * block ){
	
	try{
		switch( block->getType() ){
			case ItemType : {
				if ( cached[ block->getPath() ] == NULL ){
					cached[ block->getPath() ] = new Item( block->getPath(), makeStimulation( block->getStimulationType(), block->getStimulationValue() ) ); 
					Global::debug( 1 ) << "Cached " << block->getPath() << endl;
				}

				return makeItem( (Item *) cached[ block->getPath() ]->copy(), block );

				break;
			}
			case NetworkCharacterType : {
				if ( cached[ block->getPath() ] == NULL ){
					cached[ block->getPath() ] = new NetworkCharacter( block->getPath(), 0 );
					Global::debug( 1 ) << "Cached " << block->getPath() << endl;
				}
				return makeNetworkCharacter( (NetworkCharacter *) cached[ block->getPath() ]->copy(), block );
				break;
			}
                        case NetworkPlayerType : {
				if ( cached[ block->getPath() ] == NULL ){
					cached[ block->getPath() ] = new NetworkPlayer( block->getPath(), 0 );
					Global::debug( 1 ) << "Cached " << block->getPath() << endl;
				}
				return makeNetworkPlayer( (NetworkPlayer *) cached[ block->getPath() ]->copy(), block );
				break;
			}
			case ActorType : {
				if ( cached[ block->getPath() ] == NULL ){
					cached[ block->getPath() ] = new Actor( block->getPath() );
					Global::debug( 1 ) << "Cached " << block->getPath() << endl;
				}

				return makeActor( (Actor *) cached[ block->getPath() ]->copy(), block );
				break;
			}
			case EnemyType : {
				if ( cached[ block->getPath() ] == NULL ){
					cached[ block->getPath() ] = new Enemy( block->getPath() );
					Global::debug( 1 ) << "Cached " << block->getPath() << endl;
				}

				return makeEnemy( (Enemy *) cached[ block->getPath() ]->copy(), block );
				break;
			}
			case CatType : {
				if ( cached[ block->getPath() ] == NULL ){
					cached[ block->getPath() ] = new Cat( block->getPath() );
					Global::debug( 1 ) << "Cached " << block->getPath() << endl;
				}

				return makeCat( (Cat *) cached[ block->getPath() ]->copy(), block );
				break;
			}
			default : {
				Global::debug( 0 ) <<__FILE__<<": No type given for: "<<block->getPath()<<endl;
				break;
			}
		}
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load " << block->getPath() << " because " << le.getReason() << endl;
	}

	return NULL;
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
