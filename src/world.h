#ifndef _paintown_world_h
#define _paintown_world_h

class Object;
class Scene;
class Bitmap;

#include <vector>
#include <string>

#include "util/load_exception.h"
#include "network.h"

struct PlayerTracker{
	double min_x;
	Object * player;
};

class World{
public:
	World();
	World( const std::vector< Object * > & players, const std::string & path, int screen_size = 320 ) throw ( LoadException );

	virtual ~World();

	void Quake( int q );

	inline const int getQuake() const {
		return quake_time;
	}

	virtual void act();
	virtual void draw( Bitmap * work );
	virtual void addObject( Object * o );

	virtual const bool finished() const;

	virtual void reloadLevel() throw( LoadException );

	/* upper left hand corner of the screen */
	virtual int getX();
	virtual int getY();

	virtual void doScene( int min_x, int max_x );

	virtual int getMaximumZ();
	virtual int getMinimumZ();
				
	void killAllHumans( Object * player );

	virtual void addMessage( Network::Message m );
	Network::Message createBangMessage( int x, int y, int z );

protected:

	void loadLevel( const std::string & path ) throw( LoadException );

	virtual void deleteObjects( std::vector< Object * > * objects );

	virtual void doLogic();

	virtual const bool isPlayer( Object * o ) const;

protected:
	std::vector< PlayerTracker > players;
	Object * bang;
	
	std::vector< Object * > objects;

	Scene * scene;

	int quake_time;
	// int min_x;
	int screen_size;
	std::string path;

public:
	enum{
		NOTHING,
		CREATE_CHARACTER,
		CREATE_CAT,
		CREATE_BANG,
		CREATE_ITEM,
		CREATE_PROJECTILE,
		NEXT_BLOCK,
		FINISH,
	};

};

#endif
