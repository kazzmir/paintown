#include "r-tech1/configuration.h"
#include "r-tech1/input/keyboard.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/token.h"
#include "r-tech1/file-system.h"
#include "r-tech1/input/input.h"
#include "r-tech1/debug.h"
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
// define_config(current_game, "current-game");
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
static const Joystick::Key InvalidJoystick = Joystick::Invalid;
    
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

static std::string last(const vector<string> & what){
    return what.at(what.size() - 1);
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

/* remove a path if it exists */
static void removeToken(Token * data, const std::string & path){
    Token * found = data->findToken(path);
    /* See if the token already exists. If it does then remove it from its
     * parent and add a new token to the parent with the updated value.
     */
    if (found != NULL){
        Token * parent = found->getParent();
        parent->removeToken(found);
    }
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
        for (unsigned int index = 1; index < paths.size() - 1; index++){
            string where = paths[index];
            Token * next = start->findToken(string("_/") + where);
            if (next == NULL){
                ostringstream out;
                bool first = true;
                for (unsigned int from = index; from < paths.size(); from++){
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
    updateToken(getRawData(), string(config_configuration) + "/" + name, value);
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
    ignore = ignore;
}

/* hopefully this is only used right before setting all the values
 * since the key mappings are bogus in here'
 */
Configuration::Configuration(){
}

Configuration::Configuration(const Configuration & config){
    menuFont = config.menuFont;
}

Configuration & Configuration::operator=(const Configuration & config){
    Disable disable;
    setMenuFont(config.getMenuFont());
    return *this;
}
    
Configuration::~Configuration(){
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

static string joystickKeyName(Joystick::Key key){
    switch (key){
        case Joystick::Up: return "up";
        case Joystick::Down: return "down";
        case Joystick::Left: return "left";
        case Joystick::Right: return "right";
        case Joystick::Button1: return "button1";
        case Joystick::Button2: return "button2";
        case Joystick::Button3: return "button3";
        case Joystick::Button4: return "button4";
        case Joystick::Button5: return "button5";
        case Joystick::Button6: return "button6";
        case Joystick::Start: return "start";
        case Joystick::Quit: return "quit";
        case Joystick::Invalid: return "invalid";
    }
    return "?";
}

static string removeQuotes(string input){
    size_t find = input.find('"');
    while (find != string::npos){
        input.erase(find, 1);
    }
    return input;
}

static string joystickPath(Joystick::Key key, int config, const std::string & name){
    ostringstream base;
    base << config_configuration << "/" << config_input << "/" << INPUT_TYPE << "/";
    base << "joystick" << "/" << config << "/";
    base << removeQuotes(name) << "/" << joystickKeyName(key);
    return base.str();
}
    
void Configuration::setCustomButton(Joystick::Key key, int config, const std::string & name, int button){
    if (key != Joystick::Invalid){
        std::string base = joystickPath(key, config, name);
        removeToken(getRawData(), base);
        updateToken(getRawData(), base + "/button", button);
        saveConfiguration();
    }
}
    
bool Configuration::getCustomButton(Joystick::Key key, int config, const std::string & name, int & button){
    return getRawData()->match(joystickPath(key, config, name) + "/button", button);
}
    
bool Configuration::getCustomAxis(Joystick::Key key, int config, const std::string & name, int & stick, int & axis, double & low, double & high){
    std::string base = joystickPath(key, config, name);

    return getRawData()->match(base + "/stick", stick) &&
           getRawData()->match(base + "/axis", axis) &&
           getRawData()->match(base + "/low", low) &&
           getRawData()->match(base + "/high", high);
}
    
void Configuration::setCustomAxis(Joystick::Key key, int config, const string & name, int stick, int axis, double low, double high){
    if (key != Joystick::Invalid){
        std::string base = joystickPath(key, config, name);

        removeToken(getRawData(), base);
        updateToken(getRawData(), base + "/stick", stick);
        updateToken(getRawData(), base + "/axis", axis);
        updateToken(getRawData(), base + "/low", low);
        updateToken(getRawData(), base + "/high", high);
        saveConfiguration();
    }
}

void Configuration::setKey(int config, const string & name, int value){
    if (value != InvalidKey){
        ostringstream path;
        path << config_configuration << "/" << config_input << "/" << INPUT_TYPE << "/" << config << "/keys/" << name;
        updateToken(getRawData(), path.str(), value);
        saveConfiguration();
    }
}

void Configuration::setRight(int config, int i){
    setKey(config, "right", i);
}

void Configuration::setLeft(int config, int i){
    setKey(config, "left", i);
}

void Configuration::setUp(int config, int i){
    setKey(config, "up", i);
}

void Configuration::setDown(int config, int i){
    setKey(config, "down", i);
}

void Configuration::setAttack1(int config, int i){
    setKey(config, "attack1", i);
}

void Configuration::setAttack2(int config, int i){
    setKey(config, "attack2", i);
}

void Configuration::setAttack3(int config, int i){
    setKey(config, "attack3", i);
}

void Configuration::setAttack4(int config, int i){
    setKey(config, "attack4", i);
}

void Configuration::setAttack5(int config, int i){
    setKey(config, "attack5", i);
}

void Configuration::setAttack6(int config, int i){
    setKey(config, "attack6", i);
}

void Configuration::setJump(int config, int i){
    setKey(config, "jump", i);
}

int Configuration::getKey(int config, const string & name, int defaultValue){
    ostringstream path;
    path << config_input << "/" << INPUT_TYPE << "/" << config << "/keys/" << name;
    return getProperty(path.str(), defaultValue);
}

/* this nonsense is just to convert a regular integer into an enum */
/*
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
        case Joystick::Button5 : return Joystick::Button5;
        case Joystick::Button6 : return Joystick::Button6;
        case Joystick::Start : return Joystick::Start;
        case Joystick::Quit : return Joystick::Quit;
    }
    return Joystick::Invalid;
}
*/

/*
Configuration::JoystickInput Configuration::getJoystickKey(int config, const string & name, JoystickInput defaultValue){
    ostringstream path;
    path << config_input << "/" << config << "/joystick/" << name;
    return intToJoystick(getProperty(path.str(), defaultValue));
}
*/

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

/*
void Configuration::setJoystickKey(int config, const std::string & name, const Configuration::JoystickInput & what){
    if (what != Joystick::Invalid){
        ostringstream path;
        path << config_configuration << "/" << config_input << "/" << config << "/joystick/" << name;
        updateToken(data.raw(), path.str(), what);
        saveConfiguration();
    }
}

void Configuration::setJoystickRight(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "right", i);
}

void Configuration::setJoystickLeft(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "left", i);
}

void Configuration::setJoystickUp(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "up", i);
}

void Configuration::setJoystickDown(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "down", i);
}

void Configuration::setJoystickAttack1(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "attack1", i);
}

void Configuration::setJoystickAttack2(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "attack2", i);
}

void Configuration::setJoystickAttack3(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "attack3", i);
}

void Configuration::setJoystickAttack4(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "attack4", i);
}

void Configuration::setJoystickAttack5(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "attack5", i);
}

void Configuration::setJoystickAttack6(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "attack6", i);
}

void Configuration::setJoystickJump(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "jump", i);
}

void Configuration::setJoystickQuit(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "quit", i);
}

void Configuration::setJoystickStart(int config, Configuration::JoystickInput i){
    setJoystickKey(config, "start", i);
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

Configuration::JoystickInput Configuration::getJoystickStart(int config){
    return getJoystickKey(config, "start", Joystick::Start);
}
*/

/*
string Configuration::getCurrentGame(){
    return getProperty(config_current_game, string(""));
}

/ * assumes the directory is readable and has a dir/dir.txt in it * /
void Configuration::setCurrentGame(const std::string & str){
    setProperty(config_current_game, str);
    saveConfiguration();
}
*/

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

Token * Configuration::getRawData(){
    if (data == NULL){
        loadConfigurations();
        // data = new Token();
    }
    return data.raw();
}

void Configuration::loadConfigurations(){
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
    } catch (const LoadException & le){
        Global::debug(0) << "Notice: Could not load configuration file " << Storage::instance().configFile().path() << ": " << le.getTrace() << endl;
    } catch (const TokenException & t){
        Global::debug(0) << "Notice: could not open configuration file '" << Storage::instance().configFile().path() << "': " << t.getTrace() << endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Notice: could not open configuration file '" << Storage::instance().configFile().path() << "': " << fail.getTrace() << endl;
    }
}

void Configuration::saveConfiguration(){
    if (!save){
        return;
    }

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

int Configuration::getProperty(const std::string & path, int defaultValue){
    return getPropertyX(getRawData(), path, defaultValue);
}

void Configuration::setProperty(const std::string & path, int value){
    updateToken(getRawData(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

void Configuration::setProperty(const std::string & path, Token * value){
    updateToken(getRawData(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

Token * Configuration::getProperty(const std::string & path){
    return getPropertyX(getRawData(),path);
}

double Configuration::getProperty(const std::string & path, double defaultValue){
    return getPropertyX(getRawData(), path, defaultValue);
}

void Configuration::setProperty(const std::string & path, double value){
    updateToken(getRawData(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

bool Configuration::getProperty(const std::string & path, bool defaultValue){
    return getPropertyX(getRawData(), path, defaultValue);
}

void Configuration::setProperty(const std::string & path, bool value){
    updateToken(getRawData(), string(config_configuration) + "/" + path, value);
    saveConfiguration();
}

std::string Configuration::getProperty(const std::string & path, const std::string & defaultValue){
    return getPropertyX(getRawData(), path, defaultValue);
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
        setProperty("play-mode", string(config_cooperative));
    } else if (mode == Configuration::FreeForAll){
        setProperty("play-mode", string(config_free_for_all));
    }
}
    
std::string Configuration::getLanguage(){
    return getProperty("language", string());
}

void Configuration::setLanguage(const std::string & str){
    setProperty("language", str);
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

#ifdef GCW0
static int DefaultWidth = 320;
static int DefaultHeight = 240;
#else
static int DefaultWidth = 640;
static int DefaultHeight = 480;
#endif

/* TODO: All the screen width/height stuff shares a lot of code. Refactor */
void Configuration::setScreenWidth(int i){
    int width = DefaultWidth;
    int height = DefaultHeight;
    Token * screen = getPropertyX(getRawData(), config_screen_size);
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
    int width = DefaultWidth;
    int height = DefaultHeight;
    Token * screen = getPropertyX(getRawData(), config_screen_size);
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
    int width = DefaultWidth;
    int height = DefaultHeight;
    Token * screen = getPropertyX(getRawData(), config_screen_size);
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
    int width = DefaultWidth;
    int height = DefaultHeight;
    Token * screen = getPropertyX(getRawData(), config_screen_size);
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
