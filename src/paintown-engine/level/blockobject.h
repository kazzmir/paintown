#ifndef _block_object_h
#define _block_object_h

#include "util/load_exception.h"
#include "util/file-system.h"
#include "../factory/object_factory.h"
#include <string>

class Token;

class BlockObject{
public:
	BlockObject();
	BlockObject(const BlockObject & copy);
	BlockObject(const Token * tok);

/* getters */
public: 

	inline const std::string & getAlias() const {
		return alias;
	}

	inline ObjectFactory::ObjectType getType() const {
		return type;
	}

	inline const std::string & getName() const {
		return name;
	}

	inline const Filesystem::AbsolutePath & getPath() const {
            return path;
	}

	inline const std::string getStimulationType() const {
		return stimulationType;
	}

	inline double getStimulationValue() const {
		return stimulationValue;
	}

	inline int getMap() const {
		return map;
	}

	inline int getAggression() const {
		return aggression;
	}

	inline int getHealth() const {
		return health;
	}

	inline void getCoords( int & x, int & z ) const {
		x = coords_x;
		z = coords_z;
	}

/* setters */
public:

	inline void setType(const ObjectFactory::ObjectType l) {
		type = l;
	}

	inline void setPath(const Filesystem::AbsolutePath & p){
		path = p;
	}

	inline void setAggression( const int a ){
		aggression = a;
	}

	inline void setName( const std::string & s ) {
		name = s;
	}

	inline void setAlias( const std::string & s ) {
		alias = s;
	}

	inline void setMap( int m ) {
		map = m;
	}

	inline void setStimulationType( std::string s ){
		stimulationType = s;
	}

        inline void setStimulationValue( double i ){
            stimulationValue = i;
	}

	inline void setHealth( int h ) {
		health = h;
	}

	inline void setCoords( int x, int z ) {
		coords_x = x;
		coords_z = z;
	}

    inline void setId(const int id){
        this->id = id;
    }

    inline int getId() const{
        return id;
    }

	virtual ~BlockObject();

protected:

	/* the type of object this is */
        ObjectFactory::ObjectType type;

        int aggression;

        std::string name;
        std::string alias;
        Filesystem::AbsolutePath path;

        /* the sprite to map to */
        int map;

        int health;

        int coords_x, coords_z;
        int id;

        std::string stimulationType;
        double stimulationValue;
};

#endif
