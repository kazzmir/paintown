#include "menu/option_mugen_menu.h"
#include "menu/menu.h"
#include "util/token.h"
#include "return_exception.h"
#include "util/token_exception.h"
#include "util/funcs.h"
#include "level/utils.h"
#include "globals.h"
#include "loading.h"
#include <pthread.h>

#include "mugen/parse-cache.h"
#include "mugen/mugen_menu.h"
#include "mugen/mugen_exception.h"
#include "mugen/config.h"
#include "return_exception.h"

using namespace std;

OptionMugenMenu::OptionMugenMenu(Token *token) throw (LoadException): MenuOption(token, Event), _menu(0){
    if ( *token != "mugen" ){
        throw LoadException("Not a mugen motif menu");
    }
    
    // Load up configuration
    Mugen::Data::initialize();

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
                // Set the default motif
                Mugen::Data::setMotif(temp);
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
    // Load menu with default motif
    _menu = new MugenMenu(Mugen::Data::getMotif());
    // Set this menu as an option
    _menu->setAsOption(true);

    // Lets check if this menu is going bye bye
    //if ( _menu->checkRemoval() ) setForRemoval(true);
}

OptionMugenMenu::~OptionMugenMenu(){
    // Delete our menu
    if (_menu){
        delete _menu;
    }
}

void OptionMugenMenu::logic(){
	// Nothing
}

void OptionMugenMenu::run(bool &endGame){
    Mugen::ParseCache cache;
    // Load er up and throw up a load box to inform the user
    // Box::msgDialog(*getParent()->getWork(),"Loading M.U.G.E.N.!",2);
    pthread_t loading;
    Level::LevelInfo info;
    info.setLoadingMessage("Loading M.U.G.E.N");
    Loader::startLoading(&loading, (void*) &info);
    try {
        _menu->loadData();
    } catch (const MugenException & ex){
        string m("Problem with loading MUGEN menu: ");
        m += ex.getFullReason();
        Loader::stopLoading(loading);
        throw LoadException(m);
    }
    Loader::stopLoading(loading);

    try {
        // Run
        _menu->run();
    } catch (const ReturnException & re){
        // Say what?
        // Do not quit game
    }
}

