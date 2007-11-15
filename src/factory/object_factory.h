#ifndef _object_factory_h
#define _obejct_factory_h

class Object;
class BlockObject;
class Actor;
class Heart;
class Item;
class Enemy;

#include <map>
#include <vector>

using namespace std;

/* factory class for instantiating new objects from a BlockObject */
class ObjectFactory{
public:
	static Object * createObject( BlockObject * block );
	static void destroy();

private:
	ObjectFactory();
	Object * makeObject( BlockObject * block );

	Object * makeItem( Item * item, BlockObject * block );
	Object * makeEnemy( Enemy * enemy, BlockObject * block );
	Object * makeActor( Actor * ret, BlockObject * block );

	~ObjectFactory();

private:
	map< string, Object * > cached;
	static ObjectFactory * factory;
	vector< Heart * > hearts;
	
};

#endif
