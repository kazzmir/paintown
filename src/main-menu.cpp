#include <sstream>
#include "util/bitmap.h"

#include "factory/collector.h"
#include "network/network.h"
#include "util/token_exception.h"
#include "util/system.h"
#include "mugen/exception.h"
#include "mugen/menu.h"
#include "mugen/game.h"
#include "util/music.h"
#include "menu/menu.h"
#include "menu/menu-exception.h"
#include "util/input/input-manager.h"
#include "paintown-engine/game/mod.h"
#include "exceptions/shutdown_exception.h"
#include "exceptions/exception.h"
#include "util/timedifference.h"
#include "util/funcs.h"
#include "util/ftalleg.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "globals.h"
#include "util/debug.h"
#include "network/server.h"
#include "configuration.h"
#include "util/init.h"
#include "mugen/config.h"

#include <string.h>
#include <vector>

using namespace std;

#define NUM_ARGS(d) (sizeof(d)/sizeof(char*))
static const char * WINDOWED_ARG[] = {"-w", "fullscreen", "nowindowed", "no-windowed"};
static const char * DATAPATH_ARG[] = {"-d", "--data", "data", "datapath", "data-path", "path"};
static const char * DEBUG_ARG[] = {"-l", "--debug", "debug"};
static const char * MUSIC_ARG[] = {"-m", "music", "nomusic", "no-music"};
static const char * NETWORK_SERVER_ARG[] = {"server", "network-server"};
static const char * MUGEN_ARG[] = {"mugen", "--mugen"};
static const char * MUGEN_INSTANT_ARG[] = {"mugen:training"};
static const char * JOYSTICK_ARG[] = {"joystick", "nojoystick", "no-joystick"};

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

static bool isArg( const char * s1, const char * s2[], int num){
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
        Global::debug(0) << " " << all(JOYSTICK_ARG, NUM_ARGS(JOYSTICK_ARG)) << " : Disable joystick input" << endl;
#ifdef HAVE_NETWORKING
	Global::debug(0) << " " << all(NETWORK_SERVER_ARG, NUM_ARGS(NETWORK_SERVER_ARG)) << " : Go straight to the network server" << endl;
#endif
	Global::debug(0) << endl;
}

static void addArgs(vector<const char *> & args, const char * strings[], int num){
    for (int i = 0; i < num; i++){
        args.push_back(strings[i]);
    }
}

static void setMugenMotif(const Filesystem::AbsolutePath & path){
    TokenReader reader;
    Token * head = reader.readToken(path.path());
    const Token * motif = head->findToken("menu/option/mugen/motif");
    if (motif != NULL){
        string path;
        motif->view() >> path;
        Mugen::Data::getInstance().setMotif(Filesystem::RelativePath(path));
    }
}

static Filesystem::AbsolutePath mainMenuPath(){
    string menu = Paintown::Mod::getCurrentMod()->getMenu();
    return Filesystem::find(Filesystem::RelativePath(menu));
    /*
    string file = Filesystem::find(currentMod());
    TokenReader tr(file);
    Token * head = tr.readToken();
    Token * menu = head->findToken("game/menu");
    if (menu == NULL){
        throw LoadException(file + " does not contain a game/menu token");
    }
    string path;
    *menu >> path;
    // string path = "/menu/main.txt"
    return Filesystem::find(path);
    */
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
    Global::debug(0) << "Mugen training mode player 1 '" << player1 << "' player2 '" << player2 << "' stage '" << stage << "'" << endl;
    Mugen::Game::startTraining(player1, player2, stage);
}

int paintown_main( int argc, char ** argv ){
    
    /* -1 means use whatever is in the configuration */
    int gfx = -1;

    bool music_on = true;
    bool joystick_on = true;
    bool mugen = false;
    bool just_network_server = false;
    Collector janitor;

    struct MugenInstant{
        MugenInstant():
            enabled(false){
            }

        bool enabled;
        string player1;
        string player2;
        string stage;
    } mugenInstant;

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
#ifdef HAVE_NETWORKING
    ADD_ARGS(NETWORK_SERVER_ARG);
#endif
#undef ADD_ARGS

    for ( int q = 1; q < argc; q++ ){
        if (isArg(argv[q], WINDOWED_ARG, NUM_ARGS(WINDOWED_ARG))){
            gfx = Global::FULLSCREEN;
        } else if ( isArg( argv[ q ], DATAPATH_ARG, NUM_ARGS(DATAPATH_ARG) ) ){
            q += 1;
            if ( q < argc ){
                Util::setDataPath( argv[ q ] );
            }
        } else if ( isArg( argv[ q ], MUSIC_ARG, NUM_ARGS(MUSIC_ARG) ) ){
            music_on = false;
        } else if (isArg(argv[q], MUGEN_ARG, NUM_ARGS(MUGEN_ARG))){
            mugen = true;
        } else if (isArg(argv[q], JOYSTICK_ARG, NUM_ARGS(JOYSTICK_ARG))){
            joystick_on = false;
        } else if (isArg(argv[q], MUGEN_INSTANT_ARG, NUM_ARGS(MUGEN_INSTANT_ARG))){ 
            q += 1;
            if (q < argc){
                mugenInstant.enabled = parseMugenInstant(argv[q], &mugenInstant.player1, &mugenInstant.player2, &mugenInstant.stage);
            } else {
                Global::debug(0) << "Expected an argument. Example: mugen:training kfm,ken,falls" << endl;
            }
        } else if ( isArg( argv[ q ], DEBUG_ARG, NUM_ARGS(DEBUG_ARG) ) ){
            q += 1;
            if ( q < argc ){
                istringstream i( argv[ q ] );
                int f;
                i >> f;
                Global::setDebug( f );
            }
#ifdef HAVE_NETWORKING
        } else if (isArg(argv[q], NETWORK_SERVER_ARG, NUM_ARGS(NETWORK_SERVER_ARG))){
            just_network_server = true;
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

    if (!Filesystem::exists(Util::getDataPath2())){
        Global::debug(0) << "Cannot find data path '" << Util::getDataPath2().path() << "'! Either use the -d switch to specify the data directory or find the data directory and move it to that path" << endl;
        return 1;
    }

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

    try{
        Paintown::Mod::loadPaintownMod(Configuration::getCurrentGame());
    } catch (const Filesystem::NotFound & e){
        Global::debug(0) << "Could not load mod " << Configuration::getCurrentGame() << ": " << e.getTrace() << endl;
        Paintown::Mod::loadDefaultMod();
    }

    Configuration::setJoystickEnabled(joystick_on);

    InputManager input;
    Music music(music_on);

    while (true){
        try{
            /* fadein from white */
            //Menu game(true, Bitmap::makeColor(255, 255, 255));
            //game.load(mainMenuPath());
            if (just_network_server){
#ifdef HAVE_NETWORKING
                Network::networkServer();
#endif
            } else if (mugen){
                setMugenMotif(mainMenuPath());
                Mugen::run();
            } else if (mugenInstant.enabled){
                setMugenMotif(mainMenuPath());
                runMugenTraining(mugenInstant.player1, mugenInstant.player2, mugenInstant.stage);
            } else {
                Menu::Menu game(mainMenuPath());
                game.run(Menu::Context());
            }
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
        } catch (...){
            Global::debug(0) << "Uncaught exception!" << endl;
        }
        
        break;
    }

    Configuration::saveConfiguration();

    Global::debug(0) << "Bye!" << endl;

    return 0;
}
