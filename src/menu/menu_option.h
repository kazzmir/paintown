#ifndef _paintown_menu_option_h
#define _paintown_menu_option_h

#include <string>
#include <vector>
#include "gui/box.h"
#include "util/load_exception.h"

class Bitmap;
class Token;
class Menu;

class MenuOption
{
	public:
		// Do logic before run part
		virtual void logic()=0;
		
		// Do drawing or backgrounds in run part
		virtual void draw(Bitmap *work);
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame)=0;
		
		// This is to pass paramaters to an option ie a bar or something
		// return true to pause key input
		virtual bool leftKey();
		virtual bool rightKey();
		
		enum state
		{
			Selected = 0,
			Deselected,
			Run
		};
		
		enum OptionType{
			Event = 0,
			Option,
			AdjustableOption
		};
		
		MenuOption(Token *token, const OptionType t = Option) throw (LoadException);
	
		virtual ~MenuOption();

	protected:
		std::vector<Bitmap *> &getBitmaps();
	private:
		state currentState;
		OptionType mType;
		std::string text;
		std::string infoText;
		Box infoTextLocation;
		Bitmap *bmp;
		int adjustLeftColor;
		int adjustRightColor;
		bool runnable;
		bool forRemoval;
		
		// Image resource for use in individual options
		std::vector<Bitmap *>bitmaps;
		unsigned int bitmapCounter;
		
		// Stuff we don't need to be concerned with
		friend class Menu;
		int ID;
		inline void setID(int id){ ID = id; }
		inline int getID(){ return ID; }
		
		// This is the owner of this option
		Menu *parent;
	public:
		
		inline void setState(const state s) { currentState = s; }
		
		inline state getState() const { return currentState; }
		
		inline void setType(const OptionType t) { mType = t; }
		
		inline OptionType getType() const { return mType; }
		
		inline void setText(const std::string &t) { text = t; }
		
		inline std::string getText() const { return text; }
		
		inline void setInfoText(const std::string &t) { infoText = t; }
		
		inline std::string getInfoText() const { return infoText; }
		
		inline void setInfoTextLocation(int x, int y) { infoTextLocation.position.x = x; infoTextLocation.position.y = y; }
		
		inline Box getInfoTextLocation() const { return infoTextLocation; }
		
		inline void setBitmap(Bitmap *b) { bmp = b; }
		
		inline Bitmap *getBitmap() const { return bmp; }
		
		inline void setLeftAdjustColor(int c) { adjustLeftColor = c; }
		
		inline int getLeftAdjustColor() const { return adjustLeftColor; }
		
		inline void setRightAdjustColor(int c) { adjustRightColor = c; }
		
		inline int getRightAdjustColor() const { return adjustRightColor; }
		
		inline void setRunnable(const bool r) { runnable = r; }
		
		inline bool isRunnable() const { return runnable; }
		
		inline void setForRemoval(const bool r) { forRemoval = r; }
		
		inline bool scheduledForRemoval() const { return forRemoval; }
};

#endif
