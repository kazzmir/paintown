#ifndef _paintown_mugen_options_h
#define _paintown_mugen_options_h

#include "mugen/menu.h"
#include "util/load_exception.h"

#include <vector>
#include <string>

class Token;
class MugenFont;

namespace Mugen {

class Character;

class OptionArcade: public ItemOption {
public:
    OptionArcade(const std::string &name);
    virtual ~OptionArcade();
    
    void executeOption(const Mugen::PlayerType &, bool & endGame);

private:
	    
};

/*! Handles key reconfiguration */
class OptionVersus: public Mugen::ItemOption {
public:
    OptionVersus(const std::string &name) throw (LoadException);
    virtual ~OptionVersus();
    void executeOption(const Mugen::PlayerType &, bool & endGame);
    
private:
	    
};
   
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
