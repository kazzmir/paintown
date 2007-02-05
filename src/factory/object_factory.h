#ifndef _object_factory_h
#define _obejct_factory_h

class Object;
class BlockObject;
class Heart;

#include <map>

using namespace std;

/* factory class for instantiating new objects from a BlockObject */
class ObjectFactory{
public:
	static Object * createObject( BlockObject * block );
	static void destroy();

private:
	ObjectFactory();
	Object * makeObject( BlockObject * block );

	Object * makeItem( BlockObject * block );
	Object * makeEnemy( BlockObject * block );

	~ObjectFactory();

private:
	map< string, Object * > cached;
	static ObjectFactory * factory;
	vector< Heart * > hearts;
	
};

#endif
