#ifndef _world_h
#define _world_h

class Object;
class Scene;

#include <vector>
#include <string>

using namespace std;

class World{
public:
	World();
	World( Object * player, const string & path, int screen_size = 320 );

	~World();

	void Quake( int q );

	inline const int getQuake() const {
		return quake_time;
	}

	void act();
	void draw( Bitmap * work );
	void addObject( Object * o );

	const bool finished() const;

	void reloadLevel();

	/* upper left hand corner of the screen */
	int getX();
	int getY();

	int getMaximumZ();
	int getMinimumZ();

protected:
	
	void loadLevel( const string & path );

	void doLogic();

protected:
	Object * const player;
	Object * bang;
	
	vector< Object * > objects;

	Scene * scene;

	int quake_time;
	int min_x;
	int screen_size;
	string path;

};

#endif
