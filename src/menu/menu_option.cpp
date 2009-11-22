#include "util/bitmap.h"
#include "menu/menu_option.h"
#include "util/token.h"
#include "util/funcs.h"
#include "util/language-string.h"
#include "globals.h"
#include "menu/menu_animation.h"

using namespace std;

MenuOption::MenuOption(Token *token, const OptionType type) throw (LoadException):
parent(0),
currentState(Deselected),
text(""),
infoText(""),
bmp(0),
adjustLeftColor(Bitmap::makeColor( 255, 255, 255 )),
adjustRightColor(Bitmap::makeColor( 255, 255, 255 )),
runnable(true),
forRemoval(false){
    setType(type);

    if(token){
      Token tok(*token);

      while ( tok.hasTokens() ){
	  try{
	      Token * token;
	      tok >> token;
	      if ( *token == "info" ){
		  // get info text add to option
		  std::string temp;
		  *token >> temp;
		  setInfoText(temp);
	      } 
	      else if( *token == "option-anim" ) {
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

/* tries to read the name for the option. Either use
 *  - legacy way: (name "foobar") which sets the name to "foobar
 *  in english
 *  - new way: (name (language "english" "foobar") ...) which adds
 *  "foobar" as a name in "english". further uses of `language'
 *  add new languages for the name.
 *
 *  Using both style is actually allowed but a (language "english" "foo")
 *  will override the legacy definition.
 */
void MenuOption::readName(Token * token){
    try{
        LanguageString name;
        string temp;
        if (token->match("_/name", temp)){
            name.add(temp);
        }

        string language, words;
        Token::Matcher matcher = token->getMatcher("_/name/language");
        while (matcher.match(language, words)){
            name.add(words, language);
        }

        this->setText(name);

        if (getText().empty()){
            throw LoadException("No name set, this option should have a name!");
        }
    } catch (const TokenException & ex){
        string m( "Menu parse error: " );
        m += ex.getReason();
        throw LoadException(m);
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

void MenuOption::setParent(Menu *menu){
    this->parent = menu;
}

