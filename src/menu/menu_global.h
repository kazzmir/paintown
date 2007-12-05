#ifndef _paintown_menu_globals_h
#define _paintown_menu_globals_h

#include <string>		

class MenuGlobals
{
	public:
		/*! ctor dtor */
		MenuGlobals();
		virtual ~MenuGlobals();
		
		/*! set the music */
		static void setMusic(const std::string &file);
		
		/*! game speed */
		static double getGameSpeed();
		
		/*! set speed */
		static void setGameSpeed(double s);
		
	private:
		//! Current music
		std::string music;
		
		//! Game speed
		static double gamespeed;
		
};
#endif
