#include "util/bitmap.h"

#include "options.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "menu.h"
#include "configuration.h"
#include "menu/menu_global.h"
#include "menu/menu-exception.h"
#include "mugen/menu.h"
#include "mugen/config.h"
#include "init.h"

#include "level/utils.h"
#include "object/versus_player.h"
#include "object/versus_enemy.h"

#include "object/object.h"
#include "object/player.h"
#include "object/buddy_player.h"

#include "input/keyboard.h"
#include "game.h"
#include "menu/tab_menu.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "globals.h"
#include "exceptions/shutdown_exception.h"
#include "exceptions/exception.h"
#include "network/server.h"
#include "network/client.h"
#include "util/font.h"
#include "gui/box.h"
#include "util/thread.h"

#include "util/fire.h"
#include "input/input-map.h"
#include "input/input-manager.h"

#include "openbor/pack-reader.h"

#include "game/mod.h"

#include <sstream>
#include <algorithm>
#include <time.h>

using namespace std;
using namespace Gui;

static Level::LevelInfo doLevelMenu(const std::string dir, const Menu::Context & context){
    vector<Level::LevelInfo> & possible = Paintown::Mod::getCurrentMod()->getLevels();
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
        //temp.setParent(parent);
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

OptionAdventure::OptionAdventure(Token *token):
MenuOption(token, Event){
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
    Object * player = NULL;
    try{
        //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
        Level::LevelInfo info = doLevelMenu("/levels", context);

        if (parent != NULL){
            parent->waitForSelect();
        }

        Global::debug(1) << "Selecting players" << endl;
        int remap = 0;
        Filesystem::AbsolutePath path = Paintown::Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap);
        
        PlayerFuture future(path, MenuGlobals::getInvincible(), MenuGlobals::getLives(), remap);
        vector<Util::Future<Object *> *> players;
        players.push_back(&future);
        Game::realGame(players, info);
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Error while loading: " << le.getTrace() << endl;
    } catch ( const Exception::Return & r ){
        /* Game::selectPlayer can throw ReturnException, he will wait
         * for the keypress to be released, so we don't have to do it
         */
    }

    /* player will be null if an exception occurred before selectPlayer was called */
    /*
    if ( player != NULL ){
        delete player;
    }
    */
}

OptionAdventureCpu::OptionAdventureCpu(Token *token):
MenuOption(token, Event){
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
    int max_buddies = MenuGlobals::getNpcBuddies();

    Keyboard key;
    Object * player = NULL;
    vector<Util::Future<Object*>* > futures;
    vector< Object * > buddies;
    try{
        //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
        Level::LevelInfo info = doLevelMenu("/levels", context);
        if (parent != NULL){
            parent->waitForSelect();
        }

        /*
        player = Game::selectPlayer(MenuGlobals::getInvincible(), "Pick a player", info);
        player->setObjectId(-1);
        ((Player *)player)->setLives( MenuGlobals::getLives() );
        vector< Object * > players;
        players.push_back( player );
        */
        
        int remap;
        Filesystem::AbsolutePath path = Paintown::Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap);
        Util::Future<Object*> * player = new PlayerFuture(path, MenuGlobals::getInvincible(), MenuGlobals::getLives(), remap);
        futures.push_back(player);

        for ( int i = 0; i < max_buddies; i++ ){
            ostringstream out;
            out << "Pick buddy " << nthWord(i+1);
            int remap;
            Filesystem::AbsolutePath path = Paintown::Mod::getCurrentMod()->selectPlayer(out.str(), info, remap);
            futures.push_back(new BuddyFuture(path, player, remap, -(i+2)));
            /*
            Object * b = Game::selectPlayer(false, out.str(), info);
            // buddies.push_back( b );
            Object * buddy = new BuddyPlayer( (Character *) player, *(Character *) b );
            / * buddies start at -2 and go down * /
            buddy->setObjectId(-(i + 2));
            / *
            buddies.push_back( buddy );
            players.push_back( buddy );
            * /
            */
        }

        Game::realGame(futures, info);
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Could not load player: " << le.getTrace() << endl;
    } catch ( const Exception::Return & r ){
        /* replace this with parent->waitAll() or something. we only care that
         * the player doesn't accidentally press some menu key, like esc or enter.
         * but they can press other keys that were useful in game because those
         * keys don't have any effect on the menu.
         */
    }

    /*
    if ( player != NULL ){
        delete player;
    }
    for ( vector< Object * >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
        delete *it;
    }
    */
    
    for (vector<Util::Future<Object*>*>::iterator it = futures.begin(); it != futures.end(); it++){
        delete *it;
    }
}

OptionChangeMod::OptionChangeMod(Token *token):
MenuOption(token, Event){
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
        Token * name_token = reader.readToken()->findToken("game/name");
        if (name_token != NULL){
            string name;
            *name_token >> name;
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
        
        menu.run(context);
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

OptionCredits::OptionCredits( Token * token ):
MenuOption(token, Event),
background(0),
music(""),
color(Bitmap::makeColor(255,255,255)),
title(Bitmap::makeColor(0,255,255)){
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
	
	if ( *token != "credits" )
		throw LoadException(__FILE__, __LINE__, "Not a credit menu");

        readName(token);
	
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "music" ) {
				/* Set music for credits */
				*tok >> music;
			} else if ( *tok == "background" ) {
				/* Create an image and push it back on to vector */
				std::string temp;
				*tok >> temp;
				if ( background ){
					delete background;
				}
				background = new Bitmap(Filesystem::find(Filesystem::RelativePath(temp)).path());
				if ( background->getError() ){
					delete background;
					background = NULL;
				}
			} else if ( *tok == "additional" ) {
				std::string str;
				while ( tok->hasTokens() ){
					*tok >> str;
					credits.push_back(str);
				}
			} else if ( *tok == "titlecolor" ) {
				int r,b,g;
				*tok >> r >> g >> b;
				title = Bitmap::makeColor( r, b, g );
			} else if ( *tok == "color" ) {
				int r,b,g;
				*tok >> r >> g >> b;
				color = Bitmap::makeColor( r, b, g );
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
	if ( background ){
		delete background;
	}
}

void OptionCredits::logic(){
}

void OptionCredits::run( const Menu::Context & context ){
    Keyboard key;

    const int maxCredits = credits.size();

    Global::speed_counter = 0;
    double min_y = GFX_Y;

    /* use Bitmap::temporaryBitmap here? */
    Bitmap tmp(Global::getScreenWidth(), Global::getScreenHeight());
    // Bitmap fireWork(GFX_X, GFX_Y);
    if (! music.empty()){
        MenuGlobals::setMusic(music);
    }

    Paintown::Fire fire;

    const Font & vFont = Font::getFont(context.getFont(), context.getFontWidth(), context.getFontHeight());

    bool quit = false;

    Bitmap::transBlender(0, 0, 0, 128);

    double think = 0;
    while (!quit){

        InputManager::poll();
        InputMap<CreditKey>::Output out = InputManager::getMap(input);
        quit = out[Exit];

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
            if (background ){
                background->Blit(tmp);
            } else {
                tmp.fill(Bitmap::makeColor(0,0,0));
            }

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
            Util::rest( 1 );
        }
    }

    InputManager::waitForRelease(input, Exit);
}

OptionDummy::OptionDummy(Token *token):
MenuOption(token, Event){
    if ( *token != "dummy" ){
        throw LoadException(__FILE__, __LINE__, "Not dummy option");
    }

    readName(token);

    if (getText().empty()){
        this->setText("Dummy");
    }
}

OptionDummy::OptionDummy( const std::string &name ):
MenuOption(0, Event){
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

OptionFullscreen::OptionFullscreen(Token *token):
MenuOption(token, AdjustableOption),
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
    out << MenuOption::getText() << ": " << (MenuGlobals::getFullscreen() ? "Yes" : "No");
    return out.str();
}

void OptionFullscreen::logic(){
	if(lblue < 255)lblue+=5;
	if(rblue < 255)rblue+=5;
	if(lgreen < 255)lgreen+=5;
	if(rgreen < 255)rgreen+=5;
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionFullscreen::run(const Menu::Context & context){
}

bool OptionFullscreen::leftKey()
{
	MenuGlobals::setFullscreen(!MenuGlobals::getFullscreen());
	lblue = lgreen = 0;
	int gfx = (MenuGlobals::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
	Bitmap::setGraphicsMode( gfx, Global::getScreenWidth(), Global::getScreenHeight() );
	return true;
}
bool OptionFullscreen::rightKey()
{
	MenuGlobals::setFullscreen(!MenuGlobals::getFullscreen());
	rblue = rgreen = 0;
	int gfx = (MenuGlobals::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
	Bitmap::setGraphicsMode( gfx, Global::getScreenWidth(), Global::getScreenHeight() );
	return true;
}

OptionInvincible::OptionInvincible(Token *token):
MenuOption(token, AdjustableOption),
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
    out << MenuOption::getText() << ": " << (MenuGlobals::getInvincible() ? "Yes" : "No");
    return out.str();
}

void OptionInvincible::logic()
{
	
	if(lblue < 255)lblue+=5;
	if(rblue < 255)rblue+=5;
	if(lgreen < 255)lgreen+=5;
	if(rgreen < 255)rgreen+=5;
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionInvincible::run(const Menu::Context & context){
}

bool OptionInvincible::leftKey()
{
	MenuGlobals::setInvincible(!MenuGlobals::getInvincible());
	lblue = lgreen = 0;
	return true;
}
bool OptionInvincible::rightKey()
{
	MenuGlobals::setInvincible(!MenuGlobals::getInvincible());
	rblue = rgreen = 0;
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

OptionJoystick::OptionJoystick(Token *token):
MenuOption(token, Event),
name(""),
player(-1),
type(Invalidkey),
keyCode(0){
    if (*token != "joystick"){
        throw LoadException(__FILE__, __LINE__, "Not joystick option");
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
            throw LoadException(__FILE__, __LINE__, ex, "Menu parse error");
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if(name.empty())throw LoadException(__FILE__, __LINE__, "No name set, this option should have a name!");
    if(type == Invalidkey)throw LoadException(__FILE__, __LINE__, "Invalid key, should be up, down, left, right, up, down, jump, attack1-6!");
    if(player == -1)throw LoadException(__FILE__, __LINE__, "Player not specified in key configuration");

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
    const Font &vFont = Font::getFont(context.getFont(),context.getFontWidth(),context.getFontHeight());
    const char * message = "Press a joystick button!";
    const int width = vFont.textLength(message) + 10;
    const int height = vFont.getHeight() + 10;
    // const int x = (getParent()->getWork()->getWidth()/2) - (width/2);
    // const int y = (getParent()->getWork()->getHeight()/2) - (height/2);
    const int x = OldMenu::Menu::Width / 2 - width/2;
    const int y = OldMenu::Menu::Height / 2 - height/2;
    Box dialog;
    dialog.location.setPosition(Gui::AbsolutePoint(0,0));
    dialog.location.setDimensions(vFont.textLength(message) + 10, vFont.getHeight() + 10);
    dialog.location.setRadius(0);
    dialog.colors.body = Bitmap::makeColor(0,0,0);
    dialog.colors.bodyAlpha = 200;
    dialog.colors.border = Bitmap::makeColor(255,255,255);
    dialog.colors.borderAlpha = 255;
    Bitmap temp = Bitmap::temporaryBitmap(width,height);
    dialog.render(temp);
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

static OptionKey::keyType convertToKeyboardKey(const std::string &k)
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

static int getKey(int player, OptionKey::keyType k)
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

static void setKey(int player, OptionKey::keyType k, int key)
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

OptionKey::OptionKey(Token *token): MenuOption(token, Event), name(""), player(-1), type(invalidkey), keyCode(0)
{
	if ( *token != "key" )
		throw LoadException(__FILE__, __LINE__, "Not key option");
	
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
				type = convertToKeyboardKey(temp);
			}
			else 
			{
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

OptionKey::~OptionKey()
{
	// Nothing
}

void OptionKey::logic(){
	char temp[255];
	sprintf( temp, "%s: %s", name.c_str(), Keyboard::keyToName(getKey(player,type)));
	setText(std::string(temp));
}

void OptionKey::run(const Menu::Context & context){
    // Do dialog
    Box::messageDialog(OldMenu::Menu::Width, OldMenu::Menu::Height, "Press a Key!",2);

    Keyboard key;
    keyCode = readKey( key );
    setKey(player,type, keyCode);
}

OptionLevel::OptionLevel(Token *token, int * set, int value):
MenuOption(token, Event),
set(set),
value(value){
  // Nothing
}

OptionLevel::~OptionLevel(){
}

void OptionLevel::logic(){
}

/* redo this to not use global state */
void OptionLevel::run(const Menu::Context & context){
	//endGame = true;
    *set = value;
    throw Menu::MenuException(__FILE__, __LINE__);
}

OptionLives::OptionLives( Token * token ):
MenuOption(token, AdjustableOption),
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
    out << MenuOption::getText() << ": " << MenuGlobals::getLives();
    return out.str();
}

void OptionLives::logic(){

    if ( lblue < 255 ){
        lblue += 5;
    }

    if ( rblue < 255 ){
        rblue += 5;
    }

    if ( lgreen < 255 ){
        lgreen += 5;
    }

    if ( rgreen < 255 ){
        rgreen += 5;
    }

    setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
    setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionLives::run(const Menu::Context & context){
}

bool OptionLives::leftKey(){
	MenuGlobals::setLives(MenuGlobals::getLives() - 1);
	if ( MenuGlobals::getLives() < 1 ){
		MenuGlobals::setLives(1);
	}
	
	lblue = lgreen = 0;
	return false;
}

bool OptionLives::rightKey(){
	MenuGlobals::setLives( MenuGlobals::getLives() + 1 );
	rblue = rgreen = 0;
	return false;
}

OptionMenu::OptionMenu(Token *token):
MenuOption(token, Event),
menu(0){
    // Check whether we have a menu or tabmenu
    /*if ( *token == "menu" ){
	menu = new Menu();
    } else {
	throw LoadException(__FILE__, __LINE__, "Not a menu");
    }*/
    // Set this menu as an option
    //menu->setAsOption(true);
    
    /*
    // Lets try loading from a file
    std::string temp;
    // Filename
    *token >> temp;
    */
    if (*token != "menu"){
        throw LoadException(__FILE__, __LINE__, "Not a menu");
    }

    if (token->numTokens() == 1){
        std::string temp;
        *token >> temp;
        menu = new Menu::Menu(Filesystem::find(Filesystem::RelativePath(temp)));
    } else {
        menu = new Menu::Menu(token);
    }

    // this->setText(menu->getName());
    // token->print("Menu: ");
    Token * tok = token->findToken("_/name");
    if (tok != NULL){
        std::string name;
        *tok >> name;
        Global::debug(0, "menu") << "Menu name: " << name << endl;
        this->setText(name);
    } else {
        // No name?
        throw LoadException(__FILE__, __LINE__, "Menu has no name");
    }
    
    // Lets check if this menu is going bye bye
    //if ( menu->checkRemoval() ) setForRemoval(true);
}

OptionMenu::~OptionMenu()
{
	// Delete our menu
	if(menu)delete menu;
}

void OptionMenu::logic()
{
	// Nothing
}

void OptionMenu::run(const Menu::Context & context){
	// Do our new menu
    menu->run(context);
}

void OptionMenu::setParent(OldMenu::Menu *menu){
    this->parent = menu;
    //this->menu->setParent(menu);
}

OptionMugenMenu::OptionMugenMenu(Token *token):
MenuOption(token, Event){
    if ( *token != "mugen" ){
        throw LoadException(__FILE__, __LINE__, "Not a mugen motif menu");
    }
    
    while ( token->hasTokens() ){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "name" ){
                // Create an image and push it back on to vector
                std::string temp;
                *tok >> temp;
                this->setText(temp);
            } else if (*tok == "motif"){
                // Load Motif from file
                std::string temp;
                // Filename
                *tok >> temp;
                // Set the default motif
                Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(temp));
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
    Mugen::run();
}

#ifdef HAVE_NETWORKING
OptionNetworkHost::OptionNetworkHost(Token *token):
MenuOption(token, Event){
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
	Keyboard key;
        try{
            Network::networkServer(parent);
        } catch (const Exception::Return &e){
        }
	key.clear();
	key.poll();
	key.wait();
}

OptionNetworkJoin::OptionNetworkJoin(Token *token):
MenuOption(token, Event){
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
	Keyboard key;
	key.poll();
	key.wait();
        try{
            Network::networkClient();
        } catch (const Exception::Return &r){
        }

	key.clear();
	key.poll();
	key.wait();
}
#endif

OptionNpcBuddies::OptionNpcBuddies( Token * token ):
MenuOption(token, AdjustableOption),
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
    out << MenuOption::getText() << ": " << MenuGlobals::getNpcBuddies();
    return out.str();
}

void OptionNpcBuddies::logic(){

    if ( lblue < 255 ){
        lblue += 5;
    }

    if ( rblue < 255 ){
        rblue += 5;
    }

    if ( lgreen < 255 ){
        lgreen += 5;
    }

    if ( rgreen < 255 ){
        rgreen += 5;
    }

    setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
    setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionNpcBuddies::run(const Menu::Context & context){
}

bool OptionNpcBuddies::leftKey(){
	MenuGlobals::setNpcBuddies(MenuGlobals::getNpcBuddies() - 1);
	if ( MenuGlobals::getNpcBuddies() < 1 ){
		MenuGlobals::setNpcBuddies(1);
	}
	
	lblue = lgreen = 0;
	return false;
}

bool OptionNpcBuddies::rightKey(){
	MenuGlobals::setNpcBuddies( MenuGlobals::getNpcBuddies() + 1 );
	rblue = rgreen = 0;
	return false;
}

OptionPlayMode::OptionPlayMode(Token *token):
MenuOption(token, AdjustableOption),
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
    if (MenuGlobals::freeForAll()){
        out << "Free for all";
    } else if (MenuGlobals::cooperative()){
        out << "Cooperative";
    }

    return out.str();
}

void OptionPlayMode::logic(){
    

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

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
}

void OptionPlayMode::run(const Menu::Context & context){
}
    
void OptionPlayMode::changeMode(){
    if (MenuGlobals::freeForAll()){
        MenuGlobals::setCooperative();
    } else if (MenuGlobals::cooperative()){
        MenuGlobals::setFreeForAll();
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

OptionQuit::OptionQuit(Token *token):
MenuOption(token, Event){
    if ( *token != "quit" ){
        throw LoadException(__FILE__, __LINE__, "Not quit option");
    }

    readName(token);
}

OptionQuit::OptionQuit( const std::string &name ):
MenuOption(0, Event){
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

OptionScreenSize::OptionScreenSize(Token *token):
MenuOption(token, AdjustableOption),
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

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
}

void OptionScreenSize::run(const Menu::Context & context){
}

void OptionScreenSize::setMode(int width, int height){
    if (width != Configuration::getScreenWidth() ||
        height != Configuration::getScreenHeight()){

        Global::debug(1) << "Changing mode to " << width << " x " << height << endl;
        int gfx = Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED;
        int ok = Bitmap::setGraphicsMode(gfx, width, height);
        if (ok == 0){
            Global::debug(1) << "Success" << endl;
            Configuration::setScreenWidth(width);
            Configuration::setScreenHeight(height);
        } else {
            Global::debug(1) << "Fail" << endl;
            int ok = Bitmap::setGraphicsMode(gfx, Configuration::getScreenWidth(), Configuration::getScreenHeight());
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

OptionSelectFont::OptionSelectFont(Token *token):
MenuOption(token, AdjustableOption),
typeAdjust(fontName),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "font-select" ){
        throw LoadException(__FILE__, __LINE__, "Not a font selector");
    }

    while (token->hasTokens()){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "adjust" ){
                std::string temp;
                *tok >> temp;
                if ( temp == "name" ) typeAdjust = fontName;
                else if ( temp == "width" ) typeAdjust = fontWidth;
                else if ( temp == "height" ) typeAdjust = fontHeight;
                else throw LoadException(__FILE__, __LINE__, "Incorrect value \"" + temp + "\" in font-select");
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
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

    // Find and set fonts now
    if (typeAdjust == fontName){
        try{
            Filesystem::AbsolutePath fontsDirectory = Filesystem::find(Filesystem::RelativePath("fonts"));
            Global::debug(1, "fonts") << "Font directory " << fontsDirectory.path() << endl;
            vector<Filesystem::AbsolutePath> ttfFonts = Filesystem::getFiles(fontsDirectory, "*.ttf");
            Global::debug(1, "fonts") << "Found ttf fonts " << joinPaths(ttfFonts, ", ") << endl;
            vector<Filesystem::AbsolutePath> otfFonts = Filesystem::getFiles(fontsDirectory, "*.otf");
            Global::debug(1, "fonts") << "Found otf fonts " << joinPaths(otfFonts, ", ") << endl;

            /*
            std::back_insert_iterator< std::vector<Filesystem::AbsolutePath> > inserter(fonts);
            copy(ttfFonts.begin(), ttfFonts.end(), inserter);
            copy(otfFonts.begin(), otfFonts.end(), inserter);
            for (vector<Filesystem::AbsolutePath>::iterator it = fonts.begin(); it != fonts.end(); it++){
                Filesystem::AbsolutePath & name = *it;
                name = Filesystem::cleanse(name).path();
            }
            */
            for (vector<Filesystem::AbsolutePath>::iterator it = ttfFonts.begin(); it != ttfFonts.end(); it++){
                fonts.push_back(Filesystem::cleanse(*it).path());
            }

            for (vector<Filesystem::AbsolutePath>::iterator it = otfFonts.begin(); it != otfFonts.end(); it++){
                fonts.push_back(Filesystem::cleanse(*it).path());
            }

            /* use for debugging */
            // fonts.push_back("fonts/arial.ttf");
        } catch (const Filesystem::NotFound & e){
            throw LoadException(__FILE__, __LINE__, e, "Could not load font");
        }
    }
}

OptionSelectFont::~OptionSelectFont(){
    // Nothing
}

void OptionSelectFont::logic(){
    switch (typeAdjust){
        case fontName:	  
            setText("Current Font: " + OldMenu::Menu::getFont().path());
            break;
        case fontWidth:{
            ostringstream temp;
            temp << "Font Width: " << OldMenu::Menu::getFontWidth();
            setText(temp.str());
            break;
        }
        case fontHeight:{
            ostringstream temp;
            temp << "Font Height: " << OldMenu::Menu::getFontHeight();
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

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
}

void OptionSelectFont::run(const Menu::Context & context){
}

bool OptionSelectFont::leftKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(false);
            break;
        case fontWidth:
            OldMenu::Menu::setFontWidth(OldMenu::Menu::getFontWidth() - 1);
            break;
        case fontHeight:
            OldMenu::Menu::setFontHeight(OldMenu::Menu::getFontHeight() - 1);
            break;
        default:
            break;
    }
    // Menu::setFont(current, width, height);
    lblue = lgreen = 0;
    return true;
}

bool OptionSelectFont::rightKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(true);
            break;
        case fontWidth:
            OldMenu::Menu::setFontWidth(OldMenu::Menu::getFontWidth() + 1);
            break;
        case fontHeight:
            OldMenu::Menu::setFontHeight(OldMenu::Menu::getFontHeight() + 1);
            break;
        default:
            break;
    }
    // Menu::setFont(current, width, height);
    rblue = rgreen = 0;
    return true;
}

void OptionSelectFont::nextIndex(bool forward){
    if (fonts.size() == 0){
        return;
    }

    int index = 0;
    for (unsigned int i = 0 ; i < fonts.size() ; ++i){
        if (OldMenu::Menu::getFont().path() == fonts[i]){
            if (forward){
                index = i + 1;
                if (index >= (int) fonts.size()){
                    index = 0;
                }
            } else {
                index = i - 1;
                if (index < 0){
                    index = (int)fonts.size()-1;
                }
            }
        }    
    }
    OldMenu::Menu::setFontName(fonts[index]);
}

OptionSpeed::OptionSpeed(Token *token): MenuOption(token, AdjustableOption), name(""), lblue(255), lgreen(255), rblue(255), rgreen(255)
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
    out << MenuOption::getText() << ": " << MenuGlobals::getGameSpeed();
    return out.str();
}

void OptionSpeed::logic(){
    /*
	//ostringstream temp;
	char temp[255];
	sprintf( temp, "%s: %0.2f", name.c_str(), MenuGlobals::getGameSpeed() );
	setText(std::string(temp));
        */
	
	if(lblue < 255)lblue+=5;
	if(rblue < 255)rblue+=5;
	if(lgreen < 255)lgreen+=5;
	if(rgreen < 255)rgreen+=5;
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionSpeed::run(const Menu::Context & context){
}

bool OptionSpeed::leftKey()
{
	MenuGlobals::setGameSpeed(MenuGlobals::getGameSpeed() - 0.05);
	if( MenuGlobals::getGameSpeed() < 0.1 )MenuGlobals::setGameSpeed(0.1);
	
	lblue = lgreen = 0;
	return false;
}
bool OptionSpeed::rightKey()
{
	MenuGlobals::setGameSpeed(MenuGlobals::getGameSpeed() + 0.05);
	
	rblue = rgreen = 0;
	return false;
}

OptionTabMenu::OptionTabMenu(Token *token):
MenuOption(token, Event),
_menu(0){
    // Check whether we have a menu or tabmenu
    if (*token == "tabmenu"){
	_menu = new TabMenu();
    } else {
	throw LoadException(__FILE__, __LINE__, "Not a tabbed menu");
    }
    // Set this menu as an option
    _menu->setAsOption(true);
    
    /*
    // Lets try loading from a file
    std::string temp;
    // Filename
    *token >> temp;
    */

    if (token->numTokens() == 1){
        std::string temp;
        *token >> temp;
        _menu->load(Filesystem::find(Filesystem::RelativePath(temp)));
    } else {
        _menu->load(token);
    }

    this->setText(_menu->getName());
    
    // Lets check if this menu is going bye bye
    if ( _menu->checkRemoval() ) setForRemoval(true);
}

OptionTabMenu::~OptionTabMenu()
{
	// Delete our menu
	if(_menu)delete _menu;
}

void OptionTabMenu::logic()
{
	// Nothing
}

void OptionTabMenu::run(const Menu::Context & context){
	// Do our new menu
	_menu->run();
}

OptionVersus::OptionVersus( Token *token ):
MenuOption(token, Event), human(false){
    if ( *token != "versus" ){
        throw LoadException(__FILE__, __LINE__, "Not versus");
    }

    readName(token);

    while ( token->hasTokens() ){
        try{ 
            Token * tok;
            *token >> tok;
            if( *tok == "cpu" ) {
                human = false;
            } else if( *tok == "human" ) {
                human = true;
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
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

OptionVersus::~OptionVersus(){
}

void OptionVersus::logic(){
}

void OptionVersus::run(const Menu::Context & context){
#if 0
	/*
	Keyboard key;
	Object * player = NULL;
	Object * enemy = NULL;
	try{
		if(human) 
		{
			vector<Object *>temp = versusSelect( false );
			player = temp[0];
			enemy = temp[1];
			if(!player || !enemy)throw LoadException("Null player");
			//player = selectPlayer( false );
			//enemy = selectPlayer( false );
			enemy->setAlliance( ALLIANCE_ENEMY );
			
			for ( int i = 0; i < 3; i += 1 ){
				VersusPlayer en( 1, *(Player *) enemy );
				VersusPlayer pl( 0, *(Player *) player );
				playVersusMode( &pl, &en, i + 1 );
			}
		}
		else
		{
			player = selectPlayer( false );
			enemy = selectPlayer( false );
			enemy->setAlliance( ALLIANCE_ENEMY );
			
			for ( int i = 0; i < 3; i += 1 ){
				VersusEnemy en( *(Player *) enemy );
				VersusPlayer pl( 0, *(Player *) player );
				playVersusMode( &pl, &en, i + 1 );
			}
		}
		key.wait();
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		key.wait();
	}
	*/

	Keyboard key;
	Object * player = NULL;
	Object * enemy = NULL;
	try{
		if ( human ){
			vector<Object *>temp = Game::versusSelect( false );
			player = temp[0];
			enemy = temp[1];
			if ( ! player || !enemy ){
				throw LoadException(__FILE__, __LINE__, "Null player");
			}

			//player = selectPlayer( false );
			//enemy = selectPlayer( false );
			enemy->setAlliance( ALLIANCE_ENEMY );

			for ( int i = 0; i < 3; i += 1 ){
				VersusPlayer en(*(Player *) enemy );
				VersusPlayer pl(*(Player *) player );
                                en.setConfig(1);
                                pl.setConfig(0);
				Game::playVersusMode( &pl, &en, i + 1 );
			}
		} else {
                    Level::LevelInfo info;
                    player = Game::selectPlayer( false, "Pick your player", info);
                    enemy = Game::selectPlayer( false, "Pick enemy", info);
                    enemy->setAlliance( ALLIANCE_ENEMY );

                    for ( int i = 0; i < 3; i += 1 ){
                        VersusEnemy en( *(Player *) enemy );
                        VersusPlayer pl( *(Player *) player );
                        pl.setConfig(0);
                        Game::playVersusMode( &pl, &en, i + 1 );
                    }
		}
		key.wait();
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
	} catch ( const Exception::Return & r ){
		key.wait();
	}

	if ( player != NULL ){
		delete player;
	}
	if ( enemy != NULL ){
		delete enemy;
	}
#endif
}

OptionSound::OptionSound(Token *token):
MenuOption(token, AdjustableOption),
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

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
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

OptionMusic::OptionMusic(Token *token):
MenuOption(token, AdjustableOption),
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

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
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
