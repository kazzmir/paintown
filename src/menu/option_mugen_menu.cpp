#include "menu/option_mugen_menu.h"
#include "menu/menu.h"
#include "util/token.h"
#include "return_exception.h"
#include "util/token_exception.h"
#include "util/funcs.h"
#include "globals.h"

#include "mugen/mugen_menu.h"
#include "mugen/mugen_exception.h"

using namespace std;

OptionMugenMenu::OptionMugenMenu(Token *token) throw (LoadException): MenuOption(token, Event), _menu(0)
{
	if ( *token != "mugen" ){
		throw LoadException("Not a mugen motif menu");
	}
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" ){
				// Create an image and push it back on to vector
				std::string temp;
				*tok >> temp;
				this->setText(temp);
			} else if (*tok == "motif"){
			    // Load Motif from file
			    std::string temp;
			    // Filename
			    *tok >> temp;
			    _menu = new MugenMenu(temp);
			    try {
				_menu->loadData();
			    } catch( const MugenException &ex ){
				string m("Problem with loading MUGEN menu: ");
				m += ex.getReason();
				throw LoadException(m);
			    }
			}else {
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
		} 
	}
	// Set this menu as an option
	_menu->setAsOption(true);
	
	// Lets check if this menu is going bye bye
	//if ( _menu->checkRemoval() ) setForRemoval(true);
}

OptionMugenMenu::~OptionMugenMenu()
{
	// Delete our menu
	if(_menu)delete _menu;
}

void OptionMugenMenu::logic()
{
	// Nothing
}

void OptionMugenMenu::run(bool &endGame) throw (ReturnException) {
	// Do our new menu
	_menu->run();
}

