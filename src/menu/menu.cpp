#include "menu/menu.h"
#include "menu/menu_option.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "util/sound.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "globals.h"
#include "init.h"
#include "music.h"

// Options :O
#include "menu/option_adventure.h"
#include "menu/option_background.h"
#include "menu/option_credits.h"
#include "menu/option_key.h"
#include "menu/option_menu.h"
#include "menu/option_versus.h"

#include <queue>

Bitmap *Menu::work = 0;

static std::queue<std::string> lastPlayed;

static std::queue<MenuOption *> backgrounds;

const int yellow = Bitmap::makeColor( 255, 255, 0 );
const int white = Bitmap::makeColor( 255, 255, 255 );

RectArea::RectArea() : x(0), y(0), width(0), height(0)
{
}

RectArea::RectArea(int x, int y, int w, int h)
{
	this->x  = x;
	this->y  = y;
	this->width  = width;
	this->height  = height;
}

bool RectArea::empty()
{
	return (x==0 && y==0 && width==0 && height==0);
}

Menu::Menu() : music(""), background(0), position(), vFont(0), fontWidth(32), fontHeight(32), _name("")
{
	if(!work)work = new Bitmap(Bitmap::Screen->getWidth(), Bitmap::Screen->getHeight()); //Bitmap::Screen;
}

void Menu::load(Token *token)throw( LoadException )
{
	if ( *token != "menu" )
		throw LoadException("Not a menu");
	
	while ( token->hasTokens() )
	{
		try 
		{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" )
			{
				// Set menu name
				*tok >> _name;
			}
			else if ( *tok == "music" )
			{
				// Set music and push onto the stack
				*tok >> music;
				
				lastPlayed.push(music);
			} 
			else if ( *tok == "background" )
			{
				// Create new background and push onto the stack
				background = new OptionBg(tok);
				
				backgrounds.push(background);
			} 
			else if ( *tok == "position" )
			{
				// This handles the placement of the menu list
				*tok >> position.x >> position.y >> position.width >> position.height;
			} 
			else if ( *tok == "font" )
			{
				std::string temp;
				*tok >> temp >> fontWidth >> fontHeight;
				vFont = new FreeTypeFont(temp);
				vFont->setSize(fontWidth,fontHeight);
			}
			else if ( *tok == "menu" )
			{
				// Create a sub menu
				OptionMenu *temp = new OptionMenu(tok);
				menuOptions.push_back(temp);
			}
			else if ( *tok == "key" )
			{
				// Reconfigure a given key
				OptionKey *temp = new OptionKey(tok);
				menuOptions.push_back(temp);
			}
			else if ( *tok == "adventure" )
			{
				// Adventure mode
				OptionAdventure *temp = new OptionAdventure(tok);
				menuOptions.push_back(temp);
			}
			else if ( *tok == "versus" )
			{
				// Versus mode
				OptionVersus *temp = new OptionVersus(tok);
				menuOptions.push_back(temp);
			}
			else if ( *tok == "credits" )
			{
				// Credits mode
				OptionCredits *temp = new OptionCredits(tok);
				menuOptions.push_back(temp);
			}
			else if ( *tok == "back" || *tok == "quit" )
			{
				// Create a menu option ie options, controller config, adventure, versus, credits, etc
				
				//menuOptions.push_back();
			}
			else 
			{
				cout<<"Unhandled menu attribute: "<<endl;
				tok->print(" ");
			}
		} 
		catch ( const TokenException & ex )
		{
			// delete current;
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
		} 
		catch ( const LoadException & ex )
		{
			// delete current;
			throw ex;
		}
	}
	
	if(_name.empty())throw LoadException("No name set, the menu should have a name!");
	if(backgrounds.empty())throw LoadException("There should be at least one background in the entire menu!");
	if(position.empty())throw LoadException("The position for the menu list must be set!");
	
	if(!vFont)
	{
		std::string f = Util::getDataPath() + "/fonts/arial.ttf";
		vFont = new FreeTypeFont(f);
		vFont->setSize(fontWidth,fontHeight);
	}
	
	// Finally lets assign list order numering
	for(unsigned int i = 0; i < menuOptions.size();++i)
	{
		menuOptions[i]->setID(i);
	}
	
}

void Menu::load(const std::string &filename)throw( LoadException )
{
	// Must check for initial token, menu
	TokenReader tr( filename );

	// Token * current = tr.readToken();
	Token * token = tr.readToken();
	load(token);
}

void Menu::run() throw(ReturnException)
{
	
	Keyboard key;
	
	key.setDelay( Keyboard::Key_UP, 300 );
	key.setDelay( Keyboard::Key_DOWN, 300 );
	key.setDelay( Keyboard::Key_LEFT, 50 );
	key.setDelay( Keyboard::Key_RIGHT, 50 );
	
	Bitmap screen_buffer( 320, 240 );
	bool done = false;
	bool endGame = false;
	
	if(menuOptions.empty())throw ReturnException();
	selectedOption = menuOptions.begin();
	menuOptions.front()->setState(MenuOption::Selected);
	
	while( !endGame )
	{
		Global::speed_counter = 0;
		Global::second_counter = 0;
		int game_time = 100;
		
		if(music != "" && music != lastPlayed.front())
		{
			//Music::pause();
			//Music::fadeIn( 0.3 );
			//Music::loadSong( Util::getDataPath() + music );
			//Music::play();
		}
		while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){
	
			bool draw = false;
			key.poll();
	
			if ( Global::speed_counter > 0 )
			{
				draw = true;
				// Keys
				
				if ( key[ Keyboard::Key_UP ] )
				{
					(*selectedOption)->setState(MenuOption::Deselected);
					if(selectedOption > menuOptions.begin())selectedOption--;
					(*selectedOption)->setState(MenuOption::Selected);	
				}

				if ( key[ Keyboard::Key_DOWN ] )
				{
					(*selectedOption)->setState(MenuOption::Deselected);
					if(selectedOption < menuOptions.begin()+menuOptions.size()-1)selectedOption++;
					(*selectedOption)->setState(MenuOption::Selected);
				}
				
				// Logic
				background->logic();
				
				std::vector <MenuOption *>::iterator b = menuOptions.begin();
				std::vector <MenuOption *>::iterator e = menuOptions.end();
				for(;b!=e;++b)
				{
					(*b)->logic();
				}
				
				Global::speed_counter = 0;
			}
			
			while ( Global::second_counter > 0 )
			{
				game_time--;
				Global::second_counter--;
				if ( game_time < 0 )
					game_time = 0;
			}
		
			if ( draw )
			{
				// Draw
				background->draw(work);
				
				std::vector <MenuOption *>::iterator b = menuOptions.begin();
				std::vector <MenuOption *>::iterator e = menuOptions.end();
				for(int i=0;b!=e;++b,++i)
				{
					/* There more than likely won't be any need to draw, but hey maybe sometime in the future
					   the need might arise */
					(*b)->draw(work);
					
					// These menus are temporary, they will need to be changed
					const unsigned int color = ((*b)->getState() == MenuOption::Selected) ? yellow : white;
					vFont->printf( position.x, int(position.y + i * fontHeight *1.2), color, *work, (*b)->getText(), 0 );
				}
				
				// Finally render to screen
				work->BlitToScreen();
			}
	
			while ( Global::speed_counter < 1 )
			{
				Util::rest( 1 );
				key.poll();
			}
	
			endGame = done |= key[ Keyboard::Key_ESC ];
		}
		
		// do we got an option to run, lets do it
		if((*selectedOption)->getState() == MenuOption::Run)
		{
			(*selectedOption)->run(endGame);
			// Reset it's state
			(*selectedOption)->setState(MenuOption::Selected);
		}
		if(endGame)
		{
			//  pop out any backgrounds pushed onto the stack reseting it to the old one if applicable
			if(backgrounds.size() >= 2)
			{
				delete backgrounds.front();
				backgrounds.pop();
				background = backgrounds.front();
			}
			if(!lastPlayed.empty())
			{
				backgrounds.pop();
				//Music::pause();
				//Music::fadeIn( 0.3 );
				//Music::loadSong( Util::getDataPath() + backgrounds.front() );
				//Music::play();
				
			}
		}
	}
}


const std::string &Menu::getName()
{
	return _name;
}

void Menu::setBitmap(Bitmap *bmp)
{
	work = bmp;
}

Menu::~Menu()
{
	// cleanup
	std::vector <MenuOption *>::iterator b = menuOptions.begin();
	std::vector <MenuOption *>::iterator e = menuOptions.end();
	for(;b!=e;++b)
	{
		if((*b))delete (*b);
	}
	
	while(!backgrounds.empty())
	{
		delete backgrounds.front();
		backgrounds.pop();
	}
	if(vFont)delete vFont;
}

