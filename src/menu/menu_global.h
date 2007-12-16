#ifndef _paintown_menu_globals_h
#define _paintown_menu_globals_h

#include <string>
#include <queue>

class MenuGlobals
{
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
		
	private:
		//! Game speed
		static double gamespeed;
		
		static std::priority_queue<std::string> lastPlayed;
		
};
#endif
