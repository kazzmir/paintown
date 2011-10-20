#include <string.h>
#include <string>
#include <sstream>
#include <vector>

#include "util/bitmap.h"

#include "factory/collector.h"
#include "util/network/network.h"
#include "util/token_exception.h"
#include "util/system.h"
#include "mugen/exception.h"
#include "mugen/menu.h"
#include "mugen/game.h"
#include "util/music.h"
#include "menu/menu.h"
#include "menu/menu-exception.h"
#include "menu/optionfactory.h"
#include "util/input/input-manager.h"
#include "paintown-engine/game/options.h"
#include "paintown-engine/game/mod.h"
#include "paintown-engine/network/client.h"
#include "paintown-engine/network/server.h"
#include "exceptions/shutdown_exception.h"
#include "exceptions/exception.h"
#include "util/timedifference.h"
#include "util/funcs.h"
#include "util/ftalleg.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/parameter.h"
#include "globals.h"
#include "util/debug.h"
#include "configuration.h"
#include "util/init.h"
#include "mugen/config.h"

using std::vector;
using std::endl;
using std::string;
using std::istringstream;

#define NUM_ARGS(d) (sizeof(d)/sizeof(char*))
static const char * WINDOWED_ARG[] = {"-w", "fullscreen", "nowindowed", "no-windowed"};
static const char * DATAPATH_ARG[] = {"-d", "--data", "data", "datapath", "data-path", "path"};
static const char * DEBUG_ARG[] = {"-l", "--debug", "debug"};
static const char * MUSIC_ARG[] = {"-m", "music", "nomusic", "no-music"};
static const char * NETWORK_SERVER_ARG[] = {"server", "network-server"};
static const char * NETWORK_JOIN_ARG[] = {"network-join"};
static const char * MUGEN_ARG[] = {"mugen", "--mugen"};
static const char * MUGEN_INSTANT_ARG[] = {"mugen:training"};
static const char * MUGEN_INSTANT_WATCH_ARG[] = {"mugen:watch"};
static const char * JOYSTICK_ARG[] = {"joystick", "nojoystick", "no-joystick"};
static const char * DISABLE_QUIT_ARG[] = {"disable-quit"};
static const char * RATE_LIMIT_ARG[] = {"fps", "rate-limit"};

static const char * closestMatch(const char * s1, vector<const char *> args){
    const char * good = NULL;
    int minimum = -1;
    for (vector<const char *>::iterator it = args.begin(); it != args.end(); it++){
        const char * compare = *it;
        if (strlen(compare) > 2){
            int distance = Util::levenshtein(s1, compare);
            if (distance != -1 && (minimum == -1 || distance < minimum)){
                minimum = distance;
                good = compare;
            }
        }
    }

    return good;
}

static bool isArg(const char * s1, const char * s2[], int num){
    for (int i = 0; i < num; i++){
        if (strcasecmp(s1, s2[i]) == 0){
            return true;
        }
    }

    return false;
}

/* {"a", "b", "c"}, 3, ',' => "a, b, c"
 */
static const char * all(const char * args[], const int num, const char separate = ','){
    static char buffer[1<<10];
    strcpy(buffer, "");
    for (int i = 0; i < num; i++){
        char fuz[10];
        sprintf(fuz, "%c ", separate);
        strcat(buffer, args[i]);
        if (i != num - 1){
            strcat(buffer, fuz);
        }
    }
    return buffer;
}

static void showOptions(){
    Global::debug(0) << "Paintown by Jon Rafkind" << endl;
    Global::debug(0) << "Command line options" << endl;
    Global::debug(0) << " " << all(WINDOWED_ARG, NUM_ARGS(WINDOWED_ARG), ',') << " : Fullscreen mode" << endl;
    Global::debug(0) << " " << all(DATAPATH_ARG, NUM_ARGS(DATAPATH_ARG)) << " <path> : Use data path of <path>. Default is " << Util::getDataPath2().path() << endl;
    Global::debug(0) << " " << all(DEBUG_ARG, NUM_ARGS(DEBUG_ARG)) << " # : Enable debug statements. Higher numbers gives more debugging. Default is 0. Negative numbers are allowed. Example: -l 3" << endl;
    Global::debug(0) << " " << all(MUSIC_ARG, NUM_ARGS(MUSIC_ARG)) << " : Turn off music" << endl;
    Global::debug(0) << " " << all(MUGEN_ARG, NUM_ARGS(MUGEN_ARG)) << " : Go directly to the mugen menu" << endl;
    Global::debug(0) << " " << all(MUGEN_INSTANT_ARG, NUM_ARGS(MUGEN_INSTANT_ARG)) << " <player 1 name>,<player 2 name>,<stage> : Start training game with the specified players and stage" << endl;
    Global::debug(0) << " " << all(MUGEN_INSTANT_WATCH_ARG, NUM_ARGS(MUGEN_INSTANT_WATCH_ARG)) << " <player 1 name>,<player 2 name>,<stage> : Start watch game with the specified players and stage" << endl;
    Global::debug(0) << " " << all(JOYSTICK_ARG, NUM_ARGS(JOYSTICK_ARG)) << " : Disable joystick input" << endl;
    Global::debug(0) << " " << all(DISABLE_QUIT_ARG, NUM_ARGS(DISABLE_QUIT_ARG)) << " : Don't allow the game to exit using the normal methods" << endl;
    Global::debug(0) << " " << all(RATE_LIMIT_ARG, NUM_ARGS(RATE_LIMIT_ARG)) << " : Don't rate limit the game to the default fps (40). This is only useful for benchmarking graphics capabilities." << endl;
#ifdef HAVE_NETWORKING
    Global::debug(0) << " " << all(NETWORK_SERVER_ARG, NUM_ARGS(NETWORK_SERVER_ARG)) << " : Go straight to the network server" << endl;
    Global::debug(0) << " " << all(NETWORK_JOIN_ARG, NUM_ARGS(NETWORK_JOIN_ARG)) << " [<name>,<server ip>,<port>]: Join a network game directly. If not given, ip and port will be read from the configuration file if one exists otherwise they default to 127.0.0.1:7887. The name will be randomly generated if not given. Do not put spaces between the commas in the optional arguments." << endl;
#endif
    Global::debug(0) << endl;
}

static void addArgs(vector<const char *> & args, const char * strings[], int num){
    for (int i = 0; i < num; i++){
        args.push_back(strings[i]);
    }
}

static void setMugenMotif(const Filesystem::AbsolutePath & path){
    if (Configuration::getMugenMotif() != "default"){
        Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(Configuration::getMugenMotif()));
    } else {
        TokenReader reader;
        Token * head = reader.readToken(path.path());
        const Token * motif = head->findToken("menu/option/mugen/motif");
        if (motif != NULL){
            string path;
            motif->view() >> path;
            Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(path));
        }
    }
}

static Filesystem::AbsolutePath mainMenuPath(){
    string menu = Paintown::Mod::getCurrentMod()->getMenu();
    return Storage::instance().find(Filesystem::RelativePath(menu));
}

/*
static void hack(){
    Filesystem::AbsolutePath fontsDirectory = Filesystem::find(Filesystem::RelativePath("fonts"));
    Global::debug(1, "hack") << "Font directory " << fontsDirectory.path() << endl;
    vector<string> ttfFonts = Util::getFiles(fontsDirectory, "*.ttf");
    Global::debug(1, "hack") << "Fonts: " << ttfFonts.size() << endl;
}
*/

static bool parseMugenInstant(string input, string * player1, string * player2, string * stage){
    unsigned int comma = input.find(',');
    if (comma == string::npos){
        Global::debug(0) << "Expected three arguments separated by a comma, only 1 was given: " << input << endl;
        return false;
    }
    *player1 = input.substr(0, comma);
    input.erase(0, comma + 1);
    comma = input.find(',');

    if (comma == string::npos){
        Global::debug(0) << "Expected three arguments separated by a comma, only 2 were given: " << input << endl;
        return false;
    }

    *player2 = input.substr(0, comma);
    input.erase(0, comma + 1);
    *stage = input;

    return true;
}

static void runMugenTraining(const string & player1, const string & player2, const string & stage){
    Global::debug(0) << "Mugen training mode player1 '" << player1 << "' player2 '" << player2 << "' stage '" << stage << "'" << endl;
    Mugen::Game::startTraining(player1, player2, stage);
}

static void runMugenWatch(const string & player1, const string & player2, const string & stage){
    Global::debug(0) << "Mugen watch mode player1 '" << player1 << "' player2 '" << player2 << "' stage '" << stage << "'" << endl;
    Mugen::Game::startWatch(player1, player2, stage);
}

static vector<string> split(string input, char splitter){
    size_t find = input.find(splitter);
    vector<string> out;
    while (find != string::npos){
        out.push_back(input.substr(0, find));
        input.erase(0, find + 1);
        find = input.find(splitter);
    }
    out.push_back(input);
    return out;
}

static void parseNetworkJoin(const char * input, string & port, string & host, string & name){
    vector<string> args = split(input, ',');
    if (args.size() > 2){
        port = args[2];
    }
    if (args.size() > 1){
        host = args[1];
    }
    if (args.size() > 0){
        name = args[0];
    }
}

class MainMenuOptionFactory: public Menu::OptionFactory {
public:
    MainMenuOptionFactory(){
    }

    Paintown::OptionFactory paintownFactory;

    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const {
        MenuOption * get = paintownFactory.getOption(parent, token);
        if (get != NULL){
            return get;
        }
        return Menu::OptionFactory::getOption(parent, token);
    }
};

/* gets the mod name from system.txt */
static string systemMod(){
    TokenReader reader;
    Token * mod = reader.readTokenFromFile(Storage::instance().find(Filesystem::RelativePath("system.txt")).path().c_str());
    if (mod != NULL){
        string name;
        if (mod->match("_/default-mod", name)){
            return name;
        }
        throw LoadException(__FILE__, __LINE__, "No `default-mod' token");
    }
    throw LoadException(__FILE__, __LINE__, "Could not get system mod");
}

struct NetworkJoin{
    NetworkJoin():
        enabled(false){
        }

    bool enabled;
    string name;
    string host;
    string port;
}; 

struct MugenInstant{
    enum Kind{
        None,
        Training,
        Watch
    };

    MugenInstant():
        enabled(false),
        kind(None){
        }

    bool enabled;
    string player1;
    string player2;
    string stage;
    Kind kind;
}; 

/* dispatch to the top level function */
static int startMain(bool just_network_server, const NetworkJoin & networkJoin, const MugenInstant & mugenInstant, bool mugen, bool allow_quit){
    while (true){
        bool normal_quit = false;
        try{
            /* fadein from white */
            if (just_network_server){
#ifdef HAVE_NETWORKING
                Network::networkServer();
#endif
            } else if (networkJoin.enabled){
#ifdef HAVE_NETWORKING
                string port = networkJoin.port;
                string host = networkJoin.host;
                string name = networkJoin.name;
                if (port == ""){
                    /* FIXME: replace 7887 with a constant */
                    port = Configuration::getStringProperty(Network::propertyLastClientPort, "7887");
                }
                if (host == ""){
                    host = Configuration::getStringProperty(Network::propertyLastClientHost, "127.0.0.1");
                }
                if (name == ""){
                    name = Configuration::getStringProperty(Network::propertyLastClientName, "player");
                }
                Global::debug(1) << "Client " << name << " " << host << " " << port << endl;
                try{
                    Network::runClient(name, host, port);
                } catch (const Network::NetworkException & fail){
                    Global::debug(0) << "Error running the network client: " << fail.getMessage() << endl;
                }
#endif
            } else if (mugen){
                setMugenMotif(mainMenuPath());
                Mugen::run();
            } else if (mugenInstant.enabled && mugenInstant.kind == MugenInstant::Training){
                setMugenMotif(mainMenuPath());
                runMugenTraining(mugenInstant.player1, mugenInstant.player2, mugenInstant.stage);
            } else if (mugenInstant.enabled && mugenInstant.kind == MugenInstant::Watch){
                setMugenMotif(mainMenuPath());
                runMugenWatch(mugenInstant.player1, mugenInstant.player2, mugenInstant.stage);
            } else {
                MainMenuOptionFactory factory;
                Menu::Menu game(mainMenuPath(), factory);
                game.run(Menu::Context());
            }
            normal_quit = true;
        } catch (const Filesystem::Exception & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
        } catch (const TokenException & ex){
            Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const LoadException & ex){
            Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getTrace() << endl;
            return -1;
        } catch (const Exception::Return & ex){
        } catch (const ShutdownException & shutdown){
            Global::debug(1) << "Forced a shutdown. Cya!" << endl;
        } catch (const MugenException & m){
            Global::debug(0) << "Mugen exception: " << m.getReason() << endl;
        } catch (const ReloadMenuException & ex){
            Global::debug(1) << "Menu Reload Requested. Restarting...." << endl;
            continue;
        } catch (const ftalleg::Exception & ex){
            Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << endl;
        } catch (const Exception::Base & base){
            // Global::debug(0) << "Freetype exception caught. Error was:\n" << ex.getReason() << endl;
            Global::debug(0) << "Base exception: " << base.getTrace() << endl;
/* android doesn't have bad_alloc for some reason */
// #ifndef ANDROID
        } catch (const std::bad_alloc & fail){
            Global::debug(0) << "Failed to allocate memory. Usage is " << System::memoryUsage() << endl;
// #endif
        } catch (...){
            Global::debug(0) << "Uncaught exception!" << endl;
        }

        if (allow_quit && normal_quit){
            break;
        } else if (normal_quit && !allow_quit){
        } else if (!normal_quit){
            break;
        }
    }

    return 0;
}

/* 1. parse arguments
 * 2. initialize environment
 * 3. run main dispatcher
 * 4. quit
 */
int paintown_main(int argc, char ** argv){
    /* -1 means use whatever is in the configuration */
    int gfx = -1;

    bool music_on = true;
    bool joystick_on = true;
    bool mugen = false;
    bool just_network_server = false;
    bool allow_quit = true;
    Collector janitor;
    NetworkJoin networkJoin;
    MugenInstant mugenInstant;

    System::startMemoryUsage();

    Global::setDebug(0);
    vector<const char *> all_args;

#define ADD_ARGS(args) addArgs(all_args, args, NUM_ARGS(args))
    ADD_ARGS(WINDOWED_ARG);
    ADD_ARGS(DATAPATH_ARG);
    ADD_ARGS(DEBUG_ARG);
    ADD_ARGS(MUSIC_ARG);
    ADD_ARGS(MUGEN_ARG);
    ADD_ARGS(MUGEN_INSTANT_ARG);
    ADD_ARGS(RATE_LIMIT_ARG);
#ifdef HAVE_NETWORKING
    ADD_ARGS(NETWORK_SERVER_ARG);
    ADD_ARGS(NETWORK_JOIN_ARG);
#endif
#undef ADD_ARGS

#ifdef PS3
    /* find the directory that contains the binary and set the data path
     * to that directory + our data path
     */
    if (argc > 0){
        Path::AbsolutePath self(argv[0]);
        Util::setDataPath(self.getDirectory().join(Path::RelativePath(Util::getDataPath2().path())).path());
        /* set the home directory to wherever we started up from */
        setenv("HOME", argv[0], 1);
    }
#endif

    /* don't use the Configuration class here because its not loaded until init()
     * is called.
     */
    for ( int q = 1; q < argc; q++ ){
        if (isArg(argv[q], WINDOWED_ARG, NUM_ARGS(WINDOWED_ARG))){
            gfx = Global::FULLSCREEN;
        } else if (isArg(argv[q], DATAPATH_ARG, NUM_ARGS(DATAPATH_ARG))){
            q += 1;
            if (q < argc){
                Util::setDataPath(argv[q]);
            }
        } else if (isArg(argv[q], MUSIC_ARG, NUM_ARGS(MUSIC_ARG))){
            music_on = false;
        } else if (isArg(argv[q], MUGEN_ARG, NUM_ARGS(MUGEN_ARG))){
            mugen = true;
        } else if (isArg(argv[q], JOYSTICK_ARG, NUM_ARGS(JOYSTICK_ARG))){
            joystick_on = false;
        } else if (isArg(argv[q], RATE_LIMIT_ARG, NUM_ARGS(RATE_LIMIT_ARG))){
            Global::rateLimit = false;
        } else if (isArg(argv[q], MUGEN_INSTANT_ARG, NUM_ARGS(MUGEN_INSTANT_ARG))){
            q += 1;
            if (q < argc){
                mugenInstant.enabled = parseMugenInstant(argv[q], &mugenInstant.player1, &mugenInstant.player2, &mugenInstant.stage);
                mugenInstant.kind = MugenInstant::Training;
            } else {
                Global::debug(0) << "Expected an argument. Example: mugen:training kfm,ken,falls" << endl;
            }
        } else if (isArg(argv[q], DISABLE_QUIT_ARG, NUM_ARGS(DISABLE_QUIT_ARG))){
            allow_quit = false;
        } else if (isArg(argv[q], MUGEN_INSTANT_WATCH_ARG, NUM_ARGS(MUGEN_INSTANT_WATCH_ARG))){
            q += 1;
            if (q < argc){
                mugenInstant.enabled = parseMugenInstant(argv[q], &mugenInstant.player1, &mugenInstant.player2, &mugenInstant.stage);
                mugenInstant.kind = MugenInstant::Watch;
            } else {
                Global::debug(0) << "Expected an argument. Example: mugen:training kfm,ken,falls" << endl;
            }

        } else if (isArg(argv[q], DEBUG_ARG, NUM_ARGS(DEBUG_ARG))){
            q += 1;
            if (q < argc){
                istringstream i(argv[q]);
                int f;
                i >> f;
                Global::setDebug(f);
            }
#ifdef HAVE_NETWORKING
        } else if (isArg(argv[q], NETWORK_SERVER_ARG, NUM_ARGS(NETWORK_SERVER_ARG))){
            just_network_server = true;
        } else if (isArg(argv[q], NETWORK_JOIN_ARG, NUM_ARGS(NETWORK_JOIN_ARG))){
            networkJoin.enabled = true;
            string port;
            string host;
            string name;
            q += 1;
            if (q < argc){
                parseNetworkJoin(argv[q], port, host, name);
                networkJoin.port = port;
                networkJoin.host = host;
                networkJoin.name = name;
            }
#endif
        } else {
            const char * arg = argv[q];
            const char * closest = closestMatch(arg, all_args);
            if (closest == NULL){
                Global::debug(0) << "I don't recognize option '" << arg << "'" << endl;
            } else {
                Global::debug(0) << "You gave option '" << arg << "'. Did you mean '" << closest << "'?" << endl;
            }
        }
    }
#undef NUM_ARGS

    showOptions();

    Global::debug(0) << "Debug level: " << Global::getDebug() << endl;

    /* time how long init takes */
    TimeDifference diff;
    diff.startTime();
    /* init initializes practically everything including
     *  graphics
     *  sound
     *  input
     *  network
     *  configuration
     *  ...
     */
    if (! Global::init(gfx)){
        Global::debug(0) << "Could not initialize system" << endl;
        return -1;
    } else {
        // Configuration::setFullscreen((gfx == Global::FULLSCREEN ? true : false));
    }
    diff.endTime();
    Global::debug(0) << diff.printTime("Init took") << endl;

    Graphics::Bitmap screen(Graphics::getScreenBuffer());
    Util::Parameter<Graphics::Bitmap*> use(Graphics::screenParameter, &screen);

    /* set the game mod. check in this order
     * 1. whatever is in the configuration under (current-game ...)
     * 2. whatever is in data/system.txt
     * 3. try to load 'paintown'
     * 4. die
     */
    try{
        Paintown::Mod::loadPaintownMod(Configuration::getCurrentGame());
    } catch (...){
        if (Configuration::getCurrentGame() != ""){
            Global::debug(0) << "Could not load mod " << Configuration::getCurrentGame() << std::endl;
        }
        try{
            /* load the mod listed in data/system.txt */
            Paintown::Mod::loadPaintownMod(systemMod());
        } catch (...){
            try{
                /* failed to find anything, pray that data/paintown is there */
                Paintown::Mod::loadDefaultMod();
            } catch (...){
                Global::debug(0) << "Could not find the paintown default mod" << std::endl;
                /* Maybe just set up some ultra default thing? */
                return 0;
            }
        }
    }

    Configuration::setJoystickEnabled(joystick_on);

    InputManager input;
    Music music(music_on);

    /* Testing hack
    mugenInstant.enabled = true;
    mugenInstant.kind = MugenInstant::Watch;
    mugenInstant.player1 = "kfm";
    mugenInstant.player2 = "kfm";
    mugenInstant.stage = "kfm";
    */

    Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > defaultFont(Menu::menuFontParameter, Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(Global::DEFAULT_FONT, Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));

    startMain(just_network_server, networkJoin, mugenInstant, mugen, allow_quit);

    Configuration::saveConfiguration();

    Global::debug(0) << "Bye!" << endl;

    return 0;
}
