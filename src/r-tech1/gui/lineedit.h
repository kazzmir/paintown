#ifndef _rtech1_gui_lineedit_h
#define _rtech1_gui_lineedit_h

#include <iostream>
#include <vector>

#include "widget.h"
#include "timer.h"
#include "r-tech1/input/text-input.h"

class Font;

class keys;

namespace Gui{

class LineEdit: public Widget{
public:
    //! Constructor
    LineEdit();

    //! Destructor
    virtual ~LineEdit();

    //! Set textColor
    void setTextColor(const Graphics::Color color);

    //! Set textColor
    void setCursorColor(const Graphics::Color color);

    //! Update
    void act(const Font &);

    //! Draw
    void draw(const Font &, const Graphics::Bitmap &);
    
    //! Render
    using Gui::Widget::render;
    void render(const Graphics::Bitmap &);

    virtual void toggleEnabled();
    
    virtual void setFocused(bool enabled);
    
    virtual void addHook(int key, void (*callback)(void *), void * arg);
    
    virtual bool didChanged(unsigned long long &);
    
    virtual inline const std::string getText() const {
        return this->input.getText();
    }
    
    virtual inline void setText(const std::string & text){
        this->input.setText(text);
    }
    
    virtual inline void clear(){
        this->input.clearInput();
    }
    
    virtual inline void setBlinkRate(int rate){
        this->blinkRate = rate;
    }
    
protected:
    void renderCursor(int x, int y, const Font &, const Graphics::Bitmap &);
    Graphics::Color textColor;
    Graphics::Color cursorColor;
    TextInput input;
    int blinkRate;
    int cursorTime;
    
    bool changed;
};

}

#endif
