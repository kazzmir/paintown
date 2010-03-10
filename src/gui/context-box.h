#ifndef _paintown_gui_context_box_h
#define _paintown_gui_context_box_h

#include <string>
#include <vector>

#include "gui/widget.h"
#include "gui/box.h"

#include "util/gradient.h"

namespace Gui{
    
class ContextItem{
    public:
	ContextItem();
	virtual ~ContextItem();
	
	virtual const std::string getName()=0;
	virtual const bool isAdjustable();
        virtual const int getLeftColor();
        virtual const int getRightColor();
};

class ContextBox : public Widget{
    public:
        ContextBox();
        ContextBox(const ContextBox &);
        virtual ~ContextBox();
        
        //! copy
        ContextBox &operator=(const ContextBox &);
        //! Logic
        virtual void act();
        //! Render
        virtual void render(const Bitmap &);
        //! Next
        virtual bool next();
        //! Previous
        virtual bool previous();
	//! Adjust left
	virtual void adjustLeft();
	//! Adjust right
	virtual void adjustRight();
	//! open context box
	virtual void open();
	//! Close context box
	virtual void close();
        //! Set context list
        virtual inline void setList(const std::vector<ContextItem *> & list){
            this->context = list;
            this->current = 0;
        }
        //! Set current font
	virtual inline void setFont(const std::string & font, int width, int height){
	    this->font = font;
	    this->fontWidth = width;
	    this->fontHeight = height;
	}
        //! Get current index
        virtual inline unsigned int getCurrentIndex(){
            return this->current;
        }
        //! Is active?
	virtual inline bool isActive(){
	    return (this->fadeState != NotActive);
	}
    //!set fadespeed
    virtual inline void setFadeSpeed(int speed){
        this->fadeSpeed = speed;
    }
    //!set fade alpha
    virtual inline void setFadeAlpha(int alpha){
        this->fadeAlpha = alpha;
    }
    private:
	
	void doFade();
	
	void calculateText();
	
	void drawText(const Bitmap &);
	
	enum FadeState{
	    NotActive,
	    FadeInBox,
	    FadeInText,
	    Active,
	    FadeOutText,
	    FadeOutBox,
	};
        //! Current index
        unsigned int current;
	
	//! Current fade state
	FadeState fadeState;

        //! Context list
        std::vector<ContextItem *> context;
	
	//! Current font
	std::string font;
	int fontWidth;
	int fontHeight;
	
	//! Fade speed
	int fadeSpeed;
	
	//! Fade Aplha
	int fadeAlpha;
	
	//! Board
	Box board;
	
    //! The centered position 
    int cursorCenter;
    //! Current y coordinate to render text from
    int cursorLocation;
    //! scroll wait
    int scrollWait;
    
    //! Gradient for selected cursor
    Effects::Gradient selectedGradient;
};

}

#endif
