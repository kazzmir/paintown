#include "util/bitmap.h"
#include "util/funcs.h"
#include "menu/option_select_font.h"
#include "util/token.h"
#include "menu/menu.h"
#include "util/file-system.h"
#include "configuration.h"
#include "globals.h"
#include "init.h"
#include <sstream>

using namespace std;

OptionSelectFont::OptionSelectFont(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
typeAdjust(fontName),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "font-select" ){
        throw LoadException("Not a font selector");
    }

    while (token->hasTokens()){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "adjust" ){
                std::string temp;
                *tok >> temp;
                if ( temp == "name" ) typeAdjust = fontName;
                else if ( temp == "width" ) typeAdjust = fontWidth;
                else if ( temp == "height" ) typeAdjust = fontHeight;
                else throw LoadException("Incorrect value \"" + temp + "\" in font-select");
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    // Find and set fonts now
    if (typeAdjust == fontName){
        try{
            vector<string>temp1 = Util::getFiles(Filesystem::find("fonts"), "*.ttf");
            vector<string>temp2 = Util::getFiles(Filesystem::find("fonts"), "*.otf");
            std::back_insert_iterator< std::vector<string> > p ( fonts );
            copy ( temp1.begin(), temp1.end(), p);
            copy ( temp2.begin(), temp2.begin(), p);
        } catch (const Filesystem::NotFound & e){
            throw LoadException("Could not load font: " + e.getReason());
        }
    }
}

OptionSelectFont::~OptionSelectFont(){
    // Nothing
}

void OptionSelectFont::logic(){
    switch (typeAdjust){
        case fontName:	  
            setText("Current Font: " + Menu::getFont());
            break;
        case fontWidth:{
            ostringstream temp;
            temp << "Font Width: " << Menu::getFontWidth();
            setText(temp.str());
            break;
        }
        case fontHeight:{
            ostringstream temp;
            temp << "Font Height: " << Menu::getFontHeight();
            setText(temp.str());
            break;
        }
        default: break;
    }
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

void OptionSelectFont::run(bool &endGame) throw (ReturnException) {
}

bool OptionSelectFont::leftKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(false);
            break;
        case fontWidth:
            Menu::setFontWidth(Menu::getFontWidth() - 1);
            break;
        case fontHeight:
            Menu::setFontHeight(Menu::getFontHeight() - 1);
            break;
        default:
            break;
    }
    // Menu::setFont(current, width, height);
    lblue = lgreen = 0;
    return true;
}

bool OptionSelectFont::rightKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(true);
            break;
        case fontWidth:
            Menu::setFontWidth(Menu::getFontWidth() + 1);
            break;
        case fontHeight:
            Menu::setFontHeight(Menu::getFontHeight() + 1);
            break;
        default:
            break;
    }
    // Menu::setFont(current, width, height);
    rblue = rgreen = 0;
    return true;
}

void OptionSelectFont::nextIndex(bool forward){
    int index = 0;
    for (unsigned int i = 0 ; i < fonts.size() ; ++i){
        if (Menu::getFont() == fonts[i]){
            if (forward){
                index = i + 1;
                if (index >= (int) fonts.size()){
                    index = 0;
                }
            } else {
                index = i - 1;
                if (index < 0){
                    index = (int)fonts.size()-1;
                }
            }
        }    
    }
    Menu::setFontName(fonts[index]);
}
