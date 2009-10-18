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

#include "game/input-manager.h"
#include "game/input-map.h"

#include <queue>
#include <map>

using namespace std;

static std::string sharedFont = "";
static int sharedFontWidth = 24;
static int sharedFontHeight = 24;

const int yellow = Bitmap::makeColor(255, 255, 0);
const int white = Bitmap::makeColor(255, 255, 255);

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

Menu::Menu(const std::string & str) throw (LoadException) {
    load(str);
}

Menu::Menu(Token * token) throw (LoadException) {
    load(token);
}

Menu::Menu():
music(""),
selectSound(""),
longestTextLength(0),
motion(0),
currentDrawState( NoFade ),
work(new Bitmap(GFX_X, GFX_Y)),
menuInfo(""),
parent(0),
_name(""),
hasOptions(false),
removeOption(false),
fadeAlpha(0),
fadeSpeed(12),
background(0),
clearColor(Bitmap::makeColor(0,0,0)),
option(false){
	backboard.position.radius = 15;
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
    input.set(InputMap<MenuInput>::Joystick::Up, 0, true, Up);
    input.set(InputMap<MenuInput>::Joystick::Down, 0, true, Down);
    input.set(InputMap<MenuInput>::Joystick::Left, 0, true, Left);
    input.set(InputMap<MenuInput>::Joystick::Right, 0, true, Right);
    input.set(InputMap<MenuInput>::Joystick::Button1, 0, true, Select);
    input.set(InputMap<MenuInput>::Joystick::Button2, 0, true, Exit);
}

void Menu::load(Token *token) throw (LoadException){
	if ( *token != "menu" )
		throw LoadException("Not a menu");
	else if ( ! token->hasTokens() )
		return;
	
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
                background = new Bitmap(Filesystem::find(temp));
                if ( background->getError() ){
                    Global::debug(0) << "Problem loading Bitmap: " << Filesystem::find(temp) << endl;
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
                *tok >> backboard.position.x >> backboard.position.y >> backboard.position.width >> backboard.position.height;
            } else if ( *tok == "position-body" ) {
                // This handles the body color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> backboard.position.bodyAlpha;
                backboard.position.body = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "position-border" ) {
                // This handles the border color of the menu box
                int r,g,b;
                *tok >> r >> g >> b >> backboard.position.borderAlpha;
                backboard.position.border = Bitmap::makeColor(r,g,b);
            } else if ( *tok == "fade-speed" ) {
                // Menu fade in speed
                *tok >> fadeSpeed;
            } else if ( *tok == "font" ) {
                string str;
                *tok >> str >> sharedFontWidth >> sharedFontHeight; 
                sharedFont = Filesystem::find(str);
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
            } else if( *tok == "action" ) {
                ActionAct(tok);
            } else if( *tok == "info-position" ) {
                *tok >> optionInfoTextLocation.x >> optionInfoTextLocation.y;
            } else if( *tok == "menuinfo" ){
                *tok >> menuInfo;
            } else if( *tok == "menuinfo-position" ){
                *tok >> menuInfoLocation.x >> menuInfoLocation.y;
            } else if( *tok == "anim" ) {
                MenuAnimation *animation = new MenuAnimation(tok);
                if (animation->getLocation() == 0){
                    backgroundAnimations.push_back(animation);
                } else if (animation->getLocation() == 1){
                    foregroundAnimations.push_back(animation);
                }
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
        } catch (const Filesystem::NotFound & ex){
            throw LoadException(ex.getReason());
        }
    }
	
	if ( _name.empty() ){
		throw LoadException("No name set, the menu should have a name!");
	}
	
	if ( backboard.position.empty() ){
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
    selectedOption = menuOptions.begin();
    menuOptions.front()->setState(MenuOption::Selected);

}

void Menu::load(const std::string &filename) throw (LoadException){
    // Must check for initial token, menu
    try{
        Global::debug(1) << "Loading menu " << filename << endl;
        TokenReader tr( filename );
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
void Menu::act(bool &endGame) throw (ReturnException){

    InputMap<MenuInput>::Output inputState = InputManager::getMap(input);

    if (inputState[Up]){
        motion -= getFontHeight();
        (*selectedOption)->setState(MenuOption::Deselected);
        if (selectedOption > menuOptions.begin()){
            selectedOption--;
        } else {
            selectedOption = menuOptions.end() -1;
        }

        (*selectedOption)->setState(MenuOption::Selected);
        (*selectedOption)->resetAnimations();

        if (menuOptions.size() > 1){
            MenuGlobals::playSelectSound();
        }
    }

    if (inputState[Down]){
        motion += getFontHeight();
        (*selectedOption)->setState(MenuOption::Deselected);
        if (selectedOption < menuOptions.begin()+menuOptions.size()-1){
            selectedOption++;
        } else {
            selectedOption = menuOptions.begin();
        }

        (*selectedOption)->setState(MenuOption::Selected);
        (*selectedOption)->resetAnimations();

        if (menuOptions.size() > 1){
            MenuGlobals::playSelectSound();
        }
    }

    if (inputState[Left]){
        if ((*selectedOption)->leftKey()){
            /* ??? */
        }
    }

    if (inputState[Right]){
        if ((*selectedOption)->rightKey()){
            /* ??? */
        }
    }

    if (inputState[Exit]){
        (*selectedOption)->setState(MenuOption::Deselected);
        /*
           if (backSound != ""){
           Sound * back = Resource::getSound(backSound);
           back->play();
           }
           */

        InputManager::waitForRelease(input, Exit);
        
        throw ReturnException();
    }

    if (inputState[Select]){
        if ((*selectedOption)->isRunnable()){
            (*selectedOption)->setState(MenuOption::Run);
            // lets run it
            try{
                tryPlaySound(okSound);
                (*selectedOption)->run(endGame);
            } catch (const ReturnException & re){
                /* hack to make sure the current menu is drawn properly */
                resetFadeInfo();
                tryPlaySound(backSound);
            }
            (*selectedOption)->setState(MenuOption::Selected);
            (*selectedOption)->resetAnimations();
            // Reset music
            if ( !music.empty() ){
                MenuGlobals::setMusic(music);
            }
        }
    }

    for ( std::vector <MenuOption *>::iterator b = menuOptions.begin() ; b != menuOptions.end(); b++ ){
        (*b)->logic();
        // Recalculate placement
        checkTextLength((*b));
    }

    const double motion_speed = 1.8;
    if (motion >= motion_speed){
        motion -= motion_speed;
    } else if (motion <= -motion_speed){
        motion += motion_speed;
    } else {
        motion = 0;
    }

    // motion = 0;

    // Current option animation logic
    (*selectedOption)->updateAnimations();

    // Animations
    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
        (*i)->act();
    }
    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
        (*i)->act();
    }

    // Lets do some logic for the box with text
    updateFadeInfo();
}

void Menu::draw(const Box &area, Bitmap *bmp){
}

void Menu::run() throw (ReturnException) {
    bool done = false;
    bool endGame = false;

    if ( menuOptions.empty() ){
        return;
    }

    selectedOption = menuOptions.begin();
    menuOptions.front()->setState(MenuOption::Selected);

    if ( !selectSound.empty() ){
        MenuGlobals::setSelectSound(selectSound);
    }

    double runCounter = 0;
    while( ! endGame ){
        Global::speed_counter = 0;
        Global::second_counter = 0;
        // int game_time = 100;
        motion = 0;

        // Reset fade stuff
        resetFadeInfo();

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

        while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){

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
                    act(endGame);
                }

                Global::speed_counter = 0;
            }

            /*
            while ( Global::second_counter > 0 ){
                game_time--;
                Global::second_counter--;
                if ( game_time < 0 ){
                    game_time = 0;
                }
            }
            */

            if ( draw && (*selectedOption)->getState() != MenuOption::Run ){
                // Draw

                // Do the background
                drawBackground(work);
                // Do background animations
                for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the background of the menu of selected object 
                (*selectedOption)->drawBelow(work);
                // Draw text board
                drawTextBoard(work);
                // Draw text
                // Set clipping so that text won't go beyond it's boundaries
                work->setClipRect(backboard.position.x+2, backboard.position.y+2,backboard.position.getX2()-2,backboard.position.getY2()-2);
                drawText(backboard, work);
                work->setClipRect(0, 0, work->getWidth(), work->getHeight());
                // Draw option info text
                drawInfoBox((*selectedOption)->getInfoText(), optionInfoTextLocation, work);
                // Draw menu info text
                drawInfoBox(menuInfo, menuInfoLocation, work);
                // Draw foreground animations
                for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the foreground of the menu of selected object 
                (*selectedOption)->drawAbove(work);
                // Finally render to screen
                work->BlitToScreen();
            }

            while ( Global::speed_counter < 1 ){
                Util::rest( 1 );
                InputManager::poll();
            }

            // endGame |= keyInputManager::keyState(keys::ESC, true );
            done |= endGame;

            /* can we move the check for ESC to act() ? */
#if 0
            if (keyInputManager::keyState(keys::ESC, true )){
                (*selectedOption)->setState(MenuOption::Deselected);
                /*
                   if (backSound != ""){
                   Sound * back = Resource::getSound(backSound);
                   back->play();
                   }
                   */
                throw ReturnException();
            }
#endif
        }

        // Reset it's state
        (*selectedOption)->setState(MenuOption::Selected);
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
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

void Menu::setFontHeight(int h){
    if (h < 1){
        h = 1;
    }
    sharedFontHeight = h;
    setFont(sharedFont, getFontWidth(), getFontHeight());
}

//! set new font menu wide
void Menu::setFont(const std::string &font, int w, int h){
    if ( Util::exists(font) == true){
        /*std::map<std::string, Menu *>::iterator begin = menus.begin();
        std::map<std::string, Menu *>::iterator end = menus.end();

        for ( ;begin!=end;++begin ){
            begin->second->longestTextLength = Font::getFont(font, w, h).textLength(begin->second->menuOptions[0]->getText().c_str());
        }*/
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

//! Reset fade info
void Menu::resetFadeInfo(){
	// Set the fade stuff
	currentDrawState = FadeIn;
	fadeBox.position = backboard.position;
	fadeBox.position.width = fadeBox.position.height = 0;
	fadeBox.position.x = backboard.position.x+(backboard.position.width/2);
	fadeBox.position.y = backboard.position.y+(backboard.position.height/2);
	fadeAlpha = 0;
}

//! Do fade logic
void Menu::updateFadeInfo(){
     // Lets do some logic for the box with text
    switch ( currentDrawState ){
	case FadeIn : {
	    if (fadeBox.position.x> backboard.position.x){
		fadeBox.position.x -= fadeSpeed;
	    } else if ( fadeBox.position.x < backboard.position.x ){
		fadeBox.position.x = backboard.position.x;
	    }

	    if (fadeBox.position.y > backboard.position.y){
		fadeBox.position.y -= fadeSpeed;
	    } else if (fadeBox.position.y<backboard.position.y){
		fadeBox.position.y = backboard.position.y;
	    }

	    if (fadeBox.position.width<backboard.position.width){
		fadeBox.position.width += (fadeSpeed*2);
	    } else if (fadeBox.position.width>backboard.position.width){
		fadeBox.position.width = backboard.position.width;
	    }

	    if (fadeBox.position.height<backboard.position.height){
		fadeBox.position.height += (fadeSpeed*2);
	    } else if (fadeBox.position.height>backboard.position.height){
		fadeBox.position.height = backboard.position.height;
	    }

	    if (fadeBox.position == backboard.position){
		currentDrawState = FadeInText;
	    }

	    break;
	}
	case FadeInText : {
	    if (fadeAlpha < 255){
		fadeAlpha += (fadeSpeed+2);
	    }

	    if (fadeAlpha >= 255){
		fadeAlpha = 255;
		currentDrawState = NoFade;
	    }
	    break;
	}
	case NoFade : {
	    break;
	}
	default : {
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

//! Draw board
void Menu::drawTextBoard(Bitmap *bmp){
	switch(currentDrawState){
		case FadeIn : {
			fadeBox.render( bmp );
			break;
		}
		case FadeInText:
		case NoFade:
		default : {
			// Our box widget
			backboard.render(bmp);
			break;
		}
	}
}

//! Draw text
void Menu::drawText(const Box &area, Bitmap *bmp){
    const Font & vFont = Font::getFont(getFont(), getFontWidth(), getFontHeight());
    const double spacing = 1.3;

    const int displayTotal = (int)((area.position.height / (int)(vFont.getHeight()/spacing)) % 2 ==0 ? area.position.height / (vFont.getHeight()/spacing) - 1 : backboard.position.height / (vFont.getHeight()/spacing)) + 2;
    const int fromMiddle = (displayTotal - 1)/2;
    const int starty = (int)((area.position.height/2)-(((vFont.getHeight()/spacing) * displayTotal)/2));

    std::vector <MenuOption *>::iterator beginIter = menuOptions.begin();
    std::vector <MenuOption *>::iterator endIter = menuOptions.end();

    int middle = 0;
    int currentCounter = 0;

    /*
    int optionAlphaIncrements = 250 / (fromMiddle + 1);
    int optionAlpha = optionAlphaIncrements;
    */

    for (int i = 0; beginIter != endIter; ++beginIter, ++i){
        if (beginIter == selectedOption){
            middle = i;
            break;
        }
    }

    currentCounter = middle - fromMiddle;
    if ( currentCounter < 0 ){
        currentCounter = (menuOptions.size()) + currentCounter;
    }


    for (int i=0;i<displayTotal;++i){
        std::vector <MenuOption *>::iterator iterOption = menuOptions.begin() + currentCounter % menuOptions.size();
        const int startx = (area.position.width/2)-(vFont.textLength((*iterOption)->getText().c_str())/2);
        const unsigned int color = ((*iterOption)->getState() == MenuOption::Selected) ? yellow : white;

        int distance;
        if (i > fromMiddle){
            distance = i - fromMiddle;
        } else {
            distance = fromMiddle - i;
        }

        // distance = (currentCounter + middle) % (fromMiddle);

        // printf("counter %d distance %d middle %d\n", currentCounter, distance, middle);
        double normal = (double) distance / (double) fromMiddle;
        int textAlpha = (int)(255.0 - 164.0 * normal * normal);

	if( textAlpha < 0 && (displayTotal == 1 || displayTotal == 2 )){
	  textAlpha = 255;
	} else if (textAlpha < 0){
            textAlpha = 25;
        } else if (textAlpha > 255){
            textAlpha = 255;
        }

        int text_x = area.position.x + startx;
        int text_y = (int)((area.position.y + starty + i * vFont.getHeight()/spacing) + motion);

        switch (currentDrawState){
            case FadeIn : {
                break;
            }
            case FadeInText : {
                Bitmap::transBlender( 0, 0, 0, fadeAlpha );
                bmp->drawingMode( Bitmap::MODE_TRANS );
                if (iterOption == selectedOption) {
                    switch((*iterOption)->getType()) {
                        case MenuOption::AdjustableOption : {
                            const int triangleSize = 10;
                            int cx = (area.position.x + startx) - 15;
                            int cy = (int)(text_y + (vFont.getHeight()/spacing) / 2 + 2);
                            bmp->triangle( cx + triangleSize / 2, cy - triangleSize / 2, cx - triangleSize, cy, cx + triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getLeftAdjustColor() );

                            cx = (area.position.x+startx + vFont.textLength((*iterOption)->getText().c_str()))+15;
                            bmp->triangle( cx - triangleSize / 2, cy - triangleSize / 2, cx + triangleSize, cy, cx - triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getRightAdjustColor() );
                            break;
                            }
                        case MenuOption::Option:
                        default: {
                            break;
                        }
                    }
                }

                int alpha = (int)(textAlpha * fadeAlpha / 255.0);
                Bitmap::transBlender(0, 0, 0, alpha);
                vFont.printf(text_x, text_y, color, *bmp, (*iterOption)->getText(), 0 );
                bmp->drawingMode( Bitmap::MODE_SOLID );
                break;
            }
            case NoFade:
            default: {
                if ( iterOption == selectedOption ) {
                    switch((*iterOption)->getType()) {
                        case MenuOption::AdjustableOption : {
                            const int triangleSize = 10;
                            int cx = (area.position.x + startx) - 15;
                            int cy = (int)(text_y + (vFont.getHeight()/spacing) / 2 + 2);
                            bmp->triangle( cx + triangleSize / 2, cy - triangleSize / 2, cx - triangleSize, cy, cx + triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getLeftAdjustColor() );

                            cx = (area.position.x+startx + vFont.textLength((*iterOption)->getText().c_str()))+15;
                            bmp->triangle( cx - triangleSize / 2, cy - triangleSize / 2, cx + triangleSize, cy, cx - triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getRightAdjustColor() );
                            break;
                        }
                        case MenuOption::Option:
                        default: {
                            break;
                        }
                    }
                }

                // Global::debug(1) << "menu option '" << (*iterOption)->getText() << "' at " << text_x << ", " << text_y << " in color " << color << " alpha " << textAlpha << endl;

                Bitmap::transBlender(0, 0, 0, textAlpha);
                bmp->drawingMode( Bitmap::MODE_TRANS );
                vFont.printf(text_x, text_y, color, *bmp, (*iterOption)->getText(), 0 );
                bmp->drawingMode( Bitmap::MODE_SOLID );
                break;
            }
        }

        currentCounter = currentCounter + 1;

        /*
        optionAlpha+=optionAlphaIncrements;
        if ( optionAlpha >= 250 ){optionAlphaIncrements -= optionAlphaIncrements*2;optionAlpha=250;}
        */
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
            // area.position.body = backboard.position.body;
            area.position.body = Bitmap::makeColor(32,32,0);
            area.position.bodyAlpha = backboard.position.bodyAlpha;
            area.position.border = backboard.position.border;
            area.position.borderAlpha = backboard.position.borderAlpha;

            // Draw box
            area.render(bmp);

            // Draw text
            int sy = area.position.y - 5;
            for (vector<string>::iterator it = strings.begin(); it != strings.end(); it++){
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
