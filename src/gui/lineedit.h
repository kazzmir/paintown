#ifndef paintown_gui_lineedit_h
#define paintown_gui_lineedit_h

#include <iostream>
#include <vector>

#include "widget.h"
#include "timer.h"
#include "sigslot.h"
#include "util/input/text-input.h"

class Font;

class keys;

namespace Gui{

class LineEdit: public Widget, public sigslot::has_slots<> {
public:
    //! enumerator housing alignment positions
    enum textAlign {
        T_Left = 0,
        T_Middle,
        T_Right,
        T_Top,
        T_Bottom
    };

    //! enumerator housing alignment positions
    enum inputType {
        inputGeneral = 0,
        inputAllCaps,
        inputNoCaps,
        inputNumerical
    };

    //! Constructor
    LineEdit();

    //! Destructor
    virtual ~LineEdit();

    //! Set text
    void setText(const std::string & text);

    //! Get text
    const std::string getText();

    //! Clear text
    void clearText();

    //! Set text limit (default 0, no limit)
    void setLimit(unsigned int l);

    void hookKey(int key, void (*callback)(void *), void * arg);

    //! Set Horizontal Alignment
    void setHorizontalAlign(const textAlign & a);

    //! Set Vertical Alignment
    void setVerticalAlign(const textAlign & a);

    //! Set the type of input default general
    void setInputType(const inputType i = inputGeneral);

    //! Set textColor
    void setTextColor(const int color);

    //! Set textColor
    void setCursorColor(const int color);

    //! Set font
    void setFont(const Font *f);

    //! Set autoResizeable
    void setAutoResize(bool r);

    //! Set the cursor blink rate in miliseconds (default 500)
    void setCursorBlinkRate(unsigned int msecs);

    //! Update
    void act(const Font &);

    //! Draw
    using Widget::render;
    void render(const Bitmap &);

    //! set Focus
    void setFocused(bool focus);

    //! check Focus
    bool isFocused();

    //! Keypresses
    // sigslot::slot keyPress(const keys &k);

    bool didChanged( unsigned long long & counter );

protected:
    //! Current font the label is currently using (uses systemFont by default)
    const Font *currentSetFont;

    //! Current set text
    // std::string currentSetText;

    //! Horizontal Alignment (RIGHT, MIDDLE, LEFT)
    textAlign hAlignment;

    //! Additional checker for Horizontal alignment for easy adjustment
    textAlign hAlignMod;

    //! Vertical Alignment (TOP, MIDDLE, BOTTOM)
    textAlign vAlignment;

    //! Input type
    inputType inputTypeValue;

    //! Has changed?
    bool changed_;

    //! Auto resize
    bool autoResizable;

    //! Text horizontal position
    int textX;

    //! Text vertical position
    int textY;

    //! Cursor horizontal position
    int cursorX;

    //! Cursor vertical position
    int cursorY;

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
    bool blink;

    //! Focused?
    bool focused;

    TextInput input;

    // keeps track of changes
    unsigned long long changeCounter;

    //! If the font size changes
    void fontChange();

    inline void changed(){
        changed_ = true;
        changeCounter += 1;
    }

    inline void stable(){
        changed_ = false;
    }
};

}

#endif
