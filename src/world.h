#ifndef _paintown_world_h
#define _paintown_world_h

class Object;
class Scene;
class Bitmap;
class Block;
class Enemy;

#include <vector>
#include <deque>
#include <map>
#include <string>

#include "util/load_exception.h"
#include "network/network.h"

namespace Script{
    class Engine;
}

class World{
public:
	World();
	virtual ~World();

	virtual void Quake( int q );

	inline const int getQuake() const {
		return quake_time;
	}

	virtual void act() = 0;
	virtual void draw( Bitmap * work ) = 0;
	virtual void addObject( Object * o ) = 0;

	virtual const bool finished() const = 0;

	virtual void reloadLevel() throw( LoadException ) = 0;

        virtual Script::Engine * const getEngine() const = 0;

	/* upper left hand corner of the screen */
	virtual int getX() = 0;
	virtual int getY() = 0;

        virtual const deque<Bitmap*> & getScreenshots() = 0;

        virtual const int levelLength() const = 0;
        virtual const Block * currentBlock() const = 0;
        
        virtual inline const vector<Object*> & getObjects() const {
            return objects;
        }
        
        virtual void addEnemy(Enemy * obj) = 0;

	virtual void doScene( int min_x, int max_x ) = 0;

        virtual Object * findObject(int id) = 0;

	virtual int getMaximumZ() = 0;
	virtual int getMinimumZ() = 0;

	virtual void drawMiniMaps( bool b ) = 0;
	virtual bool shouldDrawMiniMaps() = 0;

	virtual void killAllHumans( Object * player ) = 0;

	virtual void addMessage(Network::Message m, Network::Socket from = 0) = 0;
	virtual Network::Message createBangMessage( int x, int y, int z ) = 0;

protected:
	int quake_time;
	std::vector<Object *> objects;

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
		REMOVE, /* remove object from world */
		SET_ID,
		OK,
		LOAD_LEVEL,
		DELETE_OBJ, /* remove object and delete it */
		GRAB,
		THROWN,
		IGNORE_MESSAGE,
		GAME_OVER,
                IS_PLAYER,
	};
};

#endif
