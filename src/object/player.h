#ifndef _paintown_player_h
#define _paintown_player_h

#include "character.h" 
#include "player-common.h"
#include <deque>
#include "util/load_exception.h"
#include "network/network.h"
#include "game/input.h"
#include "game/adventure_world.h"

class Animation;
class World;

class Player: public PlayerCommon {
public:

	Player( const char * filename, int config = 0 ) throw( LoadException );
	Player( const std::string & str, int config = 0 ) throw( LoadException );
	Player( const Player & pl ) throw( LoadException );
	Player( const Character & chr ) throw( LoadException );
	
	/* drawing */
	virtual void drawFront( Bitmap * work, int rel_x );

	using Character::drawLifeBar;
	virtual void drawLifeBar( int x, int y, Bitmap * work );
	
	virtual Object * copy();
	
	virtual void act( std::vector< Object * > * others, World * world, std::vector< Object * > * add );
	
	virtual void takeDamage(World & world, ObjectAttack * obj, int x);
	
	virtual void hurt( int x );
	
        virtual void attacked( World * world, Object * something, std::vector< Object * > & objects );

	virtual inline void setExplode( bool b ){
		/* */
	}

	virtual bool getExplode(){
		return false;
	}

	virtual void loseLife( int l = 1 );
	virtual void gainLife( int l = 1 );

        virtual void setInvincible(const bool b);
        virtual const int getInvincibility() const;

	inline const bool isInvincible() const {
		return this->invincible;
	}
	
        virtual void interpretMessage( Network::Message & m );
	
	/* reset some stuff when the player dies */
	virtual void deathReset();
	
        virtual Network::Message getCreateMessage();

        virtual inline void setConfig(int config){
            this->config = config;
        }

        virtual const int getConfig() const {
            return this->config;
        }

        virtual inline void ignoreLives(){
            ignore_lives = true;
        }

        virtual inline const bool ignoringLives() const {
            return ignore_lives;
        }

        virtual ~Player();
	
protected:
        Network::Message scoreMessage();

        void initializeAttackGradient();
        std::vector<PaintownInput> fillKeyCache();
        void debugDumpKeyCache(int level);
        const char * keyToName(PaintownInput key);
	bool combo( Animation * ani );
	bool combo( Animation * ani, std::deque<PaintownInput>::iterator cache_cur_key, std::deque<PaintownInput>::iterator end );
	virtual int getKey( PaintownInput x, int facing );
	virtual int getKey( PaintownInput x );

	bool canGrab( Object * enemy );
	void grabEnemy( Object * enemy );
			
	Network::Message thrownMessage( unsigned int id );
	
protected:

	/* store key presses in a stack with two ends*/
        std::deque<PaintownInput> key_cache;
        std::map< PaintownInput, bool > last_key;
	int acts;
	int show_life;

	int name_id;
        double attack_bonus;
        static const int num_attack_gradient = 50;
        int attack_gradient[num_attack_gradient];

	bool invincible;
	int config;
        bool ignore_lives;

	// int last_key;
};

#endif
