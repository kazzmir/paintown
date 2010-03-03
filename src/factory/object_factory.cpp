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
#include "util/file-system.h"
#include "object/object.h"
#include "object_factory.h"
#include "script/script.h"

using namespace std;

ObjectFactory * ObjectFactory::factory = NULL;
Object * ObjectFactory::createObject( const BlockObject * block ){
	return getFactory()->makeObject( block );
}
        
int ObjectFactory::getNextObjectId(){
    return getFactory()->_getNextObjectId();
}
        
void ObjectFactory::maxId(int id){
    getFactory()->maxObjectId(id);
}

ObjectFactory * ObjectFactory::getFactory(){
    if ( factory == NULL ){
        factory = new ObjectFactory();
    }
    return factory;
}
	
void ObjectFactory::destroy(){
    if ( factory ){
        delete factory;
        factory = NULL;
    }
}

ObjectFactory::ObjectFactory():
nextObjectId(0){
}

static Stimulation * makeStimulation( const string & str, int value ){
	if ( str == "health" ){
		return new HealthStimulation( value );
	}
	return new Stimulation();
}

Object * ObjectFactory::makeItem( Item * item, const BlockObject * block ){
	
	int x, z;
	block->getCoords( x, z );
	item->setX( x );
	item->setZ( z );
        item->setObjectId(block->getId());
	return item;

}

Object * ObjectFactory::makeActor( Actor * ret, const BlockObject * block ){
	int x, z;
	block->getCoords( x, z );
	ret->setX( x );
	ret->setZ( z );
        ret->setObjectId(block->getId());

	return ret;
}

Object * ObjectFactory::makeCat( Cat * ret, const BlockObject * block ){
	int x, z;
	block->getCoords( x, z );
	ret->setX( x );
	ret->setZ( z );

	return ret;
}

Object * ObjectFactory::makeNetworkCharacter( NetworkCharacter * guy, const BlockObject * block ){

	guy->setMap( block->getMap() );
        guy->setObjectId(block->getId());

	return guy;
}

Object * ObjectFactory::makeNetworkPlayer( NetworkPlayer * guy, const BlockObject * block ){
	guy->setMap( block->getMap() );
        guy->setObjectId(block->getId());
	return guy;
}

Object * ObjectFactory::makeEnemy( Enemy * ret, const BlockObject * block ){

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
        ret->setScriptObject(Script::Engine::getEngine()->createCharacter(ret));

	hearts.push_back( ret->getHeart() );

	return ret;
}
        
int ObjectFactory::_getNextObjectId(){
    return nextObjectId++;
}

void ObjectFactory::maxObjectId(int id){
    if (id >= nextObjectId){
        nextObjectId = id + 1;
    }
}

Object * ObjectFactory::makeObject( const BlockObject * block ){
    maxObjectId(block->getId());

    try{
        switch (block->getType()){
            case ItemType : {
                string cachePath = "item:" + block->getPath();
                if (cached[cachePath] == NULL){
                    cached[cachePath] = new Item(block->getPath(), makeStimulation( block->getStimulationType(), block->getStimulationValue() ) ); 
                    Global::debug( 1 ) << "Cached " << block->getPath() << endl;
                    Global::info("Cached " + Filesystem::cleanse(block->getPath()));
                }

                return makeItem( (Item *) cached[cachePath]->copy(), block );
            }
            case NetworkCharacterType : {
                string cachedPath = "network-character:" + block->getPath();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new NetworkCharacter( block->getPath(), 0 );
                    Global::debug( 1 ) << "Cached " << block->getPath() << endl;
                    Global::info("Cached " + Filesystem::cleanse(block->getPath()));
                }
                return makeNetworkCharacter( (NetworkCharacter *) cached[cachedPath]->copy(), block );
            }
            case NetworkPlayerType : {
                string cachedPath = "network-player:" + block->getPath();
                if (cached[cachedPath] == NULL){
                    cached[cachedPath] = new NetworkPlayer( block->getPath(), 0 );
                    Global::debug( 1 ) << "Cached " << block->getPath() << endl;
                    Global::info("Cached " + Filesystem::cleanse(block->getPath()));
                }
                return makeNetworkPlayer( (NetworkPlayer *) cached[cachedPath]->copy(), block );
            }
            case ActorType : {
                string cachedPath = "actor:" + block->getPath();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new Actor( block->getPath() );
                    Global::debug( 1 ) << "Cached " << block->getPath() << endl;
                    Global::info("Cached " + Filesystem::cleanse(block->getPath()));
                }

                return makeActor( (Actor *) cached[cachedPath]->copy(), block );
            }
            case EnemyType : {
                string cachedPath = "enemy:" + block->getPath();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new Enemy( block->getPath() );
                    Global::debug( 1 ) << "Cached " << block->getPath() << endl;
                    Global::info("Cached " + Filesystem::cleanse(block->getPath()));
                }
                return makeEnemy( (Enemy *) cached[cachedPath]->copy(), block );
            }
            case CatType : {
                string cachedPath = "cat:" + block->getPath();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new Cat( block->getPath() );
                    Global::debug( 1 ) << "Cached " << block->getPath() << endl;
                    Global::info("Cached " + Filesystem::cleanse(block->getPath()));
                }

                return makeCat( (Cat *) cached[cachedPath]->copy(), block );
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
