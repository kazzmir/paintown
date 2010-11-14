#include "configuration.h"
#include "input/keyboard.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/file-system.h"
#include "input/input.h"
#include "globals.h"
#include "object/animation.h"
#include "object/object.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>

#include <map>

#ifdef USE_SDL
static const std::string INPUT_TYPE = "SDL";
#endif
#ifdef USE_ALLEGRO
static const std::string INPUT_TYPE = "Allegro";
#endif

/* text that appears in the config file */
#define define_config(n,str) static const char * config_##n = str
define_config(attack1, "attack1");
define_config(attack2, "attack2");
define_config(attack3, "attack3");
define_config(configuration, "configuration");
define_config(cooperative, "cooperative");
define_config(current_game, "current-game");
define_config(down, "down");
define_config(free_for_all, "free-for-all");
define_config(fullscreen, "fullscreen");
define_config(game_speed, "game-speed");
define_config(invincible, "invincible");
define_config(jump, "jump");
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
define_config(language, "language");
/* version of the game: 3.3, 3.4, 4.24 */
define_config(version, "version");
#undef def_config

using namespace std;

static const int InvalidKey = 0;
static const Configuration::JoystickInput InvalidJoystick = Joystick::Invalid;

Configuration Configuration::defaultPlayer1Keys(){
	Configuration config;
	config.setRight( Keyboard::Key_RIGHT );
	config.setLeft( Keyboard::Key_LEFT );
	config.setUp( Keyboard::Key_UP );
	config.setDown( Keyboard::Key_DOWN );
	config.setAttack1( Keyboard::Key_A );
	config.setAttack2( Keyboard::Key_S );
	config.setAttack3( Keyboard::Key_D );
	config.setJump( Keyboard::Key_SPACE );

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
	config.setJoystickJump(Joystick::Button4);

	return config;
}

Configuration Configuration::defaultPlayer2Keys(){
	Configuration config;
	config.setRight( Keyboard::Key_L );
	config.setLeft( Keyboard::Key_J );
	config.setUp( Keyboard::Key_I );
	config.setDown( Keyboard::Key_COMMA );
	config.setAttack1( Keyboard::Key_R );
	config.setAttack2( Keyboard::Key_T );
	config.setAttack3( Keyboard::Key_Y );
	config.setJump( Keyboard::Key_B );
	return config;
}

static map< int, Configuration * > configs;

Configuration & Configuration::config( int set ){
    if ( configs[ set ] == NULL ){
        configs[ set ] = new Configuration();
        switch( set ){
            case 0 : {
                *configs[ set ] = defaultPlayer1Keys();
                break;
            }
            case 1 : {
                *configs[ set ] = defaultPlayer2Keys();
                break;
            }
        }
    }
    return *configs[ set ];
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
jump(InvalidKey),
joystick_right(Joystick::Invalid),
joystick_left(Joystick::Invalid),
joystick_up(Joystick::Invalid),
joystick_down(Joystick::Invalid),
joystick_attack1(Joystick::Invalid),
joystick_attack2(Joystick::Invalid),
joystick_attack3(Joystick::Invalid),
joystick_jump(Joystick::Invalid){
}

Configuration::Configuration( const Configuration & config ):
right( config.getRight() ),
left( config.getLeft() ),
up( config.getUp() ),
down( config.getDown() ),
attack1( config.getAttack1() ),
attack2( config.getAttack2() ),
attack3( config.getAttack3() ),
jump( config.getJump() ),
joystick_right( config.getJoystickRight() ),
joystick_left( config.getJoystickLeft() ),
joystick_up( config.getJoystickUp() ),
joystick_down( config.getJoystickDown() ),
joystick_attack1( config.getJoystickAttack1() ),
joystick_attack2( config.getJoystickAttack2() ),
joystick_attack3( config.getJoystickAttack3() ),
joystick_jump( config.getJoystickJump() )
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
	
Configuration & Configuration::operator=( const Configuration & config ){
	setRight( config.getRight() );
	setLeft( config.getLeft() );
	setUp( config.getUp() );
	setDown( config.getDown() );
	setAttack1( config.getAttack1() );
	setAttack2( config.getAttack2() );
	setAttack3( config.getAttack3() );
	setJump( config.getJump() );

    setJoystickRight( config.getJoystickRight() );
	setJoystickLeft( config.getJoystickLeft() );
	setJoystickUp( config.getJoystickUp() );
	setJoystickDown( config.getJoystickDown() );
	setJoystickAttack1( config.getJoystickAttack1() );
	setJoystickAttack2( config.getJoystickAttack2() );
	setJoystickAttack3( config.getJoystickAttack3() );
	setJoystickJump( config.getJoystickJump() );

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
        case Input::Jump : return this->jump;
        default : return -1;
    }
}
        
Util::ReferenceCount<Menu::FontInfo> Configuration::getMenuFont(){
    return menuFont;
}

void Configuration::setMenuFont(const Util::ReferenceCount<Menu::FontInfo> & info){
    menuFont = info;
}

void Configuration::setMenuFontWidth(int x){
    menuFontWidth = x;
}

int Configuration::getMenuFontWidth(){
    return menuFontWidth;
}

void Configuration::setMenuFontHeight(int x){
    menuFontHeight = x;
}

int Configuration::getMenuFontHeight(){
    return menuFontHeight;
}

void Configuration::setRight( int i ){
    if (i != InvalidKey){
        right = i;
        saveConfiguration();
    }
}

void Configuration::setLeft( int i ){
    if (i != InvalidKey){
        left = i;
        saveConfiguration();
    }
}

void Configuration::setUp( int i ){
    if (i != InvalidKey){
        up = i;
        saveConfiguration();
    }
}

void Configuration::setDown( int i ){
    if (i != InvalidKey){
        down = i;	
        saveConfiguration();
    }
}

void Configuration::setAttack1( int i ){
    if (i != InvalidKey){
        attack1 = i;
        saveConfiguration();
    }
}

void Configuration::setAttack2( int i ){
    if (i != InvalidKey){
        attack2 = i;
        saveConfiguration();
    }
}

void Configuration::setAttack3( int i ){
    if (i != InvalidKey){
        attack3 = i;
        saveConfiguration();
    }
}

void Configuration::setJump( int i ){
    if (i != InvalidKey){
        jump = i;
        saveConfiguration();
    }
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

int Configuration::getJump() const {
    return jump;
}

void Configuration::setJoystickRight(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_right = i;
    }
}

void Configuration::setJoystickLeft(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_left = i;
    }
}

void Configuration::setJoystickUp(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_up = i;
    }
}

void Configuration::setJoystickDown(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_down = i;
    }
}

void Configuration::setJoystickAttack1(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_attack1 = i;
    }
}

void Configuration::setJoystickAttack2(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_attack2 = i;
    }
}

void Configuration::setJoystickAttack3(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_attack3 = i;
    }
}

void Configuration::setJoystickJump(Configuration::JoystickInput i ){
    if (i != Joystick::Invalid){
        joystick_jump = i;
    }
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

Configuration::JoystickInput Configuration::getJoystickJump() const {
    return joystick_jump;
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
    }
    return Joystick::Invalid;
}

void Configuration::loadConfigurations(){
    try{
        Filesystem::AbsolutePath file = Filesystem::configFile();
        TokenReader tr(file.path());
        Token * head = tr.readToken();
        if (*head != config_configuration){
            throw LoadException(__FILE__, __LINE__, string("Config file ") + Filesystem::configFile().path() + " does not use the configuration format" );
        }
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
                    attack2, attack3, jump;
                right = left = down = up = attack1 = attack2
                      = attack3 = jump = InvalidKey;

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
                    } else if ( *thing == config_jump){
                        thing->view() >> jump;
                    }
                }
                if ( number == -1 ){
                    /* should use config_number here */
                    throw LoadException(__FILE__, __LINE__, string("Config file ") + Filesystem::configFile().path() + " does not specifiy (number #) for a keyboard-configuration" );
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
                    myconfig.setJump(jump);
                }
            } else if ( *n == config_joystick_configuration ){
                int number = -1;
                JoystickInput right, left, down, up, attack1,
                    attack2, attack3, jump;
                right = left = down = up = attack1 = attack2
                      = attack3 = jump = InvalidJoystick;
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
                    } else if ( *thing == config_jump){
                        thing->view() >> temp;
                        jump = intToJoystick(temp);
                    }
                }
                if ( number == -1 ){
                    /* should use config_number here */
                    throw LoadException(__FILE__, __LINE__, string("Config file ") + Filesystem::configFile().path() + " does not specifiy (number #) for a joystick-configuration" );
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
                    myconfig.setJoystickJump(jump);
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
            } else if (*n == config_menu_font_width){
                int x;
                n->view() >> x;
                setMenuFontWidth(x);
            } else if (*n == config_menu_font_height){
                int x;
                n->view() >> x;
                setMenuFontHeight(x);
            } else if (*n == config_current_game){
                string game;
                n->view() >> game;
                setCurrentGame(game);
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
                string value;
                try{
                    n->view() >> value;
                    Configuration::setProperty(n->getName(), value);
                } catch (const TokenException & e){
                    /* ignore errors */
                }
            }
        }
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Notice: Could not load configuration file " << Filesystem::configFile().path() << ": " << le.getTrace() << endl;
    } catch ( const TokenException & t ){
        Global::debug( 0 ) << "Notice: could not open configuration file '" << Filesystem::configFile().path() << "': " << t.getTrace() << endl;
    }
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
                                 config_attack3, config_jump};

    get_func funcs[] = {&Configuration::getLeft, &Configuration::getRight,
                        &Configuration::getUp, &Configuration::getDown,
                        &Configuration::getAttack1, &Configuration::getAttack2,
                        &Configuration::getAttack3, &Configuration::getJump,
    };

    for ( unsigned int i = 0; i < sizeof(func_names)/sizeof(char*); i++ ){
        Token * button = new Token();
        *button << func_names[ i ] << (configuration->*(funcs[i]))();
        config->addToken( button );
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
                                 config_attack3, config_jump};

    get_func funcs[] = {&Configuration::getJoystickLeft, &Configuration::getJoystickRight,
                        &Configuration::getJoystickUp, &Configuration::getJoystickDown,
                        &Configuration::getJoystickAttack1, &Configuration::getJoystickAttack2,
                        &Configuration::getJoystickAttack3, &Configuration::getJoystickJump,
    };

    for ( unsigned int i = 0; i < sizeof(func_names)/sizeof(char*); i++ ){
        Token * button = new Token();
        *button << func_names[ i ] << (configuration->*(funcs[i]))();
        config->addToken( button );
    }

    return config;
}

void Configuration::saveConfiguration(){

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
    head.addToken( fullscreen );

    Token * screen = new Token();
    *screen << config_screen_size << Configuration::getScreenWidth() << Configuration::getScreenHeight();
    head.addToken(screen);

    *(head.newToken()) << config_version << Global::getVersionString();

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

    Token * lives = new Token();
    *lives << config_lives << Configuration::getLives();
    head.addToken(lives);

    Token * game = new Token();
    *game << config_current_game << Configuration::getCurrentGame();
    head.addToken(game);

    Token * npc = new Token();
    *npc << config_npc_buddies << Configuration::getNpcBuddies();
    head.addToken( npc );

    for (map<string, string>::iterator it = properties.begin(); it != properties.end(); it++){
        string name = (*it).first;
        string value = (*it).second;
        Token * property = new Token();
        *property << name << value;
        head.addToken(property);
    }

    ofstream out(Filesystem::configFile().path().c_str(), ios::trunc | ios::out );
    if ( ! out.bad() ){
        head.toString( out, string("") );
        out.close();
    }
}

/* Defaults for all configuration options */
double Configuration::gamespeed = 1.0;
bool Configuration::invincible = false;
bool Configuration::fullscreen = false;
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
Util::ReferenceCount<Menu::FontInfo> Configuration::menuFont = new Menu::DefaultFontInfo();
int Configuration::menuFontWidth = 24;
int Configuration::menuFontHeight = 24;
int Configuration::soundVolume = 80;
int Configuration::musicVolume = 80;
bool Configuration::joystickEnabled = true;
std::string Configuration::currentGameDir = "paintown";
std::map<std::string, std::string> Configuration::properties;
std::string Configuration::language = "";
// std::string Configuration::menuFont = "fonts/arial.ttf";
// Configuration::PlayMode Configuration::play_mode = Configuration::FreeForAll;

void Configuration::setProperty(string name, string value){
    properties[name] = value;
}

void Configuration::setStringProperty(const std::string & path, const std::string & value){
    setProperty(path, value);
}

std::string Configuration::getStringProperty(const std::string & path, const std::string & defaultValue){
    if (properties.find(path) == properties.end()){
        properties[path] = defaultValue;
    }
    return properties[path];
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
    return play_mode;
}

void Configuration::setPlayMode(Configuration::PlayMode mode){
    play_mode = mode;
}
    
std::string Configuration::getLanguage(){
    return language;
}

void Configuration::setLanguage(const std::string & str){
    language = str;
}

void Configuration::setScreenWidth(int i){
    screen_width = i;
}

int Configuration::getScreenWidth(){
    return screen_width;
}

void Configuration::setScreenHeight(int i){
    screen_height = i;
}

int Configuration::getScreenHeight(){
    return screen_height;
}

int Configuration::getSoundVolume(){
    return soundVolume;
}

void Configuration::setSoundVolume(int volume){
    soundVolume = volume;
}

int Configuration::getMusicVolume(){
    return musicVolume;
}

void Configuration::setMusicVolume(int volume){
    musicVolume = volume;
}

bool Configuration::isJoystickEnabled(){
    return joystickEnabled;
}
    
void Configuration::setJoystickEnabled(bool enabled){
    joystickEnabled = enabled;
}
