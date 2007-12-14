#include "menu/menu.h"
#include "menu/menu_global.h"
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

#include "menu/optionfactory.h"
#include "menu/option_background.h"

#include "menu/actionfactory.h"

#include <queue>
#include <map>

Bitmap *Menu::work = 0;

static std::priority_queue<std::string> lastPlayed;

static std::queue<MenuOption *> backgrounds;

const int yellow = Bitmap::makeColor( 255, 255, 0 );
const int white = Bitmap::makeColor( 255, 255, 255 );

static std::map<std::string, Menu *> _menus;

static void addMenu(Menu *m) throw( LoadException )
{
	std::map<std::string, Menu *>::iterator i = _menus.find(m->getName());
	if(i==_menus.end())
	{
		_menus[m->getName()] = m;
	}
	else throw LoadException("A menu by the name of \""+m->getName()+"\" already exists!"); 
}

RectArea::RectArea() : x(0), y(0), width(0), height(0), body(0), bodyAlpha(0), border(0), borderAlpha(0)
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

Menu::Menu() : music(""), background(0), position(), vFont(0), fontWidth(24), fontHeight(24), _menuflags(0),longestTextLength(0), _name("")
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
				//lastPlayed.push(music);
			} 
			else if ( *tok == "background" )
			{
				// Create new background and push onto the stack
				background = new OptionBg(tok);
				
				backgrounds.push(background);
			} 
			else if ( *tok == "position" )
			{
				// This handles the placement of the menu list and surrounding box
				*tok >> position.x >> position.y >> position.width >> position.height;
			} 
			else if ( *tok == "position-body" )
			{
				// This handles the body color of the menu box
				int r,g,b;
				*tok >> r >> g >> b >> position.bodyAlpha;
				position.body = Bitmap::makeColor(r,g,b);
			} 
			else if ( *tok == "position-border" )
			{
				// This handles the border color of the menu box
				int r,g,b;
				*tok >> r >> g >> b >> position.borderAlpha;
				position.border = Bitmap::makeColor(r,g,b);
			} 
			else if ( *tok == "font" )
			{
				std::string temp;
				*tok >> temp >> fontWidth >> fontHeight;
				vFont = new FreeTypeFont(Util::getDataPath() + temp);
				vFont->setSize(fontWidth,fontHeight);
			}
			else if( *tok == "option" )
			{
				MenuOption *temp = getOption(tok);
				if(temp)menuOptions.push_back(temp);
			}
			else if( *tok == "action" )
			{
				ActionAct(tok);
			}
			else 
			{
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
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
	addMenu(this);
	
	if(!vFont)
	{
		std::string f = Util::getDataPath() + "/fonts/arial.ttf";
		vFont = new FreeTypeFont(f);
		vFont->setSize(fontWidth,fontHeight);
	}
	
	// Finally lets assign list order numering and some other stuff
	// First length
	longestTextLength = vFont->textLength(menuOptions[0]->getText().c_str());
	for(unsigned int i = 0; i < menuOptions.size();++i)
	{
		menuOptions[i]->setID(i);
		
		checkTextLength(menuOptions[i]);
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

useflags Menu::run()
{
	
	Keyboard key;
	
	key.setDelay( Keyboard::Key_UP, 300 );
	key.setDelay( Keyboard::Key_DOWN, 300 );
	key.setDelay( Keyboard::Key_ENTER, 300 );
	key.setDelay( Keyboard::Key_LEFT, 50 );
	key.setDelay( Keyboard::Key_RIGHT, 50 );
	
	Bitmap screen_buffer( 320, 240 );
	bool done = false;
	bool endGame = false;
	
	if(menuOptions.empty())return _menuflags;
	selectedOption = menuOptions.begin();
	menuOptions.front()->setState(MenuOption::Selected);
	
	if(!music.empty())
	{
		setMusic(music);
	}
	
	while( !endGame )
	{
		Global::speed_counter = 0;
		Global::second_counter = 0;
		int game_time = 100;
		
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
				
				if ( key[ Keyboard::Key_LEFT ] )
				{
					(*selectedOption)->leftKey();
				}
				
				if ( key[ Keyboard::Key_RIGHT ] )
				{
					(*selectedOption)->rightKey();
				}
				
				if( key[ Keyboard::Key_ENTER ] )
				{
					(*selectedOption)->setState(MenuOption::Run);
				}
				
				// Logic
				if(backgrounds.front())backgrounds.front()->logic();
				
				std::vector <MenuOption *>::iterator b = menuOptions.begin();
				std::vector <MenuOption *>::iterator e = menuOptions.end();
				for(;b!=e;++b)
				{
					(*b)->logic();
					
					// Recalculate placement
					checkTextLength((*b));
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
				if(backgrounds.front())backgrounds.front()->draw(work);
				
				if(position.bodyAlpha < 255)
				{
					Bitmap::transBlender( 0, 0, 0, position.bodyAlpha );
					work->drawingMode( Bitmap::MODE_TRANS );
					work->rectangleFill( position.x, position.y, position.getX2(), position.getY2(), position.body );
				}
				else 
				{
					work->drawingMode( Bitmap::MODE_SOLID );
					work->rectangleFill( position.x, position.y, position.getX2(), position.getY2(), position.body );
				}
				if(position.borderAlpha < 255)
				{
					Bitmap::transBlender( 0, 0, 0, position.bodyAlpha );
					work->drawingMode( Bitmap::MODE_TRANS );
					work->rectangle(  position.x, position.y, position.getX2(), position.getY2(), position.border );
				}
				else
				{
					work->drawingMode( Bitmap::MODE_SOLID );
					work->rectangle(  position.x, position.y, position.getX2(), position.getY2(), position.border );
				}
				work->drawingMode( Bitmap::MODE_SOLID );
				
				std::vector <MenuOption *>::iterator b = menuOptions.begin();
				std::vector <MenuOption *>::iterator e = menuOptions.end();
				const int startx = (position.width/2)-(longestTextLength/2);
				const int starty = (position.height/2)-((vFont->getHeight()*(menuOptions.size()-1))/2);
				for(int i=0;b!=e;++b,++i)
				{
					/* There more than likely won't be any need to draw, but hey maybe sometime in the future
					   the need might arise */
					(*b)->draw(work);
					
					// These menus are temporary, they will need to be changed
					const unsigned int color = ((*b)->getState() == MenuOption::Selected) ? yellow : white;
					switch((*b)->getType())
					{
						case MenuOption::adjustableOption:
							{
								//Change into triangles later
								work->circleFill((position.x+startx)-15, int((position.y + starty) + i * fontHeight *1.2) + (fontHeight/2) + 2,fontHeight/3,(*b)->getLeftAdjustColor());
								work->circleFill((position.x+startx + vFont->textLength((*b)->getText().c_str()))+15, int((position.y + starty) + i * fontHeight *1.2) + (fontHeight/2) + 2,fontHeight/3,(*b)->getRightAdjustColor());
							}
							break;
						case MenuOption::option:
						default:
							break;
					}
					vFont->printf( position.x + startx, int((position.y + starty) + i * fontHeight *1.2), color, *work, (*b)->getText(), 0 );
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
		
		// Stupid keyboard
		key.wait();
		// do we got an option to run, lets do it
		if((*selectedOption)->getState() == MenuOption::Run)
		{
			(*selectedOption)->run(endGame);
			// Reset it's state
			(*selectedOption)->setState(MenuOption::Selected);
		}
		if(!music.empty() && !lastPlayed.empty())
		{
			if(lastPlayed.top() != music)
			{
				lastPlayed.pop();
				Music::pause();
				Music::loadSong( Util::getDataPath() + lastPlayed.top() );
				Music::play();
			}
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
			
			// Deselect selected entry
			(*selectedOption)->setState(MenuOption::Deselected);
		}
	}
	
	return _menuflags;
}


Menu *Menu::getMenu(const std::string &name)
{
	std::map<std::string, Menu *>::iterator i = _menus.find(name);
	if(i!=_menus.end())return i->second;
	return 0;
}

const std::string &Menu::getName()
{
	return _name;
}

void Menu::setBitmap(Bitmap *bmp)
{
	work = bmp;
}

void Menu::setMusic(const std::string &file)
{
	lastPlayed.push(file);
	if(Music::loadSong( Util::getDataPath() + file ))
	{
		Music::pause();
		Music::play();
	}
}

//! Set longest length
void Menu::checkTextLength(MenuOption *opt)
{
	// Set longest text length depending on type
	switch(opt->getType())
	{
		case MenuOption::adjustableOption:
			{
				int len = vFont->textLength(opt->getText().c_str()) + 10;
				if(len > longestTextLength)longestTextLength = len;
			}
			break;
		case MenuOption::option:
		default:
			{
				int len = vFont->textLength(opt->getText().c_str());
				if(len > longestTextLength)longestTextLength = len;
			}
			break;
	}
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
	//if(background)delete background;
	if(vFont)delete vFont;
}

