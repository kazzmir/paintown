#include "util/bitmap.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "menu/menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/font.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "globals.h"
#include "init.h"
#include "music.h"

#include "menu/optionfactory.h"
#include "menu/option_background.h"

#include "menu/actionfactory.h"

#include "menu/menu_global.h"

#include "gui/keyinput_manager.h"

#include <queue>
#include <map>

Bitmap *Menu::work = 0;

static std::queue<MenuOption *> backgrounds;

// static FreeTypeFont *font;
static Font *font;

const int yellow = Bitmap::makeColor( 255, 255, 0 );
const int white = Bitmap::makeColor( 255, 255, 255 );

static std::map<std::string, Menu *> menus;

int Menu::fadeSpeed = 12;

int fadeAlpha=0;

int infoPositionX = 0;

int infoPositionY = 0;

static void addMenu( Menu * m ) throw( LoadException ){
	std::map<std::string, Menu *>::iterator i = menus.find(m->getName());
	if ( i == menus.end() ){
		menus[m->getName()] = m;
	} else {
		throw LoadException("A menu by the name of \""+m->getName()+"\" already exists!"); 
	}
}

Menu::Menu():
music(""),
background(0),
vFont(0),
fontWidth(24),
fontHeight(24),
_menuflags(0),
longestTextLength(0),
_name(""),
currentDrawState( FadeIn ){
	if ( ! work ){
		work = new Bitmap( GFX_X, GFX_Y ); //Bitmap::Screen;
	}
	backboard.position.radius = 15;
}

void Menu::load(Token *token)throw( LoadException ){
	if ( *token != "menu" )
		throw LoadException("Not a menu");
	
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
			} else if ( *tok == "background" ) {
				// Create new background and push onto the stack
				background = new OptionBg(tok);
				
				backgrounds.push(background);
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
				std::string temp;
				*tok >> temp >> fontWidth >> fontHeight;
				vFont = new FreeTypeFont(Util::getDataPath() + temp);
				vFont->setSize(fontWidth,fontHeight);
			} else if( *tok == "option" ) {
				MenuOption *temp = getOption(tok);
				if(temp)menuOptions.push_back(temp);
			} else if( *tok == "action" ) {
				ActionAct(tok);
			} else if( *tok == "info-position" ) {
				*tok >> infoPositionX >> infoPositionY;
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

	if ( backgrounds.empty() ){
		throw LoadException("There should be at least one background in the entire menu!");
	}

	if ( backboard.position.empty() ){
		throw LoadException("The position for the menu list must be set!");
	}

	addMenu( this );
	
	if ( ! vFont ){
		std::string f = Util::getDataPath() + "/fonts/arial.ttf";
		vFont = new FreeTypeFont(f);
		vFont->setSize( fontWidth,fontHeight );
	}
	
	if( ! infoPositionX || ! infoPositionY ){
		throw LoadException("The position for the menu info boxes must be set!");
	}
	
	// Finally lets assign list order numering and some other stuff
	// First length
	longestTextLength = vFont->textLength(menuOptions[0]->getText().c_str());

	for ( unsigned int i = 0; i < menuOptions.size(); i++ ){
		menuOptions[i]->setID(i);
		checkTextLength(menuOptions[i]);
	}
}

void Menu::load(const std::string &filename) throw (LoadException){
    // Must check for initial token, menu
    try{
        TokenReader tr( filename );
        Token * token = tr.readToken();
        load(token);
    } catch (const TokenException & e){
        throw LoadException(e.getReason());
    }
}

useflags Menu::run(){
	
	Bitmap screen_buffer( 320, 240 );
	bool done = false;
	bool endGame = false;
	
	if ( menuOptions.empty() ){
		return _menuflags;
	}

	selectedOption = menuOptions.begin();
	menuOptions.front()->setState(MenuOption::Selected);
	
	if ( !music.empty() ){
		MenuGlobals::setMusic(music);
	}
	
        double runCounter = 0;
	while( ! endGame ){
		Global::speed_counter = 0;
		Global::second_counter = 0;
		int game_time = 100;		
		font = vFont;
		
		// Reset fade stuff
		resetFadeInfo();
		
		while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){
	
			bool draw = false;
			
			keyInputManager::update();
	
			if ( Global::speed_counter > 0 ){
				draw = true;
                                runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
                                while ( runCounter >= 1.0 ){
                                    runCounter -= 1;
                                    // Keys
                                    
                                    if ( keyInputManager::keyState(keys::UP, true ) ){	
                                            (*selectedOption)->setState(MenuOption::Deselected);
                                            if ( selectedOption > menuOptions.begin() ){
                                                    selectedOption--;
                                            }
					    else selectedOption = menuOptions.end() -1;
                                            (*selectedOption)->setState(MenuOption::Selected);	
                                    }

                                    if ( keyInputManager::keyState(keys::DOWN, true ) ){
                                            (*selectedOption)->setState(MenuOption::Deselected);
                                            if ( selectedOption < menuOptions.begin()+menuOptions.size()-1 ){
                                                    selectedOption++;
                                            }
					    else selectedOption = menuOptions.begin();
                                            (*selectedOption)->setState(MenuOption::Selected);
                                    }
                                    
                                    if ( keyInputManager::keyState(keys::LEFT, true ) ){
                                            if ( (*selectedOption)->leftKey()){
                                                    
                                            }
                                    }
                                    
                                    if ( keyInputManager::keyState(keys::RIGHT, true ) ){
                                            if ( (*selectedOption)->rightKey()){
                                                    
                                            }
                                    }
                                    
                                    if ( keyInputManager::keyState(keys::ENTER, true ) ){
                                            if((*selectedOption)->isRunnable())(*selectedOption)->setState( MenuOption::Run );
                                    }
                                    
                                    // Logic
                                    if ( backgrounds.front() ){
                                            backgrounds.front()->logic();
                                    }
                                    
                                    std::vector <MenuOption *>::iterator b = menuOptions.begin();
                                    std::vector <MenuOption *>::iterator e = menuOptions.end();
                                    for ( ; b != e; b++ ){
                                            (*b)->logic();
                                            
                                            // Recalculate placement
                                            checkTextLength((*b));
                                    }
                                    
                                    // Lets do some logic for the box with text
                                    switch ( currentDrawState ){
                                            case FadeIn : {

                                                    if ( fadeBox.position.x> backboard.position.x){
                                                            fadeBox.position.x -= fadeSpeed;
                                                    } else if ( fadeBox.position.x < backboard.position.x ){
                                                            fadeBox.position.x = backboard.position.x;
                                                    }

                                                    if ( fadeBox.position.y > backboard.position.y ){
                                                                      fadeBox.position.y-=fadeSpeed;
                                                    } else if ( fadeBox.position.y<backboard.position.y ){
                                                                      fadeBox.position.y=backboard.position.y;
                                                    }

                                                    if(fadeBox.position.width<backboard.position.width)fadeBox.position.width+=(fadeSpeed*2);
                                                    else if(fadeBox.position.width>backboard.position.width)fadeBox.position.width=backboard.position.width;
                                                    if(fadeBox.position.height<backboard.position.height)fadeBox.position.height+=(fadeSpeed*2);
                                                    else if(fadeBox.position.height>backboard.position.height)fadeBox.position.height=backboard.position.height;
                                                    if(fadeBox.position == backboard.position)currentDrawState = FadeInText;
                                                    break;
                                            }
                                            case FadeInText : {
                                                    if ( fadeAlpha<255 ){
                                                            fadeAlpha+=(fadeSpeed+2);
                                                    }

                                                    if ( fadeAlpha >= 255 ){
                                                            fadeAlpha=255;
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
				work->clear();
				// Draw
				if ( backgrounds.front() ){
					backgrounds.front()->draw(work);
				}

				// Draw text board
				drawTextBoard(work);
				// Draw text
				drawText(work);
				// Draw info text
				drawInfoText(work);
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
		if((*selectedOption)->getState() == MenuOption::Run){
			try{
				(*selectedOption)->run(endGame);
			} catch ( const ReturnException & re ){
			}
			// Reset it's state
			(*selectedOption)->setState(MenuOption::Selected);
			if ( !music.empty() ){
				MenuGlobals::setMusic(music);
			}
		}

		if(!music.empty()){
			if(MenuGlobals::currentMusic() != music){
				MenuGlobals::popMusic();
			}
		}

		if(endGame){
			//  pop out any backgrounds pushed onto the stack reseting it to the old one if applicable
			if(backgrounds.size() >= 2){
				delete backgrounds.front();
				backgrounds.pop();
				background = backgrounds.front();
			}
			
			// Deselect selected entry
			(*selectedOption)->setState(MenuOption::Deselected);
		}
	}
	
	return _menuflags;
}


Menu *Menu::getMenu(const std::string &name){
	std::map<std::string, Menu *>::iterator i = menus.find(name);
	if ( i!=menus.end() ){
			  return i->second;
	}
	return 0;
}

const std::string &Menu::getName(){
	return _name;
}

void Menu::setBitmap(Bitmap *bmp){
	work = bmp;
}

/*! Get current background in Bitmap */
Bitmap *Menu::getBackground()
{
	//return dynamic_cast<OptionBg *>(backgrounds.front())->getCurrentBackground();
	OptionBg *temp = (OptionBg *)backgrounds.front();
	return temp->getCurrentBackground();
}

Font *Menu::getFont(){
	return font;
}

//! Set longest length
void Menu::checkTextLength(MenuOption *opt){
	// Set longest text length depending on type
	switch(opt->getType()){
		case MenuOption::AdjustableOption : {
			int len = vFont->textLength(opt->getText().c_str()) + 10;
			if(len > longestTextLength)longestTextLength = len;
			break;
		}
		case MenuOption::Option:
		default : {
			int len = vFont->textLength(opt->getText().c_str());
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
        const double spacing = 1.3;

	const int displayTotal = (backboard.position.height / int(vFont->getHeight()/spacing)) % 2 ==0 ? backboard.position.height / (vFont->getHeight()/spacing) - 1 : backboard.position.height / (vFont->getHeight()/spacing);
	const int fromMiddle = (displayTotal - 1)/2;
	const int starty = (backboard.position.height/2)-(((vFont->getHeight()/spacing) * displayTotal)/2);
	
	std::vector <MenuOption *>::iterator beginIter = menuOptions.begin();
	std::vector <MenuOption *>::iterator endIter = menuOptions.end();
	
	int middle = 0;
	int currentCounter = 0;
	
	for(int i=0;beginIter!=endIter;++beginIter,++i){
		if ( beginIter == selectedOption ){middle = i;break;}
	}
	
	currentCounter = middle - fromMiddle;
	if ( currentCounter < 0 ) currentCounter = (menuOptions.size()) + currentCounter;
	
	for(int i=0;i<displayTotal;++i){
	      std::vector <MenuOption *>::iterator iterOption = menuOptions.begin() + currentCounter;
	      const int startx = (backboard.position.width/2)-(vFont->textLength((*iterOption)->getText().c_str())/2);
	      const unsigned int color = ((*iterOption)->getState() == MenuOption::Selected) ? yellow : white;
	      vFont->printf( backboard.position.x + startx, int((backboard.position.y + starty) + i * vFont->getHeight()/spacing), color, *work, (*iterOption)->getText(), 0 );
	      currentCounter++;
	      if ( currentCounter > signed(menuOptions.size()-1) ) currentCounter = 0;
	}
	/*
	std::vector <MenuOption *>::iterator b = menuOptions.begin();
	std::vector <MenuOption *>::iterator e = menuOptions.end();
	const int startx = (backboard.position.width/2)-(longestTextLength/2);
	const int starty = (backboard.position.height/2)-((vFont->getHeight()*(menuOptions.size()-1))/2);
	for(int i=0;b!=e;++b,++i){ */
		/* There more than likely won't be any need to draw, but hey maybe sometime in the future
		the need might arise */
	/*
		(*b)->draw(work);
		switch(currentDrawState){
			case FadeIn : {
				break;
			}
			case FadeInText : {
				const unsigned int color = ((*b)->getState() == MenuOption::Selected) ? yellow : white;
				Bitmap::transBlender( 0, 0, 0, fadeAlpha );
				work->drawingMode( Bitmap::MODE_TRANS );
				switch((*b)->getType()) {
					case MenuOption::AdjustableOption : {
						const int triangleSize = 10;
						int cx = (backboard.position.x + startx) - 15;
						int cy = (int)(backboard.position.y + starty + i * fontHeight * 1.2 + fontHeight / 2 + 2);
						work->triangle( cx + triangleSize / 2, cy - triangleSize / 2, cx - triangleSize, cy, cx + triangleSize / 2, cy + triangleSize / 2, (*b)->getLeftAdjustColor() );
								
						cx = (backboard.position.x+startx + vFont->textLength((*b)->getText().c_str()))+15;
						work->triangle( cx - triangleSize / 2, cy - triangleSize / 2, cx + triangleSize, cy, cx - triangleSize / 2, cy + triangleSize / 2, (*b)->getRightAdjustColor() );
						break;
					}
					case MenuOption::Option:
					default:
						break;
				}
				vFont->printf( backboard.position.x + startx, int((backboard.position.y + starty) + i * fontHeight *1.2), color, *work, (*b)->getText(), 0 );
				work->drawingMode( Bitmap::MODE_SOLID );
			}
				break;
			case NoFade:
			default:
				// These menus are temporary, they will need to be changed
				const unsigned int color = ((*b)->getState() == MenuOption::Selected) ? yellow : white;
				switch((*b)->getType()) {
					case MenuOption::AdjustableOption : {
						const int triangleSize = 10;
						int cx = (backboard.position.x + startx) - 15;
						int cy = (int)(backboard.position.y + starty + i * fontHeight * 1.2 + fontHeight / 2 + 2);
						work->triangle( cx + triangleSize / 2, cy - triangleSize / 2, cx - triangleSize, cy, cx + triangleSize / 2, cy + triangleSize / 2, (*b)->getLeftAdjustColor() );
								
						cx = (backboard.position.x+startx + vFont->textLength((*b)->getText().c_str()))+15;
						work->triangle( cx - triangleSize / 2, cy - triangleSize / 2, cx + triangleSize, cy, cx - triangleSize / 2, cy + triangleSize / 2, (*b)->getRightAdjustColor() );
						break;
					}
					case MenuOption::Option:
					default:
						break;
				}
				vFont->printf( backboard.position.x + startx, int((backboard.position.y + starty) + i * fontHeight *1.2), color, *work, (*b)->getText(), 0 );
				break;
		}
	}
	*/
}


// Draw info text
void Menu::drawInfoText ( Bitmap *work ){
    if ( (*selectedOption)->getInfoText().empty() ) return;
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
                int w = vFont->textLength((*it).c_str()) + 10;
                if (w > maxWidth){
                    maxWidth = w;
                }
                height += vFont->getHeight();
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
                vFont->printf(area.position.x + 5, sy, white, *work, str, 0 );
                sy += vFont->getHeight();
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
	
	while(!backgrounds.empty()){
		delete backgrounds.front();
		backgrounds.pop();
	}
	//if(background)delete background;
	if ( vFont ){
		delete vFont;
	}
}
