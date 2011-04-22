#ifndef _heart_factory_h
#define _heart_factory_h

#include <vector>

class Heart;

class HeartFactory{
public:

	static Heart * createHeart();

	static void destroy();

protected:

	Heart * makeHeart();
	bool clearHearts();

	HeartFactory();
	~HeartFactory();

        std::vector< Heart * > hearts;
	static HeartFactory * my_heart;

};

#endif
