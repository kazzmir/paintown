#include "util/bitmap.h"
#include "menu/menu_option.h"
#include "util/token.h"
#include "util/funcs.h"
#include "util/language-string.h"
#include "globals.h"
#include "gui/animation.h"

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
                  readInfo(token);
                  /*
		  // get info text add to option
		  std::string temp;
		  *token >> temp;
		  setInfoText(temp);
                  */
	      } else if( *token == "option-anim" ) {
		  Gui::Animation *animation = new Gui::Animation(token);
		  if (animation->getDepth() == Gui::Animation::Background0){
		    backgroundAnimations.push_back(animation);
		} else if (animation->getDepth() == Gui::Animation::Foreground0){
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
	      throw LoadException(__FILE__, __LINE__, ex, "Menu option parse error");
	  } catch ( const LoadException & ex ) {
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
 *
 *  Expects the parent token to be passed in:
 *  (whatever (name ...))
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
            throw LoadException(__FILE__, __LINE__, "No name set, this option should have a name!");
        }
    } catch (const TokenException & ex){
        throw LoadException(__FILE__, __LINE__, ex, "Menu option parse error");
    }
}

/* same deal as readName, except this excepts just the info token:
 * (info ...)
 */
void MenuOption::readInfo(Token * token){
    try{
        LanguageString name;
        string temp;
        if (token->match("info", temp)){
            name.add(temp);
        }

        string language, words;
        Token::Matcher matcher = token->getMatcher("info/language");
        while (matcher.match(language, words)){
            name.add(words, language);
        }

        this->setInfoText(name);
    } catch (const TokenException & ex){
        throw LoadException(__FILE__, __LINE__, ex, "Menu option parse error");
    }
}

MenuOption::~MenuOption(){
    // Kill all animations
    for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
    for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
}

void MenuOption::drawBelow(Bitmap *work){
    for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    (*i)->draw(*work);
	}
    }
}

void MenuOption::drawAbove(Bitmap *work){
    for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    (*i)->draw(*work);
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
    for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    (*i)->reset();
	}
    }
    for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    (*i)->reset();
	}
    }
}

void MenuOption::updateAnimations(){
    for (std::vector<Gui::Animation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	if (*i){
	    (*i)->act();
	}
    }
    for (std::vector<Gui::Animation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	if (*i){
	    (*i)->act();
	}
    }
}

void MenuOption::setParent(Menu *menu){
    this->parent = menu;
}
