#include <iostream>
#include <map>

#include "paintown-engine/level/blockobject.h"
#include "paintown-engine/object/stimulation.h"
#include "paintown-engine/object/enemy.h"
#include "paintown-engine/object/network_character.h"
#include "paintown-engine/object/network_player.h"
#include "paintown-engine/object/actor.h"
#include "paintown-engine/object/cat.h"
#include "paintown-engine/object/heart.h"
#include "paintown-engine/object/item.h"
#include "paintown-engine/object/object.h"
#include "object_factory.h"
#include "paintown-engine/script/script.h"
#include "util/load_exception.h"
#include "util/file-system.h"
#include "util/debug.h"

using namespace std;

ObjectFactory * ObjectFactory::factory = NULL;
Paintown::Object * ObjectFactory::createObject( const BlockObject * block ){
    return getFactory()->makeObject(block);
}
        
int ObjectFactory::getNextObjectId(){
    return getFactory()->_getNextObjectId();
}
        
void ObjectFactory::maxId(int id){
    getFactory()->maxObjectId(id);
}

ObjectFactory * ObjectFactory::getFactory(){
    if (factory == NULL){
        factory = new ObjectFactory();
    }
    return factory;
}
	
void ObjectFactory::destroy(){
    if (factory){
        delete factory;
        factory = NULL;
    }
}

ObjectFactory::ObjectFactory():
nextObjectId(0){
}

static Paintown::Stimulation * makeStimulation(const string & str, double value){
    if (str == "health"){
        return new Paintown::HealthStimulation(value);
    }
    if (str == "invincibility"){
        return new Paintown::InvincibilityStimulation(value);
    }
    if (str == "speed"){
        return new Paintown::SpeedStimulation(value);
    }
    return new Paintown::Stimulation();
}

Paintown::Object * ObjectFactory::makeItem( Paintown::Item * item, const BlockObject * block ){
    int x, z;
    block->getCoords(x, z);
    item->setStimulation(makeStimulation(block->getStimulationType(), block->getStimulationValue())); 
    item->setX(x);
    item->setZ(z);
    item->setObjectId(block->getId());
    return item;
}

Paintown::Object * ObjectFactory::makeActor( Paintown::Actor * ret, const BlockObject * block ){
    int x, z;
    block->getCoords(x, z);
    ret->setX(x);
    ret->setZ(z);
    ret->setObjectId(block->getId());

    return ret;
}

Paintown::Object * ObjectFactory::makeCat( Paintown::Cat * ret, const BlockObject * block ){
    int x, z;
    block->getCoords(x, z);
    ret->setX(x);
    ret->setZ(z);

    return ret;
}

Paintown::Object * ObjectFactory::makeNetworkCharacter( Paintown::NetworkCharacter * guy, const BlockObject * block ){

    guy->setMap(block->getMap());
    guy->setObjectId(block->getId());

    return guy;
}

Paintown::Object * ObjectFactory::makeNetworkPlayer( Paintown::NetworkPlayer * guy, const BlockObject * block ){
    guy->setMap(block->getMap());
    guy->setObjectId(block->getId());
    return guy;
}

Paintown::Object * ObjectFactory::makeEnemy( Paintown::Enemy * ret, const BlockObject * block ){
    int x, z;
    block->getCoords(x, z);
    ret->setX(x);
    ret->setZ(z);
    if (block->getAggression() > 0){
        ret->setAggression(block->getAggression());
    }

    ret->setName(block->getAlias());
    ret->setMap(block->getMap());
    ret->setMaxHealth(block->getHealth());
    ret->setHealth(block->getHealth());
    ret->setObjectId(block->getId());
    ret->setScriptObject(Script::Engine::getEngine()->createCharacter(ret));

    hearts.push_back(ret->getHeart());

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

Paintown::Object * ObjectFactory::makeObject( const BlockObject * block ){
    maxObjectId(block->getId());

    try{
        switch (block->getType()){
            case ItemType: {
                string cachePath = "item:" + block->getPath().path();
                if (cached[cachePath] == NULL){
                    cached[cachePath] = new Paintown::Item(block->getPath(), makeStimulation( block->getStimulationType(), block->getStimulationValue())); 
                    Global::debug(1) << "Cached " << block->getPath().path() << endl;
                    Global::info("Cached " + Storage::instance().cleanse(block->getPath()).path());
                }

                return makeItem( (Paintown::Item *) cached[cachePath]->copy(), block );
            }
            case NetworkCharacterType: {
                string cachedPath = "network-character:" + block->getPath().path();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new Paintown::NetworkCharacter( block->getPath(), 0 );
                    Global::debug( 1 ) << "Cached " << block->getPath().path() << endl;
                    Global::info("Cached " + Storage::instance().cleanse(block->getPath()).path());
                }
                return makeNetworkCharacter( (Paintown::NetworkCharacter *) cached[cachedPath]->copy(), block );
            }
            case NetworkPlayerType: {
                string cachedPath = "network-player:" + block->getPath().path();
                if (cached[cachedPath] == NULL){
                    cached[cachedPath] = new Paintown::NetworkPlayer( block->getPath(), 0 );
                    Global::debug(1) << "Cached " << block->getPath().path() << endl;
                    Global::info("Cached " + Storage::instance().cleanse(block->getPath()).path());
                }
                return makeNetworkPlayer( (Paintown::NetworkPlayer *) cached[cachedPath]->copy(), block );
            }
            case ActorType : {
                string cachedPath = "actor:" + block->getPath().path();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new Paintown::Actor( block->getPath() );
                    Global::debug( 1 ) << "Cached " << block->getPath().path() << endl;
                    Global::info("Cached " + Storage::instance().cleanse(block->getPath()).path());
                }

                return makeActor( (Paintown::Actor *) cached[cachedPath]->copy(), block );
            }
            case EnemyType : {
                string cachedPath = "enemy:" + block->getPath().path();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new Paintown::Enemy(block->getPath());
                    Global::debug(1) << "Cached " << block->getPath().path() << endl;
                    Global::info("Cached " + Storage::instance().cleanse(block->getPath()).path());
                }
                /* Hack! Set the map here so the original cached object stores
                 * remaps, thus saving time later
                 */
                ((Paintown::Enemy*) cached[cachedPath])->setMap(block->getMap());
                return makeEnemy( (Paintown::Enemy *) cached[cachedPath]->copy(), block );
            }
            case CatType : {
                string cachedPath = "cat:" + block->getPath().path();
                if ( cached[cachedPath] == NULL ){
                    cached[cachedPath] = new Paintown::Cat( block->getPath() );
                    Global::debug( 1 ) << "Cached " << block->getPath().path() << endl;
                    Global::info("Cached " + Storage::instance().cleanse(block->getPath()).path());
                }

                return makeCat( (Paintown::Cat *) cached[cachedPath]->copy(), block );
            }
            default : {
                Global::debug( 0 ) <<__FILE__<<": No type given for: "<<block->getPath().path()<<endl;
                break;
            }
        }
    } catch ( const LoadException & le ){
        Global::debug(0) << "Could not load " << block->getPath().path() << " because " << le.getTrace() << endl;
    }

    return NULL;
}

ObjectFactory::~ObjectFactory(){
    for (map< string, Paintown::Object * >::iterator it = cached.begin(); it != cached.end(); it++){
        delete (*it).second;
    }

    /*
       for ( vector< Heart * >::iterator it = hearts.begin(); it != hearts.end(); it++ ){
       delete *it;
       }
       */
}
