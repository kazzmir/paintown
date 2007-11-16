#ifndef _block_object_h
#define _block_object_h

#include "util/load_exception.h"
#include <string>

using namespace std;

class Token;


class BlockObject{
public:
	BlockObject( Token * tok ) throw ( LoadException );

/* getters */
public:

	inline const string & getAlias() const {
		return alias;
	}

	inline const int getType() const {
		return type;
	}

	inline const string & getName() const {
		return name;
	}

	inline const string & getPath() const {
		return path;
	}

	inline const string getStimulationType(){
		return stimulationType;
	}

	inline const int getStimulationValue(){
		return stimulationValue;
	}

	inline const int getMap() const {
		return map;
	}

	inline const int getAggression() const {
		return aggression;
	}

	inline const int getHealth() const {
		return health;
	}

	inline void getCoords( int & x, int & z ) {
		x = coords_x;
		z = coords_z;
	}

/* setters */
public:

	inline void setType( const int l ) {
		type = l;
	}

	inline void setPath( const string & p ){
		path = p;
	}

	inline void setAggression( const int a ){
		aggression = a;
	}

	inline void setName( const string & s ) {
		name = s;
	}

	inline void setAlias( const string & s ) {
		alias = s;
	}

	inline void setMap( int m ) {
		map = m;
	}

	inline void setStimulationType( string s ){
		stimulationType = s;
	}

	inline void setStimulationValue( int i ){
		stimulationValue = i;
	}

	inline void setHealth( int h ) {
		health = h;
	}

	inline void setCoords( int x, int z ) {
		coords_x = x;
		coords_z = z;
	}

	~BlockObject();

protected:

	/* the type of object this is */
	int type;

	int aggression;

	string name;
	string alias;
	string path;

	/* the sprite to map to */
	int map;

	int health;

	int coords_x, coords_z;

	string stimulationType;
	int stimulationValue;
};

#endif
