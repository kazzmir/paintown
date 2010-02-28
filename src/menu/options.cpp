#include "util/bitmap.h"
#include "options.h"
#include "util/token.h"
#include "menu.h"
#include "configuration.h"
#include <sstream>

using namespace std;

OptionSound::OptionSound(Token *token):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if (*token != "sound" ){
        throw LoadException("Not a sound option");
    }

    readName(token);
}

OptionSound::~OptionSound(){
}

void OptionSound::logic(){
    ostringstream temp;
    temp << "Sound: " << Configuration::getSoundVolume();
    setText(temp.str());

    if (lblue < 255){
        lblue += 5;
    }

    if (rblue < 255){
        rblue += 5;
    }

    if (lgreen < 255){
        lgreen += 5;
    }

    if (rgreen < 255){
        rgreen += 5;
    }

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
}

void OptionSound::run(bool &endGame){
}

void OptionSound::changeSound(int much){
    int volume = Configuration::getSoundVolume();
    volume += much;
    if (volume < 0){
        volume = 0;
    }

    if (volume > 100){
        volume = 100;
    }

    Configuration::setSoundVolume(volume);
}

bool OptionSound::leftKey(){
    changeSound(-1);
    
    lblue = lgreen = 0;
    return true;
}

bool OptionSound::rightKey(){
    changeSound(+1);

    rblue = rgreen = 0;
    return true;
}
