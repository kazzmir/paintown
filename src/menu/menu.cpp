#include "util/bitmap.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "menu/menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/font.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "resource.h"
#include "globals.h"
#include "init.h"
#include "configuration.h"
#include "music.h"

#include "menu/optionfactory.h"
#include "menu/actionfactory.h"
#include "menu/menu_global.h"
#include "menu/menu_animation.h"
#include "gui/keyinput_manager.h"

#include <queue>
#include <map>

using namespace std;

Bitmap *Menu::work = 0;
int Menu::clearColor = Bitmap::makeColor(0,0,0);

// The top level menu, it is required to be main or whatever this set to
static std::string parentMenu = "main";

static std::string sharedFont = "";
static int sharedFontWidth = 24;
static int sharedFontHeight = 24;

const int yellow = Bitmap::makeColor(255, 255, 0);
const int white = Bitmap::makeColor(255, 255, 255);

static std::map<std::string, Menu *> menus;

int Menu::fadeSpeed = 12;

/* why are these global? */
static int fadeAlpha=0;
static int infoPositionX = 0;
static int infoPositionY = 0;

// Creates unique ID's for options so that they can be flagged for removal
static unsigned int menuOptionID = 0;

static void addMenu( Menu * m ) throw( LoadException ){
	std::map<std::string, Menu *>::iterator i = menus.find(m->getName());
	if ( i == menus.end() ){
		// ignore level selector
		if (m->getName() == "level-select" )return;
		menus[m->getName()] = m; 
		
	} else {
		throw LoadException("A menu by the name of \""+m->getName()+"\" already exists!"); 
	}
}

Menu::Menu():
music(""),
selectSound(""),
longestTextLength(0),
_name(""),
hasOptions(false),
removeOption(false),
currentDrawState( FadeIn ),
background(0),
option(false){
	if ( ! work ){
		work = new Bitmap( GFX_X, GFX_Y ); //Bitmap::Screen;
	}
	backboard.position.radius = 15;
}

void Menu::load(Token *token)throw( LoadException ){
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
				background = new Bitmap(Util::getDataPath() +temp);
				if ( background->getError() ){
				    Global::debug(0) << "Problem loading Bitmap: " << Util::getDataPath() + temp << endl;
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
                            sharedFont = Util::getDataPath() + str;
			} else if( *tok == "option" ) {
				MenuOption *temp = getOption(tok);
				if(temp){
				    addOption(temp);
				}
			} else if( *tok == "action" ) {
				ActionAct(tok);
			} else if( *tok == "info-position" ) {
				*tok >> infoPositionX >> infoPositionY;
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
		}
	}
	
	if ( _name.empty() ){
		throw LoadException("No name set, the menu should have a name!");
	}

	/* Since the introduction of animations... no need to enforce a background
	 * Instead if there is no background... the background will be cleared to color defaults to black 0,0,0
	if ( !background && getName() == parentMenu ){
		throw LoadException("There should be at least one background in the main menu!");
	}*/
	
	if (! infoPositionX || ! infoPositionY){
		throw LoadException("The position for the menu info boxes must be set!");
	}

	if ( backboard.position.empty() ){
		throw LoadException("The position for the menu list must be set!");
	}
	
	if ( ! hasOptions ) {
		if( getName() == parentMenu ){
		    throw LoadException("This is the main menu, it is required that it has options!");
		} else{
		  Global::debug(0) << "The menu \"" << getName() << "\" has no options & will be omitted from the top level menu!" << endl;
		    removeOption = true;
		  return;
		}
	}
	
	addMenu( this );

        /*
        if (sharedFont == ""){
            sharedFont = Configuration::getMenuFont();
            // sharedFont = "fonts/arial.ttf";
        }
        */
        if (Configuration::getMenuFont() != ""){
            sharedFont = Configuration::getMenuFont();
        }
	
	// Finally lets assign list order numering and some other stuff
	// First length
	longestTextLength = Font::getFont(getFont(), getFontWidth(), getFontHeight()).textLength(menuOptions[0]->getText().c_str());
	
	// Before we finish lets get rid of the cruft
	for( std::vector< MenuOption *>::iterator optBegin = menuOptions.begin() ; optBegin != menuOptions.end(); ){
	      if( (*optBegin)->scheduledForRemoval() ){
		Global::debug(0) << "Removed option: " << (*optBegin)->getText() << endl;
		delete (*optBegin);
		optBegin = menuOptions.erase(optBegin);
	      }
	      else optBegin++;
	}

	for( unsigned int i = 0; i < menuOptions.size(); i++ ){
		checkTextLength(menuOptions[i]);
	}
}

void Menu::load(const std::string &filename) throw (LoadException){
    // Must check for initial token, menu
    try{
        TokenReader tr( filename );
        Token * token = tr.readToken();
        load(token);
	
	if( !option){
	    if( !getMenu(parentMenu) ){
	      throw LoadException("\"main\" menu not found, the top level directory must be named \"main\"!");
	    }
	}
	
    } catch (const TokenException & e){
        throw LoadException(e.getReason());
    }
}

void Menu::run(){

    Bitmap screen_buffer(320, 240);
    bool done = false;
    bool endGame = false;

    if ( menuOptions.empty() ){
        return;
    }

    selectedOption = menuOptions.begin();
    menuOptions.front()->setState(MenuOption::Selected);

    if ( !music.empty() ){
        MenuGlobals::setMusic(music);
    }

    if ( !selectSound.empty() ){
        MenuGlobals::setSelectSound(selectSound);
    }

    double runCounter = 0;
    while( ! endGame ){
        Global::speed_counter = 0;
        Global::second_counter = 0;
        int game_time = 100;
        motion = 0;

        /*
           sharedFont = ourFont;
           sharedFontWidth = fontWidth;
           sharedFontHeight = fontHeight;
           */

        // Reset fade stuff
        resetFadeInfo();

        // Reset animations
        for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
            (*i)->reset();
        }
        for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
            (*i)->reset();
        }

        while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){

            bool draw = false;
            const char vi_up = 'k';
            const char vi_down = 'j';
            const char vi_left = 'h';
            const char vi_right = 'l';

            keyInputManager::update();

            if ( Global::speed_counter > 0 ){
                draw = true;
                runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
                while ( runCounter >= 1.0 ){
                    runCounter -= 1;
                    // Keys

                    if ( keyInputManager::keyState(keys::UP, true ) ||
                            /* for vi people like me */
                            keyInputManager::keyState(vi_up, true )){	

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

                    if ( keyInputManager::keyState(keys::DOWN, true ) ||
                            /* for vi people like me */
                            keyInputManager::keyState(vi_down, true )){

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

                    if (keyInputManager::keyState(keys::LEFT, true) ||
                        keyInputManager::keyState(vi_left, true)){

                        if ((*selectedOption)->leftKey()){
                            /* ??? */
                        }
                    }

                    if ( keyInputManager::keyState(keys::RIGHT, true )||
                         keyInputManager::keyState(vi_right, true )){

                        if ((*selectedOption)->rightKey()){
                            /* ??? */
                        }
                    }

                    if ( keyInputManager::keyState(keys::ENTER, true ) ){
                        if ((*selectedOption)->isRunnable()){
                            (*selectedOption)->setState(MenuOption::Run);
                        }
                    }

                    std::vector <MenuOption *>::iterator b = menuOptions.begin();
                    std::vector <MenuOption *>::iterator e = menuOptions.end();
                    for ( ; b != e; b++ ){
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

                Global::speed_counter = 0;
            }

            while ( Global::second_counter > 0 ){
                game_time--;
                Global::second_counter--;
                if ( game_time < 0 ){
                    game_time = 0;
                }
            }

            if ( draw ){
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
                drawText(work);
                // Draw info text
                drawInfoText(work);
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
                keyInputManager::update();
            }

            endGame = done |= keyInputManager::keyState(keys::ESC, true );
        }

        // do we got an option to run, lets do it
        if ((*selectedOption)->getState() == MenuOption::Run){
            try{
                if (backSound != ""){
                    Sound * ok = Resource::getSound(okSound);
                    ok->play();
                }
                (*selectedOption)->run(endGame);
            } catch ( const ReturnException & re ){
            }
            // Reset it's state
            (*selectedOption)->setState(MenuOption::Selected);
            if ( !music.empty() ){
                MenuGlobals::setMusic(music);
            }
            if ( !selectSound.empty() ){
                MenuGlobals::setSelectSound(selectSound);
            }
        }

        if (!music.empty()){
            if(MenuGlobals::currentMusic() != music){
                MenuGlobals::popMusic();
            }
        }

        if (!selectSound.empty()){
            if(MenuGlobals::currentSelectSound() != selectSound){
                MenuGlobals::popSelectSound();
            }
        }

        if (endGame){
            // Deselect selected entry
            (*selectedOption)->setState(MenuOption::Deselected);
            if (backSound != ""){
                Sound * back = Resource::getSound(backSound);
                back->play();
            }
        }
    }
}

Menu *Menu::getMenu(const std::string &name){
	std::map<std::string, Menu *>::iterator i = menus.find(name);
	if ( i!=menus.end() ){
			  return i->second;
	}
	return 0;
}

std::string &Menu::getParentMenu(){
	return parentMenu;
}

/*! Add options to menu */
void Menu::addOption(MenuOption *opt){
  if(opt){
      menuOptionID++;
      opt->setID(menuOptionID);
      opt->parent = this;
      hasOptions = true;
      menuOptions.push_back(opt);
  }
}

void Menu::setBitmap(Bitmap *bmp){
	work = bmp;
}

/*! Get current background in Bitmap */
Bitmap *Menu::getBackground()
{
	return getMenu(parentMenu)->background;
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
        std::map<std::string, Menu *>::iterator begin = menus.begin();
        std::map<std::string, Menu *>::iterator end = menus.end();

        for ( ;begin!=end;++begin ){
            begin->second->longestTextLength = Font::getFont(font, w, h).textLength(begin->second->menuOptions[0]->getText().c_str());
        }
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

void Menu::drawBackground(Bitmap *work){
	if ( !background ){
	      Bitmap *temp = getMenu( parentMenu )->background;
	      if ( temp ){
		  temp->Stretch(*work); 
	      } else {
		  work->fill(clearColor);
	      }
	}
	else background->Stretch(*work);
}

//! Draw board
void Menu::drawTextBoard(Bitmap *work){
	switch(currentDrawState){
		case FadeIn : {
			fadeBox.render( work );
			break;
		}
		case FadeInText:
		case NoFade:
		default : {
			// Our box widget
			backboard.render(work);
			break;
		}
	}
}

//! Draw text
void Menu::drawText(Bitmap *work){
    const Font & vFont = Font::getFont(getFont(), getFontWidth(), getFontHeight());
    const double spacing = 1.3;

    const int displayTotal = (int)((backboard.position.height / (int)(vFont.getHeight()/spacing)) % 2 ==0 ? backboard.position.height / (vFont.getHeight()/spacing) - 1 : backboard.position.height / (vFont.getHeight()/spacing)) + 2;
    const int fromMiddle = (displayTotal - 1)/2;
    const int starty = (int)((backboard.position.height/2)-(((vFont.getHeight()/spacing) * displayTotal)/2));

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
    // Set clipping so that text won't go beyond it's boundaries
    work->setClipRect(backboard.position.x+2, backboard.position.y+2,backboard.position.getX2()-2,backboard.position.getY2()-2);
    for (int i=0;i<displayTotal;++i){
        std::vector <MenuOption *>::iterator iterOption = menuOptions.begin() + currentCounter % menuOptions.size();
        const int startx = (backboard.position.width/2)-(vFont.textLength((*iterOption)->getText().c_str())/2);
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

        int text_x = backboard.position.x + startx;
        int text_y = (int)(backboard.position.y + starty + i * vFont.getHeight()/spacing) + motion;

        switch (currentDrawState){
            case FadeIn : {
                break;
            }
            case FadeInText : {
                Bitmap::transBlender( 0, 0, 0, fadeAlpha );
                work->drawingMode( Bitmap::MODE_TRANS );
                if (iterOption == selectedOption) {
                    switch((*iterOption)->getType()) {
                        case MenuOption::AdjustableOption : {
                            const int triangleSize = 10;
                            int cx = (backboard.position.x + startx) - 15;
                            int cy = (int)(text_y + (vFont.getHeight()/spacing) / 2 + 2);
                            work->triangle( cx + triangleSize / 2, cy - triangleSize / 2, cx - triangleSize, cy, cx + triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getLeftAdjustColor() );

                            cx = (backboard.position.x+startx + vFont.textLength((*iterOption)->getText().c_str()))+15;
                            work->triangle( cx - triangleSize / 2, cy - triangleSize / 2, cx + triangleSize, cy, cx - triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getRightAdjustColor() );
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
                vFont.printf(text_x, text_y, color, *work, (*iterOption)->getText(), 0 );
                work->drawingMode( Bitmap::MODE_SOLID );
                break;
            }
            case NoFade:
            default: {
                if ( iterOption == selectedOption ) {
                    switch((*iterOption)->getType()) {
                        case MenuOption::AdjustableOption : {
                            const int triangleSize = 10;
                            int cx = (backboard.position.x + startx) - 15;
                            int cy = (int)(text_y + (vFont.getHeight()/spacing) / 2 + 2);
                            work->triangle( cx + triangleSize / 2, cy - triangleSize / 2, cx - triangleSize, cy, cx + triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getLeftAdjustColor() );

                            cx = (backboard.position.x+startx + vFont.textLength((*iterOption)->getText().c_str()))+15;
                            work->triangle( cx - triangleSize / 2, cy - triangleSize / 2, cx + triangleSize, cy, cx - triangleSize / 2, cy + triangleSize / 2, (*iterOption)->getRightAdjustColor() );
                            break;
                        }
                        case MenuOption::Option:
                        default: {
                            break;
                        }
                    }
                }

                Bitmap::transBlender(0, 0, 0, textAlpha);
                work->drawingMode( Bitmap::MODE_TRANS );
                vFont.printf(text_x, text_y, color, *work, (*iterOption)->getText(), 0 );
                work->drawingMode( Bitmap::MODE_SOLID );
                break;
            }
        }

        currentCounter = currentCounter + 1;

        /*
        optionAlpha+=optionAlphaIncrements;
        if ( optionAlpha >= 250 ){optionAlphaIncrements -= optionAlphaIncrements*2;optionAlpha=250;}
        */
    }
    work->setClipRect(0, 0, work->getWidth(), work->getHeight());
}

// Draw info text
void Menu::drawInfoText ( Bitmap *work ){
    if ( (*selectedOption)->getInfoText().empty() ) return;
    const Font & vFont = Font::getFont(getFont(), getFontWidth(), getFontHeight());
    switch ( currentDrawState ){
        case FadeIn :
            break;
        case FadeInText :
            break;
        case NoFade:
        default: {
            Box area = (*selectedOption)->getInfoTextLocation();
            vector<string> strings;
            size_t start = 0;
            size_t last = 0;
            const string & optionText = (*selectedOption)->getInfoText();
            start = optionText.find("\n");
            while (start != string::npos){
                strings.push_back(optionText.substr(last, start - last));
                last = start + 1;
                start = optionText.find("\n", last);
            }
            strings.push_back(optionText.substr(last));

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
            area.position.height = height + 20;
            area.position.x = area.position.x !=0 ? area.position.x - (area.position.width / 2) : infoPositionX - (area.position.width / 2);
            area.position.y = area.position.y !=0 ? area.position.y - (area.position.height / 2) : infoPositionY - (area.position.height / 2);
            // area.position.body = backboard.position.body;
            area.position.body = Bitmap::makeColor(32,32,0);
            area.position.bodyAlpha = backboard.position.bodyAlpha;
            area.position.border = backboard.position.border;
            area.position.borderAlpha = backboard.position.borderAlpha;

            // Draw box
            area.render(work);

            // Draw text
            int sy = area.position.y + 10;
            for (vector<string>::iterator it = strings.begin(); it != strings.end(); it++){
                string & str = *it;
                vFont.printf(area.position.x + 5, sy, white, *work, str, 0 );
                sy += vFont.getHeight();
            }
            break;
        }
    }
}

Menu::~Menu(){
	// cleanup
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
