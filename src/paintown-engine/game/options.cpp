#include "options.h"
#include "r-tech1/menu/menu_option.h"
#include "r-tech1/menu/options.h"
#include "r-tech1/menu/menu-exception.h"
#include "r-tech1/menu/menu.h"
#include "r-tech1/configuration.h"
#include "r-tech1/file-system.h"
#include "r-tech1/funcs.h"
#include "r-tech1/token.h"
#include "r-tech1/input/input-source.h"
#include "r-tech1/input/joystick.h"
#include "r-tech1/tokenreader.h"
#include "../object/player.h"
#include "../object/buddy_player.h"
#include "../object/gib.h"
#include "../level/utils.h"
#include "mod.h"
#include "game.h"
#include "../network/server.h"
#include "../network/client.h"
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
        Util::ReferenceCount<Menu::DefaultRenderer> renderer = Util::ReferenceCount<Menu::DefaultRenderer>(new Menu::DefaultRenderer());
        Menu::Menu temp(renderer);
        
        int index = 0;
        Gui::ContextBox & box = renderer->getBox();
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
        try{
            //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
            Level::LevelInfo info = doLevelMenu("/levels", context);

            Global::debug(1) << "Selecting players" << endl;
            int remap = 0;
            Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap, InputSource(false).addKeyboard(0).addJoystick(0));

            // PlayerFuture future(path, Configuration::getInvincible(), Configuration::getLives(), remap, Util::ReferenceCount<InputSource>(new InputSource(InputSource(false).addKeyboard(0).addJoystick(0))));
            Util::ReferenceCount<InputSource> source(new InputSource(InputSource(false).addKeyboard(0).addJoystick(0)));
            Paintown::Player* playerObject = new Paintown::Player(path, source);
            playerObject->setInvincible(Configuration::getInvincible());
            playerObject->setMap(remap);
            playerObject->setLives(Configuration::getLives());

            vector<Util::ReferenceCount<Object>> players;
            players.push_back(Util::ReferenceCount<Object>(playerObject));
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
        Util::ReferenceCount<Menu::DefaultRenderer> renderer = Util::ReferenceCount<Menu::DefaultRenderer>(new Menu::DefaultRenderer());
        Menu::Menu menu(renderer);
        const Gui::ContextBox & box = renderer->getBox();
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
        Util::ReferenceCount<Menu::DefaultRenderer> renderer = Util::ReferenceCount<Menu::DefaultRenderer>(new Menu::DefaultRenderer());
        Menu::Menu menu(renderer);
        int select = 0;
        Gui::ContextBox & box = renderer->getBox();
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
                possible.push_back(Util::ReferenceCount<InputSource>(new InputSource(InputSource(false).addKeyboard(keyboard))));
                ostringstream out;
                out << "Keyboard " << (keyboard + 1);
                names.push_back(out.str());
            }
        }

        for (int i = 0; i < Joystick::numberOfJoysticks(); i++){
            possible.push_back(Util::ReferenceCount<InputSource>(new InputSource(InputSource(false).addJoystick(i))));
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

    vector<Util::ReferenceCount<Object>> selectPlayers(const Menu::Context & context, int playersCount, const vector<Util::ReferenceCount<InputSource> > & sources){
        vector<Util::ReferenceCount<Object>> players;

        for (int player = 0; player < playersCount; player++){
            int remap = 0;
            ostringstream out;
            out << "Pick player " << (player + 1);
            Level::LevelInfo info;
            Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer(out.str(), info, remap, *sources[player]);

            Paintown::Player* playerObject = new Paintown::Player(path, sources[player]);
            playerObject->setInvincible(Configuration::getInvincible());
            playerObject->setMap(remap);
            playerObject->setLives(Configuration::getLives());
            players.push_back(Util::ReferenceCount<Object>(playerObject));

            /*
            Util::Future<Object*> * selection = new PlayerFuture(path, Configuration::getInvincible(), Configuration::getLives(), remap, sources[player]);
            futures.push_back(selection);
            */
        }

        return players;
    }

    void run(const Menu::Context & context){
        try{

            /* 1. ask how many players
             * 2. each player selects a form of input (keyboard, joystick 1/2/3/4)
             * 3. do character selection for each player
             * 4. bind players together
             * 5. run game as normal
             */

            int playerCount = howManyPlayers(context);
            vector<Util::ReferenceCount<InputSource> > sources = getInputSources(context, playerCount);
            Level::LevelInfo info = doLevelMenu("/levels", context);

            vector<Util::ReferenceCount<Object>> players = selectPlayers(context, playerCount, sources);
            Game::realGameLocal(players, info);

        } catch (const Exception::Return & fail){
            /* nothing */
        }
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
        vector<Util::ReferenceCount<Object>> players;
        // vector<Object *> buddies;
        try{
            Level::LevelInfo info = doLevelMenu("/levels", context);

            int remap;
            InputSource source = InputSource(false).addKeyboard(0).addJoystick(0);
            Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer("Pick a player", info, remap, source);
            // Util::Future<Object*> * player = new PlayerFuture(path, Configuration::getInvincible(), Configuration::getLives(), remap, Util::ReferenceCount<InputSource>(new InputSource(source)));
            // futures.push_back(player);
            Paintown::Player* player = new Paintown::Player(path, Util::ReferenceCount<InputSource>(new InputSource(source)));
            player->setInvincible(Configuration::getInvincible());
            player->setMap(remap);
            player->setLives(Configuration::getLives());
            players.push_back(Util::ReferenceCount<Object>(player));

            vector<Util::ReferenceCount<Paintown::Object>> buddies;

            for ( int i = 0; i < max_buddies; i++ ){
                ostringstream out;
                out << "Pick buddy " << nthWord(i+1);
                int remap;
                Filesystem::AbsolutePath path = Mod::getCurrentMod()->selectPlayer(out.str(), info, remap, source);
                /*
                Util::Future<Object*> * buddy = new BuddyFuture(path, player, remap, -(i+2));
                futures.push_back(buddy);
                */
                Paintown::Player base(path, Util::ReferenceCount<InputSource>(NULL));
                base.setMap(remap);
                Paintown::Object * buddy = new BuddyPlayer(player, base);
                buddy->setObjectId(-(i+2));
                players.push_back(Util::ReferenceCount<Paintown::Object>(buddy));
            }

            Game::realGame(players, info);
        } catch ( const LoadException & le ){
            Global::debug( 0 ) << "Could not load player: " << le.getTrace() << endl;
        } catch (const Exception::Return & ignore){
            throw Menu::Reload(__FILE__, __LINE__);
        }

        /*
        for (vector<Util::Future<Object*>*>::iterator it = futures.begin(); it != futures.end(); it++){
            delete *it;
        }
        */
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

    static bool isModFile(const Util::ReferenceCount<Storage::File> & path){
        try{
            TokenReader reader;
            // Global::debug(1) << "Checking for a mod in " << path << endl;
            if (*reader.readTokenFromFile(*path) == "game"){
                return true;
            }
        } catch (const TokenException & e){
            return false;
        }
        return false;
    }

    static string getModName(const Filesystem::AbsolutePath & path){
        try{
            TokenReader reader;
            // Global::debug(1) << "Checking for a mod in " << path << endl;
            Token * token = reader.readTokenFromFile(*Storage::instance().open(path));
            string name;
            if (token->match("game/name", name)){
                return name;
            }
            return "Unknown";
        } catch (const TokenException & e){
            return "Unknown";
        }
        return "Unknown";
    }

    static string getOpenborModName(const Filesystem::AbsolutePath & path){
        return "Openbor mod";
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

        ModType(const Filesystem::AbsolutePath & path, string name, Kind type):
            path(path),
            name(name),
            type(type){
            }

        Filesystem::AbsolutePath path; 
        string name;
        Kind type;
    };

    /* Search for zip files with mods in them.
     * For each zip file check what the top level directory is, then
     * try to find a file ending with .txt that contains a mod description.
     */
    static vector<ModType> findZippedMods(){
        vector<ModType> mods;
        const vector<Filesystem::AbsolutePath> zips = Storage::instance().getContainerFiles(Filesystem::RelativePath("."));
        for (vector<Filesystem::AbsolutePath>::const_iterator it = zips.begin(); it != zips.end(); it++){
            Filesystem::AbsolutePath zip = *it;
            Global::debug(0) << "Check container file " << zip.path() << std::endl;
            vector<string> files = Storage::instance().containerFileList(zip);

            Filesystem::AbsolutePath overlayed = Storage::instance().find(Filesystem::RelativePath("."));
            Storage::instance().addOverlay(zip, overlayed);
            bool found = false;

            for (vector<string>::iterator file_it = files.begin(); file_it != files.end(); file_it++){
                string file = *file_it;
                if (file.find(".txt") != string::npos){
                    Filesystem::RelativePath modFileRelative(file);
                    try{
                        Filesystem::AbsolutePath modFile = Storage::instance().find(modFileRelative);
                        if (isModFile(Storage::instance().open(modFile))){
                            mods.push_back(ModType(modFile, getModName(modFile), ModType::Paintown));
                            found = true;
                            /* Only find one file that works */
                            break;
                        }
                    } catch (const Filesystem::NotFound & fail){
                    }
                }
            }

            if (!found){
                Storage::instance().removeOverlay(zip, overlayed);
            }
        }
        return mods;
    }

    static vector<ModType> findOpenborMods(){
        vector<ModType> mods;
        try{
            vector<Filesystem::AbsolutePath> pakFiles = Storage::instance().getFiles(Storage::instance().find(Filesystem::RelativePath("paks")), "*", true);
            for (vector<Filesystem::AbsolutePath>::iterator it = pakFiles.begin(); it != pakFiles.end(); it++){
                const Filesystem::AbsolutePath & path = *it;
                if (isOpenBorPackfile(path)){
                    Global::debug(0) << "Found openbor pakfile " << path.path() << endl;
                    mods.push_back(ModType(path, getOpenborModName(path), ModType::Openbor));
                }
            }
        } catch (const Filesystem::NotFound & n){
            Global::debug(0) << "Could not find any pak files: " << n.getTrace() << endl;
        }

        return mods;
    }

    static vector<ModType> findNormalMods(){
        vector<ModType> mods;
        vector<Filesystem::AbsolutePath> directories = Storage::instance().findDirectories(Filesystem::RelativePath("."));
        for (vector<Filesystem::AbsolutePath>::iterator dir = directories.begin(); dir != directories.end(); dir++){
            try{
                /* FIXME: use join here..? */
                Filesystem::AbsolutePath file((*dir).path() + "/" + Storage::instance().cleanse(*dir).path() + ".txt");
                if (Storage::instance().exists(file) && isModFile(Storage::instance().open(file))){
                    mods.push_back(ModType(file, getModName(file), ModType::Paintown));
                }
            } catch (const Filesystem::NotFound & fail){
            }
        }
        return mods;
    }

    static vector<ModType> findMods(){
        vector<ModType> mods;

        vector<ModType> normalMods = findNormalMods();
        mods.insert(mods.end(), normalMods.begin(), normalMods.end());

        vector<ModType> zippedMods = findZippedMods();
        mods.insert(mods.end(), zippedMods.begin(), zippedMods.end());

        vector<ModType> openBorMods = findOpenborMods();
        mods.insert(mods.end(), openBorMods.begin(), openBorMods.end());

        return mods;
    }

    /*
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
    */

    static string modName(const ModType & mod){
        return mod.name;
    }

    static void changeMod(const ModType & mod){
        switch (mod.type){
            case ModType::Paintown : {
                Configuration::setProperty("paintown/mod", mod.path.path());
                Paintown::Mod::loadPaintownMod(mod.path);
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
            Util::ReferenceCount<Menu::DefaultRenderer> renderer = Util::ReferenceCount<Menu::DefaultRenderer>(new Menu::DefaultRenderer());
            Menu::Menu menu(renderer);
            vector<ModType> mods = findMods();
            map<int, ModType*> modMap;
            int index = 0;
            std::vector<OptionLevel *> options;
            Gui::ContextBox & box = renderer->getBox();
            box.setListType(Gui::ContextBox::Normal);
            box.getListValues().setDistanceFade(false);
            box.getListValues().setOtherColor(Graphics::makeColor(192, 192, 192));
            for (vector<ModType>::iterator it = mods.begin(); it != mods.end(); it++){
                OptionLevel *opt = new OptionLevel(box, 0, &select, index);
                string name = modName(*it);
                modMap[index] = &(*it);
                opt->setText(name);
                opt->setInfoText("Choose this mod");
                if (Util::upcase(name).compare(Util::upcase(Mod::getCurrentMod()->getName())) == 0){
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

class OptionGibs: public MenuOption {
public:
    OptionGibs(const Gui::ContextBox & parent, const Token *token):
    MenuOption(parent, token){
        setRunnable(false);

        if (*token != "gibs" ){
            throw LoadException(__FILE__, __LINE__, "Not a gibs option");
        }

        readName(token);
        originalName = getName();
    }

    void logic(){
        ostringstream temp;
        temp << originalName << ": " << Configuration::getProperty(Gib::GibProperty, 5);
        setText(temp.str());
    }

    void run(const Menu::Context & context){
    }

    void changeGibs(int much){
        int gibs = Configuration::getProperty("paintown/gibs", 5);
        gibs += much;
        if (gibs < 0){
            gibs = 0;
        }

        if (gibs > 10){
            gibs = 10;
        }

        Configuration::setProperty("paintown/gibs", gibs);
    }

    bool leftKey(){
        changeGibs(-1);
        return true;
    }

    bool rightKey(){
        changeGibs(+1);
        return true;
    }

    ~OptionGibs(){
    }

private:
    std::string originalName;
};

class OptionPlayMode: public MenuOption {
public:
    OptionPlayMode(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token),
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

    ~OptionPlayMode(){
        // Nothing
    }
    
    std::string getText() const {
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

    void logic(){
    }

    void run(const Menu::Context & context){
    }
    
    void changeMode(){
        if (Configuration::getPlayMode() == Configuration::FreeForAll){
            Configuration::setPlayMode(Configuration::Cooperative);
        } else if (Configuration::getPlayMode() == Configuration::Cooperative){
            Configuration::setPlayMode(Configuration::FreeForAll);
        }
    }

    bool leftKey(){
        changeMode();
        lblue = lgreen = 0;
        return true;
    }

    bool rightKey(){
        changeMode();
        rblue = rgreen = 0;
        return true;
    }

private:

    int lblue, lgreen;
    int rblue, rgreen;
};

class OptionNpcBuddies: public MenuOption {
public:
    OptionNpcBuddies(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token)
        /*
        ,
        lblue(255),
        lgreen(255),
        rblue(255),
        rgreen(255)
        */
        {
            setRunnable(false);

            if ( *token != "npc" ){
                throw LoadException(__FILE__, __LINE__,  "Not npc option" );
            }

            readName(token);
        }

    ~OptionNpcBuddies(){
        // Nothing
    }
        
    std::string getText() const {
        ostringstream out;
        out << MenuOption::getText() << ": " << Configuration::getNpcBuddies();
        return out.str();
    }

    void logic(){
    }

    void run(const Menu::Context & context){
    }

    bool leftKey(){
        Configuration::setNpcBuddies(Configuration::getNpcBuddies() - 1);
        if ( Configuration::getNpcBuddies() < 1 ){
            Configuration::setNpcBuddies(1);
        }

        return false;
    }

    bool rightKey(){
        Configuration::setNpcBuddies( Configuration::getNpcBuddies() + 1 );
        // rblue = rgreen = 0;
        return false;
    }

private:
    /*
    int lblue, lgreen;
    int rblue, rgreen;
    */
};

class OptionInvincible: public MenuOption {
public:

    OptionInvincible(const Gui::ContextBox & parent, const Token *token):
        MenuOption(parent, token)
        /*
        ,
        lblue(255),
        lgreen(255),
        rblue(255),
        rgreen(255)
        */
        {
            setRunnable(false);

            if ( *token != "invincible" )
                throw LoadException(__FILE__, __LINE__, "Not invincible option");

            readName(token);
        }

    ~OptionInvincible(){
    }

    std::string getText() const {
        ostringstream out;
        out << MenuOption::getText() << ": " << (Configuration::getInvincible() ? "Yes" : "No");
        return out.str();
    }

    void logic(){
    }

    void run(const Menu::Context & context){
    }

    bool leftKey(){
        Configuration::setInvincible(!Configuration::getInvincible());
        return true;
    }

    bool rightKey(){
        Configuration::setInvincible(!Configuration::getInvincible());
        return true;
    }

private:
    /*
    int lblue, lgreen;
    int rblue, rgreen;
    */
};

class OptionLives: public MenuOption {
public:

    OptionLives(const Gui::ContextBox & parent, const Token * token):
        MenuOption(parent, token)
        /*
        ,
        lblue(255),
        lgreen(255),
        rblue(255),
        rgreen(255)
        */
        {
            setRunnable(false);

            if ( *token != "lives" ){
                throw LoadException(__FILE__, __LINE__,  "Not lives option" );
            }

            readName(token);
        }

    ~OptionLives(){
    }
    
    std::string getText() const {
        ostringstream out;
        out << MenuOption::getText() << ": " << Configuration::getLives();
        return out.str();
    }

    void logic(){
    }

    void run(const Menu::Context & context){
    }

    bool leftKey(){
        Configuration::setLives(Configuration::getLives() - 1);
        if ( Configuration::getLives() < 1 ){
            Configuration::setLives(1);
        }

        return false;
    }

    bool rightKey(){
        Configuration::setLives(Configuration::getLives() + 1);
        return false;
    }

private:
    /*
    int lblue, lgreen;
    int rblue, rgreen;
    */
};

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
    } else if (*child == "gibs"){
        return new OptionGibs(parent, child);
    } else if (*child == "play-mode"){
        return new OptionPlayMode(parent, child);
    } else if (*child == "npc"){
        return new OptionNpcBuddies(parent, child);
    } else if (*child == "lives" ){
        return new OptionLives(parent, child);
    } else if (*child == "invincible" ){
        return new OptionInvincible(parent, child);
#ifdef HAVE_NETWORKING
    } else if (*child == "network-host" ){
        return new OptionNetworkHost(parent, child);
    } else if (*child == "network-join" ){
        return new OptionNetworkJoin(parent, child);
#endif
    }

    return NULL;
}

OptionFactory::~OptionFactory(){

}

}
