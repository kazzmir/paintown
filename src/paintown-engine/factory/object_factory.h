#ifndef _object_factory_h
#define _object_factory_h

#include <map>
#include <vector>
#include <string>

namespace Paintown{
    class Object;
    class Actor;
    class Cat;
    class Item;
    class BreakableItem;
    class Enemy;
    class NetworkCharacter;
    class NetworkPlayer;
}

class Heart;
class BlockObject;

/* factory class for instantiating new objects from a BlockObject */
class ObjectFactory{
public:
	static Paintown::Object * createObject( const BlockObject * block );
        static int getNextObjectId();
        static void maxId(int id);
	static void destroy();
        
private:
        static ObjectFactory * getFactory();

public:
        enum ObjectType{
            NoneType = -1,
            ItemType = 1,
            EnemyType = 2,
            ActorType = 3,
            CatType = 4,
            NetworkCharacterType = 5,
            NetworkPlayerType = 6,
            BreakableItemType = 7,
        };
	
private:
	ObjectFactory();
        Paintown::Object * makeObject( const BlockObject * block );

        Paintown::Object * makeBreakableItem(Paintown::BreakableItem * item, const BlockObject * block);
        Paintown::Object * makeItem(Paintown::Item * item, const BlockObject * block);
	Paintown::Object * makeEnemy(Paintown::Enemy * enemy, const BlockObject * block);
	Paintown::Object * makeActor(Paintown::Actor * ret, const BlockObject * block);
	Paintown::Object * makeCat(Paintown::Cat * ret, const BlockObject * block);
	Paintown::Object * makeNetworkCharacter(Paintown::NetworkCharacter * guy, const BlockObject * block);
	Paintown::Object * makeNetworkPlayer(Paintown::NetworkPlayer * guy, const BlockObject * block);

        int _getNextObjectId();
        void maxObjectId(int id);

	~ObjectFactory();

private:
        std::map< std::string, Paintown::Object * > cached;
	static ObjectFactory * factory;
        std::vector< Heart * > hearts;
        int nextObjectId;
};

#endif
