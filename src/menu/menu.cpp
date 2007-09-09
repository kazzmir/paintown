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

#include <queue>

Bitmap *Menu::work = Bitmap::Screen;

static std::string lastPlayed = "";

static std::queue<MenuOption *> backgrounds;

Menu::Menu() : music("")
{
}

void Menu::load(Token *token)throw( LoadException )
{
	try 
	{
		if ( *token != "menu" )
			throw LoadException("Not a menu");

		while ( token->hasTokens() )
		{

			Token * tok;
			*token >> tok;
			if ( *tok == "music" )
			{
				// Set music
				*tok >> music;
			} 
			else if ( *tok == "background" )
			{
				// Create new background and push onto the stack
				//background = new obj() <-- D:
				
				backgrounds.push(background);
			}
			else if ( *tok == "menu" )
			{
				// Create a menu option ie options, controller config, adventure, versus, credits, etc
			}
			else 
			{
				cout<<"Unhandled menu attribute: "<<endl;
				tok->print(" ");
			}
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
	
	if(backgrounds.empty())throw LoadException("There should be at least one background in the entire menu!");
	
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
	
	Bitmap screen_buffer( 320, 240 );
	bool done = false;
	bool endGame = false;
	
	if(menuOptions.empty())throw ReturnException();
	selectedOption = menuOptions.begin();
	
	while( !endGame )
	{
		Global::speed_counter = 0;
		Global::second_counter = 0;
		int game_time = 100;
		
		if(music != "" && music != lastPlayed)
		{
			Music::pause();
			Music::fadeIn( 0.3 );
			Music::loadSong( Util::getDataPath() + music );
			Music::play();
			lastPlayed = music;
		}
		while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){
	
			bool draw = false;
			key.poll();
	
			if ( Global::speed_counter > 0 )
			{
				draw = true;
				// Keys
				
				// Logic
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
				std::vector <MenuOption *>::iterator b = menuOptions.begin();
				std::vector <MenuOption *>::iterator e = menuOptions.end();
				for(;b!=e;++b)
				{
					(*b)->draw(work);
				}
			}
	
			while ( Global::speed_counter < 1 )
			{
				Util::rest( 1 );
				key.poll();
			}
	
			done |= key[ Keyboard::Key_ESC ];
		}
		
		// do we got an option to run, lets do it
		if((*selectedOption)->getState() == MenuOption::Run)
		{
			(*selectedOption)->run(endGame);
			// Reset it's state
			(*selectedOption)->setState(MenuOption::Selected);
			//  pop out any backgrounds pushed onto the stack reseting it to the old one if applicable
			if(backgrounds.size() >= 2)
			{
				delete backgrounds.front();
				backgrounds.pop();
				background = backgrounds.front();
			}
		}
	}
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
}

