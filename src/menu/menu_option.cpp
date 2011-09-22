#include "util/bitmap.h"
#include "util/token.h"
#include "util/funcs.h"
#include "util/language-string.h"
#include "util/debug.h"
#include "util/gui/animation.h"
#include "menu_option.h"

using namespace std;

MenuOption::MenuOption(const Gui::ContextBox & parent, const Token *token):
Gui::ContextItem("", parent),
currentState(Deselected),
text(""),
infoText(""),
runnable(true){

    if (token){
      const Token & tok = *token;

      TokenView view = tok.view();
      while (view.hasMore()){
	  try{
	      const Token * token;
	      view >> token;
	      if ( *token == "info" ){
                  readInfo(token);
                  /*
		  // get info text add to option
		  std::string temp;
		  *token >> temp;
		  setInfoText(temp);
                  */
	      } else if( *token == "option-anim" ) {
	      } else {
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
void MenuOption::readName(const Token * token){
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
void MenuOption::readInfo(const Token * token){
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
}
    
/* subclasses can lazily load stuff by overriding this method */
void MenuOption::open(){
}

/* do any dynamic cleanup here */
void MenuOption::close(){
}

// This is to pass paramaters to an option ie a bar or something
bool MenuOption::leftKey(){
    return false;
}

bool MenuOption::rightKey(){
    return false;
}

const std::string MenuOption::getName() const {
    return getText();
}

