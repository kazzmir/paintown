#include "util/bitmap.h"
#include "util/font.h"
#include "menu/option_key.h"
#include "util/token.h"
#include "configuration.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include "util/keyboard.h"
#include "util/funcs.h"

#include "gui/box.h"

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

OptionKey::OptionKey(Token *token) throw (LoadException): MenuOption(token, Event), name(""), player(-1), type(invalidkey), keyCode(0)
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

void OptionKey::run(bool &endGame)
{
	//int x, y, width, height;
	const Font &vFont = Font::getFont(Util::getDataPath() + Menu::getFont(),Menu::getFontWidth(),Menu::getFontHeight());
	Box dialog;
	dialog.position.width = vFont.textLength("Press a Key!") + 10;
	dialog.position.height = vFont.getHeight() + 10;
	dialog.position.x = (Menu::getBackground()->getWidth()/2) - (dialog.position.width/2);
	dialog.position.y = (Menu::getBackground()->getHeight()/2) - (dialog.position.height/2);
	dialog.position.radius = 10;
	dialog.position.body = Bitmap::makeColor(0,0,0);
	dialog.position.bodyAlpha = 200;
	dialog.position.border = Bitmap::makeColor(255,255,255);
	dialog.position.borderAlpha = 255;
	dialog.render(Menu::work);
	vFont.printf( dialog.position.x+5, dialog.position.y+5, Bitmap::makeColor(255,255,255), *Menu::work, "Press a Key!", -1);
	Menu::work->BlitToScreen();
	Keyboard key;
	keyCode = readKey( key );
	setKey(player,type, keyCode);
}

