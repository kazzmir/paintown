#ifndef _heart_factory_h
#define _heart_factory_h

#include <vector>

using namespace std;

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

	vector< Heart * > hearts;
	static HeartFactory * my_heart;

};

#endif
