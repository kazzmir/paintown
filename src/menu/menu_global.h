#ifndef _paintown_menu_globals_h
#define _paintown_menu_globals_h

#include <string>
#include <queue>
#include "util/load_exception.h"

class Sound;

class MenuGlobals{
public:
	/*! ctor dtor */
	MenuGlobals();
	virtual ~MenuGlobals();
	
	/*! set the music */
	static void setMusic(const std::string &file);
	
	/*! get current music */
	static const std::string currentMusic();
	
	/*! pop music */
	static void popMusic();
	
	/*! set the select sound */
	static void setSelectSound(const std::string &file);
	
	/*! get current select sound */
	static const std::string currentSelectSound();
	
	/*! play current select sound */
	static void playSelectSound() throw (LoadException);
	
	/*! pop select sound */
	static void popSelectSound();
	
	/*! game speed */
	static double getGameSpeed();
	
	/*! set speed */
	static void setGameSpeed(double s);
	
	/*! invincible */
	static bool getInvincible();
	
	/*! set invincible */
	static void setInvincible(bool i);
	
	/*! game fullscreen */
	static bool getFullscreen();
	
	/*! set speed */
	static void setFullscreen(bool f);
	
	/*! game lives */
	static int getLives();
	
	/*! set lives */
	static void setLives(int l);

	static int getNpcBuddies();
	static void setNpcBuddies( int i );

        /* free for all lets players attack each other in network mode.
         * cooperative means they are on the same team.
         */
        static bool freeForAll();
        static bool cooperative();
        static void setFreeForAll();
        static void setCooperative();
	
private:
		
	static std::priority_queue<std::string> lastPlayed;
	
	static std::priority_queue<std::string> selectSound;	
};
#endif
