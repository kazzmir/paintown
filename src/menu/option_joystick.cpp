#include "util/bitmap.h"
#include "util/font.h"
#include "menu/option_joystick.h"
#include "util/token.h"
#include "configuration.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "input/input-manager.h"
#include "globals.h"
#include "init.h"
#include "util/keyboard.h"
#include "util/funcs.h"
#include <sstream>

#include "gui/box.h"

using namespace std;

static OptionJoystick::JoystickType convertToKey(const std::string &k){
    std::string temp = k;
    for(unsigned int i=0;i<temp.length();i++){
        temp[i] = tolower(temp[i]);
    }

    if (temp == "up") return OptionJoystick::Up;
    if (temp == "down") return OptionJoystick::Down;
    if (temp == "left") return OptionJoystick::Left;
    if (temp == "right") return OptionJoystick::Right;
    if (temp == "jump") return OptionJoystick::Jump;
    if (temp == "attack1") return OptionJoystick::Attack1;
    if (temp == "attack2") return OptionJoystick::Attack2;
    if (temp == "attack3") return OptionJoystick::Attack3;
    if (temp == "attack4") return OptionJoystick::Attack4;
    if (temp == "attack5") return OptionJoystick::Attack5;
    if (temp == "attack6") return OptionJoystick::Attack6;

    return OptionJoystick::Invalidkey;
}

static Configuration::JoystickInput getKey(int player, OptionJoystick::JoystickType k){
    switch(k){
        case OptionJoystick::Up:
            return Configuration::config(player).getJoystickUp();
        case OptionJoystick::Down:
            return Configuration::config(player).getJoystickDown();
        case OptionJoystick::Left:
            return Configuration::config(player).getJoystickLeft();
        case OptionJoystick::Right:
            return Configuration::config(player).getJoystickRight();
        case OptionJoystick::Jump:
            return Configuration::config(player).getJoystickJump();
        case OptionJoystick::Attack1:
            return Configuration::config(player).getJoystickAttack1();
        case OptionJoystick::Attack2:
            return Configuration::config(player).getJoystickAttack2();
        case OptionJoystick::Attack3:
            return Configuration::config(player).getJoystickAttack3();
        case OptionJoystick::Attack4:
        case OptionJoystick::Attack5:
        case OptionJoystick::Attack6:
        default:
            break;
    }

    return Configuration::config(player).getJoystickUp();
}

static void setKey(int player, OptionJoystick::JoystickType k, Configuration::JoystickInput key){
    switch(k){
        case OptionJoystick::Up:
            Configuration::config(player).setJoystickUp( key );
            break;
        case OptionJoystick::Down:
            Configuration::config(player).setJoystickDown( key );
            break;
        case OptionJoystick::Left:
            Configuration::config(player).setJoystickLeft( key );
            break;
        case OptionJoystick::Right:
            Configuration::config(player).setJoystickRight( key );
            break;
        case OptionJoystick::Jump:
            Configuration::config(player).setJoystickJump( key );
            break;
        case OptionJoystick::Attack1:
            Configuration::config(player).setJoystickAttack1( key );
            break;
        case OptionJoystick::Attack2:
            Configuration::config(player).setJoystickAttack2( key );
            break;
        case OptionJoystick::Attack3:
            Configuration::config(player).setJoystickAttack3( key );
            break;
        case OptionJoystick::Attack4:
        case OptionJoystick::Attack5:
        case OptionJoystick::Attack6:
        default:
            break;
    }
}

static Configuration::JoystickInput readJoystick() throw (ReturnException){
    vector<Joystick::Key> keys;
    keys.push_back(Joystick::Up);
    keys.push_back(Joystick::Down);
    keys.push_back(Joystick::Left);
    keys.push_back(Joystick::Right);
    keys.push_back(Joystick::Button1);
    keys.push_back(Joystick::Button2);
    keys.push_back(Joystick::Button3);
    keys.push_back(Joystick::Button4);

    InputMap<Joystick::Key> input;
    for (vector<Joystick::Key>::iterator it = keys.begin(); it != keys.end(); it++){
        input.set(*it, 0, true, *it);
    }
    input.set(Keyboard::Key_ESC, 0, true, Joystick::Invalid);

    while (true){
        InputManager::poll();
        InputMap<Joystick::Key>::Output output = InputManager::getMap(input);
        for (vector<Joystick::Key>::iterator it = keys.begin(); it != keys.end(); it++){
            Joystick::Key key = *it;
            if (output[key]){
                return key;
            }
        }
        if (output[Joystick::Invalid]){
            InputManager::waitForRelease(input, Joystick::Invalid);
            throw ReturnException();
        }
        Util::rest(1);
    }
    return Joystick::Up;
}

OptionJoystick::OptionJoystick(Token *token) throw (LoadException):
MenuOption(token, Event),
name(""),
player(-1),
type(Invalidkey),
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
    if(type == Invalidkey)throw LoadException("Invalid key, should be up, down, left, right, up, down, jump, attack1-6!");
    if(player == -1)throw LoadException("Player not specified in key configuration");

    ostringstream out;
    out << name << ": " << Joystick::keyToName(getKey(player, type));
    setText(out.str());
}

OptionJoystick::~OptionJoystick(){
    // Nothing
}

void OptionJoystick::logic(){
    /*
    char temp[255];
    sprintf( temp, "%s: %s", name.c_str(), Joystick::keyToName(getKey(player,type)));
    setText(std::string(temp));
    */
}

void OptionJoystick::run(bool &endGame) throw (ReturnException) {
    //int x, y, width, height;
    const Font &vFont = Font::getFont(Menu::getFont(),Menu::getFontWidth(),Menu::getFontHeight());
    const char * message = "Press a joystick button!";
    const int width = vFont.textLength(message) + 10;
    const int height = vFont.getHeight() + 10;
    const int x = (getParent()->getWork()->getWidth()/2) - (width/2);
    const int y = (getParent()->getWork()->getHeight()/2) - (height/2);
    Box dialog;
    dialog.position.x = 0;
    dialog.position.y = 0;
    dialog.position.width = vFont.textLength(message) + 10;
    dialog.position.height = vFont.getHeight() + 10;
    dialog.position.radius = 0;
    dialog.position.body = Bitmap::makeColor(0,0,0);
    dialog.position.bodyAlpha = 200;
    dialog.position.border = Bitmap::makeColor(255,255,255);
    dialog.position.borderAlpha = 255;
    Bitmap temp = Bitmap::temporaryBitmap(width,height);
    dialog.render(&temp);
    vFont.printf( 5, 5, Bitmap::makeColor(255,255,255), temp, message, -1);
    temp.BlitToScreen(x,y);
    
    setKey(player, type, readJoystick());

    ostringstream out;
    out << name << ": " << Joystick::keyToName(getKey(player, type));
    setText(out.str());

    /*
    Keyboard key;
    keyCode = readKey(key);
    setKey(player,type, keyCode);
    */
}
