#include "configuration.h"
#include "util/input/keyboard.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/file-system.h"
#include "util/input/input.h"
#include "globals.h"
#include "util/debug.h"
/*
#include "paintown-engine/object/animation.h"
#include "paintown-engine/object/object.h"
*/
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

#if 0
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
#endif

// static map< int, Configuration * > configs;

/*
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


/* Create a token that contains the entire path.
 * path = "foo/bar/baz"
 * out = (foo (bar (baz)))
 */
static Token * createToken(const string & path){
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
    return out;
}



void Configuration::setDefaultKeys(int config){
    /* Set the keys token to an empty value */
    ostringstream path;
    path << config_input << "/" << config << "/keys";
    setProperty(path.str(), createToken("keys"));

    /* Call the get methods to reset the defaults */
    int ignore;
    ignore = getRight(config);
    ignore = getLeft(config);
    ignore = getUp(config);
    ignore = getDown(config);
    ignore = getAttack1(config);
    ignore = getAttack2(config);
    ignore = getAttack3(config);
    ignore = getAttack4(config);
    ignore = getAttack5(config);
    ignore = getAttack6(config);
    ignore = getJump(config);
}

/* hopefully this is only used right before setting all the values
 * since the key mappings are bogus in here'
 */
Configuration::Configuration(){
}

Configuration::Configuration(const Configuration & config){
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
    setMenuFont(config.getMenuFont());
    return *this;
}
	
Configuration::~Configuration(){
}

/*
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
*/

/*
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
*/
        
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
    setProperty(config_menu_font_width, x);
    saveConfiguration();
}

int Configuration::getMenuFontWidth(){
    return getProperty(config_menu_font_width, 24);
}

void Configuration::setMenuFontHeight(int x){
    setProperty(config_menu_font_height, x);
    saveConfiguration();
}

int Configuration::getMenuFontHeight(){
    return getProperty(config_menu_font_height, 24);
}

void Configuration::setKey(int * key, int value){
    if (value != InvalidKey){
        *key = value;
        saveConfiguration();
    }
}

void Configuration::setRight(int i){
    // setKey(&right, i);
}

void Configuration::setLeft( int i ){
    // setKey(&left, i);
}

void Configuration::setUp( int i ){
    // setKey(&up, i);
}

void Configuration::setDown( int i ){
    // setKey(&down, i);
}

void Configuration::setAttack1( int i ){
    // setKey(&attack1, i);
}

void Configuration::setAttack2( int i ){
    // setKey(&attack2, i);
}

void Configuration::setAttack3( int i ){
    // setKey(&attack3, i);
}

void Configuration::setAttack4( int i ){
    // setKey(&attack4, i);
}

void Configuration::setAttack5( int i ){
    // setKey(&attack5, i);
}

void Configuration::setAttack6( int i ){
    // setKey(&attack6, i);
}

void Configuration::setJump( int i ){
    // setKey(&jump, i);
}

int Configuration::getKey(int config, const string & name, int defaultValue){
    ostringstream path;
    path << config_input << "/" << config << "/keys/" << name;
    return getProperty(path.str(), defaultValue);
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

Configuration::JoystickInput Configuration::getJoystickKey(int config, const string & name, JoystickInput defaultValue){
    ostringstream path;
    path << config_input << "/" << config << "/joystick/" << name;
    return intToJoystick(getProperty(path.str(), defaultValue));
}

int Configuration::getRight(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_RIGHT; break;
        case 1: normal = Keyboard::Key_L; break;
    }
    return getKey(config, "right", normal);
}

int Configuration::getLeft(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_LEFT; break;
        case 1: normal = Keyboard::Key_J; break;
    }
    return getKey(config, "left", normal);
}

int Configuration::getUp(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_UP; break;
        case 1: normal = Keyboard::Key_I; break;
    }
    return getKey(config, "up", normal);
}

int Configuration::getDown(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_DOWN; break;
        case 1: normal = Keyboard::Key_COMMA; break;
    }
    return getKey(config, "down", normal);
}

int Configuration::getAttack1(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_A; break;
        case 1: normal = Keyboard::Key_R; break;
    }
    return getKey(config, "attack1", normal);
}

int Configuration::getAttack2(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_S; break;
        case 1: normal = Keyboard::Key_T; break;
    }
    return getKey(config, "attack2", normal);
}

int Configuration::getAttack3(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_D; break;
        case 1: normal = Keyboard::Key_Y; break;
    }
    return getKey(config, "attack3", normal);
}

int Configuration::getAttack4(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_Z; break;
        case 1: normal = Keyboard::Key_F; break;
    }
    return getKey(config, "attack4", normal);
}

int Configuration::getAttack5(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_X; break;
        case 1: normal = Keyboard::Key_G; break;
    }

    return getKey(config, "attack5", normal);
}

int Configuration::getAttack6(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_C; break;
        case 1: normal = Keyboard::Key_H; break;
    }
    return getKey(config, "attack6", normal);
}

int Configuration::getJump(int config){
    int normal = 0;
    switch (config){
        case 0: normal = Keyboard::Key_SPACE; break;
        case 1: normal = Keyboard::Key_B; break;
    }
    return getKey(config, "jump", normal);
}

void Configuration::setJoystickKey(JoystickInput & key, const JoystickInput & what){
    if (what != Joystick::Invalid){
        key = what;
        saveConfiguration();
    }
}

void Configuration::setJoystickRight(Configuration::JoystickInput i){
    // setJoystickKey(joystick_right, i);
}

void Configuration::setJoystickLeft(Configuration::JoystickInput i){
    // setJoystickKey(joystick_left, i);
}

void Configuration::setJoystickUp(Configuration::JoystickInput i){
    // setJoystickKey(joystick_up, i);
}

void Configuration::setJoystickDown(Configuration::JoystickInput i){
    // setJoystickKey(joystick_down, i);
}

void Configuration::setJoystickAttack1(Configuration::JoystickInput i){
    // setJoystickKey(joystick_attack1, i);
}

void Configuration::setJoystickAttack2(Configuration::JoystickInput i){
    // setJoystickKey(joystick_attack2, i);
}

void Configuration::setJoystickAttack3(Configuration::JoystickInput i){
    // setJoystickKey(joystick_attack3, i);
}

void Configuration::setJoystickAttack4(Configuration::JoystickInput i){
    // setJoystickKey(joystick_attack4, i);
}

void Configuration::setJoystickAttack5(Configuration::JoystickInput i){
    // setJoystickKey(joystick_attack5, i);
}

void Configuration::setJoystickAttack6(Configuration::JoystickInput i){
    // setJoystickKey(joystick_attack6, i);
}

void Configuration::setJoystickJump(Configuration::JoystickInput i){
    // setJoystickKey(joystick_jump, i);
}

void Configuration::setJoystickQuit(Configuration::JoystickInput i){
    // setJoystickKey(joystick_quit, i);
}

Configuration::JoystickInput Configuration::getJoystickRight(int config){
    return getJoystickKey(config, "right", Joystick::Right);
}

Configuration::JoystickInput Configuration::getJoystickLeft(int config){
    return getJoystickKey(config, "left", Joystick::Left);
}

Configuration::JoystickInput Configuration::getJoystickUp(int config){
    return getJoystickKey(config, "up", Joystick::Up);
}

Configuration::JoystickInput Configuration::getJoystickDown(int config){
    return getJoystickKey(config, "down", Joystick::Down);
}

Configuration::JoystickInput Configuration::getJoystickAttack1(int config){
    return getJoystickKey(config, "attack1", Joystick::Button1);
}

Configuration::JoystickInput Configuration::getJoystickAttack2(int config){
    return getJoystickKey(config, "attack2", Joystick::Button2);
}

Configuration::JoystickInput Configuration::getJoystickAttack3(int config){
    return getJoystickKey(config, "attack3", Joystick::Button3);
}

Configuration::JoystickInput Configuration::getJoystickAttack4(int config){
    return getJoystickKey(config, "attack4", Joystick::Button4);
}

Configuration::JoystickInput Configuration::getJoystickAttack5(int config){
    return getJoystickKey(config, "attack5", Joystick::Button5);
}

Configuration::JoystickInput Configuration::getJoystickAttack6(int config){
    return getJoystickKey(config, "attack6", Joystick::Button6);
}

Configuration::JoystickInput Configuration::getJoystickJump(int config){
    return getJoystickKey(config, "jump", Joystick::Button4);
}

Configuration::JoystickInput Configuration::getJoystickQuit(int config){
    return getJoystickKey(config, "quit", Joystick::Quit);
}

string Configuration::getCurrentGame(){
    return getProperty(config_current_game, string(""));
}

/* assumes the directory is readable and has a dir/dir.txt in it */
void Configuration::setCurrentGame(const std::string & str){
    setProperty(config_current_game, str);
    saveConfiguration();
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

static Token * removeDuplicates(Token * token){
    /* TODO */
    return token;
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
        data = removeDuplicates(head->copy());

#if 0
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
                /*
            } else if (*n == config_mugen_motif){
                string motif;
                n->view() >> motif;
                setMugenMotif(motif);
                */
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
#endif
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Notice: Could not load configuration file " << Storage::instance().configFile().path() << ": " << le.getTrace() << endl;
    } catch ( const TokenException & t ){
        Global::debug( 0 ) << "Notice: could not open configuration file '" << Storage::instance().configFile().path() << "': " << t.getTrace() << endl;
    }
}

/* todo: combine saveKeyboard and saveJoystick, probably using a templated function */
/*
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
    return NULL;
}
*/

/*
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
*/

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

    if (data != NULL){
        ofstream out(Storage::instance().configFile().path().c_str(), ios::trunc | ios::out);
        if (! out.bad()){
            data->toString(out, string(""));
            out << endl;
            out.close();
        }
    }
}

Util::ReferenceCount<Menu::FontInfo> Configuration::menuFont;
bool Configuration::joystickEnabled = true;
// std::map<std::string, std::string> Configuration::properties;
// std::string Configuration::mugenMotif = "default";
/* Original default was 40 */
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

static std::string last(const vector<string> & what){
    return what.at(what.size() - 1);
}

/* Create a token with the given path and give it a value */
template <class Value>
static Token * createToken(const string & path, const Value & value){
    Token * out = createToken(path);
    *out << value;
    return out;
}

template <class Value, class Value2>
static Token * createToken(const string & path, const Value & value, const Value2 & value2){
    Token * out = createToken(path);
    *out << value;
    *out << value2;
    return out;
}

static void updateToken(Token * data, const std::string & path, Token * add){
    if (data == NULL){
        delete add;
    }

    Token * found = data->findToken(path);
    /* See if the token already exists. If it does then remove it from its
     * parent and add a new token to the parent with the updated value.
     */
    if (found != NULL){
        Token * parent = found->getParent();
        parent->removeToken(found);
        parent->addToken(add);
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

                /* If we run out of found paths then create a token with an empty
                 * value for the unfound paths we have so far and call updateToken
                 * again. This time the entire path will be there
                 * so the 'found = ...' logic will work.
                 */
                start->addToken(createToken(out.str(), string("")));
                updateToken(data, path, add);
                return;
            } else {
                start = next;
            }
        }

        /* It probably shouldn't be possible to get here. If the entire
         * path was there then the findToken() logic above should have worked.
         */
        start->addToken(add);
    }
}

template <class Value>
static void updateToken(Token * data, const string & path, const Value & value){
    if (data == NULL){
        return;
    }

    updateToken(data, path, createToken(last(split(path, '/')), value));
}

void Configuration::setProperty(const string & name, const string & value){
    updateToken(data.raw(), string(config_configuration) + "/" + name, value);
    saveConfiguration();
}

static Token * getPropertyX(Token * data, const std::string & path){
    return data->findToken(string(config_configuration) + "/" + path);
}

template <class Out>
static Out getPropertyX(Token * data, const std::string & path, const Out & defaultValue){
    if (data == NULL){
        return defaultValue;
    }
    Out out;
    if (data->match(string(config_configuration) + "/" + path, out)){
        return out;
    }
    updateToken(data, string(config_configuration) + "/" + path, defaultValue);
    return defaultValue;
}

int Configuration::getProperty(const std::string & path, int defaultValue){
    return getPropertyX(data.raw(), path, defaultValue);
}

void Configuration::setProperty(const std::string & path, int value){
    updateToken(data.raw(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

void Configuration::setProperty(const std::string & path, Token * value){
    updateToken(data.raw(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

double Configuration::getProperty(const std::string & path, double defaultValue){
    return getPropertyX(data.raw(), path, defaultValue);
}

void Configuration::setProperty(const std::string & path, double value){
    updateToken(data.raw(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

bool Configuration::getProperty(const std::string & path, bool defaultValue){
    return getPropertyX(data.raw(), path, defaultValue);
}

void Configuration::setProperty(const std::string & path, bool value){
    updateToken(data.raw(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

std::string Configuration::getProperty(const std::string & path, const std::string & defaultValue){
    return getPropertyX(data.raw(), path, defaultValue);
}

double Configuration::getGameSpeed(){
    return getProperty(config_game_speed, 1.0);
}

void Configuration::setGameSpeed(double s){
    setProperty(config_game_speed, s);
    saveConfiguration();
}

bool Configuration::getInvincible(){
    return getProperty(config_invincible, false);
}

void Configuration::setInvincible(bool i){
    setProperty(config_invincible, i);
    saveConfiguration();
}

bool Configuration::getFullscreen(){
/* Defaults for all configuration options */
#if defined(PS3) || defined(ANDROID) || defined(IPHONE)
    return getProperty(config_fullscreen, true);
#else
    return getProperty(config_fullscreen, false);
#endif
}

void Configuration::setFullscreen(bool f){
    setProperty(config_fullscreen, f);
    saveConfiguration();
}

int Configuration::getLives(){
    return getProperty(config_lives, 4);
}

void Configuration::setLives(int l){
    setProperty(config_lives, l);
    saveConfiguration();
}
	
int Configuration::getNpcBuddies(){
    return getProperty(config_npc_buddies, 1);
}

void Configuration::setNpcBuddies(int i){
    setProperty(config_npc_buddies,i);
    saveConfiguration();
}
        
Configuration::PlayMode Configuration::getPlayMode(){
    string mode = getProperty("play-mode", string(config_cooperative));
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
    return getProperty("language", string());
}

void Configuration::setLanguage(const std::string & str){
    setProperty("language", str);
    // language = str;
    saveConfiguration();
}


/* In case this is ever useful again */
/*
#ifdef MINPSPW
// default resolution for the psp is 480x272
int Configuration::screen_width = 480;
int Configuration::screen_height = 272;
#else
int Configuration::screen_width = 640;
int Configuration::screen_height = 480;
#endif
*/

/* TODO: All the screen width/height stuff shares a lot of code. Refactor */
void Configuration::setScreenWidth(int i){
    int width = 640;
    int height = 480;
    Token * screen = getPropertyX(data.raw(), config_screen_size);
    if (screen != NULL){
        try{
            screen->view() >> width >> height;
        } catch (const TokenException & fail){
        }
    }
    width = i;
    setProperty(config_screen_size, createToken(config_screen_size, width, height));
    saveConfiguration();
}

int Configuration::getScreenWidth(){
    int width = 640;
    int height = 480;
    Token * screen = getPropertyX(data.raw(), config_screen_size);
    if (screen != NULL){
        try{
            screen->view() >> width >> height;
        } catch (const TokenException & fail){
        }
    } else {
        setProperty(config_screen_size, createToken(config_screen_size, width, height));
    }
    return width;
}

void Configuration::setScreenHeight(int i){
    int width = 640;
    int height = 480;
    Token * screen = getPropertyX(data.raw(), config_screen_size);
    if (screen != NULL){
        try{
            screen->view() >> width >> height;
        } catch (const TokenException & fail){
        }
    }
    height = i;
    setProperty(config_screen_size, createToken(config_screen_size, width, height));
    saveConfiguration();
}

int Configuration::getScreenHeight(){
    int width = 640;
    int height = 480;
    Token * screen = getPropertyX(data.raw(), config_screen_size);
    if (screen != NULL){
        try{
            screen->view() >> width >> height;
        } catch (const TokenException & fail){
        }
    } else {
        setProperty(config_screen_size, createToken(config_screen_size, width, height));
    }
    return height;
}

int Configuration::getSoundVolume(){
    return getProperty(config_sound, 70);
}

void Configuration::setSoundVolume(int volume){
    setProperty(config_sound, volume);
    saveConfiguration();
}

int Configuration::getMusicVolume(){
    return getProperty(config_music, 70);
}

void Configuration::setMusicVolume(int volume){
    setProperty(config_music, volume);
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
    return getProperty(config_quality_filter, string("none"));
}

void Configuration::setQualityFilter(const std::string & filter){
    setProperty(config_quality_filter, filter);
    saveConfiguration();
}
    
int Configuration::getFps(){
    return getProperty(config_fps, 50);
}

void Configuration::setFps(int what){
    setProperty(config_fps, what);
    saveConfiguration();
}
