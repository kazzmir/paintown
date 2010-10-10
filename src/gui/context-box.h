#ifndef _paintown_gui_context_box_h
#define _paintown_gui_context_box_h

#include <string>
#include <vector>

#include "gui/widget.h"
#include "gui/popup-box.h"

#include "util/gradient.h"
#include "util/file-system.h"

namespace Gui{
    
class ContextItem{
    public:
	ContextItem();
	virtual ~ContextItem();
	
	virtual const std::string getName()=0;
	virtual bool isAdjustable();
        virtual int getLeftColor();
        virtual int getRightColor();
};

class ContextBox : public Widget {
    public:
        ContextBox();
        ContextBox(const ContextBox &);
        virtual ~ContextBox();
        
        //! copy
        ContextBox &operator=(const ContextBox &);
        //! Logic
        virtual void act();
        //! Render
        using Widget::render;
        virtual void render(const Bitmap &);
        virtual void render(const Bitmap &, const Font & font);
        //! Next
        virtual bool next(const Font &);
        //! Previous
        virtual bool previous(const Font &);
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
        /*
	virtual inline void setFont(const Filesystem::RelativePath & font, int width, int height){
	    this->font = font;
	    this->fontWidth = width;
	    this->fontHeight = height;
	}
        */
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
            this->board.setFadeSpeed(speed);
        }
        //!set fade alpha
        virtual inline void setFadeAlpha(int alpha){
            this->fadeAlpha = alpha;
        }
        
        //! use gradient?
        virtual inline void setUseGradient(bool useGradient){
            this->useGradient = useGradient;
        }
        
        //! Set to use only text on background
        virtual inline void setRenderOnlyText(bool render){
	    this->renderOnlyText = render;
	}

    private:
	
	void doFade();
	
	void calculateText(const Font & font);
	
	void drawText(const Bitmap &, const Font & font);
	
	enum FadeState{
	    NotActive,
	    FadeIn,
	    Active,
	    FadeOut,
	};
        //! Current index
        unsigned int current;
	
	//! Current fade state
	FadeState fadeState;

        //! Context list
        std::vector<ContextItem *> context;
	
	//! Current font
        /*
        Filesystem::RelativePath font;
	int fontWidth;
	int fontHeight;
        */
	
	//! Fade speed
	int fadeSpeed;
	
	//! Fade Aplha
	int fadeAlpha;
	
	//! Board
	PopupBox board;
	
        //! The centered position 
        int cursorCenter;
        //! Current y coordinate to render text from
        int cursorLocation;
        //! scroll wait
        int scrollWait;
        
        //! Gradient for selected cursor
        Effects::Gradient selectedGradient;

        //! Use gradient
        bool useGradient;
	
	//! Render Text only
	bool renderOnlyText;
};

}

#endif
