#include "configuration.h"
#include "util/input/keyboard.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/file-system.h"
#include "util/input/input.h"
#include "globals.h"
#include "util/debug.h"
#include "paintown-engine/object/animation.h"
#include "paintown-engine/object/object.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>

#include <map>

#ifdef USE_SDL
#if SDL_VERSION_ATLEAST(1, 3, 0)
static const std::string INPUT_TYPE = "SDL1.3";
#else
static const std::string INPUT_TYPE = "SDL";
#endif
#endif
#ifdef USE_ALLEGRO
static const std::string INPUT_TYPE = "Allegro";
#endif
#ifdef USE_ALLEGRO5
static const std::string INPUT_TYPE = "Allegro5";
#endif

bool Configuration::save = true;
Util::ReferenceCount<Token> Configuration::data;

/* text that appears in the config file */
#define define_config(n,str) static const char * config_##n = str
define_config(attack1, "attack1");
define_config(attack2, "attack2");
define_config(attack3, "attack3");
define_config(attack4, "attack4");
define_config(attack5, "attack5");
define_config(attack6, "attack6");
define_config(configuration, "configuration");
define_config(cooperative, "cooperative");
define_config(current_game, "current-game");
define_config(down, "down");
define_config(free_for_all, "free-for-all");
define_config(fullscreen, "fullscreen");
define_config(game_speed, "game-speed");
define_config(invincible, "invincible");
define_config(jump, "jump");
define_config(quit, "quit");
define_config(keyboard_configuration, "keyboard-configuration");
define_config(input, "input");
define_config(joystick_configuration, "joystick-configuration");
define_config(left, "left");
define_config(lives, "lives");
define_config(menu_font, "menu-font");
define_config(menu_font_width, "menu-font-width");
define_config(menu_font_height, "menu-font-height");
define_config(npc_buddies, "npc-buddies");
define_config(number, "number");
define_config(play_mode, "play-mode");
define_config(right, "right");
define_config(screen_size, "screen-size");
define_config(sound, "sound");
define_config(music, "music");
define_config(up, "up");
define_config(quality_filter, "quality-filter");
define_config(fps, "frames-per-second");
define_config(language, "language");
define_config(mugen_motif, "mugen-motif");
/* version of the game: 3.3, 3.4, 4.24 */
define_config(version, "version");
#undef def_config

using namespace std;

static const int InvalidKey = 0;
static const Configuration::JoystickInput InvalidJoystick = Joystick::Invalid;
    
Util::ReferenceCount<Configuration> Configuration::rootProperty;

/* don't save the configuration while loading it */
class Disable{
public:
    Disable(){
        last = Configuration::getSave();
        Configuration::disableSave();
    }

    ~Disable(){
        Configuration::setSave(last);
    }

    bool last;
};

Configuration Configuration::defaultPlayer1Keys(){
    Disable disable;
    Configuration config;
    config.setRight(Keyboard::Key_RIGHT);
    config.setLeft(Keyboard::Key_LEFT);
    config.setUp(Keyboard::Key_UP);
    config.setDown(Keyboard::Key_DOWN);
    config.setAttack1(Keyboard::Key_A);
    config.setAttack2(Keyboard::Key_S);
    config.setAttack3(Keyboard::Key_D);
    config.setAttack4(Keyboard::Key_Z);
    config.setAttack5(Keyboard::Key_X);
    config.setAttack6(Keyboard::Key_C);
    config.setJump(Keyboard::Key_SPACE);

    /* these mappings should agree with input-manager.cpp:convertJoystickKey,
     * but otherwise they are completely arbitrary
     */
    config.setJoystickRight(Joystick::Right);
    config.setJoystickLeft(Joystick::Left);
    config.setJoystickUp(Joystick::Up);
    config.setJoystickDown(Joystick::Down);
    config.setJoystickAttack1(Joystick::Button1);
    config.setJoystickAttack2(Joystick::Button2);
    config.setJoystickAttack3(Joystick::Button3);
    config.setJoystickAttack4(Joystick::Button4);
    config.setJoystickAttack5(Joystick::Button5);
    config.setJoystickAttack6(Joystick::Button6);
    config.setJoystickJump(Joystick::Button4);
    config.setJoystickQuit(Joystick::Quit);

    return config;
}

Configuration Configuration::defaultPlayer2Keys(){
    Disable disable;
    Configuration config;
    config.setRight(Keyboard::Key_L);
    config.setLeft(Keyboard::Key_J);
    config.setUp(Keyboard::Key_I);
    config.setDown(Keyboard::Key_COMMA);
    config.setAttack1(Keyboard::Key_R);
    config.setAttack2(Keyboard::Key_T);
    config.setAttack3(Keyboard::Key_Y);
    config.setAttack4(Keyboard::Key_F);
    config.setAttack5(Keyboard::Key_G);
    config.setAttack6(Keyboard::Key_H);
    config.setJump(Keyboard::Key_B);

    /* these mappings should agree with input-manager.cpp:convertJoystickKey,
     * but otherwise they are completely arbitrary
     */
    config.setJoystickRight(Joystick::Right);
    config.setJoystickLeft(Joystick::Left);
    config.setJoystickUp(Joystick::Up);
    config.setJoystickDown(Joystick::Down);
    config.setJoystickAttack1(Joystick::Button1);
    config.setJoystickAttack2(Joystick::Button2);
    config.setJoystickAttack3(Joystick::Button3);
    config.setJoystickAttack4(Joystick::Button4);
    config.setJoystickAttack5(Joystick::Button5);
    config.setJoystickAttack6(Joystick::Button6);
    config.setJoystickJump(Joystick::Button4);
    config.setJoystickQuit(Joystick::Quit);

    return config;
}

static map< int, Configuration * > configs;

Configuration & Configuration::config(int set){
    if (configs[set] == NULL ){
        configs[set] = new Configuration();
        switch (set){
            case 0: {
                *configs[set] = defaultPlayer1Keys();
                break;
            }
            case 1: {
                *configs[set] = defaultPlayer2Keys();
                break;
            }
        }
    }
    return *configs[set];
}

void Configuration::setDefaultKeys(int x){
    switch (x){
        case 0: {
            config(x) = defaultPlayer1Keys();
            break;
        }
        case 1: {
            config(x) = defaultPlayer2Keys();
            break;
        }
    }
}

/* hopefully this is only used right before setting all the values
 * since the key mappings are bogus in here'
 */
Configuration::Configuration():
right(InvalidKey),
left(InvalidKey),
up(InvalidKey),
down(InvalidKey),
attack1(InvalidKey),
attack2(InvalidKey),
attack3(InvalidKey),
attack4(InvalidKey),
attack5(InvalidKey),
attack6(InvalidKey),
jump(InvalidKey),
joystick_right(Joystick::Right),
joystick_left(Joystick::Left),
joystick_up(Joystick::Up),
joystick_down(Joystick::Down),
joystick_attack1(Joystick::Button1),
joystick_attack2(Joystick::Button2),
joystick_attack3(Joystick::Button3),
joystick_attack4(Joystick::Button4),
joystick_attack5(Joystick::Button5),
joystick_attack6(Joystick::Button6),
joystick_jump(Joystick::Button4),
joystick_quit(Joystick::Quit){
}

Configuration::Configuration(const Configuration & config):
right(config.getRight()),
left(config.getLeft()),
up(config.getUp()),
down(config.getDown()),
attack1(config.getAttack1()),
attack2(config.getAttack2()),
attack3(config.getAttack3()),
attack4(config.getAttack4()),
attack5(config.getAttack5()),
attack6(config.getAttack6()),
jump(config.getJump()),
joystick_right(config.getJoystickRight()),
joystick_left(config.getJoystickLeft()),
joystick_up(config.getJoystickUp()),
joystick_down(config.getJoystickDown()),
joystick_attack1(config.getJoystickAttack1()),
joystick_attack2(config.getJoystickAttack2()),
joystick_attack3(config.getJoystickAttack3()),
joystick_attack4(config.getJoystickAttack4()),
joystick_attack5(config.getJoystickAttack5()),
joystick_attack6(config.getJoystickAttack6()),
joystick_jump(config.getJoystickJump()),
joystick_quit(config.getJoystickQuit())
{
    menuFont = config.menuFont;
}

/*
Configuration::Configuration( const int right, const int left, const int up, const int down, const int attack1, const int attack2, const int attack3, const int jump ):
right( right ),
left( left ),
up( up ),
down( down ),
attack1( attack1 ),
attack2( attack2 ),
attack3( attack3 ),
jump( jump ){
}
*/
	
Configuration & Configuration::operator=(const Configuration & config){
    Disable disable;
    setRight(config.getRight());
    setLeft(config.getLeft());
    setUp(config.getUp());
    setDown(config.getDown());
    setAttack1(config.getAttack1());
    setAttack2(config.getAttack2());
    setAttack3(config.getAttack3());
    setAttack4(config.getAttack4());
    setAttack5(config.getAttack5());
    setAttack6(config.getAttack6());
    setJump(config.getJump());

    setJoystickRight(config.getJoystickRight());
    setJoystickLeft(config.getJoystickLeft());
    setJoystickUp(config.getJoystickUp());
    setJoystickDown(config.getJoystickDown());
    setJoystickAttack1(config.getJoystickAttack1());
    setJoystickAttack2(config.getJoystickAttack2());
    setJoystickAttack3(config.getJoystickAttack3());
    setJoystickJump(config.getJoystickJump());
    setJoystickQuit(config.getJoystickQuit());

    setMenuFont(config.getMenuFont());
    setMenuFontWidth(config.getMenuFontWidth());
    setMenuFontHeight(config.getMenuFontHeight());
    return *this;
}
	
Configuration::~Configuration(){
}

Configuration::JoystickInput Configuration::getJoystickKey(Input::PaintownInput which, int facing) const {
    switch (which){
        case Input::Forward : {
            if ( facing == Paintown::Object::FACING_LEFT ) 
                return this->joystick_left;
            else 	return this->joystick_right;
        }
        case Input::Back : {
            if ( facing == Paintown::Object::FACING_LEFT )
                return this->joystick_right;
            else	return this->joystick_left;
        }
        case Input::Up : return this->joystick_up;
        case Input::Down : return this->joystick_down;
        case Input::Attack1 : return this->joystick_attack1;
        case Input::Attack2 : return this->joystick_attack2;
        case Input::Attack3 : return this->joystick_attack3;
        case Input::Attack4 : return this->joystick_attack4;
        case Input::Attack5 : return this->joystick_attack5;
        case Input::Attack6 : return this->joystick_attack6;
        case Input::Jump : return this->joystick_jump;
        default : return Joystick::Invalid;
    }
}

int Configuration::getKey( Input::PaintownInput which, int facing ) const {
    switch (which){
        case Input::Forward : {
            if ( facing == Paintown::Object::FACING_LEFT ) 
                return this->left;
            else 	return this->right;
        }
        case Input::Back : {
            if ( facing == Paintown::Object::FACING_LEFT )
                return this->right;
            else	return this->left;
        }
        case Input::Up : return this->up;
        case Input::Down : return this->down;
        case Input::Attack1 : return this->attack1;
        case Input::Attack2 : return this->attack2;
        case Input::Attack3 : return this->attack3;
        case Input::Attack4 : return this->attack4;
        case Input::Attack5 : return this->attack5;
        case Input::Attack6 : return this->attack6;
        case Input::Jump : return this->jump;
        default : return -1;
    }
}
        
Util::ReferenceCount<Menu::FontInfo> Configuration::getMenuFont(){
    return menuFont;
}
    
bool Configuration::hasMenuFont(){
    return menuFont != NULL;
}

void Configuration::setMenuFont(const Util::ReferenceCount<Menu::FontInfo> & info){
    menuFont = info;
    saveConfiguration();
}

void Configuration::setMenuFontWidth(int x){
    menuFontWidth = x;
    saveConfiguration();
}

int Configuration::getMenuFontWidth(){
    return menuFontWidth;
}

void Configuration::setMenuFontHeight(int x){
    menuFontHeight = x;
    saveConfiguration();
}

int Configuration::getMenuFontHeight(){
    return menuFontHeight;
}

void Configuration::setKey(int * key, int value){
    if (value != InvalidKey){
        *key = value;
        saveConfiguration();
    }
}

void Configuration::setRight( int i ){
    setKey(&right, i);
}

void Configuration::setLeft( int i ){
    setKey(&left, i);
}

void Configuration::setUp( int i ){
    setKey(&up, i);
}

void Configuration::setDown( int i ){
    setKey(&down, i);
}

void Configuration::setAttack1( int i ){
    setKey(&attack1, i);
}

void Configuration::setAttack2( int i ){
    setKey(&attack2, i);
}

void Configuration::setAttack3( int i ){
    setKey(&attack3, i);
}

void Configuration::setAttack4( int i ){
    setKey(&attack4, i);
}

void Configuration::setAttack5( int i ){
    setKey(&attack5, i);
}

void Configuration::setAttack6( int i ){
    setKey(&attack6, i);
}

void Configuration::setJump( int i ){
    setKey(&jump, i);
}

int Configuration::getRight() const {
    return right;
}

int Configuration::getLeft() const {
    return left;
}

int Configuration::getUp() const {
    return up;
}

int Configuration::getDown() const {
    return down;
}

int Configuration::getAttack1() const {
    return attack1;
}

int Configuration::getAttack2() const {
    return attack2;
}

int Configuration::getAttack3() const {
    return attack3;
}

int Configuration::getAttack4() const {
    return attack4;
}

int Configuration::getAttack5() const {
    return attack5;
}

int Configuration::getAttack6() const {
    return attack6;
}

int Configuration::getJump() const {
    return jump;
}

void Configuration::setJoystickKey(JoystickInput & key, const JoystickInput & what){
    if (what != Joystick::Invalid){
        key = what;
        saveConfiguration();
    }
}

void Configuration::setJoystickRight(Configuration::JoystickInput i){
    setJoystickKey(joystick_right, i);
}

void Configuration::setJoystickLeft(Configuration::JoystickInput i){
    setJoystickKey(joystick_left, i);
}

void Configuration::setJoystickUp(Configuration::JoystickInput i){
    setJoystickKey(joystick_up, i);
}

void Configuration::setJoystickDown(Configuration::JoystickInput i){
    setJoystickKey(joystick_down, i);
}

void Configuration::setJoystickAttack1(Configuration::JoystickInput i){
    setJoystickKey(joystick_attack1, i);
}

void Configuration::setJoystickAttack2(Configuration::JoystickInput i){
    setJoystickKey(joystick_attack2, i);
}

void Configuration::setJoystickAttack3(Configuration::JoystickInput i){
    setJoystickKey(joystick_attack3, i);
}

void Configuration::setJoystickAttack4(Configuration::JoystickInput i){
    setJoystickKey(joystick_attack4, i);
}

void Configuration::setJoystickAttack5(Configuration::JoystickInput i){
    setJoystickKey(joystick_attack5, i);
}

void Configuration::setJoystickAttack6(Configuration::JoystickInput i){
    setJoystickKey(joystick_attack6, i);
}

void Configuration::setJoystickJump(Configuration::JoystickInput i){
    setJoystickKey(joystick_jump, i);
}

void Configuration::setJoystickQuit(Configuration::JoystickInput i){
    setJoystickKey(joystick_quit, i);
}

Configuration::JoystickInput Configuration::getJoystickRight() const {
    return joystick_right;
}

Configuration::JoystickInput Configuration::getJoystickLeft() const {
    return joystick_left;
}

Configuration::JoystickInput Configuration::getJoystickUp() const {
    return joystick_up;
}

Configuration::JoystickInput Configuration::getJoystickDown() const {
    return joystick_down;
}

Configuration::JoystickInput Configuration::getJoystickAttack1() const {
    return joystick_attack1;
}

Configuration::JoystickInput Configuration::getJoystickAttack2() const {
    return joystick_attack2;
}

Configuration::JoystickInput Configuration::getJoystickAttack3() const {
    return joystick_attack3;
}

Configuration::JoystickInput Configuration::getJoystickAttack4() const {
    return joystick_attack4;
}

Configuration::JoystickInput Configuration::getJoystickAttack5() const {
    return joystick_attack5;
}

Configuration::JoystickInput Configuration::getJoystickAttack6() const {
    return joystick_attack6;
}

Configuration::JoystickInput Configuration::getJoystickJump() const {
    return joystick_jump;
}

Configuration::JoystickInput Configuration::getJoystickQuit() const {
    return joystick_quit;
}

string Configuration::getCurrentGame(){
    return currentGameDir;
}

/* assumes the directory is readable and has a dir/dir.txt in it */
void Configuration::setCurrentGame(const std::string & str){
    currentGameDir = str;
}

/* this nonsense is just to convert a regular integer into an enum */
static Configuration::JoystickInput intToJoystick(int a){
    switch (a){
        case Joystick::Up : return Joystick::Up;
        case Joystick::Down : return Joystick::Down;
        case Joystick::Left : return Joystick::Left;
        case Joystick::Right : return Joystick::Right;
        case Joystick::Button1 : return Joystick::Button1;
        case Joystick::Button2 : return Joystick::Button2;
        case Joystick::Button3 : return Joystick::Button3;
        case Joystick::Button4 : return Joystick::Button4;
        case Joystick::Quit : return Joystick::Quit;
    }
    return Joystick::Invalid;
}

void Configuration::disableSave(){
    save = false;
}

void Configuration::setSave(bool what){
    save = what;
}

bool Configuration::getSave(){
    return save;
}

void Configuration::loadConfigurations(){
    Disable disable;

    data = new Token();
    *data << config_configuration;

    try{
        Filesystem::AbsolutePath file = Storage::instance().configFile();
        TokenReader tr;
        Token * head = tr.readTokenFromFile(file.path());

        if (*head != config_configuration){
            throw LoadException(__FILE__, __LINE__, string("Config file ") + Storage::instance().configFile().path() + " does not use the configuration format" );
        }

        /* Store the entire configuration tree */
        data = head->copy();

        TokenView view = head->view();
        while (view.hasMore()){
            const Token * n;
            view >> n;
            /* these operate on the global vars directly
             * to avoid calling saveConfiguration
             * if setFoo() was called
             */
            if ( *n == config_keyboard_configuration ){
                int number = -1;
                int right, left, down, up, attack1,
                    attack2, attack3, attack4, attack5, attack6,
                    jump;
                right = left = down = up = attack1 = attack2
                      = attack3 = attack4 = attack5 = attack6 =
                      jump = InvalidKey;

                /* before the 'version' key was added all backends were
                 * Allegro so that is the default.
                 */
                std::string input = "Allegro";

                TokenView keyboardView = n->view();
                while (keyboardView.hasMore()){
                    const Token * thing;
                    keyboardView >> thing;
                    if ( *thing == config_number){
                        thing->view() >> number;
                    } else if (*thing == config_input){
                        thing->view() >> input;
                    } else if ( *thing == config_left){
                        thing->view() >> left;
                    } else if ( *thing == config_right){
                        thing->view() >> right;
                    } else if ( *thing == config_down){
                        thing->view() >> down;
                    } else if ( *thing == config_up){
                        thing->view() >> up;
                    } else if ( *thing == config_attack1){
                        thing->view() >> attack1;
                    } else if ( *thing == config_attack2){
                        thing->view() >> attack2;
                    } else if ( *thing == config_attack3){
                        thing->view() >> attack3;
                    } else if ( *thing == config_attack4){
                        thing->view() >> attack4;
                    } else if ( *thing == config_attack5){
                        thing->view() >> attack5;
                    } else if ( *thing == config_attack6){
                        thing->view() >> attack6;
                    } else if ( *thing == config_jump){
                        thing->view() >> jump;
                    }
                }
                if ( number == -1 ){
                    /* should use config_number here */
                    throw LoadException(__FILE__, __LINE__, string("Config file ") + Storage::instance().configFile().path() + " does not specifiy (number #) for a keyboard-configuration" );
                }

                if (input == INPUT_TYPE){
                    Configuration & myconfig = config(number);
                    myconfig.setRight(right);
                    myconfig.setLeft(left);
                    myconfig.setUp(up);
                    myconfig.setDown(down);
                    myconfig.setAttack1(attack1);
                    myconfig.setAttack2(attack2);
                    myconfig.setAttack3(attack3);
                    myconfig.setAttack4(attack4);
                    myconfig.setAttack5(attack5);
                    myconfig.setAttack6(attack6);
                    myconfig.setJump(jump);
                }
            } else if ( *n == config_joystick_configuration ){
                int number = -1;
                JoystickInput right, left, down, up, attack1,
                    attack2, attack3, attack4, attack5, attack6, jump, quit;
                right = left = down = up = attack1 = attack2
                      = attack3 = attack4 = attack5 = attack6 =
                      jump = quit = InvalidJoystick;
                /* see above */
                std::string input = "Allegro";

                TokenView joystickView = n->view();
                while (joystickView.hasMore()){
                    int temp;
                    const Token * thing;
                    joystickView >> thing;
                    if ( *thing == config_number){
                        thing->view() >> number;
                    } else if (*thing == config_input){
                        thing->view() >> input;
                    } else if ( *thing == config_left){
                        thing->view() >> temp;
                        left = intToJoystick(temp);
                    } else if ( *thing == config_right){
                        thing->view() >> temp;
                        right = intToJoystick(temp);
                    } else if ( *thing == config_down){
                        thing->view() >> temp;
                        down = intToJoystick(temp);
                    } else if ( *thing == config_up){
                        thing->view() >> temp;
                        up = intToJoystick(temp);
                    } else if ( *thing == config_attack1){
                        thing->view() >> temp;
                        attack1 = intToJoystick(temp);
                    } else if ( *thing == config_attack2){
                        thing->view() >> temp;
                        attack2 = intToJoystick(temp);
                    } else if ( *thing == config_attack3){
                        thing->view() >> temp;
                        attack3 = intToJoystick(temp);
                    } else if ( *thing == config_attack4){
                        thing->view() >> temp;
                        attack4 = intToJoystick(temp);
                    } else if ( *thing == config_attack5){
                        thing->view() >> temp;
                        attack5 = intToJoystick(temp);
                    } else if ( *thing == config_attack6){
                        thing->view() >> temp;
                        attack6 = intToJoystick(temp);
                    } else if ( *thing == config_jump){
                        thing->view() >> temp;
                        jump = intToJoystick(temp);
                    } else if ( *thing == config_quit){
                        thing->view() >> temp;
                        quit = intToJoystick(temp);
                    }
                }
                if ( number == -1 ){
                    /* should use config_number here */
                    throw LoadException(__FILE__, __LINE__, string("Config file ") + Storage::instance().configFile().path() + " does not specifiy (number #) for a joystick-configuration" );
                }

                if (input == INPUT_TYPE){
                    Configuration & myconfig = config(number);
                    myconfig.setJoystickRight(right);
                    myconfig.setJoystickLeft(left);
                    myconfig.setJoystickUp(up);
                    myconfig.setJoystickDown(down);
                    myconfig.setJoystickAttack1(attack1);
                    myconfig.setJoystickAttack2(attack2);
                    myconfig.setJoystickAttack3(attack3);
                    myconfig.setJoystickAttack4(attack4);
                    myconfig.setJoystickAttack5(attack5);
                    myconfig.setJoystickAttack6(attack6);
                    myconfig.setJoystickJump(jump);
                    myconfig.setJoystickQuit(quit);
                }
            } else if ( *n == config_game_speed){
                n->view() >> gamespeed;
            } else if ( *n == config_invincible){
                n->view() >> invincible;
            } else if ( *n == config_fullscreen){
                n->view() >> fullscreen;
            } else if (*n == config_lives){
                n->view() >> lives;
            } else if (*n == config_menu_font){
                string font;
                n->view() >> font;
                /* FIXME */
                // setMenuFont(font);
            } else if (*n == config_version){
                string version;
                n->view() >> version;
                /* FIXME: check the version here */
            } else if (*n == config_sound){
                int x;
                n->view() >> x;
                setSoundVolume(x);
            } else if (*n == config_language){
                string what;
                n->view() >> what;
                setLanguage(what);
            } else if (*n == config_music){
                int x;
                n->view() >> x;
                setMusicVolume(x);
            } else if (*n == config_mugen_motif){
                string motif;
                n->view() >> motif;
                setMugenMotif(motif);
            } else if (*n == config_quality_filter){
                string filter;
                n->view() >> filter;
                setQualityFilter(filter);
            } else if (*n == config_fps){
                int fps = 40;
                n->view() >> fps;
                setFps(fps);
            } else if (*n == config_menu_font_width){
                int x;
                n->view() >> x;
                setMenuFontWidth(x);
            } else if (*n == config_menu_font_height){
                int x;
                n->view() >> x;
                setMenuFontHeight(x);
            } else if (*n == config_current_game){
                try{
                    string game;
                    n->view() >> game;
                    setCurrentGame(game);
                } catch (const TokenException & fail){
                    Global::debug(0) << "Warning: could not get the current game from the configuration" << std::endl;
                }
            } else if ( *n == config_npc_buddies){
                n->view() >> npc_buddies;
            } else if (*n == config_screen_size){
                int w, h;
                n->view() >> w >> h;
                Configuration::setScreenWidth(w);
                Configuration::setScreenHeight(h);
            } else if (*n == config_play_mode){
                string mode;
                n->view() >> mode;
                if (mode == config_cooperative){
                    Configuration::setPlayMode(Configuration::Cooperative);
                } else if (mode == config_free_for_all){
                    Configuration::setPlayMode(Configuration::FreeForAll);
                }
            } else {
                // string value;
                try{
                    /*
                    Util::ReferenceCount<Configuration> root = getRootConfiguration();
                    root->parseProperty(n);
                    */
                    /*
                    n->view() >> value;
                    root->setProperty(n->getName(), value);
                    */
                } catch (const TokenException & e){
                    /* ignore errors */
                }
            }
        }
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Notice: Could not load configuration file " << Storage::instance().configFile().path() << ": " << le.getTrace() << endl;
    } catch ( const TokenException & t ){
        Global::debug( 0 ) << "Notice: could not open configuration file '" << Storage::instance().configFile().path() << "': " << t.getTrace() << endl;
    }
}

void Configuration::parseProperty(const Token * token){
#if 0
    /* Its either plain data or its a nested node which means its a namespace */
    string value;
    if (token->match("_", value)){
        try{
            token->view() >> value;
            setProperty(token->getName(), value);
        } catch (const TokenException & e){
            /* ignore errors */
        }
    } else {
        string nameSpace = token->getName();
        Util::ReferenceCount<Configuration> module = getNamespace(nameSpace); 
        TokenView view = token->view();
        while (view.hasMore()){
            const Token * next = NULL;
            view >> next;
            module->parseProperty(next);
        }
    }
#endif
}

std::string Configuration::getMugenMotif(){
    return mugenMotif;
}

void Configuration::setMugenMotif(const std::string & motif){
    mugenMotif = motif;
    saveConfiguration();
}

/* todo: combine saveKeyboard and saveJoystick, probably using a templated function */
Token * Configuration::saveKeyboard( int num, Configuration * configuration ){
    typedef int (Configuration::*get_func)() const;
    Token * config = new Token();

    config->addToken( new Token(config_keyboard_configuration, false ) );

    *config->newToken() << config_number << num;
    *config->newToken() << config_input << INPUT_TYPE;

    const char * func_names[] = {config_left, config_right,
                                 config_up, config_down,
                                 config_attack1, config_attack2,
                                 config_attack3, config_attack4,
                                 config_attack5, config_attack6,
                                 config_jump};

    get_func funcs[] = {&Configuration::getLeft, &Configuration::getRight,
                        &Configuration::getUp, &Configuration::getDown,
                        &Configuration::getAttack1, &Configuration::getAttack2,
                        &Configuration::getAttack3, &Configuration::getAttack4,
                        &Configuration::getAttack5, &Configuration::getAttack6,
                        &Configuration::getJump
    };

    for ( unsigned int i = 0; i < sizeof(func_names)/sizeof(char*); i++ ){
        Token * button = new Token();
        *button << func_names[i] << (configuration->*(funcs[i]))();
        config->addToken(button);
    }

    return config;
}

Token * Configuration::saveJoystick( int num, Configuration * configuration ){
    typedef JoystickInput (Configuration::*get_func)() const;
    Token * config = new Token();

    config->addToken( new Token(config_joystick_configuration, false ) );

    *config->newToken() << config_number << num;
    *config->newToken() << config_input << INPUT_TYPE;

    const char * func_names[] = {config_left, config_right,
                                 config_up, config_down,
                                 config_attack1, config_attack2,
                                 config_attack3, config_attack4,
                                 config_attack5, config_attack6,
                                 config_jump, config_quit};

    get_func funcs[] = {&Configuration::getJoystickLeft, &Configuration::getJoystickRight,
                        &Configuration::getJoystickUp, &Configuration::getJoystickDown,
                        &Configuration::getJoystickAttack1, &Configuration::getJoystickAttack2,
                        &Configuration::getJoystickAttack3, &Configuration::getJoystickAttack4,
                        &Configuration::getJoystickAttack5, &Configuration::getJoystickAttack6,
                        &Configuration::getJoystickJump,
                        &Configuration::getJoystickQuit
    };

    for ( unsigned int i = 0; i < sizeof(func_names)/sizeof(char*); i++ ){
        Token * button = new Token();
        *button << func_names[ i ] << (configuration->*(funcs[i]))();
        config->addToken( button );
    }

    return config;
}

vector<Token*> Configuration::getPropertyTokens(){
    /* All these tokens will be deleted by the calling function */
    vector<Token*> tokens;
    for (map<string, string>::iterator it = properties.begin(); it != properties.end(); it++){
        string name = (*it).first;
        string value = (*it).second;
        Token * property = new Token();
        *property << name << value;
        tokens.push_back(property);
    }

    for (map<std::string, Util::ReferenceCount<Configuration> >::iterator it = namespaces.begin(); it != namespaces.end(); it++){
        string name = it->first;
        Util::ReferenceCount<Configuration> configuration = it->second;
        if (configuration != NULL){
            Token * sub = new Token();
            tokens.push_back(sub);
            *sub << name;

            vector<Token*> propertyTokens = configuration->getPropertyTokens();
            for (vector<Token*>::iterator nit = propertyTokens.begin(); nit != propertyTokens.end(); nit++){
                sub->addToken(*nit);
            }
        }
    }

    return tokens;
}

void Configuration::saveConfiguration(){

    if (!save){
        return;
    }

#if 0
    /* head will delete all these tokens in its destructor */
    Token head;
    head << config_configuration;
    for ( map< int, Configuration * >::iterator it = configs.begin(); it != configs.end(); it++ ){
        int num = it->first;
        Configuration * configuration = it->second;
        head.addToken(saveKeyboard(num, configuration));
        head.addToken(saveJoystick(num, configuration));
    }

    Token * speed = new Token();
    *speed << config_game_speed << Configuration::getGameSpeed();
    head.addToken( speed );

    Token * invincible = new Token();
    *invincible << config_invincible << Configuration::getInvincible();
    head.addToken( invincible );

    Token * sound = new Token();
    *sound << config_sound << Configuration::getSoundVolume();
    head.addToken(sound);
    
    Token * music = new Token();
    *music << config_music << Configuration::getMusicVolume();
    head.addToken(music);

    Token * fullscreen = new Token();
    *fullscreen << config_fullscreen << Configuration::getFullscreen();
    head.addToken(fullscreen);

    Token * screen = new Token();
    *screen << config_screen_size << Configuration::getScreenWidth() << Configuration::getScreenHeight();
    head.addToken(screen);

    *(head.newToken()) << config_version << Global::getVersionString();

    *(head.newToken()) << config_quality_filter << getQualityFilter();

    *(head.newToken()) << config_fps << getFps();

    if (Configuration::getLanguage() != ""){
        *(head.newToken()) << config_language << Configuration::getLanguage();
    }

    if (Configuration::getMenuFont() != NULL){
        /* FIXME */
        /*
        Token * font = new Token();
        *font << config_menu_font << Configuration::getMenuFont();
        head.addToken(font);
        */
    }

    {
        Token * width = new Token();
        *width << config_menu_font_width << Configuration::getMenuFontWidth();
        head.addToken(width);
    }

    {
        Token * height = new Token();
        *height << config_menu_font_height << Configuration::getMenuFontHeight();
        head.addToken(height);
    }

    Token * mode = new Token();
    string smode;
    if (Configuration::getPlayMode() == Configuration::Cooperative){
        smode = config_cooperative;
    } else if (Configuration::getPlayMode() == Configuration::FreeForAll){
        smode = config_free_for_all;
    }
    *mode << config_play_mode << smode;
    head.addToken(mode);

    Token * mugenMotifToken = new Token();
    *mugenMotifToken << config_mugen_motif << Configuration::getMugenMotif();
    head.addToken(mugenMotifToken);

    Token * lives = new Token();
    *lives << config_lives << Configuration::getLives();
    head.addToken(lives);

    Token * game = new Token();
    *game << config_current_game << Configuration::getCurrentGame();
    head.addToken(game);

    Token * npc = new Token();
    *npc << config_npc_buddies << Configuration::getNpcBuddies();
    head.addToken(npc);

    /*
    vector<Token*> propertyTokens = getRootConfiguration()->getPropertyTokens();
    for (vector<Token*>::iterator it = propertyTokens.begin(); it != propertyTokens.end(); it++){
        head.addToken(*it);
    }
    */

    /*
    for (map<string, string>::iterator it = properties.begin(); it != properties.end(); it++){
        string name = (*it).first;
        string value = (*it).second;
        Token * property = new Token();
        *property << name << value;
        head.addToken(property);
    }
    */
#endif

    ofstream out(Storage::instance().configFile().path().c_str(), ios::trunc | ios::out);
    if (! out.bad()){
        data->toString(out, string(""));
        out << endl;
        out.close();
    }
}

/* Defaults for all configuration options */
double Configuration::gamespeed = 1.0;
bool Configuration::invincible = false;
#if defined(PS3) || defined(ANDROID) || defined(IPHONE)
bool Configuration::fullscreen = true;
#else
bool Configuration::fullscreen = false;
#endif
int Configuration::lives = 4;
int Configuration::npc_buddies = 1;
Configuration::PlayMode Configuration::play_mode = Configuration::Cooperative;

#ifdef MINPSPW
/* default resolution for the psp is 480x272 */
int Configuration::screen_width = 480;
int Configuration::screen_height = 272;
#else
int Configuration::screen_width = 640;
int Configuration::screen_height = 480;
#endif
Util::ReferenceCount<Menu::FontInfo> Configuration::menuFont;
int Configuration::menuFontWidth = 24;
int Configuration::menuFontHeight = 24;
int Configuration::soundVolume = 70;
int Configuration::musicVolume = 70;
bool Configuration::joystickEnabled = true;
std::string Configuration::currentGameDir = "";
// std::map<std::string, std::string> Configuration::properties;
std::string Configuration::mugenMotif = "default";
std::string Configuration::qualityFilter = "none";
/* Original default was 40 */
int Configuration::fps = 40;
// std::string Configuration::menuFont = "fonts/arial.ttf";
// Configuration::PlayMode Configuration::play_mode = Configuration::FreeForAll;

/*
Util::ReferenceCount<Configuration> Configuration::getNamespace(const std::string & name){
    if (namespaces[name] == NULL){
        namespaces[name] = Util::ReferenceCount<Configuration>(new Configuration());
    }
    return namespaces[name];
}
*/

/* FIXME: move this to some utils module */
static std::vector<std::string> split(std::string str, char splitter){
    std::vector<std::string> strings;
    size_t next = str.find(splitter);
    while (next != std::string::npos){
        strings.push_back(str.substr(0, next));
        str = str.substr(next+1);
        next = str.find(splitter);
    }
    if (str != ""){
        strings.push_back(str);
    }

    return strings;
}

template <class Value>
static Token * createToken(const string & path, const Value & value){
    vector<string> paths = split(path, '/');
    Token * out = new Token();
    Token * current = out;
    for (vector<string>::iterator it = paths.begin(); it != paths.end(); it++){
        current->addToken(new Token(*it, false));
        if (paths.end() - it > 1){
            Token * next = new Token();
            current->addToken(next);
            current = next;
        }
    }
    *current << value;
    Global::debug(1) << "Create token: " << out->toString() << endl;
    return out;
}

template <class Value>
static void updateToken(Token * data, const string & path, const Value & value){
    if (data == NULL){
        return;
    }

    Token * found = data->findToken(path);
    /* See if the token already exists. If it does then remove it from its
     * parent and add a new token to the parent with the updated value.
     */
    if (found != NULL){
        Token * parent = found->getParent();
        string name = found->getName();
        parent->removeToken(found);
        parent->addToken(createToken(name, value));
    } else {
        const vector<string> paths = split(path, '/');
        Token * start = data;
        for (int index = 1; index < paths.size() - 1; index++){
            string where = paths[index];
            Token * next = start->findToken(string("_/") + where);
            if (next == NULL){
                ostringstream out;
                bool first = true;
                for (int from = index; from < paths.size(); from++){
                    if (!first){
                        out << "/";
                    }
                    out << paths[from];
                    first = false;
                }
                start->addToken(createToken(out.str(), value));
                return;
            } else {
                start = next;
            }
        }
        start->addToken(createToken(paths[paths.size() - 1], value));
    }
}

void Configuration::setProperty(const string & name, const string & value){
    updateToken(data.raw(), string(config_configuration) + "/" + name, value);
    saveConfiguration();
}

template <class Out>
static Out getPropertyX(Token * data, const std::string & path, const Out & defaultValue){
    Out out;
    if (data->match(string(config_configuration) + "/" + path, out)){
        return out;
    }
    updateToken(data, string(config_configuration) + "/" + path, defaultValue);
    return defaultValue;

    /*
    if (properties.find(path) == properties.end()){
        properties[path] = defaultValue;
    }
    return properties[path];
    */
}

int Configuration::getProperty(const std::string & path, int defaultValue){
    return getPropertyX(data.raw(), path, defaultValue);
}

void Configuration::setProperty(const std::string & path, int value){
    updateToken(data.raw(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

std::string Configuration::getProperty(const std::string & path, const std::string & defaultValue){
    return getPropertyX(data.raw(), path, defaultValue);
    /*
    std::string out;
    if (data->match(string(config_configuration) + "/" + path, out)){
        return out;
    }
    updateToken(string(config_configuration) + "/" + path, defaultValue);
    return defaultValue;
    */

    /*
    if (properties.find(path) == properties.end()){
        properties[path] = defaultValue;
    }
    return properties[path];
    */
}

double Configuration::getGameSpeed(){
    return gamespeed;
}

void Configuration::setGameSpeed(double s){
    gamespeed = s;
    saveConfiguration();
}

bool Configuration::getInvincible(){
    return invincible;
}

void Configuration::setInvincible(bool i){
    invincible = i;
    saveConfiguration();
}

bool Configuration::getFullscreen(){
    return fullscreen;
}

void Configuration::setFullscreen(bool f){
    fullscreen = f;
    saveConfiguration();
}

int Configuration::getLives(){
    return lives;
}

void Configuration::setLives(int l){
    lives = l;
    saveConfiguration();
}
	
int Configuration::getNpcBuddies(){
    return npc_buddies;
}

void Configuration::setNpcBuddies( int i ){
    npc_buddies = i;
    saveConfiguration();
}
        
Configuration::PlayMode Configuration::getPlayMode(){
    string mode = getProperty("play-mode", config_cooperative);
    if (mode == config_cooperative){
        return Cooperative;
    }
    if (mode == config_free_for_all){
        return FreeForAll;
    }
    return Cooperative;
}

void Configuration::setPlayMode(Configuration::PlayMode mode){
    if (mode == Configuration::Cooperative){
        setProperty("play-mode", config_cooperative);
    } else if (mode == Configuration::FreeForAll){
        setProperty("play-mode", config_free_for_all);
    }
}
    
std::string Configuration::getLanguage(){
    return getProperty("language", "");
}

void Configuration::setLanguage(const std::string & str){
    setProperty("language", str);
    // language = str;
    saveConfiguration();
}

void Configuration::setScreenWidth(int i){
    screen_width = i;
    saveConfiguration();
}

int Configuration::getScreenWidth(){
    return screen_width;
}

void Configuration::setScreenHeight(int i){
    screen_height = i;
    saveConfiguration();
}

int Configuration::getScreenHeight(){
    return screen_height;
}

int Configuration::getSoundVolume(){
    return getProperty(config_sound, 70);
}

void Configuration::setSoundVolume(int volume){
    setProperty(config_sound, volume);
    saveConfiguration();
}

int Configuration::getMusicVolume(){
    return musicVolume;
}

void Configuration::setMusicVolume(int volume){
    musicVolume = volume;
    saveConfiguration();
}

bool Configuration::isJoystickEnabled(){
    return joystickEnabled;
}
    
void Configuration::setJoystickEnabled(bool enabled){
    joystickEnabled = enabled;
    saveConfiguration();
}
    
std::string Configuration::getQualityFilter(){
    return qualityFilter;
}

void Configuration::setQualityFilter(const std::string & filter){
    qualityFilter = filter;
    saveConfiguration();
}
    
int Configuration::getFps(){
    return fps;
}

void Configuration::setFps(int what){
    fps = what;
    saveConfiguration();
}
    
/*
Util::ReferenceCount<Configuration> Configuration::getRootConfiguration(){
    if (rootProperty == NULL){
        rootProperty = Util::ReferenceCount<Configuration>(new Configuration());
    }

    return rootProperty;
}
*/
