#ifndef _paintown_world_h
#define _paintown_world_h

class Object;
class Player;
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

	inline int getQuake() const {
		return quake_time;
	}

	virtual void act() = 0;
	virtual void draw( Bitmap * work ) = 0;
	virtual void addObject( Object * o ) = 0;

	virtual bool finished() const = 0;

	virtual void reloadLevel() throw( LoadException ) = 0;

    virtual Script::Engine * getEngine() const = 0;

	/* upper left hand corner of the screen */
	virtual int getX() = 0;
	virtual int getY() = 0;

    virtual double ticks(const double in) const;

    /* this shouldn't be here */
    virtual const std::deque<Bitmap*> & getScreenshots() = 0;

    virtual int levelLength() const = 0;
    virtual const Block * currentBlock() const = 0;

    virtual inline const std::vector<Object*> & getObjects() const {
        return objects;
    }

    /* set to paused */
    virtual void pause() = 0;
    /* set to unpaused */
    virtual void unpause() = 0;
    /* set to paused if unpaused, or vice-versa */
    virtual void changePause() = 0;
    virtual bool isPaused() = 0;

    /* an object has notified us that it is dying */
    virtual void dyingObject(const Object & obj);
    virtual void dyingObject(const Player & obj);

    /* bleh.. */
    virtual void addEnemy(Enemy * obj) = 0;

    virtual Object * findObject(int id) = 0;

	virtual int getMaximumZ() = 0;
	virtual int getMinimumZ() = 0;

	virtual void drawMiniMaps( bool b ) = 0;
	virtual bool shouldDrawMiniMaps() = 0;

	virtual void killAllHumans( Object * player ) = 0;

	virtual void addMessage(Network::Message m, Network::Socket from = 0, Network::Socket to = 0) = 0;
	virtual Network::Message createBangMessage( int x, int y, int z ) = 0;

protected:
	int quake_time;
	std::vector<Object *> objects;

public:
        /* serialized actions the world can take. mostly for use with networking
         * WARNING: changing these values (like moving them around) will break
         * backwards compatibility.
         * If a message with one of these types is sent then the message
         * id *must* be set to 0, which indicates a server message.
         */
        enum{
            /* sanity check, no one should ever pass `NOTHING' around */
            NOTHING = 0,
            /* create a new character */
            CREATE_CHARACTER,
            /* create a cat */
            CREATE_CAT,
            /* create a bang effect */
            CREATE_BANG,
            /* create an item */
            CREATE_ITEM,
            /* create a projectile */
            CREATE_PROJECTILE,
            /* advance a block in the game */
            NEXT_BLOCK,
            /* the level is finished */
            FINISH,
            /* remove object from world */
            REMOVE,
            /* set id of the player */
            SET_ID,
            /* acknowledge a message */
            OK,
            /* load a new level */
            LOAD_LEVEL,
            /* remove object and delete it */
            DELETE_OBJ,
            /* grab a character */
            GRAB,
            /* throw a character */
            THROWN,
            /* dummy message (different from NOTHING) */
            IGNORE_MESSAGE,
            /* entire game is over, time to go outside */
            GAME_OVER,
            /* create a player. used to differentiate between players and
             * regular characters
             */
            IS_PLAYER,
            /* pause the game */
            PAUSE,
            /* unpause the game */
            UNPAUSE,
            /* request a ping */
            PING_REQUEST,
            /* reply to a ping */
            PING_REPLY,
            /* client has quit */
            QUIT,
            /* chat message */
            CHAT,
            /* client name and stuff */
            CLIENT_INFO,
        };
};

#endif
