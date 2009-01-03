#include "util/bitmap.h"
#include "menu/menu_option.h"
#include "util/token.h"
#include "globals.h"

MenuOption::MenuOption(Token *token, const OptionType t) throw (LoadException):
currentState(Deselected),
text(""),
infoText(""),
bmp(0),
adjustLeftColor(Bitmap::makeColor( 255, 255, 255 )),
adjustRightColor(Bitmap::makeColor( 255, 255, 255 )),
runnable(true),
ID(0){
    setType(t);

    Token tok(*token);

    while ( tok.hasTokens() ){
        try{
            Token * token;
            tok >> token;
            if ( *token == "info" ){
                // get info text and location and add to option
                std::string temp;
                int x=0, y=0;
                *token >> temp;
                setInfoText(temp);
                try{
                    *token >> x >> y;
                } catch (const TokenException & ex){
                    x = 0;
                    y = 0;
                }
                setInfoTextLocation(x,y);
            } else {
                Global::debug( 3 ) << "Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    token->print(" ");
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
}

MenuOption::~MenuOption(){
    // Nothing
}

// This is to pass paramaters to an option ie a bar or something
bool MenuOption::leftKey(){
    return false;
}

bool MenuOption::rightKey(){
    return false;
}
