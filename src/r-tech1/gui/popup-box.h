#ifndef _rtech1_gui_popup_box_h
#define _rtech1_gui_popup_box_h

#include <string>
#include <vector>

#include "widget.h"
#include "box.h"

#include "r-tech1/graphics/gradient.h"

namespace Gui{
    
class PopupBox : public Widget {
    public:
        PopupBox();
        PopupBox(const PopupBox &);
        virtual ~PopupBox();
        
        //! copy
        PopupBox &operator=(const PopupBox &);
        //! Logic
        virtual void act(const Font &);
        //! Render
        using Widget::render;
        virtual void render(const Graphics::Bitmap &);
        //! Open box
    virtual void open();
    //! Close box
    virtual void close();

        //! Is active?
    virtual inline bool isActive(){
        return (this->fadeState != Closed);
    }

        virtual inline bool isOpen(){
            return this->fadeState == Open ||
                   this->fadeState == FadeIn;
        }

        //!set fadespeed
        virtual inline void setFadeSpeed(int speed){
            this->fadeSpeed = speed;
        }
        //! Get current box coordinates
        virtual inline const Coordinate & getArea(){
            return this->board.location;
        }
        //! Get current box transformations
        virtual inline const Gui::Transformations & getTransforms(){
        return this->board.transforms;
    }
    private:
    
    void doFade();
    
    enum FadeState{
        Closed,
        FadeIn,
        Open,
        FadeOut,
    };
        //! Current fade state
    FadeState fadeState;

        //! Fade speed
    int fadeSpeed;
    
    //! Board
    Box board;
};

}

#endif
