#include "util/bitmap.h"
#include "menu/menu_option.h"
#include "util/token.h"
#include "util/funcs.h"
#include "globals.h"
#include "menu/menu_animation.h"

using namespace std;

MenuOption::MenuOption(Token *token, const OptionType t) throw (LoadException):
currentState(Deselected),
text(""),
infoText(""),
bmp(0),
adjustLeftColor(Bitmap::makeColor( 255, 255, 255 )),
adjustRightColor(Bitmap::makeColor( 255, 255, 255 )),
runnable(true),
forRemoval(false),
ID(0),
parent(0){
    setType(t);

    if(token){
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
	      } 
	      else if( *token == "anim" ) {
		  MenuAnimation *animation = new MenuAnimation(token);
		  if (animation->getLocation() == 0){
		    backgroundAnimations.push_back(animation);
		} else if (animation->getLocation() == 1){
		    foregroundAnimations.push_back(animation);
		}
	      } 
	      else {
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
}

MenuOption::~MenuOption(){
    // Kill all animations
    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
}

void MenuOption::drawBelow(Bitmap *work){
    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    (*i)->draw(work);
	}
    }
}

void MenuOption::drawAbove(Bitmap *work){
    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    (*i)->draw(work);
	}
    }
}

// This is to pass paramaters to an option ie a bar or something
bool MenuOption::leftKey(){
    return false;
}

bool MenuOption::rightKey(){
    return false;
}

void MenuOption::resetAnimations(){
    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    (*i)->reset();
	}
    }
    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    (*i)->reset();
	}
    }
}

void MenuOption::updateAnimations(){
    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    (*i)->act();
	}
    }
    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    (*i)->act();
	}
    }
}
