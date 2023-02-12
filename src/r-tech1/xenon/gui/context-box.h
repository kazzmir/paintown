#ifndef _rtech1_gui_context_box_h
#define _rtech1_gui_context_box_h

#include <string>
#include <vector>

#include "widget.h"
#include "popup-box.h"
#include "scroll-list.h"

#include "r-tech1/language-string.h"
#include "r-tech1/graphics/gradient.h"
#include "r-tech1/file-system.h"

#include "r-tech1/pointer.h"

class Token;

namespace Gui{

Effects::Gradient standardGradient(int max);
Effects::Gradient modifiedGradient(Graphics::Color low, Graphics::Color high, int max);

class ListValues{
public:
    ListValues();
    ListValues(const ListValues &);
    virtual ~ListValues();
    const ListValues & operator=(const ListValues &);
    
    void getValues(const Token *);
    
    virtual inline bool getInterpolate() const {
        return this->interpolate;
    }
    virtual inline void setInterpolate(bool interpolate){
        this->interpolate = interpolate;
    }
    virtual inline Graphics::Color getLowColor() const {
        return this->lowColor;
    }
    virtual inline void setLowColor(Graphics::Color color){
        this->lowColor = color;
    }
    virtual inline Graphics::Color getHighColor() const {
        return this->highColor;
    }
    virtual inline void setHighColor(Graphics::Color color){
        this->highColor = color;
    }
    virtual inline int getMaxGradient() const {
        return this->maxGradient;
    }
    virtual inline void setMaxGradient(int max){
        this->maxGradient = max;
    }
    virtual inline Graphics::Color getSelectedColor() const {
        return this->selectedColor;
    }
    virtual inline void setSelectedColor(Graphics::Color color){
        this->selectedColor = color;
    }
    virtual inline int getSelectedAlpha() const {
        return this->selectedAlpha;
    }
    virtual inline void setSelectedAlpha(int alpha){
        this->selectedAlpha = alpha;
    }
    virtual inline Graphics::Color getOtherColor() const {
        return this->otherColor;
    }
    virtual inline void setOtherColor(Graphics::Color color){
        this->otherColor = color;
    }
    virtual inline int getOtherAlpha() const {
        return this->otherAlpha;
    }
    virtual inline void setOtherAlpha(int alpha){
        this->otherAlpha = alpha;
    }
    virtual inline double getDistanceFadeMultiplier() const{
        return this->distanceFadeMultiplier;
    }
    virtual inline void setDistanceFadeMultiplier(double multiplier){
        this->distanceFadeMultiplier = multiplier;
    }

    virtual inline bool getDistanceFade() const {
        return this->fade;
    }

    /* Whether or not non-selected items are faded proportional to their distance from the
     * selected item.
     */
    virtual inline void setDistanceFade(bool fade){
        this->fade = fade;
    }
    
protected:
    bool interpolate;
    Graphics::Color lowColor, highColor;
    int maxGradient;
    Graphics::Color selectedColor;
    int selectedAlpha;
    Graphics::Color otherColor;
    int otherAlpha;
    double distanceFadeMultiplier;
    bool fade;
};

class ContextBox;
class ScrollListInterface;

class ContextItem: public ScrollItem {
public:
    ContextItem(const std::string &, const ContextBox &);
    virtual ~ContextItem();
    
    virtual void draw(int x, int y, const Graphics::Bitmap & where, const Font & font, int distance) const;
    virtual int size(const Font & font) const;

    virtual void setText(const LanguageString & t);
    inline const LanguageString & getLanguageText() const {
        return text;
    }
    virtual inline std::string getText() const { return text.get(); }

protected:
    LanguageString text;
    const ContextBox & parent;
};

class ContextBox: public Widget {
public:
    ContextBox();
    ContextBox(const ContextBox &);
    virtual ~ContextBox();

    //! copy
    ContextBox &operator=(const ContextBox &);
    //! Logic
    virtual void act(const Font &);
    //! Render
    using Widget::render;
    virtual void render(const Graphics::Bitmap &);
    virtual void render(const Graphics::Bitmap &, const Font & font);
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
    virtual void setList(const std::vector<Util::ReferenceCount<ContextItem> > & list);
    virtual void addItem(const Util::ReferenceCount<ContextItem> & item);

    /*! Scroll or Normal */
    enum ListType{
        Normal,
        Scroll,
    };

    virtual void setListType(const ListType &);
    virtual void setListWrap(bool wrap);

    virtual Graphics::Color getSelectedColor() const;

    //! Get current index
    virtual inline unsigned int getCurrentIndex(){
        return this->list->getCurrentIndex();
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

    virtual inline int getFadeAlpha() const {
        return this->fadeAlpha;
    }

    //! Set to use only text on background
    virtual inline void setRenderOnlyText(bool render){
        this->renderOnlyText = render;
    }

    virtual inline PopupBox & getBoard(){
        return board;
    }

    virtual inline const Gui::ListValues & getListValues() const {
        return this->values;
    }
    
    virtual inline Gui::ListValues & getListValues(){
        return this->values;
    }

    virtual void setListValues(const Gui::ListValues & values);

private:

    void doFade();

    void drawText(const Graphics::Bitmap &, const Font & font);

    enum FadeState{
        NotActive,
        FadeIn,
        Active,
        FadeOut,
    };

    //! Current fade state
    FadeState fadeState;

    //! Context list
    Util::ReferenceCount<ScrollListInterface> list;

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

    //! Render Text only
    bool renderOnlyText;

    /*! List Values */
    Gui::ListValues values;
};

}

#endif
