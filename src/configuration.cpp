#include "configuration.h"
#include "input/keyboard.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "input/input.h"
#include "globals.h"
#include "object/animation.h"
#include "object/object.h"
#include <sstream>
#include <fstream>
#include <stdlib.h>

#include <map>

#ifdef _WIN32
#define _WIN32_IE 0x400
#include <shlobj.h>
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
define_config(joystick_configuration, "joystick-configuration");
define_config(left, "left");
define_config(lives, "lives");
define_config(menu_font, "menu-font");
define_config(npc_buddies, "npc-buddies");
define_config(number, "number");
define_config(play_mode, "play-mode");
define_config(right, "right");
define_config(screen_size, "screen-size");
define_config(up, "up");
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
	return *this;
}
	
Configuration::~Configuration(){
}

int Configuration::getKey( Input::PaintownInput which, int facing ){
	switch( which ){
        case Input::Forward : {
			if ( facing == Object::FACING_LEFT ) 
                return this->left;
            else 	return this->right;
		}
        case Input::Back : {
			if ( facing == Object::FACING_LEFT )
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
        
std::string Configuration::getMenuFont(){
    return menuFont;
}

void Configuration::setMenuFont(const std::string & str){
    menuFont = str;
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

#ifdef _WIN32
static string configFile(){
	ostringstream str;
	char path[ MAX_PATH ];
	SHGetSpecialFolderPathA( 0, path, CSIDL_APPDATA, false );
	str << path << "/paintown_configuration.txt";
	return str.str();
}
#else
static string configFile(){
	ostringstream str;
	str << getenv( "HOME" ) << "/.paintownrc";
	return str.str();
}
#endif

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
        string file = configFile();
        TokenReader tr( file );
        Token * head = tr.readToken();
        if (*head != config_configuration){
            throw LoadException( string("Config file ") + configFile() + " does not use the configuration format" );
        }
        while ( head->hasTokens() ){
            Token * n;
            *head >> n;
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

                while ( n->hasTokens() ){
                    Token * thing;
                    *n >> thing;
                    if ( *thing == config_number){
                        *thing >> number;
                    } else if ( *thing == config_left){
                        *thing >> left;
                    } else if ( *thing == config_right){
                        *thing >> right;
                    } else if ( *thing == config_down){
                        *thing >> down;
                    } else if ( *thing == config_up){
                        *thing >> up;
                    } else if ( *thing == config_attack1){
                        *thing >> attack1;
                    } else if ( *thing == config_attack2){
                        *thing >> attack2;
                    } else if ( *thing == config_attack3){
                        *thing >> attack3;
                    } else if ( *thing == config_jump){
                        *thing >> jump;
                    }
                }
                if ( number == -1 ){
                    /* should use config_number here */
                    throw LoadException( string("Config file ") + configFile() + " does not specifiy (number #) for a keyboard-configuration" );
                }
                Configuration & myconfig = config(number);
                myconfig.setRight(right);
                myconfig.setLeft(left);
                myconfig.setUp(up);
                myconfig.setDown(down);
                myconfig.setAttack1(attack1);
                myconfig.setAttack2(attack2);
                myconfig.setAttack3(attack3);
                myconfig.setJump(jump);
            } else if ( *n == config_joystick_configuration ){
                int number = -1;
                JoystickInput right, left, down, up, attack1,
                    attack2, attack3, jump;
                right = left = down = up = attack1 = attack2
                      = attack3 = jump = InvalidJoystick;

                while ( n->hasTokens() ){
                    int temp;
                    Token * thing;
                    *n >> thing;
                    if ( *thing == config_number){
                        *thing >> number;
                    } else if ( *thing == config_left){
                        *thing >> temp;
                        left = intToJoystick(temp);
                    } else if ( *thing == config_right){
                        *thing >> temp;
                        right = intToJoystick(temp);
                    } else if ( *thing == config_down){
                        *thing >> temp;
                        down = intToJoystick(temp);
                    } else if ( *thing == config_up){
                        *thing >> temp;
                        up = intToJoystick(temp);
                    } else if ( *thing == config_attack1){
                        *thing >> temp;
                        attack1 = intToJoystick(temp);
                    } else if ( *thing == config_attack2){
                        *thing >> temp;
                        attack2 = intToJoystick(temp);
                    } else if ( *thing == config_attack3){
                        *thing >> temp;
                        attack3 = intToJoystick(temp);
                    } else if ( *thing == config_jump){
                        *thing >> temp;
                        jump = intToJoystick(temp);
                    }
                }
                if ( number == -1 ){
                    /* should use config_number here */
                    throw LoadException( string("Config file ") + configFile() + " does not specifiy (number #) for a joystick-configuration" );
                }
                Configuration & myconfig = config(number);
                myconfig.setJoystickRight(right);
                myconfig.setJoystickLeft(left);
                myconfig.setJoystickUp(up);
                myconfig.setJoystickDown(down);
                myconfig.setJoystickAttack1(attack1);
                myconfig.setJoystickAttack2(attack2);
                myconfig.setJoystickAttack3(attack3);
                myconfig.setJoystickJump(jump);
            } else if ( *n == config_game_speed){
                *n >> gamespeed;
            } else if ( *n == config_invincible){
                *n >> invincible;
            } else if ( *n == config_fullscreen){
                *n >> fullscreen;
            } else if (*n == config_lives){
                *n >> lives;
            } else if (*n == config_menu_font){
                string font;
                *n >> font;
                setMenuFont(font);
            } else if (*n == config_current_game){
                string game;
                *n >> game;
                setCurrentGame(game);
            } else if ( *n == config_npc_buddies){
                *n >> npc_buddies;
            } else if (*n == config_screen_size){
                int w, h;
                *n >> w >> h;
                Configuration::setScreenWidth(w);
                Configuration::setScreenHeight(h);
            } else if (*n == config_play_mode){
                string mode;
                *n >> mode;
                if (mode == config_cooperative){
                    Configuration::setPlayMode(Configuration::Cooperative);
                } else if (mode == config_free_for_all){
                    Configuration::setPlayMode(Configuration::FreeForAll);
                }
            }
        }
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Could not load configuration file " << configFile() << ": " << le.getReason() << endl;
    } catch ( const TokenException & t ){
        Global::debug( 0 ) << "Error loading configuration file '" << configFile() << "': " << t.getReason() << endl;
    }
}

/* todo: combine saveKeyboard and saveJoystick, probably using a templated function */
Token * Configuration::saveKeyboard( int num, Configuration * configuration ){
    typedef int (Configuration::*get_func)() const;
    Token * config = new Token();

    config->addToken( new Token(config_keyboard_configuration, false ) );
    Token * number = new Token();
    *number << config_number;
    config->addToken( number );
    *number << num;

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
    Token * number = new Token();
    *number << config_number;
    config->addToken( number );
    *number << num;

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

    Token * fullscreen = new Token();
    *fullscreen << config_fullscreen << Configuration::getFullscreen();
    head.addToken( fullscreen );

    Token * screen = new Token();
    *screen << config_screen_size << Configuration::getScreenWidth() << Configuration::getScreenHeight();
    head.addToken(screen);

    if (Configuration::getMenuFont() != ""){
        Token * font = new Token();
        *font << config_menu_font << Configuration::getMenuFont();
        head.addToken(font);
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

    ofstream out( configFile().c_str(), ios::trunc | ios::out );
    if ( ! out.bad() ){
        head.toString( out, string("") );
        out.close();
    }
}

double Configuration::gamespeed = 1.0;
bool Configuration::invincible = false;
bool Configuration::fullscreen = false;
int Configuration::lives = 4;
int Configuration::npc_buddies = 1;
Configuration::PlayMode Configuration::play_mode = Configuration::Cooperative;
int Configuration::screen_width = 640;
int Configuration::screen_height = 480;
std::string Configuration::menuFont = "";
std::string Configuration::currentGameDir = "paintown";
// std::string Configuration::menuFont = "fonts/arial.ttf";
// Configuration::PlayMode Configuration::play_mode = Configuration::FreeForAll;

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
