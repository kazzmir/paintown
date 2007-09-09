#ifndef _paintown_menu_option_h
#define _paintown_menu_option_h

#include <string>

class Bitmap;

class MenuOption
{
	public:
		// Do logic before run part
		virtual void logic()=0;
	
		// Do drawing before run part
		virtual void draw(Bitmap *work)=0;
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame)=0;
		
		enum state
		{
			Selected = 0,
			Deselected,
			Run
		};
		
		enum type
		{
			event = 0,
			option
		};
		
		MenuOption(const type t = option);
	
		virtual ~MenuOption();
		
	private:
		state currentState;
		type mType;
		std::string text;
		Bitmap *bmp;
	public:
		
		inline void setState(const state s) { currentState = s; }
		
		inline state getState() const { return currentState; }
		
		inline void setType(const type t) { mType = t; }
		
		inline type getType() const { return mType; }
		
		inline void setText(const std::string &t) { text = t; }
		
		inline std::string getText() const { return text; }
		
		inline void setBitmap(Bitmap *b) { bmp = b; }
		
		inline Bitmap *getBitmap() const { return bmp; }
};

#endif
