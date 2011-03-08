#include "util/bitmap.h"

#include "options.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "menu.h"
#include "configuration.h"
#include "menu/menu-exception.h"
#include "mugen/menu.h"
#include "mugen/config.h"
#include "util/init.h"

#include "util/music.h"

#include "paintown-engine/object/versus_player.h"
// #include "paintown-engine/object/versus_enemy.h"

#include "paintown-engine/object/object.h"
#include "paintown-engine/object/player.h"
#include "paintown-engine/object/buddy_player.h"

#include "util/input/keyboard.h"
#include "paintown-engine/game/game.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "factory/font_factory.h"
#include "globals.h"
#include "exceptions/shutdown_exception.h"
#include "exceptions/exception.h"
#include "paintown-engine/network/server.h"
#include "paintown-engine/network/client.h"
#include "util/font.h"
#include "util/gui/box.h"
#include "util/thread.h"

#include "util/loading.h"
#include "util/fire.h"
#include "util/input/input-map.h"
#include "util/input/input-manager.h"

#include "openbor/pack-reader.h"

#include "paintown-engine/game/mod.h"

#include "platformer/game.h"

#include <sstream>
#include <algorithm>
#include <time.h>

using namespace std;
using namespace Gui;

Level::LevelInfo doLevelMenu(const std::string dir, const Menu::Context & context){
    vector<Level::LevelInfo> possible = Paintown::Mod::getCurrentMod()->getLevels();
    if (possible.size() == 0){
        throw LoadException(__FILE__, __LINE__, "No level sets defined!");
    }

    if (possible.size() == 1){
        return possible[0];
    }
    
    // FIXME what is this crap? Do we need it?
    /* don't hardcode 60, base it on the size of the font */
    int count = possible.size() * 60;
    /* what is 250?? */
    if (count > 250){
        count = 250;
    }

    try{
        Menu::Menu temp;
        
        int index = 0;
        for ( unsigned int i = 0; i < possible.size(); i++ ){
            OptionLevel *opt = new OptionLevel(0, &index, i);
            opt->setText(possible[i].getName());
            opt->setInfoText("Select a set of levels to play");
            temp.addOption(opt);
        }
        // Run it
        try {
            temp.run(context);
        } catch (const Menu::MenuException & ex){
        }
        return possible[index];
    } catch (const TokenException & ex){
        // Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getReason() << endl;
        throw LoadException(__FILE__, __LINE__, ex, "Could not load levels");
    }
    throw LoadException(__FILE__, __LINE__, "No level chosen!");
}

OptionAdventure::OptionAdventure(const Token *token):
MenuOption(token){
    if ( *token != "adventure" ){
        throw LoadException(__FILE__, __LINE__, "Not an adventure");
    }

    readName(token);
}

OptionAdventure::~OptionAdventure(){
	// Nothing
}

void OptionAdventure::logic(){
}

void OptionAdventure::run(const Menu::Context & context){
    Paintown::Object * player = NULL;
    try{
        //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
        Level::LevelInfo info = doLevelMenu("/levels", context);

        Global::debug(1) << "Selecting players" << endl;
        int remap = 0;
        Filesystem::AbsolutePath path = Paintown::Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap);
        
        Paintown::PlayerFuture future(path, Configuration::getInvincible(), Configuration::getLives(), remap);
        vector<Util::Future<Paintown::Object *> *> players;
        players.push_back(&future);
        Game::realGame(players, info);
    } catch ( const LoadException & le ){
        Global::debug(0) << "Error while loading: " << le.getTrace() << endl;
    } catch (const Exception::Return & ignore){
        throw Menu::Reload(__FILE__, __LINE__);
    }

    /* player will be null if an exception occurred before selectPlayer was called */
    /*
    if ( player != NULL ){
        delete player;
    }
    */
}

OptionAdventureCpu::OptionAdventureCpu(const Token *token):
MenuOption(token){
    if (*token != "adventure-cpu"){
        throw LoadException(__FILE__, __LINE__, "Not an adventure");
    }

    readName(token);
}

OptionAdventureCpu::~OptionAdventureCpu(){
}

void OptionAdventureCpu::logic(){
}

static string nthWord(int i){
    switch (i){
        case 1 : return "first";
        case 2 : return "second";
        case 3 : return "third";
        case 4 : return "fourth";
        case 5 : return "fifth";
        case 6 : return "sixth";
        case 7 : return "seventh";
        case 8 : return "eigth";
        case 9 : return "ninth";
        case 10 : return "tenth";
        default : {
            ostringstream out;
            out << i;
            return out.str();
        }
    }
}

void OptionAdventureCpu::run(const Menu::Context & context){
    int max_buddies = Configuration::getNpcBuddies();

    // Keyboard key;
    Paintown::Object * player = NULL;
    vector<Util::Future<Paintown::Object*>* > futures;
    vector<Paintown::Object *> buddies;
    try{
        Level::LevelInfo info = doLevelMenu("/levels", context);
        
	int remap;
        Filesystem::AbsolutePath path = Paintown::Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap);
        Util::Future<Paintown::Object*> * player = new Paintown::PlayerFuture(path, Configuration::getInvincible(), Configuration::getLives(), remap);
        futures.push_back(player);

        for ( int i = 0; i < max_buddies; i++ ){
            ostringstream out;
            out << "Pick buddy " << nthWord(i+1);
            int remap;
            Filesystem::AbsolutePath path = Paintown::Mod::getCurrentMod()->selectPlayer(out.str(), info, remap);
            Util::Future<Paintown::Object*> * buddy = new Paintown::BuddyFuture(path, player, remap, -(i+2));
            futures.push_back(buddy);
        }

        Game::realGame(futures, info);
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Could not load player: " << le.getTrace() << endl;
    } catch (const Exception::Return & ignore){
        throw Menu::Reload(__FILE__, __LINE__);
    }

    for (vector<Util::Future<Paintown::Object*>*>::iterator it = futures.begin(); it != futures.end(); it++){
        delete *it;
    }
}

OptionChangeMod::OptionChangeMod(const Token *token):
MenuOption(token){
    if ( *token != "change-mod" ){
        throw LoadException(__FILE__, __LINE__, "Not a change mod");
    }

    /* TODO: fix */
    setText("Change mod");
}

OptionChangeMod::~OptionChangeMod(){
	// Nothing
}

void OptionChangeMod::logic(){
}

static bool isModFile(const std::string & path){
    try{
        TokenReader reader(path);
        Global::debug(1) << "Checking for a mod in " << path << endl;
        if (*reader.readToken() == "game"){
            return true;
        }
    } catch (const TokenException & e){
        return false;
    }
    return false;
}

static bool isOpenBorPackfile(Filesystem::AbsolutePath path){
    try{
        Bor::PackReader reader(path);
        return true;
    } catch (const Bor::PackError & error){
        Global::debug(0) << "Error reading pak file: " << endl;
        return false;
    }
}

namespace {

struct ModType{
    enum Kind{
        Paintown,
        Openbor
    };

    ModType(const Filesystem::AbsolutePath & path, Kind type):
    path(path),
    type(type){
    }

    Filesystem::AbsolutePath path; 
    Kind type;
};

}

static vector<ModType> findMods(){
    vector<ModType> mods;

    vector<Filesystem::AbsolutePath> directories = Filesystem::findDirectories(Filesystem::RelativePath("."));
    for (vector<Filesystem::AbsolutePath>::iterator dir = directories.begin(); dir != directories.end(); dir++){
        string file = (*dir).path() + "/" + Filesystem::cleanse(*dir).path() + ".txt";
        if (isModFile(file)){
            mods.push_back(ModType(Filesystem::AbsolutePath(file), ModType::Paintown));
        }
    }

    try{
        vector<Filesystem::AbsolutePath> pakFiles = Filesystem::getFiles(Filesystem::find(Filesystem::RelativePath("paks")), "*", true);
        for (vector<Filesystem::AbsolutePath>::iterator it = pakFiles.begin(); it != pakFiles.end(); it++){
            const Filesystem::AbsolutePath & path = *it;
            if (isOpenBorPackfile(path)){
                Global::debug(0) << "Found openbor pakfile " << path.path() << endl;
                mods.push_back(ModType(path, ModType::Openbor));
            }
        }
    } catch (const Filesystem::NotFound & n){
        Global::debug(0) << "Could not find any pak files: " << n.getTrace() << endl;
    }

    return mods;
}

static string modNamePaintown(const Filesystem::AbsolutePath & path){
    try{
        TokenReader reader(path.path());
        Global::debug(1) << "Checking for a mod in " << path.path() << endl;
        const Token * name_token = reader.readToken()->findToken("game/name");
        if (name_token != NULL){
            string name;
            name_token->view() >> name;
            return name;
        }
        return Filesystem::cleanse(path).path();
    } catch (const TokenException & e){
        return Filesystem::cleanse(path).path();
    }
}

static string modNameOpenbor(const Filesystem::AbsolutePath & path){
    return "OpenBor " + Filesystem::cleanse(path).path();
}

static string modName(const ModType & mod){
    switch (mod.type){
        case ModType::Paintown : return modNamePaintown(mod.path);
        case ModType::Openbor : return modNameOpenbor(mod.path);
        default : return "unknown!!";
    }
}

static void changeMod(const ModType & mod){
    switch (mod.type){
        case ModType::Paintown : {
            string path = mod.path.path();
            size_t slash = path.rfind('/');
            size_t txt = path.rfind(".txt");
            if (slash != string::npos && txt != string::npos){
                string name = path.substr(slash + 1, path.length() - slash - 5);
                Configuration::setCurrentGame(name);
                Paintown::Mod::loadPaintownMod(name);
            } else {
                Global::debug(0) << "Could not change mod to " << path << endl;
            }
            break;
        }
        case ModType::Openbor : {
            Paintown::Mod::loadOpenborMod(mod.path);
            break;
        }
    }
}

void OptionChangeMod::run(const Menu::Context & context){
    try{
        int select = 0;
        Menu::Menu menu;
        vector<ModType> mods = findMods();
        map<int, ModType*> modMap;
        int index = 0;
        std::vector<OptionLevel *> options;
        for (vector<ModType>::iterator it = mods.begin(); it != mods.end(); it++){
            OptionLevel *opt = new OptionLevel(0, &select, index);
            string name = modName(*it);
            modMap[index] = &(*it);
            opt->setText(name);
            opt->setInfoText("Choose this mod");
            if (name.compare(Util::upcase(Configuration::getCurrentGame())) == 0){
                options.insert(options.begin(),opt);
            } else {
                options.push_back(opt);
            }
            index += 1;
        }

        for (std::vector<OptionLevel *>::iterator it = options.begin(); it != options.end(); ++it){
            menu.addOption(*it);
        }

        if (index == 0){
            Global::debug(0) << "No mods found!" << endl;
            return;
        }
        
        try{
            menu.run(context);
        } catch (const Exception::Return & ignore){
            throw Menu::Reload(__FILE__, __LINE__);
        } catch (const Menu::MenuException & ignore){
        }

        changeMod(*modMap[select]);

        // Reload the menu
        throw ReloadMenuException();

    } catch (const LoadException & le){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << le.getTrace() << endl;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << e.getTrace() << endl;
    }
}

/* true if the arguments passed in match todays date.
 * pass 0 for any argument that you don't care about (it will match any date)
 */
static bool todaysDate(int month, int day, int year){
    int currentMonth;
    int currentDay;
    int currentYear;
    time_t result = time(NULL);
    struct tm * local = localtime(&result);
    return (month == 0 || month == (local->tm_mon + 1)) &&
           (day == 0 || day == local->tm_mday) &&
           (year == 0 || year == local->tm_year + 1900);
}

static bool jonBirthday(){
    return todaysDate(3, 25, 0);
}

static bool miguelBirthday(){
    return todaysDate(8, 11, 0);
}

OptionCredits::OptionCredits( const Token * token ):
MenuOption(token),
music(""),
color(Graphics::makeColor(255,255,255)),
title(Graphics::makeColor(0,255,255)){
    /* Always */
    if (jonBirthday()){
        credits.push_back("Happy birthday, Jon!");
        credits.push_back("");
    }

    if (miguelBirthday()){
        credits.push_back("Happy Birthday, Miguel!");
        credits.push_back("");
    }

    credits.push_back("Paintown");
    credits.push_back("");
    credits.push_back("Programming");
    credits.push_back("Jon Rafkind");
    credits.push_back("");
    credits.push_back("Contributions");
    credits.push_back("Miguel Gavidia");
    credits.push_back("");
    credits.push_back("Level design");
    credits.push_back("Jon Rafkind");
    credits.push_back("Miguel Gavidia");
    credits.push_back("");
    credits.push_back("Music");
    credits.push_back("aqua.s3m - Purple Motion");
    credits.push_back("c_heaven.xm - One Man Project");
    credits.push_back("elw-sick.xm - elwood");
    credits.push_back("experience.xm - elwood");
    credits.push_back("fall.xm - elwood");
    credits.push_back("kajahtaa.xm - cube");
    credits.push_back("kilimanz.mod - ???");
    credits.push_back("SM_TechTown.it - SaMPLeMaSTeR");
    credits.push_back("");
    credits.push_back("Email: jon@rafkind.com");
    credits.push_back("");
    
    if ( *token != "credits" ){
	throw LoadException(__FILE__, __LINE__, "Not a credit menu");
    }

    readName(token);
    
    TokenView view = token->view();
    while (view.hasMore()){
	try{
	    const Token * tok;
	    view >> tok;
	    if ( *tok == "music" ) {
                /* Set music for credits */
                tok->view() >> music;
            } else if ( *tok == "background" ) {
                /* Create an image and push it back on to vector */
                std::string temp;
                tok->view() >> temp;
                background = Filesystem::RelativePath(temp);
            } else if ( *tok == "additional" ) {
                std::string str;
                TokenView additionalView = tok->view();
                while (additionalView.hasMore()){
                    additionalView >> str;
                    credits.push_back(str);
                }
            } else if ( *tok == "titlecolor" ) {
                int r,b,g;
                tok->view() >> r >> g >> b;
                title = Graphics::makeColor( r, b, g );
	    } else if ( *tok == "color" ) {
                int r,b,g;
                tok->view() >> r >> g >> b;
                color = Graphics::makeColor( r, b, g );
	    } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
	} catch ( const TokenException & ex ) {
		throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
	} catch ( const LoadException & ex ) {
		throw ex;
	}
    }
	
    input.set(Keyboard::Key_ESC, 0, true, Exit);
    input.set(Joystick::Button2, 0, true, Exit);
}

OptionCredits::~OptionCredits(){
    /*
	if ( background ){
		delete background;
	}
        */
}

void OptionCredits::logic(){
}

void OptionCredits::run(const Menu::Context & context){
    // Keyboard key;
    Graphics::Bitmap backgroundImage(Filesystem::find(background).path());

    const int maxCredits = credits.size();

    Global::speed_counter = 0;
    double min_y = GFX_Y;

    /* use Bitmap::temporaryBitmap here? no! BlitToScreen uses temporaryBitmap */
    Graphics::Bitmap tmp(Menu::Menu::Width, Menu::Menu::Height);
    // Bitmap fireWork(GFX_X, GFX_Y);
    if (! music.empty()){
        //MenuGlobals::setMusic(music);
        if (Music::loadSong(Filesystem::find(Filesystem::RelativePath(music)).path())){
	    Music::pause();
	    Music::play();
	}
    }

    Paintown::Fire fire;

    const Font & vFont = Configuration::getMenuFont()->get(context.getFont()->get());

    bool quit = false;

    Graphics::Bitmap::transBlender(0, 0, 0, 128);

    double think = 0;
    while (!quit){

        InputManager::poll();
        vector<InputMap<CreditKey>::InputEvent> out = InputManager::getEvents(input);
        for (vector<InputMap<CreditKey>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
            const InputMap<CreditKey>::InputEvent & event = *it;
            if (event.enabled){
                if (event.out == Exit){
                    quit = true;
                }
            }
        }

        bool draw = false;
        if (Global::speed_counter > 0){
            think += Global::speed_counter * Global::LOGIC_MULTIPLIER;
            draw = true;

            while (think >= 1.0){
                think -= 1;
                min_y -= 0.9;
                if (min_y < -(int)(maxCredits * vFont.getHeight() * 1.1)){
                    min_y = GFX_Y;
                }
                fire.update();
            }

            Global::speed_counter = 0;
        }

        if (draw){
            /*
            if (background){
                background->Blit(tmp);
            } else {
                tmp.fill(Bitmap::makeColor(0,0,0));
            }
            */
            backgroundImage.Blit(tmp);

            /*
            fire.draw(fireWork);
            fireWork.drawTrans(0, 0, tmp);
            */

            int y = (int) min_y;
            vector<std::string>::iterator b = credits.begin();
            vector<std::string>::iterator e = credits.end();
            bool isTitle = true;
            for ( /**/ ; b != e; b++ ){
                if ( isTitle ){
                    vFont.printf( 100, y, title, tmp, (*b), 0 );
                    isTitle = false;
                } else {
                    vFont.printf( 100, y, color, tmp, (*b), 0 );
                }
                y += vFont.getHeight() + 2;

                if ( (*b).empty() ){
                    isTitle = true;
                }
            }
            
            fire.draw(tmp);

            tmp.BlitToScreen();
        } else {
            Util::rest(1);
        }
    }

    InputManager::waitForRelease(input, Exit);
    throw Menu::Reload(__FILE__, __LINE__);
    // throw Exception::Return(__FILE__, __LINE__);
}

OptionDummy::OptionDummy(const Token *token):
MenuOption(token){
    if ( *token != "dummy" ){
        throw LoadException(__FILE__, __LINE__, "Not dummy option");
    }

    readName(token);

    if (getText().empty()){
        this->setText("Dummy");
    }
}

OptionDummy::OptionDummy( const std::string &name ):
MenuOption(0){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to dummy");
    }
    this->setText(name);
}

OptionDummy::~OptionDummy(){
}

void OptionDummy::logic(){
}

void OptionDummy::run(const Menu::Context & context){
}

OptionFullscreen::OptionFullscreen(const Token *token):
MenuOption(token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "fullscreen" )
		throw LoadException(__FILE__, __LINE__, "Not fullscreen option");

        readName(token);
}

OptionFullscreen::~OptionFullscreen()
{
	// Nothing
}
    
std::string OptionFullscreen::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << (Configuration::getFullscreen() ? "Yes" : "No");
    return out.str();
}

void OptionFullscreen::logic(){;
}

static void changeScreenMode(){
    Configuration::setFullscreen(!Configuration::getFullscreen());
    int gfx = (Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
    Graphics::setGraphicsMode(gfx, Global::getScreenWidth(), Global::getScreenHeight());
}

void OptionFullscreen::run(const Menu::Context & context){
    changeScreenMode();
}

bool OptionFullscreen::leftKey(){
    changeScreenMode();
    return true;
}
bool OptionFullscreen::rightKey(){
    changeScreenMode();
    return true;
}

OptionInvincible::OptionInvincible(const Token *token):
MenuOption(token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "invincible" )
		throw LoadException(__FILE__, __LINE__, "Not invincible option");

        readName(token);
}

OptionInvincible::~OptionInvincible()
{
	// Nothing
}
                
std::string OptionInvincible::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << (Configuration::getInvincible() ? "Yes" : "No");
    return out.str();
}

void OptionInvincible::logic()
{

}

void OptionInvincible::run(const Menu::Context & context){
}

bool OptionInvincible::leftKey(){
	Configuration::setInvincible(!Configuration::getInvincible());
	return true;
}
bool OptionInvincible::rightKey(){
	Configuration::setInvincible(!Configuration::getInvincible());
	return true;
}

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

static Configuration::JoystickInput readJoystick(){
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
            throw Exception::Return(__FILE__, __LINE__);
        }
        Util::rest(1);
    }
    return Joystick::Up;
}

OptionJoystick::OptionJoystick(const Token *token):
MenuOption(token),
name(""),
player(-1),
type(Invalidkey),
keyCode(0){
    if (*token != "joystick"){
        throw LoadException(__FILE__, __LINE__, "Not joystick option");
    }

    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if ( *tok == "name" ){
                tok->view() >> name;
            } else if ( *tok == "player" ) {
                tok->view() >> player;
            } else if ( *tok == "type" ) {
                std::string temp;
                tok->view() >> temp;
                type = convertToKey(temp);
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                tok->print(" ");
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if (name.empty()){
        throw LoadException(__FILE__, __LINE__, "No name set, this option should have a name!");
    }

    if (type == Invalidkey){
        throw LoadException(__FILE__, __LINE__, "Invalid joystick button, should be up, down, left, right, up, down, jump, attack1-6!");
    }

    if (player == -1){
        throw LoadException(__FILE__, __LINE__, "Player not specified in joystick configuration");
    }

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

void OptionJoystick::run(const Menu::Context & context){
    //int x, y, width, height;
    const Font & vFont = Configuration::getMenuFont()->get(context.getFont()->get());
    const char * message = "Press a joystick button!";
    const int width = vFont.textLength(message) + 10;
    const int height = vFont.getHeight() + 10;
    // const int x = (getParent()->getWork()->getWidth()/2) - (width/2);
    // const int y = (getParent()->getWork()->getHeight()/2) - (height/2);
    const int x = Menu::Menu::Width / 2 - width/2;
    const int y = Menu::Menu::Height / 2 - height/2;
    Box dialog;
    dialog.location.setPosition(Gui::AbsolutePoint(0,0));
    dialog.location.setDimensions(vFont.textLength(message) + 10, vFont.getHeight() + 10);
    dialog.location.setRadius(0);
    dialog.colors.body = Graphics::makeColor(0,0,0);
    dialog.colors.bodyAlpha = 200;
    dialog.colors.border = Graphics::makeColor(255,255,255);
    dialog.colors.borderAlpha = 255;
    Graphics::Bitmap temp = Graphics::Bitmap::temporaryBitmap(width,height);
    dialog.render(temp, vFont);
    vFont.printf( 5, 5, Graphics::makeColor(255,255,255), temp, message, -1);
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

static OptionKey::keyType convertToKeyboardKey(const std::string &k){
    std::string temp = k;
    for(unsigned int i=0;i<temp.length();i++){
        temp[i] = tolower(temp[i]);
    }
    if (temp == "up") return OptionKey::up;
    if (temp == "down") return OptionKey::down;
    if (temp == "left") return OptionKey::left;
    if (temp == "right") return OptionKey::right;
    if (temp == "jump") return OptionKey::jump;
    if (temp == "attack1") return OptionKey::attack1;
    if (temp == "attack2") return OptionKey::attack2;
    if (temp == "attack3") return OptionKey::attack3;
    if (temp == "attack4") return OptionKey::attack4;
    if (temp == "attack5") return OptionKey::attack5;
    if (temp == "attack6") return OptionKey::attack6;

    return OptionKey::invalidkey;
}

static int getKey(int player, OptionKey::keyType k){
    switch(k){
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
            return Configuration::config(player).getAttack4();
        case OptionKey::attack5:
            return Configuration::config(player).getAttack5();
        case OptionKey::attack6:
            return Configuration::config(player).getAttack6();
        default:
            break;
    }

    return 0;
}

static void setKey(int player, OptionKey::keyType k, int key){
    switch(k){
        case OptionKey::up:
            Configuration::config(player).setUp(key);
            break;
        case OptionKey::down:
            Configuration::config(player).setDown(key);
            break;
        case OptionKey::left:
            Configuration::config(player).setLeft(key);
            break;
        case OptionKey::right:
            Configuration::config(player).setRight(key);
            break;
        case OptionKey::jump:
            Configuration::config(player).setJump(key);
            break;
        case OptionKey::attack1:
            Configuration::config(player).setAttack1(key);
            break;
        case OptionKey::attack2:
            Configuration::config(player).setAttack2(key);
            break;
        case OptionKey::attack3:
            Configuration::config(player).setAttack3(key);
            break;
        case OptionKey::attack4:
            Configuration::config(player).setAttack4(key);
            break;
        case OptionKey::attack5:
            Configuration::config(player).setAttack5(key);
            break;
        case OptionKey::attack6:
            Configuration::config(player).setAttack6(key);
            break;
        default:
            break;
    }
}

/*
static int readKey( Keyboard & key ){
    int k = key.readKey();
    key.wait();
    return k;
}
*/

OptionKey::OptionKey(const Token *token): MenuOption(token), name(""), player(-1), type(invalidkey), keyCode(0)
{
    if ( *token != "key" )
        throw LoadException(__FILE__, __LINE__, "Not key option");

    TokenView view = token->view();
    while (view.hasMore()) {
        try {
            const Token * tok;
            view >> tok;
            if ( *tok == "name" ) {
                tok->view() >> name;
            } else if ( *tok == "player" ) {
                tok->view() >> player;
            } else if ( *tok == "type" ) {
                std::string temp;
                tok->view() >> temp;
                type = convertToKeyboardKey(temp);
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                tok->print(" ");
            }
        } catch ( const TokenException & ex ){
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if(name.empty())throw LoadException(__FILE__, __LINE__, "No name set, this option should have a name!");
    if(type == invalidkey)throw LoadException(__FILE__, __LINE__, "Invalid key, should be up, down, left, right, up, down, jump, attack1-6!");
    if(player == -1)throw LoadException(__FILE__, __LINE__, "Player not specified in key configuration");

    char temp[255];
    sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
    setText(std::string(temp));
}

OptionKey::~OptionKey(){
	// Nothing
}

void OptionKey::logic(){
    char temp[255];
    sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
    setText(std::string(temp));
}

void OptionKey::run(const Menu::Context & context){
    // Do dialog
    //Box::messageDialog(Menu::Menu::Width, Menu::Menu::Height, "Press a Key!",2);

    /*
    Keyboard key;
    key.wait();
    */
    Graphics::Bitmap temp(Menu::Menu::Width, Menu::Menu::Height);
    // Menu::Context tempContext = context;
    Menu::Context tempContext(context);
    tempContext.initialize();
    Menu::InfoBox keyDialog;
    // keyDialog.setFont(tempContext.getFont());
    //keyDialog.location.set(-1,-1,1,1);
    const int width = temp.getWidth();
    const int height = temp.getHeight();
    const Font & font = Configuration::getMenuFont()->get(context.getFont()->get());
    const int radius = 15;
    keyDialog.setText("Press a Key!");
    keyDialog.location.setDimensions(font.textLength("Press a Key!") + radius, font.getHeight() + radius);
    keyDialog.location.setCenterPosition(Gui::RelativePoint(0, 0));
    // keyDialog.location.setPosition(Gui::AbsolutePoint((width/2)-(keyDialog.location.getWidth()/2), (height/2)-(keyDialog.location.getHeight()/2)));
    // keyDialog.location.setPosition2(Gui::AbsolutePoint((
    keyDialog.location.setRadius(radius);
    keyDialog.colors.body = Graphics::makeColor(0,0,0);
    keyDialog.colors.bodyAlpha = 180;
    keyDialog.colors.border = Graphics::makeColor(255,255,255);
    keyDialog.colors.borderAlpha = 255;
    keyDialog.open();
    InputManager::waitForClear();
    while (!InputManager::anyInput() && keyDialog.isActive()){
        InputManager::poll();
	keyDialog.act(font);
        /*
	if (keyDialog.isActive()){
            InputManager::poll();
	}
        */
	tempContext.act();
	tempContext.render(0, temp);
	keyDialog.render(temp, font);
	temp.BlitToScreen();
    }
    tempContext.finish();
    keyCode = InputManager::readKey();
    setKey(player,type, keyCode);
    InputManager::waitForClear();
}

OptionLevel::OptionLevel(const Token *token, int * set, int value):
MenuOption(token),
set(set),
value(value){
  // Nothing
}

OptionLevel::~OptionLevel(){
}

void OptionLevel::logic(){
}

void OptionLevel::run(const Menu::Context & context){
    *set = value;
    throw Menu::MenuException(__FILE__, __LINE__);
}

OptionLives::OptionLives( const Token * token ):
MenuOption(token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "lives" ){
		throw LoadException(__FILE__, __LINE__,  "Not lives option" );
	}

        readName(token);
}

OptionLives::~OptionLives(){
}
    
std::string OptionLives::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << Configuration::getLives();
    return out.str();
}

void OptionLives::logic(){

}

void OptionLives::run(const Menu::Context & context){
}

bool OptionLives::leftKey(){
	Configuration::setLives(Configuration::getLives() - 1);
	if ( Configuration::getLives() < 1 ){
		Configuration::setLives(1);
	}
	
	return false;
}

bool OptionLives::rightKey(){
	Configuration::setLives( Configuration::getLives() + 1 );
	
	return false;
}

OptionMenu::OptionMenu(const Token *token):
MenuOption(token),
menu(0){
    if (*token != "menu"){
        throw LoadException(__FILE__, __LINE__, "Not a menu");
    }

    if (token->numTokens() == 1){
        std::string temp;
        token->view() >> temp;
        menu = new Menu::Menu(Filesystem::find(Filesystem::RelativePath(temp)));
    } else {
        menu = new Menu::Menu(token);
    }

    this->setText(menu->getName());
    this->setInfoText(menu->getInfo());
    
    // Lets check if this menu is going bye bye
    //if ( menu->checkRemoval() ) setForRemoval(true);
}

OptionMenu::~OptionMenu(){
    // Delete our menu
    if (menu){
        delete menu;
    }
}

void OptionMenu::logic(){
    // Nothing
}

void OptionMenu::run(const Menu::Context & context){
    // Do our new menu
    try{
        menu->run(context);
    } catch (const Exception::Return ignore){
        throw Menu::Reload(__FILE__, __LINE__);
    }
}

OptionMugenMenu::OptionMugenMenu(const Token *token):
MenuOption(token){
    if ( *token != "mugen" ){
        throw LoadException(__FILE__, __LINE__, "Not a mugen motif menu");
    }
    
    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if ( *tok == "name" ){
                // Create an image and push it back on to vector
                std::string temp;
                tok->view() >> temp;
                this->setText(temp);
            } else if (*tok == "motif"){
                // Load Motif from file
                std::string temp;
                // Filename
                tok->view() >> temp;
                // Set the default motif
                try{
                    if (Configuration::getMugenMotif() == "default"){
                        Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(temp));
                    } else {
                        Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(Configuration::getMugenMotif()));
                    }
                } catch (const Filesystem::NotFound & fail){
                    throw LoadException(__FILE__, __LINE__, fail, "Can't load the MUGEN menu");
                }
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } 
    }

    /*
    // Load menu with default motif
    _menu = new MugenMenu(Mugen::Data::getInstance().getMotif());
    // Set this menu as an option
    _menu->setAsOption(true);
    */

    // Lets check if this menu is going bye bye
    //if ( _menu->checkRemoval() ) setForRemoval(true);
}

OptionMugenMenu::~OptionMugenMenu(){
    // Delete our menu
    /*
    if (_menu){
        delete _menu;
    }
    */
}

void OptionMugenMenu::logic(){
	// Nothing
}

void OptionMugenMenu::run(const Menu::Context & context){
    try{
        Mugen::run();
    } catch (const LoadException & le){
        ostringstream out;
        out << "Press ENTER to continue\n";
        out << "\n";
        out << "We are very sorry but an error has occured while trying to load MUGEN.";
        Util::showError(le, out.str());
        InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC);
    }
    throw Menu::Reload(__FILE__, __LINE__);
    // throw Exception::Return(__FILE__, __LINE__);
}

#ifdef HAVE_NETWORKING
OptionNetworkHost::OptionNetworkHost(const Token *token):
MenuOption(token){
    if ( *token != "network-host" ){
        throw LoadException(__FILE__, __LINE__, "Not a network-host");
    }

    readName(token);
}

OptionNetworkHost::~OptionNetworkHost(){
	// Nothing
}

void OptionNetworkHost::logic(){
}

void OptionNetworkHost::run(const Menu::Context & context){
    // Keyboard key;
    try{
        Network::networkServer();
    } catch (const Exception::Return &e){
    }
    /* FIXME */
    /*
    key.clear();
    key.poll();
    key.wait();
    */
    throw Menu::Reload(__FILE__, __LINE__);
    // throw Exception::Return(__FILE__, __LINE__);
}

OptionNetworkJoin::OptionNetworkJoin(const Token *token):
MenuOption(token){
    if ( *token != "network-join" ){
        throw LoadException(__FILE__, __LINE__, "Not a network-join");
    }

    readName(token);
}

OptionNetworkJoin::~OptionNetworkJoin(){
	// Nothing
}

void OptionNetworkJoin::logic(){
}

void OptionNetworkJoin::run(const Menu::Context & context){
    /*
    Keyboard key;
    key.poll();
    key.wait();
    */
    /* FIXME */
    try{
        Network::networkClient();
    } catch (const Exception::Return &r){
    }

    /* 
    key.clear();
    key.poll();
    key.wait();
    */
    throw Menu::Reload(__FILE__, __LINE__);
    // throw Exception::Return(__FILE__, __LINE__);
}
#endif

OptionNpcBuddies::OptionNpcBuddies( const Token * token ):
MenuOption(token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "npc" ){
        throw LoadException(__FILE__, __LINE__,  "Not npc option" );
    }

    readName(token);
}

OptionNpcBuddies::~OptionNpcBuddies(){
	// Nothing
}
        
std::string OptionNpcBuddies::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << Configuration::getNpcBuddies();
    return out.str();
}

void OptionNpcBuddies::logic(){

}

void OptionNpcBuddies::run(const Menu::Context & context){
}

bool OptionNpcBuddies::leftKey(){
	Configuration::setNpcBuddies(Configuration::getNpcBuddies() - 1);
	if ( Configuration::getNpcBuddies() < 1 ){
		Configuration::setNpcBuddies(1);
	}
	
	return false;
}

bool OptionNpcBuddies::rightKey(){
	Configuration::setNpcBuddies( Configuration::getNpcBuddies() + 1 );
	rblue = rgreen = 0;
	return false;
}

OptionPlayMode::OptionPlayMode(const Token *token):
MenuOption(token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "play-mode" ){
        throw LoadException(__FILE__, __LINE__, "Not a play-mode");
    }

    readName(token);
}

OptionPlayMode::~OptionPlayMode(){
    // Nothing
}
    
std::string OptionPlayMode::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": ";

    /* TODO: language translations of these */
    if (Configuration::getPlayMode() == Configuration::FreeForAll){
        out << "Free for all";
    } else if (Configuration::getPlayMode() == Configuration::Cooperative){
        out << "Cooperative";
    }

    return out.str();
}

void OptionPlayMode::logic(){
    
}

void OptionPlayMode::run(const Menu::Context & context){
}
    
void OptionPlayMode::changeMode(){
    if (Configuration::getPlayMode() == Configuration::FreeForAll){
        Configuration::setPlayMode(Configuration::Cooperative);
    } else if (Configuration::getPlayMode() == Configuration::Cooperative){
        Configuration::setPlayMode(Configuration::FreeForAll);
    }
}

bool OptionPlayMode::leftKey(){
    changeMode();
    lblue = lgreen = 0;
    return true;
}

bool OptionPlayMode::rightKey(){
    changeMode();
    rblue = rgreen = 0;
    return true;
}

OptionReturn::OptionReturn(const Token * token):
MenuOption(token){
    if (*token != "return"){
        throw LoadException(__FILE__, __LINE__, "Not a return option");
    }
    readName(token);
}

void OptionReturn::logic(){
}

/* maybe this option is misnamed, but its supposed to quit the current game
 * and go back to the main menu
 */
void OptionReturn::run(const Menu::Context & context){
    throw Exception::Quit(__FILE__, __LINE__);
}

OptionReturn::~OptionReturn(){
}

OptionContinue::OptionContinue(const Token * token):
MenuOption(token){
    if (*token != "continue"){
        throw LoadException(__FILE__, __LINE__, "Not a continue option");
    }
    readName(token);
}

void OptionContinue::logic(){
}

void OptionContinue::run(const Menu::Context & context){
    throw Exception::Return(__FILE__, __LINE__);
}

OptionContinue::~OptionContinue(){
}

OptionQuit::OptionQuit(const Token *token):
MenuOption(token){
    if ( *token != "quit" ){
        throw LoadException(__FILE__, __LINE__, "Not quit option");
    }

    readName(token);
}

OptionQuit::OptionQuit(const std::string &name):
MenuOption(0){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to quit");
    }
    this->setText(name);
}

OptionQuit::~OptionQuit(){
}

void OptionQuit::logic(){
}

void OptionQuit::run(const Menu::Context & context){
    throw ShutdownException();
}


#if defined(WINDOWS) && defined(doesnt_work_yet)
#include <windows.h>
#include <stdio.h>

/* contributed by Roy Underthump from allegro.cc */
static vector<ScreenSize> getScreenResolutions(){
    HWND hwnd;
    HDC  hdc;

    // int iPixelFormat;
    int descerr;
    int retval;

    DEVMODE d;

    PIXELFORMATDESCRIPTOR pfd;

    hwnd = GetDesktopWindow();
    hdc  = GetDC(hwnd);

    vector<ScreenSize> modes;

    for (int i = 0;; i++){
        retval = EnumDisplaySettings(0,i,&d);
        if (!retval){
            break;
        }

        descerr = DescribePixelFormat(hdc, i+1, sizeof(pfd), &pfd);
        if(!descerr){
            continue;
        }

        /*
           printf("\n#%d bpp %d width %d height %d colorbits %d fps %d",i,d.dmBitsPerPel,
           d.dmPelsWidth, d.dmPelsHeight,pfd.cColorBits,d.dmDisplayFrequency);

           if(pfd.dwFlags & PFD_SUPPORT_OPENGL)printf(" OGL OK");
         */
        modes.push_back(ScreenSize(d.dmPelsWidth, d.dmPelsHeight));
    }

    if (modes.empty()){
        modes.push_back(ScreenSize(640,480));
    }

    return modes;
}
#else
static vector<ScreenSize> getScreenResolutions(){
	vector<ScreenSize> modes;
	modes.push_back(ScreenSize(320, 240));
	modes.push_back(ScreenSize(640, 480));
	modes.push_back(ScreenSize(800, 600));
	modes.push_back(ScreenSize(1024, 768));
	modes.push_back(ScreenSize(1280, 1024));
	modes.push_back(ScreenSize(1600, 1200));
	return modes;
}
#endif

static bool doSort(const ScreenSize & a, const ScreenSize & b){
    return (a.w * a.h) < (b.w * b.h);
}

static vector<ScreenSize> sortResolutions(const vector<ScreenSize> & modes){
    vector<ScreenSize> copy(modes);
    std::sort(copy.begin(), copy.end(), doSort);
    return copy;
}

OptionScreenSize::OptionScreenSize(const Token *token):
MenuOption(token),
name(""),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    Global::debug(1) << "Get screen resolution" << endl;
    modes = sortResolutions(getScreenResolutions());

    if (Global::getDebug() >= 1){
        for (vector<ScreenSize>::iterator it = modes.begin(); it != modes.end(); it++){
            Global::debug(1) << "Screen size: " << it->w << " x " << it->h << endl;
        }
    }

    if ( *token != "screen-size" ){
        throw LoadException(__FILE__, __LINE__, "Not a screen-size");
    }

    readName(token);
}

OptionScreenSize::~OptionScreenSize(){
    // Nothing
}

void OptionScreenSize::logic(){
    ostringstream temp;
    temp << "Screen size: " << Configuration::getScreenWidth() << " x " << Configuration::getScreenHeight();
    setText(temp.str());

}

void OptionScreenSize::run(const Menu::Context & context){
}

void OptionScreenSize::setMode(int width, int height){
    if (width != Configuration::getScreenWidth() ||
        height != Configuration::getScreenHeight()){

        Global::debug(1) << "Changing mode to " << width << " x " << height << endl;
        int gfx = Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED;
        int ok = Graphics::setGraphicsMode(gfx, width, height);
        if (ok == 0){
            Global::debug(1) << "Success" << endl;
            Configuration::setScreenWidth(width);
            Configuration::setScreenHeight(height);
        } else {
            Global::debug(1) << "Fail" << endl;
            int ok = Graphics::setGraphicsMode(gfx, Configuration::getScreenWidth(), Configuration::getScreenHeight());
            Global::debug(1) << "Set mode back " << ok << endl;
        }
    }
}

/*
static int modes[][2] = {{640,480}, {800,600}, {1024,768}, {1280,1024}, {1600,1200}};
// static int max_modes = sizeof(modes) / sizeof(int[]);
static int max_modes = 5;
*/

int OptionScreenSize::findMode(int width, int height){
    for (int mode = 0; mode < (int) modes.size(); mode++){
        if (modes[mode].w == width && modes[mode].h == height){
            return mode;
        }
    }
    return -1;
}

bool OptionScreenSize::leftKey(){
    int mode = findMode(Configuration::getScreenWidth(), Configuration::getScreenHeight());
    if (mode >= 1 && mode < (int)modes.size()){
        mode -= 1;
    } else {
        mode = 0;
    }

    setMode(modes[mode].w, modes[mode].h);

    lblue = lgreen = 0;
    return true;
}

bool OptionScreenSize::rightKey(){
    int mode = findMode(Configuration::getScreenWidth(), Configuration::getScreenHeight());
    if (mode >= 0 && mode < (int)modes.size() - 1){
        mode += 1;
    } else {
        mode = 0;
    }

    setMode(modes[mode].w, modes[mode].h);

    rblue = rgreen = 0;
    return true;
}

static string joinPaths(const vector<Filesystem::AbsolutePath> & strings, const string & middle){
    ostringstream out;

    for (vector<Filesystem::AbsolutePath>::const_iterator it = strings.begin(); it != strings.end(); it++){
        out << (*it).path() << middle;
    }

    return out.str();
}

static bool sortInfo(const Util::ReferenceCount<Menu::FontInfo> & info1, 
                     const Util::ReferenceCount<Menu::FontInfo> & info2){
    string name1 = Util::lowerCaseAll(info1->getName());
    string name2 = Util::lowerCaseAll(info2->getName());
    return name1 < name2;
}

static bool isWindows(){
#ifdef WINDOWS
    return true;
#else
    return false;
#endif
}

static bool isOSX(){
#ifdef MACOSX
    return true;
#else
    return false;
#endif
}

template <class X>
static vector<X> operator+(const vector<X> & v1, const vector<X> & v2){
    vector<X> out;
    for (typename vector<X>::const_iterator it = v1.begin(); it != v1.end(); it++){
        out.push_back(*it);
    }
    for (typename vector<X>::const_iterator it = v2.begin(); it != v2.end(); it++){
        out.push_back(*it);
    }
    return out;
}

static vector<Filesystem::AbsolutePath> findSystemFonts(){
    if (isWindows()){
        const char * windows = getenv("windir");
        if (windows != NULL){
            return Filesystem::getFilesRecursive(Filesystem::AbsolutePath(string(windows) + "/fonts"), "*.ttf");
        }
        return vector<Filesystem::AbsolutePath>();
    } else if (isOSX()){
        return Filesystem::getFilesRecursive(Filesystem::AbsolutePath("/Library/Fonts"), "*.ttf");
    } else {
        /* assume unix/linux conventions */
        return Filesystem::getFilesRecursive(Filesystem::AbsolutePath("/usr/share/fonts/truetype"), "*.ttf") + 
               Filesystem::getFilesRecursive(Filesystem::AbsolutePath("/usr/local/share/fonts/truetype"), "*.ttf");

    }
}

static vector<Util::ReferenceCount<Menu::FontInfo> > findFonts(){
    vector<Util::ReferenceCount<Menu::FontInfo> > fonts;
    try{
        Filesystem::AbsolutePath fontsDirectory = Filesystem::find(Filesystem::RelativePath("fonts"));
        Global::debug(1, "fonts") << "Font directory " << fontsDirectory.path() << endl;
        vector<Filesystem::AbsolutePath> ttfFonts = Filesystem::getFiles(fontsDirectory, "*.ttf");
        Global::debug(1, "fonts") << "Found ttf fonts " << joinPaths(ttfFonts, ", ") << endl;
        vector<Filesystem::AbsolutePath> otfFonts = Filesystem::getFiles(fontsDirectory, "*.otf");
        Global::debug(1, "fonts") << "Found otf fonts " << joinPaths(otfFonts, ", ") << endl;

        for (vector<Filesystem::AbsolutePath>::iterator it = ttfFonts.begin(); it != ttfFonts.end(); it++){
            fonts.push_back(new Menu::RelativeFontInfo(Filesystem::cleanse(*it), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight()));
        }

        for (vector<Filesystem::AbsolutePath>::iterator it = otfFonts.begin(); it != otfFonts.end(); it++){
            fonts.push_back(new Menu::RelativeFontInfo(Filesystem::cleanse(*it), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight()));
        }
        
        /* linux specific fonts */
        vector<Filesystem::AbsolutePath> systemFonts = findSystemFonts();
        for (vector<Filesystem::AbsolutePath>::iterator it = systemFonts.begin(); it != systemFonts.end(); it++){
            Global::debug(1) << "Adding system font `" << (*it).path() << "'" << endl;
            fonts.push_back(new Menu::AbsoluteFontInfo(*it, Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight()));
        }

        sort(fonts.begin(), fonts.end(), sortInfo);
        
        // DEFAULT (blank)
        // fonts.insert(fonts.begin(), new Menu::DefaultFontInfo());
        fonts.insert(fonts.begin(), NULL);
    } catch (const Filesystem::NotFound & e){
        throw LoadException(__FILE__, __LINE__, e, "Could not load font");
    }

    return fonts;
}

OptionSelectFont::OptionSelectFont(const Token *token):
MenuOption(token),
typeAdjust(fontName),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "font-select" ){
        throw LoadException(__FILE__, __LINE__, "Not a font selector");
    }

    TokenView view = token->view();
    while (view.hasMore()){
        try{
            const Token * tok;
            view >> tok;
            if ( *tok == "adjust" ){
                std::string temp;
                tok->view() >> temp;
                if ( temp == "name" ) typeAdjust = fontName;
                else if ( temp == "width" ) typeAdjust = fontWidth;
                else if ( temp == "height" ) typeAdjust = fontHeight;
                else throw LoadException(__FILE__, __LINE__, "Incorrect value \"" + temp + "\" in font-select");
            } else {
                Global::debug(3) << "Unhandled menu attribute: " << endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }
}

void OptionSelectFont::open(){
    // Find and set fonts now
    if (typeAdjust == fontName){
        fonts = findFonts();
    }
}
    
void OptionSelectFont::close(){
    if (typeAdjust == fontName){
        /* the user probably loaded a bunch of different fonts that will
         * never be used again, so clear the font cache
         * TODO: dont clear the currently selected font
         */
        FontFactory::clear();
    }
}

OptionSelectFont::~OptionSelectFont(){
    // Nothing
}

void OptionSelectFont::logic(){
    /* FIXME Get current font and display info */
    switch (typeAdjust){
        case fontName:{
	    std::string name;
            if (Configuration::hasMenuFont()){
                name = Configuration::getMenuFont()->getName();
            } else {
                name = "Default";
            }
            setText("Current Font: " + name);
            break;
	}
        case fontWidth:{
            ostringstream temp;
            temp << "Font Width: " << Configuration::getMenuFontWidth();
            setText(temp.str());
            break;
        }
        case fontHeight:{
            ostringstream temp;
            temp << "Font Height: " << Configuration::getMenuFontHeight();
            setText(temp.str());
            break;
        }
        default: break;
    }
    if (lblue < 255){
        lblue += 5;
    }

    if (rblue < 255){
        rblue += 5;
    }

    if (lgreen < 255){
        lgreen += 5;
    }

    if (rgreen < 255){
        rgreen += 5;
    }
}

void OptionSelectFont::run(const Menu::Context & context){
    // throw Menu::MenuException(__FILE__, __LINE__);
    /* throw something to quit back to the previous menu */
}

bool OptionSelectFont::leftKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(false);
            break;
        case fontWidth:
            Configuration::setMenuFontWidth(Configuration::getMenuFontWidth() - 1);
            break;
        case fontHeight:
            Configuration::setMenuFontHeight(Configuration::getMenuFontHeight() - 1);
            break;
        default:
            break;
    }
    lblue = lgreen = 0;
    return true;
}

bool OptionSelectFont::rightKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(true);
            break;
        case fontWidth:
            Configuration::setMenuFontWidth(Configuration::getMenuFontWidth() + 1);
            break;
        case fontHeight:
            Configuration::setMenuFontHeight(Configuration::getMenuFontHeight() + 1);
            break;
        default:
            break;
    }
    rblue = rgreen = 0;
    return true;
}

static bool saneFont(const Util::ReferenceCount<Menu::FontInfo> & info){
    class Context: public Loader::LoadingContext {
    public:
        Context(const Util::ReferenceCount<Menu::FontInfo> & info):
            info(info),
            isok(false){
            }

        bool ok(){
            try{
                const Font & font = info->get();
                return font.textLength("A") != 0 &&
                    font.getHeight() != 0;
            } catch (const Exception::Base & ignore){
                return true;
            }
        }

        virtual void load(){
            isok = ok();
        }

        const Util::ReferenceCount<Menu::FontInfo> & info;
        bool isok;
    };

    if (info == NULL){
        return true;
    }

    Context context(info);
    /* an empty LevelInfo object, we don't really care about it */
    Level::LevelInfo level;
    Loader::loadScreen(context, level, Loader::SimpleCircle);
    return context.isok;
}

void OptionSelectFont::nextIndex(bool forward){
    if (fonts.size() == 0){
        return;
    }
    
    int index = 0;
    for (unsigned int i = 0 ; i < fonts.size() ; ++i){
        if ((Configuration::getMenuFont() == NULL && fonts[i] == NULL) ||
            ((Configuration::getMenuFont() != NULL && fonts[i] != NULL) &&
             (*Configuration::getMenuFont() == *fonts[i]))){
            index = i;
        }
    }

    if (forward){
	index++;
	if (index >= (int) fonts.size()){
	    index = 0;
	}
    } else {
	index--;
	if (index < 0){
	    index = (int)fonts.size()-1;
	}
    }

    while (!saneFont(fonts[index])){
        Global::debug(0) << "Warning: erasing font `" << fonts[index]->getName() << "'" << endl;
        int where = 0;
        vector<Util::ReferenceCount<Menu::FontInfo> >::iterator it;
        for (it = fonts.begin(); it != fonts.end() && where != index; it++, where++){
        }
        fonts.erase(it);
        if (index >= (int) fonts.size()){
            index = fonts.size() - 1;
        }
    }

    Configuration::setMenuFont(fonts[index]);

    /* FIXME */
    /*
    if (fonts[index] == "Default"){
	Configuration::setMenuFont("");
    } else {
	Configuration::setMenuFont(fonts[index]);
    }
    */
}

OptionSpeed::OptionSpeed(const Token *token): MenuOption(token), name(""), lblue(255), lgreen(255), rblue(255), rgreen(255)
{
    setRunnable(false);

    if ( *token != "speed" )
        throw LoadException(__FILE__, __LINE__, "Not speed option");

    readName(token);
}

OptionSpeed::~OptionSpeed(){
	// Nothing
}


std::string OptionSpeed::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << Configuration::getGameSpeed();
    return out.str();
}

void OptionSpeed::logic(){
    /*
	//ostringstream temp;
	char temp[255];
	sprintf( temp, "%s: %0.2f", name.c_str(), MenuGlobals::getGameSpeed() );
	setText(std::string(temp));
        */
	
}

void OptionSpeed::run(const Menu::Context & context){
}

bool OptionSpeed::leftKey(){
    Configuration::setGameSpeed(Configuration::getGameSpeed() - 0.05);
    if (Configuration::getGameSpeed() < 0.1){
        Configuration::setGameSpeed(0.1);
    }
    return false;
}
bool OptionSpeed::rightKey(){
    Configuration::setGameSpeed(Configuration::getGameSpeed() + 0.05);
    rblue = rgreen = 0;
    return false;
}

OptionTabMenu::OptionTabMenu(const Token *token):
MenuOption(token),
menu(0){
    if (token->numTokens() == 1){
        std::string temp;
        token->view() >> temp;
        menu = new Menu::Menu(Filesystem::find(Filesystem::RelativePath(temp)), Menu::Menu::Tabbed);
    } else {
        menu = new Menu::Menu(token, Menu::Menu::Tabbed);
    }

    // this->setText(menu->getName());
    // token->print("Menu: ");
    const Token * tok = token->findToken("_/name");
    if (tok != NULL){
        std::string name;
        tok->view() >> name;
        // Global::debug(0, "menu") << "Menu name: " << name << endl;
        this->setText(name);
    } else {
        // No name?
        throw LoadException(__FILE__, __LINE__, "Menu has no name");
    }
}

OptionTabMenu::~OptionTabMenu(){
    // Delete our menu
    if (menu){
        delete menu;
    }
}

void OptionTabMenu::logic(){
	// Nothing
}

void OptionTabMenu::run(const Menu::Context & context){
    // Do our new menu
    // menu->run(context);
    try{
        menu->run(context);
    } catch (const Exception::Return ignore){
        throw Menu::Reload(__FILE__, __LINE__);
    }
}

OptionSound::OptionSound(const Token *token):
MenuOption(token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if (*token != "sound" ){
        throw LoadException(__FILE__, __LINE__, "Not a sound option");
    }

    readName(token);
}

OptionSound::~OptionSound(){
}

void OptionSound::logic(){
    ostringstream temp;
    temp << "Sound: " << Configuration::getSoundVolume();
    setText(temp.str());
}

void OptionSound::run(const Menu::Context & context){
}

void OptionSound::changeSound(int much){
    int volume = Configuration::getSoundVolume();
    volume += much;
    if (volume < 0){
        volume = 0;
    }

    if (volume > 100){
        volume = 100;
    }

    Configuration::setSoundVolume(volume);
}

bool OptionSound::leftKey(){
    changeSound(-1);
    
    lblue = lgreen = 0;
    return true;
}

bool OptionSound::rightKey(){
    changeSound(+1);

    rblue = rgreen = 0;
    return true;
}

OptionMusic::OptionMusic(const Token *token):
MenuOption(token),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if (*token != "music" ){
        throw LoadException(__FILE__, __LINE__, "Not a music option");
    }

    readName(token);
}

void OptionMusic::logic(){
    ostringstream temp;
    temp << "Music: " << Configuration::getMusicVolume();
    setText(temp.str());
}

void OptionMusic::run(const Menu::Context & context){
}

void OptionMusic::changeMusic(int much){
    int volume = Configuration::getMusicVolume();
    volume += much;
    if (volume < 0){
        volume = 0;
    }

    if (volume > 100){
        volume = 100;
    }

    Configuration::setMusicVolume(volume);
    Music::setVolume((double) volume / 100.0);
}

bool OptionMusic::leftKey(){
    changeMusic(-1);
    
    lblue = lgreen = 0;
    return true;
}

bool OptionMusic::rightKey(){
    changeMusic(+1);
    
    lblue = lgreen = 0;
    return true;
}

OptionMusic::~OptionMusic(){
}

OptionLanguage::OptionLanguage(const Token * token):
MenuOption(token){
    readName(token);
    const Token * start = token->getRootParent();
    vector<const Token*> tokens = start->findTokens("*/language");
    vector<string> all;
    for (vector<const Token*>::iterator it = tokens.begin(); it != tokens.end(); it++){
        string language;
        const Token * token = *it;
        if (token->match("language", language)){
            all.push_back(language);
        }
    }
    sort(all.begin(), all.end());
    unique_copy(all.begin(), all.end(), back_insert_iterator<vector<string> >(languages));
    // Global::debug(0) << "Found " << languages.size() << " languages" << endl;
}

void OptionLanguage::run(const Menu::Context & context){
    class LanguageOption: public MenuOption {
    public:
        LanguageOption(const string & language):
        MenuOption(NULL){
            setText(language);
            setInfoText(language);
        }

        virtual void logic(){
        }

        virtual void run(const ::Menu::Context & context){
            Configuration::setLanguage(getText());
            Configuration::saveConfiguration();
            throw ::Menu::MenuException(__FILE__, __LINE__);
        }
    };

    Menu::Menu temp;
    Util::ReferenceCount<Menu::FontInfo> info = new Menu::RelativeFontInfo(Filesystem::RelativePath("fonts/arial.ttf"), 24, 24);
    temp.setFont(info);

    for (vector<string>::iterator it = languages.begin(); it != languages.end(); it++){
        temp.addOption(new LanguageOption(*it));
    }

    try {
        temp.run(context);
    } catch (const Exception::Return & ignore){
    } catch (const Menu::MenuException & ex){
    }

    throw Menu::Reload(__FILE__, __LINE__);
    // throw Exception::Return(__FILE__, __LINE__);
}
    
void OptionLanguage::logic(){
}

OptionPlatformer::OptionPlatformer(const Token * token):
MenuOption(token){
    readName(token);
    
    if (!token->match("_/game", gameLocation)){
	throw LoadException(__FILE__, __LINE__, "Missing game location for Platformer");
    }
}

OptionPlatformer::~OptionPlatformer(){
}

void OptionPlatformer::logic(){
}

void OptionPlatformer::run(const Menu::Context & context){
    
    Platformer::Game game(gameLocation);
    game.run();
    
    throw Menu::Reload(__FILE__, __LINE__);
}

OptionMugenMotif::OptionMugenMotif(const Token * token):
MenuOption(token){
    readName(token);
}

OptionMugenMotif::~OptionMugenMotif(){
}

void OptionMugenMotif::logic(){
}

static bool isMugenMotif(const Filesystem::AbsolutePath & path){
    try{
        string name = Mugen::Util::probeDef(path, "info", "name");
        return true;
    } catch (...){
        return false;
    }
}

static vector<Filesystem::AbsolutePath> listMotifs(){
    Filesystem::AbsolutePath data = Filesystem::find(Filesystem::RelativePath("mugen/data"));
    vector<Filesystem::AbsolutePath> defs = Filesystem::getFilesRecursive(data, "system.def");
    vector<Filesystem::AbsolutePath> good;
    for (vector<Filesystem::AbsolutePath>::iterator it = defs.begin(); it != defs.end(); it++){
        const Filesystem::AbsolutePath & file = *it;
        if (isMugenMotif(file)){
            Global::debug(1) << "Motif: " << file.path() << endl;
            good.push_back(file);
        }
    }
    return good;
}

void OptionMugenMotif::run(const Menu::Context & context){
    class Context: public Loader::LoadingContext {
    public:
        Context():
            index(-1){
            }

        virtual void load(){
            paths = listMotifs();
            vector<Filesystem::AbsolutePath> paths = listMotifs();
            for (unsigned int i = 0; i < paths.size(); i++){
                OptionLevel *option = new OptionLevel(0, &index, i);
                option->setText(Mugen::Util::probeDef(paths[i], "info", "name"));
                option->setInfoText(Filesystem::cleanse(paths[i]).path());
                menu.addOption(option);
            }
        }

        vector<Filesystem::AbsolutePath> paths;
        int index;
        Menu::Menu menu;
    };

    Context state;
    /* an empty LevelInfo object, we don't really care about it */
    Level::LevelInfo level;
    Loader::loadScreen(state, level, Loader::SimpleCircle);
    
    if (state.paths.size() <= 1){
        return;
    }

    try {
        state.menu.run(context);
    } catch (const Menu::MenuException & ex){
    } catch (const Exception::Return & ok){
    }
        
    if (state.index != -1){
        Filesystem::RelativePath motif = Filesystem::cleanse(state.paths[state.index]).removeFirstDirectory();
        Global::debug(1) << "Set muge motif to " << motif.path() << endl;
        Configuration::setMugenMotif(motif.path());
        Mugen::Data::getInstance().setMotif(motif);
    }
}
