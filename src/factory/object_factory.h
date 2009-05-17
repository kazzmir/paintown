#ifndef _object_factory_h
#define _object_factory_h

#include <map>
#include <vector>
#include <string>

class Object;
class BlockObject;
class Actor;
class Cat;
class Heart;
class Item;
class Enemy;
class NetworkCharacter;
class NetworkPlayer;

/* factory class for instantiating new objects from a BlockObject */
class ObjectFactory{
public:
	static Object * createObject( BlockObject * block );
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
        };
	
private:
	ObjectFactory();
	Object * makeObject( BlockObject * block );

	Object * makeItem(Item * item, BlockObject * block);
	Object * makeEnemy(Enemy * enemy, BlockObject * block);
	Object * makeActor(Actor * ret, BlockObject * block);
	Object * makeCat(Cat * ret, BlockObject * block);
	Object * makeNetworkCharacter(NetworkCharacter * guy, BlockObject * block);
	Object * makeNetworkPlayer(NetworkPlayer * guy, BlockObject * block);

        int _getNextObjectId();
        void maxObjectId(int id);

	~ObjectFactory();

private:
        std::map< std::string, Object * > cached;
	static ObjectFactory * factory;
        std::vector< Heart * > hearts;
        int nextObjectId;
};

#endif
