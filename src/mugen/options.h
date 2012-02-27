#ifndef _paintown_mugen_options_h
#define _paintown_mugen_options_h

#include "menu.h"
#include "util/menu/optionfactory.h"

#include <vector>
#include <string>

class Token;

namespace Mugen {

class Font;

class Character;

class OptionArcade: public ItemOption {
public:
    OptionArcade(const std::string &name);
    virtual ~OptionArcade();
    
    void executeOption(const Mugen::PlayerType &, bool & endGame);
};

/*! Handles key reconfiguration */
class OptionVersus: public Mugen::ItemOption {
public:
    OptionVersus(const std::string &name);
    virtual ~OptionVersus();
    void executeOption(const Mugen::PlayerType &, bool & endGame);
};

class Option{
public:
    Option();
    virtual ~Option();
    
    virtual void next()=0;
    virtual void prev()=0;
    virtual void enter();
    virtual void render(Font &, int x1, int x2, int y, const Graphics::Bitmap &);
    virtual int getWidth(Font &);
    virtual inline const Mugen::Point & getPosition() const {
        return this->position;
    }
    virtual inline void toggleSelected(){
        this->selected = !this->selected;
    }
protected:
    Mugen::Point position;
    std::string optionName;
    std::string currentValue;
    
private:
    bool selected;
    // box
    int alpha,alphaMod;
};

/*! Handles Mugen Options */
class OptionOptions: public ItemOption {
public:
    OptionOptions(const std::string &name);
    
    virtual ~OptionOptions();
    void executeOption(const Mugen::PlayerType &, bool & endGame);

private:

    std::vector<class Option *> options;
    std::vector<class Option *>::const_iterator selectedOption;
    
    // void doOptions(Font & font, int x, int y, const Graphics::Bitmap &);
};

/* For the top level paintown menu */
class OptionFactory: public Menu::OptionFactory {
public:
    OptionFactory();
    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const;
    virtual ~OptionFactory();
};

}

#endif
