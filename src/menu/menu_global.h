#ifndef _paintown_menu_globals_h
#define _paintown_menu_globals_h

#include <string>
#include <queue>

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
	
private:
		
	static std::priority_queue<std::string> lastPlayed;
	
};
#endif
