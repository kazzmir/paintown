#include "util/bitmap.h"
#include "util/font.h"
#include "menu/option_joystick.h"
#include "util/token.h"
#include "configuration.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include "util/keyboard.h"
#include "util/funcs.h"

#include "gui/box.h"

using namespace std;

static OptionJoystick::JoystickType convertToKey(const std::string &k){
	std::string temp = k;
	for(unsigned int i=0;i<temp.length();i++){
		temp[i] = tolower(temp[i]);
	}

	if(temp == "up")return OptionJoystick::up;
	if(temp == "down")return OptionJoystick::down;
	if(temp == "left")return OptionJoystick::left;
	if(temp == "right")return OptionJoystick::right;
	if(temp == "jump")return OptionJoystick::jump;
	if(temp == "attack1")return OptionJoystick::attack1;
	if(temp == "attack2")return OptionJoystick::attack2;
	if(temp == "attack3")return OptionJoystick::attack3;
	if(temp == "attack4")return OptionJoystick::attack4;
	if(temp == "attack5")return OptionJoystick::attack5;
	if(temp == "attack6")return OptionJoystick::attack6;
	
	return OptionJoystick::invalidkey;
}

static int getKey(int player, OptionJoystick::JoystickType k)
{
	switch(k){
		case OptionJoystick::up:
			return Configuration::config(player).getUp();
		case OptionJoystick::down:
			return Configuration::config(player).getDown();
		case OptionJoystick::left:
			return Configuration::config(player).getLeft();
		case OptionJoystick::right:
			return Configuration::config(player).getRight();
		case OptionJoystick::jump:
			return Configuration::config(player).getJump();
		case OptionJoystick::attack1:
			return Configuration::config(player).getAttack1();
		case OptionJoystick::attack2:
			return Configuration::config(player).getAttack2();
		case OptionJoystick::attack3:
			return Configuration::config(player).getAttack3();
		case OptionJoystick::attack4:
		case OptionJoystick::attack5:
		case OptionJoystick::attack6:
		default:
			break;
	}
	
	return 0;
}

static void setKey(int player, OptionJoystick::JoystickType k, int key){
	switch(k){
		case OptionJoystick::up:
			Configuration::config(player).setUp( key );
			break;
        case OptionJoystick::down:
			Configuration::config(player).setDown( key );
			break;
        case OptionJoystick::left:
			Configuration::config(player).setLeft( key );
			break;
        case OptionJoystick::right:
			Configuration::config(player).setRight( key );
			break;
        case OptionJoystick::jump:
			Configuration::config(player).setJump( key );
			break;
        case OptionJoystick::attack1:
			Configuration::config(player).setAttack1( key );
			break;
        case OptionJoystick::attack2:
			Configuration::config(player).setAttack2( key );
			break;
        case OptionJoystick::attack3:
			Configuration::config(player).setAttack3( key );
			break;
		case OptionJoystick::attack4:
		case OptionJoystick::attack5:
		case OptionJoystick::attack6:
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

OptionJoystick::OptionJoystick(Token *token) throw (LoadException):
MenuOption(token, Event),
name(""),
player(-1),
type(invalidkey),
keyCode(0){
	if (*token != "joystick"){
		throw LoadException("Not joystick option");
    }
	
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" ){
				*tok >> name;
			} else if ( *tok == "player" ) {
				*tok >> player;
			} else if ( *tok == "type" ) {
				std::string temp;
				*tok >> temp;
				type = convertToKey(temp);
			} else {
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
				tok->print(" ");
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
	
	if(name.empty())throw LoadException("No name set, this option should have a name!");
	if(type == invalidkey)throw LoadException("Invalid key, should be up, down, left, right, up, down, jump, attack1-6!");
	if(player == -1)throw LoadException("Player not specified in key configuration");
	
        char temp[255];
	sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
	setText(std::string(temp));
}

OptionJoystick::~OptionJoystick()
{
	// Nothing
}

void OptionJoystick::logic(){
	char temp[255];
	sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
	setText(std::string(temp));
}

void OptionJoystick::run(bool &endGame) throw (ReturnException) {
	//int x, y, width, height;
	const Font &vFont = Font::getFont(Menu::getFont(),Menu::getFontWidth(),Menu::getFontHeight());
	const int width = vFont.textLength("Press a Key!") + 10;
	const int height = vFont.getHeight() + 10;
	const int x = (getParent()->getWork()->getWidth()/2) - (width/2);
	const int y = (getParent()->getWork()->getHeight()/2) - (height/2);
	Box dialog;
	dialog.position.x = 0;
	dialog.position.y = 0;
	dialog.position.width = vFont.textLength("Press a Key!") + 10;
	dialog.position.height = vFont.getHeight() + 10;
	dialog.position.radius = 0;
	dialog.position.body = Bitmap::makeColor(0,0,0);
	dialog.position.bodyAlpha = 200;
	dialog.position.border = Bitmap::makeColor(255,255,255);
	dialog.position.borderAlpha = 255;
	Bitmap temp = Bitmap::temporaryBitmap(width,height);
	dialog.render(&temp);
	vFont.printf( 5, 5, Bitmap::makeColor(255,255,255), temp, "Press a Key!", -1);
	temp.BlitToScreen(x,y);
	Keyboard key;
	keyCode = readKey( key );
	setKey(player,type, keyCode);
}

