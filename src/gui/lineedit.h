#ifndef paintown_gui_lineedit_h
#define paintown_gui_lineedit_h

#include <iostream>
#include <vector>

#include "gui/widget.h"
#include "gui/timer.h"

#include "util/font.h"


class keys;

class LineEdit : public Widget
{
	public:
		//! enumerator housing alignment positions
		enum textAlign
		{
			T_Left = 0,
			T_Middle,
			T_Right,
			T_Top,
			T_Bottom
		};
		//! Constructor
		LineEdit();
		
		//! Destructor
		~LineEdit();
		
		//! Set text
		void setText(const std::string & text);
		
		//! Get text
		const std::string & getText();
		
		//! Clear text
		void clearText();
		
		//! Set text limit (default 0, no limit)
		void setLimit(unsigned int l);
		
		//! Set Horizontal Alignment
		void setHorizontalAlign(const textAlign a);
		
		//! Set Vertical Alignment
		void setVerticalAlign(const textAlign a);
		
		//! Set textColor
		void setTextColor(const int color);
		
		//! Set textColor
		void setCursorColor(const int color);
		
		//! Set font
		void setFont(FreeTypeFont *f);
		
		//! Set autoResizeable
		void setAutoResize(bool r);
		
		//! Set the cursor blink rate in miliseconds (default 500)
		void setCursorBlinkRate(unsigned int msecs);
		
	protected:
		//! Current font the label is currently using (uses systemFont by default)
		FreeTypeFont *currentSetFont;
		
		//! Current set text
		std::string currentSetText;
		
		//! Horizontal Alignment (RIGHT, MIDDLE, LEFT)
		textAlign hAlignment;
		
		//! Additional checker for Horizontal alignment for easy adjustment
		textAlign hAlignMod;
		
		//! Vertical Alignment (TOP, MIDDLE, BOTTOM)
		textAlign vAlignment;
		
		//! Has changed?
		bool changed;
		
		//! Auto resize
		bool autoResizable;
		
		//! Text horizontal position
		double textX;
		
		//! Text vertical position
		double textY;
		
		//! Cursor horizontal position
		double cursorX;
		
		//! Cursor vertical position
		double cursorY;
		
		//! Cursor index relative to the string
		unsigned int cursorIndex;
		
		//! Text color
		int textColor;
		
		//! Text size Height check in case it changes
		int textSizeH;
		
		//! Text limit
		unsigned int limit;
		
		//! Cursor timer
		guiTimer cursorTime;
		
		//! Cursor blink rate
		unsigned int blinkRate;
		
		//! If the font size changes
		void fontChange();
		
		//! Update
		void logic();
		
		//! Draw
		void render(Bitmap *work);
		
		//! Keypresses
		void keyPress(const keys &k);
		
};

#endif
