#ifndef _mugen_option_options_h
#define _mugen_option_options_h

#include "menu.h"
#include "util/load_exception.h"

#include <string>
#include <vector>

class Token;
class MugenFont;

namespace Mugen{
    
class Option{
public:
    Option();
    virtual ~Option();
    
    virtual void next()=0;
    virtual void prev()=0;
    virtual void enter();
    virtual void render(MugenFont &, int x, int y, const Bitmap &);
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
    OptionOptions(const std::string &name) throw (LoadException);
    
    virtual ~OptionOptions();
    void executeOption(const Mugen::PlayerType &, bool & endGame);

private:

    std::vector<class Option *> options;
    std::vector<class Option *>::iterator selectedOption;
    
    void doOptions(MugenFont & font, int x, int y, const Bitmap &);
};

}

#endif
