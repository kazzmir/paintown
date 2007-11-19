#ifndef _world_h
#define _world_h

class Object;
class Scene;
class Bitmap;

#include <vector>
#include <string>

#include "util/load_exception.h"

using namespace std;

struct PlayerTracker{
	double min_x;
	Object * player;
};

class World{
public:
	World();
	World( const vector< Object * > & players, const string & path, int screen_size = 320 ) throw ( LoadException );

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

	virtual int getMaximumZ();
	virtual int getMinimumZ();
				
	void killAllHumans( Object * player );

protected:

	void loadLevel( const string & path ) throw( LoadException );

	virtual void deleteObjects( vector< Object * > * objects );

	virtual void doLogic();

	virtual const bool isPlayer( Object * o ) const;

protected:
	vector< PlayerTracker > players;
	Object * bang;
	
	vector< Object * > objects;

	Scene * scene;

	int quake_time;
	// int min_x;
	int screen_size;
	string path;

};

#endif
