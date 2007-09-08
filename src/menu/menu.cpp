#include "menu/menu.h"
#include "menu/menu_option.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "globals.h"
#include "init.h"

Bitmap *Menu::work = Bitmap::Screen;

Menu::Menu()
{
}


Menu::Menu(const Token &token)
{
}

Menu::Menu(const std::string &filename)
{
	load(filename);
}


void Menu::load(const Token &token)throw( LoadException )
{
}

void Menu::load(const std::string &filename)throw( LoadException )
{
}

void Menu::run() throw(ReturnException)
{
	
	Keyboard key;
	
	/* the game graphics are meant for 320x240 and will be stretched
	 * to fit the screen
	 */
	Bitmap work( 320, 240 );
	// Bitmap work( GFX_X, GFX_Y );
	Bitmap screen_buffer( 320, 240 );
	Global::speed_counter = 0;
	Global::second_counter = 0;
	int game_time = 100;
	bool done = false;
	bool endGame = false;
	
	if(menuOptions.empty())throw ReturnException();
	selectedOption = menuOptions.begin();
	
	while( !endGame )
	{
		while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){
	
			bool draw = false;
			key.poll();
	
			if ( Global::speed_counter > 0 ){
				
				// Logic
				
				Global::speed_counter = 0;
			}
			
			while ( Global::second_counter > 0 ){
				game_time--;
				Global::second_counter--;
				if ( game_time < 0 )
					game_time = 0;
			}
		
			if ( draw ){
				// Draw
			}
	
			while ( Global::speed_counter < 1 ){
				Util::rest( 1 );
				key.poll();
			}
	
			done |= key[ Keyboard::Key_ESC ];
		}
	}
}

void Menu::setBitmap(Bitmap *bmp)
{
	work = bmp;
}

Menu::~Menu()
{

}

