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
#include "return_exception.h"
#include "network/server.h"
#include "network/client.h"
#include "util/font.h"
#include "gui/box.h"

#include "util/fire.h"
#include "input/input-map.h"
#include "input/input-manager.h"

#include "game/mod.h"

#include <sstream>
#include <algorithm>

using namespace std;
using namespace Gui;

OptionAdventure::OptionAdventure(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "adventure" ){
        throw LoadException("Not an adventure");
    }

    readName(token);
}

OptionAdventure::~OptionAdventure(){
	// Nothing
}

void OptionAdventure::logic(){
}

void OptionAdventure::run(bool &endGame){
    Keyboard key;
    Object * player = NULL;
    try{
        //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
        Level::LevelInfo info = MenuGlobals::doLevelMenu("/levels", parent);

        /*
        if (level.empty()){
            Global::debug(0) << "*bug* Level name is empty?" << endl;
            throw an error or something
            return;
        */
        key.wait();

        player = Game::selectPlayer(MenuGlobals::getInvincible(), "Pick a player", info);
        player->setObjectId(-1);
        ((Player *)player)->setLives( MenuGlobals::getLives() );
        vector< Object * > players;
        players.push_back( player );
        Game::realGame(players, info);
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Error while loading: " << le.getReason() << endl;
    } catch ( const ReturnException & r ){
        /* Game::selectPlayer can throw ReturnException, he will wait
         * for the keypress to be released, so we don't have to do it
         */
        // key.wait();
    }

    /* player will be null if an exception occurred before selectPlayer was called */
    if ( player != NULL ){
        delete player;
    }
}

OptionAdventureCpu::OptionAdventureCpu(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if (*token != "adventure-cpu"){
        throw LoadException("Not an adventure");
    }

    readName(token);
}

OptionAdventureCpu::~OptionAdventureCpu(){
}

void OptionAdventureCpu::logic(){
}

void OptionAdventureCpu::run(bool &endGame){
    int max_buddies = MenuGlobals::getNpcBuddies();

    Keyboard key;
    Object * player = NULL;
    vector< Object * > buddies;
    try{
        //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
        Level::LevelInfo info = MenuGlobals::doLevelMenu("/levels",parent);
        key.wait();
        player = Game::selectPlayer(MenuGlobals::getInvincible(), "Pick a player", info);
        player->setObjectId(-1);
        ((Player *)player)->setLives( MenuGlobals::getLives() );
        vector< Object * > players;
        players.push_back( player );

        for ( int i = 0; i < max_buddies; i++ ){
            ostringstream out;
            out << "Pick buddy " << i+1;
            Object * b = Game::selectPlayer(false, out.str(), info);
            buddies.push_back( b );
            Object * buddy = new BuddyPlayer( (Character *) player, *(Character *) b );
            /* buddies start at -2 and go down */
            buddy->setObjectId(-(i + 2));
            buddies.push_back( buddy );
            players.push_back( buddy );
        }
        Game::realGame(players, info);
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
    } catch ( const ReturnException & r ){
        key.wait();
    }
    if ( player != NULL ){
        delete player;
    }
    for ( vector< Object * >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
        delete *it;
    }
}

OptionChangeMod::OptionChangeMod(Token *token) throw (LoadException):
MenuOption(token, Event){
    if ( *token != "change-mod" ){
        throw LoadException("Not a change mod");
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

/* FIXME: change to AbsolutePath */
static vector<string> findMods(){
    vector<string> mods;

    vector<Filesystem::AbsolutePath> directories = Filesystem::findDirectories(Filesystem::RelativePath("."));
    for (vector<Filesystem::AbsolutePath>::iterator dir = directories.begin(); dir != directories.end(); dir++){
        string file = (*dir).path() + "/" + Filesystem::cleanse(*dir).path() + ".txt";
        if (isModFile(file)){
            mods.push_back(file);
        }
    }

    return mods;
}

static Filesystem::RelativePath modName(const Filesystem::AbsolutePath & path){
    try{
        TokenReader reader(path.path());
        Global::debug(1) << "Checking for a mod in " << path.path() << endl;
        Token * name_token = reader.readToken()->findToken("game/name");
        if (name_token != NULL){
            string name;
            *name_token >> name;
            return Filesystem::RelativePath(name);
        }
        return Filesystem::cleanse(path);
    } catch (const TokenException & e){
        return Filesystem::cleanse(path);
    }
}

static void changeMod(const std::string & path){
    size_t slash = path.rfind('/');
    size_t txt = path.rfind(".txt");
    if (slash != string::npos && txt != string::npos){
        string name = path.substr(slash + 1, path.length() - slash - 5);
        Configuration::setCurrentGame(name);
        Paintown::Mod::loadMod(name);
    } else {
        Global::debug(0) << "Could not change mod to " << path << endl;
    }
}

void OptionChangeMod::run(bool &endGame){
    try{
        int select = 0;
        // Menu menu(Filesystem::find("menu/change-mod.txt"));
        // menu.addOption(new OptionLevel(0, &select, 0));
        // menu.setupOptions();
        Menu menu;
        menu.setParent(getParent());
        vector<string> mods = findMods();
        int index = 0;
        std::vector<OptionLevel *> options;
        for (vector<string>::iterator it = mods.begin(); it != mods.end(); it++){
            // menu.addOption(new OptionLevel(0, &select, 0));
            OptionLevel *opt = new OptionLevel(0, &select, index);
            opt->setText(modName(Filesystem::AbsolutePath(*it)).path());
            opt->setInfoText("Choose this mod");
            if (modName(Filesystem::AbsolutePath(*it)).path().compare(Util::upcase(Configuration::getCurrentGame())) == 0){
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
        
        menu.load(Filesystem::find(Filesystem::RelativePath("menu/change-mod.txt")));
        menu.run();
        changeMod(mods[select]);

        // Reload the menu
        throw ReloadMenuException();

    } catch (const LoadException & le){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << le.getReason() << endl;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << e.getReason() << endl;
    }
}

OptionCredits::OptionCredits( Token * token ) throw( LoadException ):
MenuOption(token, Event),
background(0),
music(""),
color(Bitmap::makeColor(255,255,255)),
title(Bitmap::makeColor(0,255,255)){
	/* Always */
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
		throw LoadException("Not a credit menu");

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
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
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

void OptionCredits::run( bool &endGame ){
    Keyboard key;

    const int maxCredits = credits.size();

    Global::speed_counter = 0;
    double min_y = GFX_Y;

    /* use Bitmap::temporaryBitmap here? */
    Bitmap tmp(GFX_X, GFX_Y);
    // Bitmap fireWork(GFX_X, GFX_Y);
    if (! music.empty()){
        MenuGlobals::setMusic(music);
    }

    Paintown::Fire fire;

    const Font & vFont = Font::getFont(Menu::getFont(), Menu::getFontWidth(), Menu::getFontHeight());

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

OptionDummy::OptionDummy(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "dummy" ){
        throw LoadException("Not dummy option");
    }

    readName(token);

    if (getText().empty()){
        this->setText("Dummy");
    }
}

OptionDummy::OptionDummy( const std::string &name ) throw( LoadException ):
MenuOption(0, Event){
    if (name.empty()){
	throw LoadException("No name given to dummy");
    }
    this->setText(name);
}

OptionDummy::~OptionDummy(){
}

void OptionDummy::logic(){
}

void OptionDummy::run(bool &endGame){
}

OptionFullscreen::OptionFullscreen(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "fullscreen" )
		throw LoadException("Not fullscreen option");

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

void OptionFullscreen::run(bool &endGame){
}

bool OptionFullscreen::leftKey()
{
	MenuGlobals::setFullscreen(!MenuGlobals::getFullscreen());
	lblue = lgreen = 0;
	int gfx = (MenuGlobals::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
	Bitmap::setGraphicsMode( gfx, GFX_X, GFX_Y );
	return true;
}
bool OptionFullscreen::rightKey()
{
	MenuGlobals::setFullscreen(!MenuGlobals::getFullscreen());
	rblue = rgreen = 0;
	int gfx = (MenuGlobals::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
	Bitmap::setGraphicsMode( gfx, GFX_X, GFX_Y );
	return true;
}

OptionInvincible::OptionInvincible(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "invincible" )
		throw LoadException("Not invincible option");

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

void OptionInvincible::run(bool &endGame){
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

void OptionJoystick::run(bool &endGame){
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
				type = convertToKeyboardKey(temp);
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

void OptionKey::run(bool &endGame){
	// Do dialog
	Box::msgDialog(*getParent()->getWork(),"Press a Key!",2);
	
	Keyboard key;
	keyCode = readKey( key );
	setKey(player,type, keyCode);
}

OptionLevel::OptionLevel(Token *token, int * set, int value) throw( LoadException ):
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
void OptionLevel::run(bool &endGame){
	endGame = true;
    *set = value;
}

OptionLives::OptionLives( Token * token ) throw( LoadException ):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "lives" ){
		throw LoadException( "Not lives option" );
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

void OptionLives::run(bool &endGame){
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

OptionMenu::OptionMenu(Token *token) throw (LoadException):
MenuOption(token, Event),
menu(0){
    // Check whether we have a menu or tabmenu
    if ( *token == "menu" ){
	menu = new Menu();
    } else {
	throw LoadException("Not a menu");
    }
    // Set this menu as an option
    menu->setAsOption(true);
    
    /*
    // Lets try loading from a file
    std::string temp;
    // Filename
    *token >> temp;
    */

    if (token->numTokens() == 1){
        std::string temp;
        *token >> temp;
        menu->load(Filesystem::find(Filesystem::RelativePath(temp)));
    } else {
        menu->load(token);
    }

    this->setText(menu->getName());
    
    // Lets check if this menu is going bye bye
    if ( menu->checkRemoval() ) setForRemoval(true);
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

void OptionMenu::run(bool &endGame){
	// Do our new menu
    menu->run();
}

void OptionMenu::setParent(Menu *menu){
    this->parent = menu;
    this->menu->setParent(menu);
}

OptionMugenMenu::OptionMugenMenu(Token *token) throw (LoadException):
MenuOption(token, Event){
    if ( *token != "mugen" ){
        throw LoadException("Not a mugen motif menu");
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
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
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

void OptionMugenMenu::run(bool &endGame){
    Mugen::run();
}

OptionNetworkHost::OptionNetworkHost(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "network-host" ){
        throw LoadException("Not a network-host");
    }

    readName(token);
}

OptionNetworkHost::~OptionNetworkHost(){
	// Nothing
}

void OptionNetworkHost::logic(){
}

void OptionNetworkHost::run(bool &endGame){
	Keyboard key;
        try{
            Network::networkServer(parent);
        } catch (const ReturnException &e){
        }
	key.clear();
	key.poll();
	key.wait();
}

OptionNetworkJoin::OptionNetworkJoin(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "network-join" ){
        throw LoadException("Not a network-join");
    }

    readName(token);
}

OptionNetworkJoin::~OptionNetworkJoin(){
	// Nothing
}

void OptionNetworkJoin::logic(){
}

void OptionNetworkJoin::run(bool &endGame){
	Keyboard key;
	key.poll();
	key.wait();
        try{
            Network::networkClient();
        } catch (const ReturnException &r){
        }

	key.clear();
	key.poll();
	key.wait();
}

OptionNpcBuddies::OptionNpcBuddies( Token * token ) throw( LoadException ):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "npc" ){
        throw LoadException( "Not npc option" );
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

void OptionNpcBuddies::run(bool &endGame){
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

OptionPlayMode::OptionPlayMode(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "play-mode" ){
        throw LoadException("Not a play-mode");
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

void OptionPlayMode::run(bool &endGame){
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

OptionQuit::OptionQuit(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "quit" ){
        throw LoadException("Not quit option");
    }

    readName(token);
}

OptionQuit::OptionQuit( const std::string &name ) throw( LoadException ):
MenuOption(0, Event){
    if (name.empty()){
	throw LoadException("No name given to quit");
    }
    this->setText(name);
}

OptionQuit::~OptionQuit(){
}

void OptionQuit::logic(){
}

void OptionQuit::run(bool &endGame){
	endGame = true;
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

OptionScreenSize::OptionScreenSize(Token *token) throw (LoadException):
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
        throw LoadException("Not a screen-size");
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

void OptionScreenSize::run(bool &endGame){
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

static string join(const vector<string> & strings, const string & middle){
    ostringstream out;

    for (vector<string>::const_iterator it = strings.begin(); it != strings.end(); it++){
        out << *it << middle;
    }

    return out.str();
}

OptionSelectFont::OptionSelectFont(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
typeAdjust(fontName),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "font-select" ){
        throw LoadException("Not a font selector");
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
                else throw LoadException("Incorrect value \"" + temp + "\" in font-select");
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
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

    // Find and set fonts now
    if (typeAdjust == fontName){
        try{
            Filesystem::AbsolutePath fontsDirectory = Filesystem::find(Filesystem::RelativePath("fonts"));
            Global::debug(1, "fonts") << "Font directory " << fontsDirectory.path() << endl;
            vector<string> ttfFonts = Util::getFiles(fontsDirectory, "*.ttf");
            Global::debug(1, "fonts") << "Found ttf fonts " << join(ttfFonts, ", ") << endl;
            vector<string> otfFonts = Util::getFiles(fontsDirectory, "*.otf");
            Global::debug(1, "fonts") << "Found otf fonts " << join(otfFonts, ", ") << endl;
            std::back_insert_iterator< std::vector<string> > inserter(fonts);
            copy(ttfFonts.begin(), ttfFonts.end(), inserter);
            copy(otfFonts.begin(), otfFonts.end(), inserter);
        } catch (const Filesystem::NotFound & e){
            throw LoadException("Could not load font: " + e.getReason());
        }
    }
}

OptionSelectFont::~OptionSelectFont(){
    // Nothing
}

void OptionSelectFont::logic(){
    switch (typeAdjust){
        case fontName:	  
            setText("Current Font: " + Menu::getFont().path());
            break;
        case fontWidth:{
            ostringstream temp;
            temp << "Font Width: " << Menu::getFontWidth();
            setText(temp.str());
            break;
        }
        case fontHeight:{
            ostringstream temp;
            temp << "Font Height: " << Menu::getFontHeight();
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

void OptionSelectFont::run(bool &endGame){
}

bool OptionSelectFont::leftKey(){
    switch (typeAdjust){
        case fontName:
            nextIndex(false);
            break;
        case fontWidth:
            Menu::setFontWidth(Menu::getFontWidth() - 1);
            break;
        case fontHeight:
            Menu::setFontHeight(Menu::getFontHeight() - 1);
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
            Menu::setFontWidth(Menu::getFontWidth() + 1);
            break;
        case fontHeight:
            Menu::setFontHeight(Menu::getFontHeight() + 1);
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
        if (Menu::getFont().path() == fonts[i]){
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
    Menu::setFontName(fonts[index]);
}

OptionSpeed::OptionSpeed(Token *token) throw (LoadException): MenuOption(token, AdjustableOption), name(""), lblue(255), lgreen(255), rblue(255), rgreen(255)
{
    setRunnable(false);

    if ( *token != "speed" )
        throw LoadException("Not speed option");

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

void OptionSpeed::run(bool &endGame){
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

OptionTabMenu::OptionTabMenu(Token *token) throw (LoadException):
MenuOption(token, Event),
_menu(0){
    // Check whether we have a menu or tabmenu
    if (*token == "tabmenu"){
	_menu = new TabMenu();
    } else {
	throw LoadException("Not a tabbed menu");
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

void OptionTabMenu::run(bool &endGame){
	// Do our new menu
	_menu->run();
}

OptionVersus::OptionVersus( Token *token ) throw( LoadException ):
MenuOption(token, Event), human(false){
    if ( *token != "versus" ){
        throw LoadException("Not versus");
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
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
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

void OptionVersus::run(bool &endGame){
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
				throw LoadException("Null player");
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
	} catch ( const ReturnException & r ){
		key.wait();
	}

	if ( player != NULL ){
		delete player;
	}
	if ( enemy != NULL ){
		delete enemy;
	}
}

OptionSound::OptionSound(Token *token):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if (*token != "sound" ){
        throw LoadException("Not a sound option");
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

void OptionSound::run(bool &endGame){
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
        throw LoadException("Not a music option");
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

void OptionMusic::run(bool &endGame){
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
