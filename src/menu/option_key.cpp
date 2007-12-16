#include "menu/option_key.h"
#include "util/token.h"
#include "configuration.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include "util/keyboard.h"
#include "util/bitmap.h"

OptionKey::keyType convertToKey(const std::string &k)
{
	std::string temp = k;
	for(unsigned int i=0;i<temp.length();i++)
	{
		temp[i] = tolower(temp[i]);
	}
	if(temp == "up")return OptionKey::up;
	if(temp == "down")return OptionKey::down;
	if(temp == "left")return OptionKey::left;
	if(temp == "right")return OptionKey::right;
	if(temp == "jump")return OptionKey::jump;
	if(temp == "attack1")return OptionKey::attack1;
	if(temp == "attack2")return OptionKey::attack2;
	if(temp == "attack3")return OptionKey::attack3;
	if(temp == "attack4")return OptionKey::attack4;
	if(temp == "attack5")return OptionKey::attack5;
	if(temp == "attack6")return OptionKey::attack6;
	
	return OptionKey::invalidkey;
}

int getKey(int player, OptionKey::keyType k)
{
	switch(k)
	{
		case OptionKey::up:
			return Configuration::config( player ).getUp();
			break;
		case OptionKey::down:
			return Configuration::config( player ).getDown();
			break;
		case OptionKey::left:
			return Configuration::config( player ).getLeft();
			break;
		case OptionKey::right:
			return Configuration::config( player ).getRight();
			break;
		case OptionKey::jump:
			return Configuration::config( player ).getJump();
			break;
		case OptionKey::attack1:
			return Configuration::config( player ).getAttack1();
			break;
		case OptionKey::attack2:
			return Configuration::config( player ).getAttack2();
			break;
		case OptionKey::attack3:
			return Configuration::config( player ).getAttack3();
			break;
		case OptionKey::attack4:
		case OptionKey::attack5:
		case OptionKey::attack6:
		default:
			break;
	}
	
	return 0;
}

void setKey(int player, OptionKey::keyType k, int key)
{
	switch(k)
	{
		case OptionKey::up:
			Configuration::config( player ).setUp( key );
			break;
		case OptionKey::down:
			Configuration::config( player ).setDown( key );
			break;
		case OptionKey::left:
			Configuration::config( player ).setLeft( key );
			break;
		case OptionKey::right:
			Configuration::config( player ).setRight( key );
			break;
		case OptionKey::jump:
			Configuration::config( player ).setJump( key );
			break;
		case OptionKey::attack1:
			Configuration::config( player ).setAttack1( key );
			break;
		case OptionKey::attack2:
			Configuration::config( player ).setAttack2( key );
			break;
		case OptionKey::attack3:
			Configuration::config( player ).setAttack3( key );
			break;
		case OptionKey::attack4:
		case OptionKey::attack5:
		case OptionKey::attack6:
		default:
			break;
	}
}

static int readKey( Keyboard & key ){
	key.wait();
	key.clear();
	int k = key.readKey();
	key.wait();
	return k;
}

OptionKey::OptionKey(Token *token)throw( LoadException ) : MenuOption(event), name(""), player(-1), type(invalidkey), keyCode(0)
{
	if ( *token != "key" )
		throw LoadException("Not key option");
	
	while ( token->hasTokens() )
	{
		try 
		{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" )
			{
				*tok >> name;
			}
			else if ( *tok == "player" )
			{
				*tok >> player;
				player -= 1;
			}
			else if ( *tok == "type" )
			{
				std::string temp;
				*tok >> temp;
				type = convertToKey(temp);
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
	
	if(name.empty())throw LoadException("No name set, this option should have a name!");
	if(type == invalidkey)throw LoadException("Invalid key, should be up, down, left, right, up, down, jump, attack1-6!");
	if(player == -1)throw LoadException("Player not specified in key configuration");
}

OptionKey::~OptionKey()
{
	// Nothing
}

void OptionKey::logic()
{
	char temp[255];
	sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
	setText(std::string(temp));
}

void OptionKey::draw(Bitmap *work)
{
}

void OptionKey::run(bool &endGame)
{
	int x, y, width, height;
	width = Menu::getFont()->textLength("Press a Key!") + 10;
	height = Menu::getFont()->getHeight() + 10;
	x = 320 - (width/2);
	y = 240 - (height/2);
	Bitmap::transBlender( 0, 0, 0, 200 );
	Bitmap::Screen->drawingMode( Bitmap::MODE_TRANS );
	Bitmap::Screen->rectangleFill( x, y, x+width, y+height, Bitmap::makeColor(0,0,0) );
	Bitmap::Screen->drawingMode( Bitmap::MODE_SOLID );
	Bitmap::Screen->rectangle( x, y, x+width, y+height, Bitmap::makeColor(255,255,255) );
	Menu::getFont()->printf( x+5, y+5, Bitmap::makeColor(255,255,255), *Bitmap::Screen, "Press a Key!", -1);
	Keyboard key;
	keyCode = readKey( key );
	setKey(player,type, keyCode);
}

