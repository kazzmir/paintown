#include "lineedit.h"

#include "gui/keys.h"

LineEdit::LineEdit()
{
	currentSetFont=0;
	textColor = 0;
	changed = autoResizable = false;
	hAlignment = hAlignMod = vAlignment = T_Middle;
	textX = textY = cursorX = cursorY = cursorIndex = limit = 0;
	textSizeH = currentSetFont->getHeight();
	cursorTime.reset();
	blinkRate = 500;
	//setAttribute(AT_Sunken | AT_Editable, true);
	//onPaint.connect(this,&LineEdit::draw);
	//onKeyPress.connect(this,&LineEdit::keyPress);
}
LineEdit::~LineEdit()
{
}

// If the font size changes
void LineEdit::fontChange()
{
	changed=true;
}

// Update
void LineEdit::logic()
{
	if((blinkRate*2)<=cursorTime.msecs())cursorTime.reset();
	if(changed)
	{
		textSizeH = currentSetFont->getHeight();
		if(autoResizable)
		{
			position.height = textSizeH+2;
			position.width = currentSetFont->textLength(currentSetText.c_str())+4;
		}
		else
		{
			if(hAlignMod==T_Left)
			{
				if(currentSetFont->textLength(currentSetText.c_str())>position.width)
				{
					hAlignment=T_Right;
				}
				else hAlignment=T_Left;
			}
		}
		switch(hAlignment)
		{
			case T_Left:
				textX = 2;
				cursorX = textX + currentSetFont->textLength(currentSetText.substr(0,cursorIndex).c_str()) + 1;
				break;
			case T_Middle:
				textX = position.width/2;
				cursorX = (textX - currentSetFont->textLength(currentSetText.c_str())/2) + currentSetFont->textLength(currentSetText.substr(0,cursorIndex).c_str()) + 1;
				break;
			case T_Right:
				textX = (position.width - 1)-2;
				cursorX = textX - currentSetFont->textLength(currentSetText.substr(0,currentSetText.length()-cursorIndex).c_str());
				break;
			case T_Bottom:
			case T_Top:
				break;
		}
		
		switch(vAlignment)
		{
			case T_Top:
				textY = 1;
				cursorY = 1;
				break;
			case T_Middle:
				textY = cursorY = (position.height - textSizeH-(2))/2;
				break;
			case T_Bottom:
				textY = (position.height - 1) - textSizeH - 1;
				cursorY = textY - textSizeH;
				break;
			case T_Right:
			case T_Left:
				break;
		}
			
		textY++;
		textX++;
		changed = false;
	}
}
		
// Draw
void LineEdit::render(Bitmap *work)
{
	//if(attributes & AT_Visible)
	//{
		currentSetFont->printf(int(position.x + textX),int(position.y + textY),textColor,*work,currentSetText,int(hAlignment));
		//if(attributes & AT_Focused)
		//{
			if(cursorTime.msecs()<=blinkRate)
			{
				work->line(position.x + cursorX,position.y + cursorY,position.x + cursorX,position.y + cursorY+textSizeH,textColor);
			}
		//}
	//}
}

// Keypresses
void LineEdit::keyPress(const keys &k)
{
	if(k.isCharacter())
	{
		if(limit!=0)
		{
			if(currentSetText.length()<limit)
			{
				//currentSetText += k.getValue();
				currentSetText.insert(cursorIndex, std::string(1,(char)k.getValue()));
				++cursorIndex;
				changed = true;
			}
		}
		else
		{
			//currentSetText += k.getValue();
			currentSetText.insert(cursorIndex, std::string(1,(char)k.getValue()));
			++cursorIndex;
			changed = true;
		}
	}
	else
	{
		switch(k.getValue())
		{
			case keys::DEL:
				if(cursorIndex<currentSetText.length())
				{
					currentSetText.erase(cursorIndex,1);
				}
				break;
			case keys::BACKSPACE:
				if(cursorIndex>0)
				{
					currentSetText.erase(cursorIndex - 1, 1);
					--cursorIndex;
				}
				break;
			case keys::RIGHT:
				if(cursorIndex<currentSetText.length())++cursorIndex;
				break;
			case keys::LEFT:
				if(cursorIndex>0)--cursorIndex;
				break;
			case keys::INSERT:
				break;
		}
		changed = true;
	}
}

// Set text
void LineEdit::setText(const std::string & text)
{
	currentSetText = text;
	if(limit!=0)
	{
		if(currentSetText.length()>limit)
		{
			while(currentSetText.length()>limit)
			{
				currentSetText.erase(currentSetText.begin()+currentSetText.length()-1);
			}
		}
	}
	cursorIndex = currentSetText.length();
	changed = true;
}


//! Get text
const std::string & LineEdit::getText()
{
	return currentSetText;
}
		
//! Clear text
void LineEdit::clearText()
{
	currentSetText.clear();
	cursorIndex=0;
	changed=true;
}

//! Set text limit
void LineEdit::setLimit(unsigned int l)
{
	limit = l;
	if(limit!=0)
	{
		if(currentSetText.length()>limit)
		{
			while(currentSetText.length()>limit)
			{
				currentSetText.erase(currentSetText.begin()+currentSetText.length()-1);
			}
		}
	}
	cursorIndex = currentSetText.length();
	changed=true;
}
		
// Set Horizontal Alignment
void LineEdit::setHorizontalAlign(const textAlign a)
{
	hAlignment = hAlignMod = a;
	changed = true;
}
		
// Set Vertical Alignment
void LineEdit::setVerticalAlign(const textAlign a)
{
	vAlignment = a;
	changed = true;
}
		
// Set textColor
void LineEdit::setTextColor(const int color)
{
	textColor = color;
}


//! Set textColor
void LineEdit::setCursorColor(const int color)
{
	textColor = color;
}

// Set font
void LineEdit::setFont(FreeTypeFont *f)
{
	currentSetFont = f;
}

// Set autoResizeable
void LineEdit::setAutoResize(bool r)
{
	autoResizable = r;
}

// Set the cursor blink rate in miliseconds (default 500)
void LineEdit::setCursorBlinkRate(unsigned int msecs)
{
	blinkRate = msecs;
}

