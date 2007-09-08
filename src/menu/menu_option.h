#ifndef _paintown_menu_option_h
#define _paintown_menu_option_h

class Bitmap;

class MenuOption
{
	public:
		MenuOption();
	
		virtual ~MenuOption();
		
		// Do logic before run part
		virtual void logic()=0;
	
		// Do drawing before run part
		virtual void draw(Bitmap *work)=0;
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame = *(new bool(false)))=0;
		
		enum state
		{
			Selected = 0,
			Deselected,
			Run
		};
	private:
		state currentState;
	public:
		
		inline void setState(state s) { currentState = s; }
		
		inline state getState() { return currentState; }
};

#endif
