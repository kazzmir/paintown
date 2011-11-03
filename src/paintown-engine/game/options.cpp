#include "options.h"
#include "menu/menu_option.h"
#include "menu/options.h"
#include "menu/menu-exception.h"
#include "../object/player.h"
#include "../object/buddy_player.h"
#include "../level/utils.h"
#include "mod.h"
#include "menu/menu.h"
#include "configuration.h"
#include "game.h"
#include "util/file-system.h"
#include "util/funcs.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "../network/server.h"
#include "../network/client.h"
#include "util/input/input-source.h"
#include "util/input/joystick.h"
#include <sstream>
#include <vector>

#include "openbor/pack-reader.h"

using namespace std;

namespace Paintown{

Level::LevelInfo doLevelMenu(const string dir, const Menu::Context & context){
    vector<Level::LevelInfo> possible = Mod::getCurrentMod()->getLevels();
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
        Gui::ContextBox & box = ((Menu::DefaultRenderer *) temp.getRenderer())->getBox();
        box.setListType(Gui::ContextBox::Normal);
        for ( unsigned int i = 0; i < possible.size(); i++ ){
            OptionLevel *opt = new OptionLevel(box, 0, &index, i);
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

OptionFactory::OptionFactory(){
}

class OptionAdventure: public MenuOption {
public:
    OptionAdventure(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token){
            if ( *token != "adventure" ){
                throw LoadException(__FILE__, __LINE__, "Not an adventure");
            }

            readName(token);
        }

    ~OptionAdventure(){
        // Nothing
    }

    void logic(){
    }

    void run(const Menu::Context & context){
        Object * player = NULL;
        try{
            //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
            Level::LevelInfo info = doLevelMenu("/levels", context);

            Global::debug(1) << "Selecting players" << endl;
            int remap = 0;
            Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap, InputSource(0, 0));

            PlayerFuture future(path, Configuration::getInvincible(), Configuration::getLives(), remap, Util::ReferenceCount<InputSource>(new InputSource(0, 0)));
            vector<Util::Future<Object *> *> players;
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
};

/* multi-player local mode */
class OptionAdventureLocal: public MenuOption {
public:
    OptionAdventureLocal(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token){
            if (*token != "adventure-local"){
                throw LoadException(__FILE__, __LINE__, "Not an adventure-local");
            }

            readName(token);
        }

    virtual ~OptionAdventureLocal(){
    }

    void logic(){
    }

    int numberOfKeyboards(){
        /* 2 keyboard configurations for now, probably make this configurable */
        return 2;
    }

    int maximumPlayers(){
        if (Keyboard::haveKeyboard()){
            return numberOfKeyboards() + Joystick::numberOfJoysticks();
        }
        return Joystick::numberOfJoysticks();
    }

    void showError(string what){
        Global::debug(0) << what << endl;
    }

    /* ask the user how many players will be in the game */
    int howManyPlayers(const Menu::Context & context){
        int maxPlayers = maximumPlayers();
        if (maxPlayers < 2){
            showError("Not enough input devices for multiplayer");
            throw Exception::Return(__FILE__, __LINE__);
        }
        int select = 0;
        Menu::Menu menu;
        const Gui::ContextBox & box = ((Menu::DefaultRenderer *) menu.getRenderer())->getBox();
        for (int i = 2; i <= maxPlayers; i++){
            OptionLevel * option = new OptionLevel(box, 0, &select, i);
            ostringstream out;
            out << i << " players";
            option->setText(out.str());
            menu.addOption(option);
        }

        try{
            menu.run(context);
        } catch (const Menu::MenuException & ignore){
        }
        return select;
    }

    /* ask the user(s) to select modes of input (keyboard, joystick...) */
    int selectInput(const Menu::Context & context, const vector<string> & names, int player){
        Menu::Menu menu;
        int select = 0;
        Gui::ContextBox & box = ((Menu::DefaultRenderer *) menu.getRenderer())->getBox();
        box.setListType(Gui::ContextBox::Normal);
        int index = 0;
        for (vector<string>::const_iterator it = names.begin(); it != names.end(); it++){
            OptionLevel * option = new OptionLevel(box, 0, &select, index);
            option->setText(*it);
            ostringstream out;
            out << "Select input for player " << player;
            option->setInfoText(out.str());
            menu.addOption(option);
            index += 1;
        }

        try{
            menu.run(context);
        } catch (const Menu::MenuException & ignore){
        }
        return select;
    }

    vector<Util::ReferenceCount<InputSource> > getInputSources(const Menu::Context & context, int players){
        vector<Util::ReferenceCount<InputSource> > sources;

        vector<string> names;
        vector<Util::ReferenceCount<InputSource> > possible;

        if (Keyboard::haveKeyboard()){
            for (int keyboard = 0; keyboard < numberOfKeyboards(); keyboard++){
                possible.push_back(Util::ReferenceCount<InputSource>(new InputSource(keyboard, -1)));
                ostringstream out;
                out << "Keyboard " << (keyboard + 1);
                names.push_back(out.str());
            }
        }

        for (int i = 0; i < Joystick::numberOfJoysticks(); i++){
            possible.push_back(Util::ReferenceCount<InputSource>(new InputSource(-1, i)));
            ostringstream out;
            out << "Joystick " << (i + 1);
            names.push_back(out.str());
        }

        for (int player = 0; player < players; player++){
            int selection = selectInput(context, names, player + 1);
            Util::ReferenceCount<InputSource> source = possible[selection];
            possible.erase(possible.begin() + selection);
            names.erase(names.begin() + selection);
            sources.push_back(source);
        }

        return sources;
    }

    vector<Util::Future<Object*>* > selectPlayers(const Menu::Context & context, int players, const vector<Util::ReferenceCount<InputSource> > & sources){
        vector<Util::Future<Object*>* > futures;

        for (int player = 0; player < players; player++){
            int remap = 0;
            ostringstream out;
            out << "Pick player " << (player + 1);
            Level::LevelInfo info;
            Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer(out.str(), info, remap, *sources[player]);
            Util::Future<Object*> * selection = new PlayerFuture(path, Configuration::getInvincible(), Configuration::getLives(), remap, sources[player]);
            futures.push_back(selection);
        }

        return futures;
    }

    void run(const Menu::Context & context){
        try{

            /* 1. ask how many players
             * 2. each player selects a form of input (keyboard, joystick 1/2/3/4)
             * 3. do character selection for each player
             * 4. bind players together
             * 5. run game as normal
             */

            int players = howManyPlayers(context);
            vector<Util::ReferenceCount<InputSource> > sources = getInputSources(context, players);
            Level::LevelInfo info = doLevelMenu("/levels", context);
            vector<Util::Future<Object*>* > futures = selectPlayers(context, players, sources);
            Game::realGameLocal(futures, info);

        } catch (const Exception::Return & fail){
            /* nothing */
        }

        /*
        Object * player = NULL;
        try{
            //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
            Level::LevelInfo info = doLevelMenu("/levels", context);

            Global::debug(1) << "Selecting players" << endl;
            int remap = 0;
            Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap);

            PlayerFuture future(path, Configuration::getInvincible(), Configuration::getLives(), remap);
            vector<Util::Future<Object *> *> players;
            players.push_back(&future);
            Game::realGame(players, info);
        } catch ( const LoadException & le ){
            Global::debug(0) << "Error while loading: " << le.getTrace() << endl;
        } catch (const Exception::Return & ignore){
            throw Menu::Reload(__FILE__, __LINE__);
        }
        */
    }
};

class OptionAdventureCpu: public MenuOption {
public:
    OptionAdventureCpu(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token){
            if (*token != "adventure-cpu"){
                throw LoadException(__FILE__, __LINE__, "Not an adventure");
            }

            readName(token);
        }

    ~OptionAdventureCpu(){
    }

    void logic(){
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

    void run(const Menu::Context & context){
        int max_buddies = Configuration::getNpcBuddies();

        Object * player = NULL;
        vector<Util::Future<Object*>* > futures;
        vector<Object *> buddies;
        try{
            Level::LevelInfo info = doLevelMenu("/levels", context);

            int remap;
            Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap, InputSource(0, 0));
            Util::Future<Object*> * player = new PlayerFuture(path, Configuration::getInvincible(), Configuration::getLives(), remap, Util::ReferenceCount<InputSource>(new InputSource(0, 0)));
            futures.push_back(player);

            for ( int i = 0; i < max_buddies; i++ ){
                ostringstream out;
                out << "Pick buddy " << nthWord(i+1);
                int remap;
                Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer(out.str(), info, remap, InputSource());
                Util::Future<Object*> * buddy = new BuddyFuture(path, player, remap, -(i+2));
                futures.push_back(buddy);
            }

            Game::realGame(futures, info);
        } catch ( const LoadException & le ){
            Global::debug( 0 ) << "Could not load player: " << le.getTrace() << endl;
        } catch (const Exception::Return & ignore){
            throw Menu::Reload(__FILE__, __LINE__);
        }

        for (vector<Util::Future<Object*>*>::iterator it = futures.begin(); it != futures.end(); it++){
            delete *it;
        }
    }
};

class OptionChangeMod: public MenuOption {
public:
    OptionChangeMod(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token){
            if ( *token != "change-mod" ){
                throw LoadException(__FILE__, __LINE__, "Not a change mod");
            }

            readName(token);
        }

    ~OptionChangeMod(){
        // Nothing
    }

    void logic(){
    }

    static bool isModFile(const std::string & path){
        try{
            TokenReader reader;
            Global::debug(1) << "Checking for a mod in " << path << endl;
            if (*reader.readTokenFromFile(path) == "game"){
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
            Global::debug(0) << "Error reading pak file: " << error.getTrace() << endl;
            return false;
        }
    }

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

    static vector<ModType> findMods(){
        vector<ModType> mods;

        vector<Filesystem::AbsolutePath> directories = Storage::instance().findDirectories(Filesystem::RelativePath("."));
        for (vector<Filesystem::AbsolutePath>::iterator dir = directories.begin(); dir != directories.end(); dir++){
            string file = (*dir).path() + "/" + Storage::instance().cleanse(*dir).path() + ".txt";
            if (isModFile(file)){
                mods.push_back(ModType(Filesystem::AbsolutePath(file), ModType::Paintown));
            }
        }

        try{
            vector<Filesystem::AbsolutePath> pakFiles = Storage::instance().getFiles(Storage::instance().find(Filesystem::RelativePath("paks")), "*", true);
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
            TokenReader reader;
            Global::debug(1) << "Checking for a mod in " << path.path() << endl;
            const Token * name_token = reader.readTokenFromFile(path.path())->findToken("game/name");
            if (name_token != NULL){
                string name;
                name_token->view() >> name;
                return name;
            }
            return Storage::instance().cleanse(path).path();
        } catch (const TokenException & e){
            return Storage::instance().cleanse(path).path();
        }
    }

    static string modNameOpenbor(const Filesystem::AbsolutePath & path){
        return "OpenBor " + Storage::instance().cleanse(path).path();
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

    void run(const Menu::Context & context){
        try{
            int select = 0;
            Menu::Menu menu;
            vector<ModType> mods = findMods();
            map<int, ModType*> modMap;
            int index = 0;
            std::vector<OptionLevel *> options;
            Gui::ContextBox & box = ((Menu::DefaultRenderer *) menu.getRenderer())->getBox();
            box.setListType(Gui::ContextBox::Normal);
            box.getListValues().setDistanceFade(false);
            box.getListValues().setOtherColor(Graphics::makeColor(192, 192, 192));
            for (vector<ModType>::iterator it = mods.begin(); it != mods.end(); it++){
                OptionLevel *opt = new OptionLevel(box, 0, &select, index);
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

};

#ifdef HAVE_NETWORKING
class OptionNetworkHost: public MenuOption {
public:
    OptionNetworkHost(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token){
            if ( *token != "network-host" ){
                throw LoadException(__FILE__, __LINE__, "Not a network-host");
            }

            readName(token);
        }

    ~OptionNetworkHost(){
        // Nothing
    }

    void logic(){
    }

    void run(const Menu::Context & context){
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
};

class OptionNetworkJoin: public MenuOption {
public:
    OptionNetworkJoin(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token){
            if ( *token != "network-join" ){
                throw LoadException(__FILE__, __LINE__, "Not a network-join");
            }

            readName(token);
        }

    ~OptionNetworkJoin(){
        // Nothing
    }

    void logic(){
    }

    void run(const Menu::Context & context){
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
};
#endif

MenuOption * OptionFactory::getOption(const Gui::ContextBox & parent, const Token *token) const {
    const Token * child;
    token->view() >> child;

    if (*child == "adventure"){
        return new OptionAdventure(parent, child);
    } else if (*child == "adventure-cpu"){
        return new OptionAdventureCpu(parent, child);
    } else if (*child == "adventure-local"){
        return new OptionAdventureLocal(parent, child);
    } else if (*child == "change-mod"){
        return new OptionChangeMod(parent, child);
#ifdef HAVE_NETWORKING
    } else if (*child == "network-host" ){
        return new OptionNetworkHost(parent, child);
    } else if (*child == "network-join" ){
        return new OptionNetworkJoin(parent, child);
#endif
    }

    return Menu::OptionFactory::getOption(parent, token);
}

OptionFactory::~OptionFactory(){

}

}
