#ifndef _paintown_menu_option_h
#define _paintown_menu_option_h

#include <string>
#include <vector>
#include "util/gui/box.h"
#include "util/load_exception.h"
#include "util/language-string.h"
#include "util/gui/context-box.h"
#include "util/gui/scroll-list.h"

class Bitmap;
class Token;
namespace Menu {
    class Context;
}

class MenuOption {
public:
    /* called when the menu containing this option is displayed to the user.
     * default behavior is to do nothing.
     */
    virtual void open();

    /* called when the menu containing this option returns.
     * default behavior is to do nothing.
     */
    virtual void close();

    // Do logic before run part
    virtual void logic() = 0;

    // Finally it has been selected, this is what shall run 
    // endGame will be set true if it is a terminating option
    virtual void run(const Menu::Context &) = 0;

    // This is to pass paramaters to an option ie a bar or something
    // return true to pause key input
    virtual bool leftKey();
    virtual bool rightKey();
    
    enum state{
        Selected = 0,
        Deselected,
        Run
    };

    enum OptionType{
        Event = 0,
        Option,
        AdjustableOption
    };

    MenuOption(const Gui::ContextBox & parent, const Token *token);
    
    template <typename T>
    Util::ReferenceCount<Gui::ScrollItem> getAsScrollItem(const Gui::ContextBox & parent){
        Util::ReferenceCount<T> item = new T(this->getName(), parent);
        return item.template convert<Gui::ScrollItem>();
    }

    virtual ~MenuOption();

protected:
    
private:
    state currentState;
    OptionType mType;
    LanguageString text;
    LanguageString infoText;
    bool runnable;

public:

    inline void setState(const state s) { currentState = s; }
    inline state getState() const { return currentState; }
    
    inline void setType(const OptionType t) { mType = t; }
    inline OptionType getType() const { return mType; }
    inline void setText(const LanguageString & t) { text = t; }

    inline const LanguageString & getLanguageText() const {
        return text;
    }

    virtual inline std::string getText() const { return text.get(); }
    inline void setInfoText(const LanguageString &t) { infoText = t; }
    inline std::string getInfoText(){ return infoText.get(); }
    inline void setRunnable(const bool r) { runnable = r; }
    inline bool isRunnable() const { return runnable; }

public:
    inline const std::string getName() const { return this->getText(); }

protected:
    void readName(const Token * token);
    void readInfo(const Token * token);
};

#endif
