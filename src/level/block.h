#ifndef _block_h
#define _block_h

#include "util/load_exception.h"
#include <vector>

class BlockObject;
class Token;
class Heart;
class Object;

class Block{
public:
	Block( Token * tok ) throw( LoadException );

	inline void setLength( const int l ){
		length = l;
	}

	inline const int getLength() const {
		return length;
	}

	bool empty();

	vector< Heart * > createObjects( int total_length, int min_x, int max_x, vector< Object * > * list );

	~Block();

protected:
	int length;
	bool wait;

	vector< BlockObject * > objects;
};

#endif
