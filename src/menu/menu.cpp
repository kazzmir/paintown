#include "util/bitmap.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "menu/menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/font.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/file-system.h"
#include "resource.h"
#include "globals.h"
#include "init.h"
#include "configuration.h"
#include "music.h"
#include "shutdown_exception.h"

#include "menu/optionfactory.h"
#include "menu/actionfactory.h"
#include "menu/menu_global.h"
#include "menu/menu_animation.h"

#include "input/input-manager.h"
#include "input/input-map.h"

#include <queue>
#include <map>

#include "gui/context-box.h"

using namespace std;
using namespace Gui;

static std::string sharedFont = "";
static int sharedFontWidth = 24;
static int sharedFontHeight = 24;

static const int GradientMax = 50;

Point::Point():
x(0),
y(0){
}

Point::Point(int x, int y):
x(x),
y(y){
}

Point::~Point(){
}

static int selectedGradientStart(){
    static int color = Bitmap::makeColor(19, 167, 168);
    return color;
}

static int selectedGradientEnd(){
    static int color = Bitmap::makeColor(27, 237, 239);
    return color;
}


static std::vector<ContextItem *> toContextList(const std::vector<MenuOption *> & list){
    std::vector<ContextItem *> contextItems;
    for (std::vector<MenuOption *>::const_iterator i = list.begin(); i != list.end(); ++i){
        contextItems.push_back(*i);
    }
    return contextItems;
}

Menu::Menu(const Filesystem::AbsolutePath & str) throw (LoadException):
selectedGradient(GradientMax, selectedGradientStart(), selectedGradientEnd()){
    load(str);
}

Menu::Menu(Token * token) throw (LoadException):
selectedGradient(GradientMax, selectedGradientStart(), selectedGradientEnd()){
    load(token);
}

Menu::Menu():
music(""),
selectSound(""),
longestTextLength(0),
currentDrawState( NoFade ),
work(new Bitmap(GFX_X, GFX_Y)),
menuInfo(""),
parent(0),
_name(""),
hasOptions(false),
removeOption(false),
background(0),
clearColor(Bitmap::makeColor(0,0,0)),
option(false),
selectedGradient(GradientMax, selectedGradientStart(), selectedGradientEnd()){
	contextMenu.position.radius = 15;
	optionInfoTextLocation.x = 320;
	optionInfoTextLocation.y = 100;
	menuInfoLocation.x = 320;
	menuInfoLocation.y = 465;
    if (work->getError()){
        Global::debug(-1) << "*BUG* Could not allocate bitmap for menu" << endl;
    }

    /* key, delay, block, output */
    /* vi keys */
    input.set(Keyboard::Key_J, 0, true, Down);
    input.set(Keyboard::Key_K, 0, true, Up);
    input.set(Keyboard::Key_H, 0, true, Left);
    input.set(Keyboard::Key_L, 0, true, Right);
    input.set(Keyboard::Key_UP, 0, true, Up);
    /* regular keys */
    input.set(Keyboard::Key_DOWN, 0, true, Down);
    input.set(Keyboard::Key_LEFT, 0, true, Left);
    input.set(Keyboard::Key_RIGHT, 0, true, Right);
    input.set(Keyboard::Key_ENTER, 0, true, Select);
    input.set(Keyboard::Key_ESC, 0, true, Exit);
    /* joystick */
    input.set(Joystick::Up, 0, true, Up);
    input.set(Joystick::Down, 0, true, Down);
    input.set(Joystick::Left, 0, true, Left);
    input.set(Joystick::Right, 0, true, Right);
    input.set(Joystick::Button1, 0, true, Select);
    input.set(Joystick::Button2, 0, true, Exit);
}

void Menu::load(Token *token) throw (LoadException){
    if ( *token != "menu" ){
        throw LoadException("Not a menu");
    } else if (!token->hasTokens()){
        return;
    }

    sharedFontWidth = Configuration::getMenuFontWidth();
    sharedFontHeight = Configuration::getMenuFontHeight();

    while ( token->hasTokens() ){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "name" ){
                // Set menu name
                *tok >> _name;
            } else if ( *tok == "music" ) {
                // Set music and push onto the stack
                *tok >> music;
            } else if( *tok == "select-sound" ) {
                *tok >> selectSound;
            } else if (*tok == "back-sound"){
                *tok >> backSound;
                try{
                    /* try to load it */
                    Resource::getSound(backSound);
                } catch (const LoadException & le){
                    Global::debug(0) << "Could not load sound " << backSound << " because " << le.getReason() << endl;
                    /* we failed, so set the backSound to nothing */
                    backSound = "";
                }
            } else if (*tok == "ok-sound"){
                *tok >> okSound;
                try{
                    /* try to load it */
                    Resource::getSound(okSound);
                } catch (const LoadException & le){
                    Global::debug(0) << "Could not load sound " << okSound << " because " << le.getReason() << endl;
                    /* we failed, so set the backSound to nothing */
                    okSound = "";
                }
            } else if ( *tok == "background" ) {
                std::string temp;
                *tok >> temp;
                if ( background ){
                    delete background;
                }
                Filesystem::AbsolutePath full = Filesystem::find(Filesystem::RelativePath(temp));
                background = new Bitmap(full.path());
                if ( background->getError() ){
                    Global::debug(0) << "Problem loading Bitmap: " << full.path() << endl;
                    delete background;
                    background = 0;
                }
            } else if ( *tok == "clear-color" ) {
                /* This is the clear color of the background if there is no background image specified within the menu
                 * Although if this is a submenu, and parent has a background that will be used instead...*/
                int r=0,g=0,b=0;
                *tok >> r >> g >> b;
                clearColor = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "position" ) {
                // This handles the placement of the menu list and surrounding box
                *tok >> contextMenu.position.x >> contextMenu.position.y >> contextMenu.position.width >> contextMenu.position.height;
            } else if ( *tok == "position-body" ) {
                // This handles the body color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> contextMenu.position.bodyAlpha;
                contextMenu.position.body = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "position-border" ) {
                // This handles the border color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> contextMenu.position.borderAlpha;
                contextMenu.position.border = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "fade-speed" ) {
                // Menu fade in speed
                int speed;
                *tok >> speed;
                contextMenu.setFadeSpeed(speed);
            } else if ( *tok == "font" ) {
                string str;
                *tok >> str >> sharedFontWidth >> sharedFontHeight; 
                /* FIXME: make sharedFont an AbsolutePath */
                sharedFont = Filesystem::find(Filesystem::RelativePath(str)).path();
            } else if( *tok == "option" ) {
                try{
                    MenuOption *temp = OptionFactory::getOption(tok);
                    if (temp){
                        addOption(temp);
                    }
                } catch (const LoadException & le){
                    Global::debug(0) << "Could not read option: " << le.getReason() << endl;
                    tok->print(" ");
                }
            } else if (*tok == "action"){
                ActionAct(tok);
            } else if (*tok == "info-position"){
                *tok >> optionInfoTextLocation.x >> optionInfoTextLocation.y;
            } else if (*tok == "menuinfo"){
                *tok >> menuInfo;
            } else if (*tok == "menuinfo-position"){
                *tok >> menuInfoLocation.x >> menuInfoLocation.y;
            } else if (*tok == "anim"){
                MenuAnimation *animation = new MenuAnimation(tok);
                if (animation->getLocation() == 0){
                    backgroundAnimations.push_back(animation);
                } else if (animation->getLocation() == 1){
                    foregroundAnimations.push_back(animation);
                }
            } else {
                Global::debug(3) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
        } catch (const LoadException & ex){
            // delete current;
            throw ex;
        } catch (const Filesystem::NotFound & ex){
            throw LoadException(ex.getReason());
        }
    }

    if ( _name.empty() ){
        throw LoadException("No name set, the menu should have a name!");
    }

    if ( contextMenu.position.empty() ){
        throw LoadException("The position for the menu '" + getName() + "' list must be set!");
    }
    // Omit menu if no options are available
    if (! hasOptions) {
        Global::debug(1) << "The menu \"" << getName() << "\" has no options & will be omitted from the top level menu!" << endl;
        removeOption = true;
    }

    // Set the sharedFont
    if (Configuration::getMenuFont() != "" && Util::exists(Configuration::getMenuFont())){
        sharedFont = Configuration::getMenuFont();
    }

    // Finally lets assign list order numbering and some other stuff
    // First length
    if (hasOptions){
        setupOptions();
    }
}

void Menu::setupOptions(){
    longestTextLength = Font::getFont(getFont(), getFontWidth(), getFontHeight()).textLength(menuOptions[0]->getText().c_str());

    // Before we finish lets get rid of the cruft
    for (std::vector< MenuOption *>::iterator optBegin = menuOptions.begin() ; optBegin != menuOptions.end(); /**/){
        if( (*optBegin)->scheduledForRemoval() ){
            Global::debug(0) << "Removed option: " << (*optBegin)->getText() << endl;
            delete (*optBegin);
            optBegin = menuOptions.erase(optBegin);
        }
        else optBegin++;
    }

    // Figure out text length
    for( unsigned int i = 0; i < menuOptions.size(); i++ ){
        checkTextLength(menuOptions[i]);
    }

    // Set initial location
    contextMenu.setList(toContextList(menuOptions));
    selectedOption = menuOptions[contextMenu.getCurrentIndex()];
    selectedOption->setState(MenuOption::Selected);

}

void Menu::load(const Filesystem::AbsolutePath & filename) throw (LoadException){
    // Must check for initial token, menu
    try{
        Global::debug(1) << "Loading menu " << filename.path() << endl;
        TokenReader tr(filename.path());
        Token * token = tr.readToken();
        load(token);
    } catch (const TokenException & e){
        throw LoadException(e.getReason());
    }
}

static void tryPlaySound(const string & path){
    if (path != ""){
        Sound * ok = Resource::getSound(path);
        if (ok != NULL){
            ok->play();
        }
    }
}

/*! Logic */
void Menu::act(bool &endGame, bool reset){

    InputMap<MenuInput>::Output inputState = InputManager::getMap(input);

    if (inputState[Up]){
        selectedOption->setState(MenuOption::Deselected);
        bool moved = contextMenu.previous();
        selectedOption = menuOptions[contextMenu.getCurrentIndex()];
        selectedOption->setState(MenuOption::Selected);
        selectedOption->resetAnimations();

        if (moved && menuOptions.size() > 1){
            MenuGlobals::playSelectSound();
        }
    }

    if (inputState[Down]){
        selectedOption->setState(MenuOption::Deselected);
        bool moved = contextMenu.next();
        selectedOption = menuOptions[contextMenu.getCurrentIndex()];
        selectedOption->setState(MenuOption::Selected);
        selectedOption->resetAnimations();

        if (moved && menuOptions.size() > 1){
            MenuGlobals::playSelectSound();
        }
    }

    if (inputState[Left]){
        if (selectedOption->leftKey()){
            /* ??? */
        }
    }

    if (inputState[Right]){
        if (selectedOption->rightKey()){
            /* ??? */
        }
    }

    if (inputState[Exit]){
        selectedOption->setState(MenuOption::Deselected);
        InputManager::waitForRelease(input, Exit);
        
        throw ReturnException();
    }

    if (inputState[Select]){
        if (selectedOption->isRunnable()){
            selectedOption->setState(MenuOption::Run);
            tryPlaySound(okSound);
            contextMenu.close();
        }
    }

    for ( std::vector <MenuOption *>::iterator b = menuOptions.begin() ; b != menuOptions.end(); b++ ){
        (*b)->logic();
    }

    // Current option animation logic
    selectedOption->updateAnimations();

    // Animations
    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
        (*i)->act();
    }
    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
        (*i)->act();
    }
}

void Menu::draw(const Box &area, Bitmap *bmp){
    contextMenu.render(*bmp);
}

void Menu::run(){
    bool done = false;
    bool endGame = false;

    if ( menuOptions.empty() ){
        return;
    }

    selectedOption = menuOptions[contextMenu.getCurrentIndex()];
    selectedOption->setState(MenuOption::Selected);

    if ( !selectSound.empty() ){
        MenuGlobals::setSelectSound(selectSound);
    }

    double runCounter = 0;
    while( ! endGame ){
        Global::speed_counter = 0;
        
        // Reset animations
        for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
            (*i)->reset();
        }
        for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
            (*i)->reset();
        }

        // Set music
        if ( !music.empty() ){
            MenuGlobals::setMusic(music);
        }

        // Set font and fade in
        contextMenu.setFont(getFont(), getFontWidth(), getFontHeight());
        contextMenu.open();
        
        done = false;
        
        bool requestsReturn = false;

        while ( !done && contextMenu.isActive() ){

            bool draw = false;

            if (Global::shutdown()){
                throw ShutdownException();
            }

            if ( Global::speed_counter > 0 ){
                draw = true;
                runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
                while ( runCounter >= 1.0 ){
                    runCounter -= 1;
                    InputManager::poll();
                    try{
                        act(endGame);
                    } catch (const ReturnException &ex){
                        done = requestsReturn = true;
                    }
                    if (selectedOption->getState() == MenuOption::Run){
                        done = true;
                    }
                    contextMenu.act();
                }

                Global::speed_counter = 0;
            }

            if ( draw && selectedOption->getState() != MenuOption::Run ){
                // Draw

                // Do the background
                drawBackground(work);
                // Do background animations
                for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the background of the menu of selected object 
                selectedOption->drawBelow(work);
                // Draw menu
                contextMenu.render(*work);
                // Draw option info text
                drawInfoBox(selectedOption->getInfoText(), optionInfoTextLocation, work);
                // Draw menu info text
                drawInfoBox(menuInfo, menuInfoLocation, work);
                // Draw foreground animations
                for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the foreground of the menu of selected object 
                selectedOption->drawAbove(work);
                // Finally render to screen
                work->BlitToScreen();
            }

            while ( Global::speed_counter < 1 ){
                Util::rest( 1 );
                InputManager::poll();
            }

            done |= endGame;
        }
        // Check if requesting return
        if (requestsReturn){
            throw ReturnException();
        }
        // lets run it
        try{
            selectedOption->run(endGame);
        } catch (const ReturnException & re){
            tryPlaySound(backSound);
        }
        selectedOption->setState(MenuOption::Selected);
        selectedOption->resetAnimations();
        // Reset music
        if ( !music.empty() ){
            MenuGlobals::setMusic(music);
        }
        // Reset it's state
        selectedOption->setState(MenuOption::Selected);
        if ( !selectSound.empty() ){
            MenuGlobals::setSelectSound(selectSound);
        }

        if (!selectSound.empty()){
            if(MenuGlobals::currentSelectSound() != selectSound){
                MenuGlobals::popSelectSound();
            }
        }
    }
}

/*! set parent */
void Menu::setParent(Menu *menu){
    this->parent = menu;
}

/*! get background */
Bitmap *Menu::getBackground(){
    if (!background){
	Menu *p = getParent();
	if (p){
	    return p->getBackground();
	} else {
	    return 0;
	}
    }
    return background;
}

/*! Add options to menu */
void Menu::addOption(MenuOption *opt){
  if (opt){
      opt->setParent(this);
      hasOptions = true;
      menuOptions.push_back(opt);
  }
}

/*! Get working bitmap */
Bitmap *Menu::getWork(){
    return work;
}

std::string &Menu::getFont(){
	return sharedFont;
}

//! get font width
int Menu::getFontWidth(){
  return sharedFontWidth;
}

//! get font height
int Menu::getFontHeight(){
  return sharedFontHeight;
}
                
void Menu::setFontName(const std::string & str){
    Configuration::setMenuFont(str);
    sharedFont = str;
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

void Menu::setFontWidth(int w){
    if (w < 1){
        w = 1;
    }

    sharedFontWidth = w;
    Configuration::setMenuFontWidth(w);
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

void Menu::setFontHeight(int h){
    if (h < 1){
        h = 1;
    }

    sharedFontHeight = h;
    Configuration::setMenuFontHeight(h);
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

//! set new font menu wide
void Menu::setFont(const std::string &font, int w, int h){
    if (Util::exists(font) == true){
        sharedFont = font;
        sharedFontWidth = w;
        sharedFontHeight = h;
    }
}

//! Set longest length
void Menu::checkTextLength(MenuOption *opt){
	// Set longest text length depending on type
	switch(opt->getType()){
		case MenuOption::AdjustableOption : {
			int len = Font::getFont(getFont(), getFontWidth(), getFontHeight()).textLength(opt->getText().c_str()) + 10;
			if(len > longestTextLength)longestTextLength = len;
			break;
		}
		case MenuOption::Option:
		default : {
			int len = Font::getFont(getFont(), getFontWidth(), getFontHeight()).textLength(opt->getText().c_str());
			if ( len > longestTextLength ){
				longestTextLength = len;
			}
			break;
		}
	}
}

void Menu::drawBackground(Bitmap *bmp){
    Bitmap *temp = getBackground();
    if ( !temp ){
	    bmp->fill(clearColor);
    } else {
	temp->Stretch(*bmp);
    }
}

int Menu::getSelectedColor(bool selected){
    if (selected){
        return selectedGradient.current();
    } else {
        static int white = Bitmap::makeColor(255,255,255);
        return white;
    }
}

// Draw info box
void Menu::drawInfoBox (const std::string &info, const Point &location, Bitmap *bmp ){
    if ( info.empty() ) return;
    const Font & vFont = Font::getFont(getFont(), getFontWidth(), getFontHeight());
    switch ( currentDrawState ){
        case FadeIn :
            break;
        case FadeInText :
            break;
        case NoFade:
        default: {
            Box area;
            vector<string> strings;
            size_t start = 0;
            size_t last = 0;
            start = info.find("\n");
            while (start != string::npos){
                strings.push_back(info.substr(last, start - last));
                last = start + 1;
                start = info.find("\n", last);
            }
            strings.push_back(info.substr(last));

            area.position.radius = 15;
            int maxWidth = 0;
            int height = 0;
            for (vector<string>::iterator it = strings.begin(); it != strings.end(); it++){
                int w = vFont.textLength((*it).c_str()) + 10;
                if (w > maxWidth){
                    maxWidth = w;
                }
                height += vFont.getHeight();
            }
            area.position.width = maxWidth;
            area.position.height = height;
            area.position.x = location.x - (area.position.width / 2);
            area.position.y = location.y - (area.position.height / 2);
            area.position.body = Bitmap::makeColor(32,32,0);
            area.position.bodyAlpha = contextMenu.position.bodyAlpha;
            area.position.border = contextMenu.position.border;
            area.position.border = contextMenu.position.borderAlpha;

            // Draw box
            area.render(*bmp);

            // Draw text
            int sy = area.position.y - 5;
            for (vector<string>::iterator it = strings.begin(); it != strings.end(); it++){
                static int white = Bitmap::makeColor(255,255,255);
                string & str = *it;
                vFont.printf(area.position.x + 5, sy, white, *bmp, str, 0 );
                sy += vFont.getHeight();
            }
            break;
        }
    }
}

Menu::~Menu(){
	// cleanup
	if (work){
	    delete work;
	}
	
	std::vector <MenuOption *>::iterator b = menuOptions.begin();
	std::vector <MenuOption *>::iterator e = menuOptions.end();
	for(;b!=e;++b){
		if((*b))delete (*b);
	}
	if( background )delete background;
	
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
