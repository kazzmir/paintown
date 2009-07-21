#include <sstream>

#include "factory/collector.h"
#include "network/network.h"
#include "util/token_exception.h"
#include "music.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "game/input-manager.h"
#include "util/timedifference.h"
#include "util/funcs.h"
#include "globals.h"
#include "configuration.h"
#include "init.h"

#include <vector>

using namespace std;

static int gfx = Global::WINDOWED;

#define NUM_ARGS(d) (sizeof(d)/sizeof(char*))
static const char * WINDOWED_ARG[] = {"-w", "fullscreen", "nowindowed", "no-windowed"};
static const char * DATAPATH_ARG[] = {"-d", "data", "datapath", "data-path", "path"};
static const char * DEBUG_ARG[] = {"-l", "debug"};
static const char * MUSIC_ARG[] = {"-m", "music", "nomusic", "no-music"};

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
	Global::debug( 0 ) << "Paintown by Jon Rafkind" << endl;
	Global::debug( 0 ) << all(WINDOWED_ARG, NUM_ARGS(WINDOWED_ARG), ',') << " : Fullscreen mode" << endl;
	Global::debug( 0 ) << all(DATAPATH_ARG, NUM_ARGS(DATAPATH_ARG)) << " <path> : Use data path of <path>. Default is " << Util::getDataPath() << endl;
	Global::debug( 0 ) << all(DEBUG_ARG, NUM_ARGS(DEBUG_ARG)) << " # : Enable debug statements. Higher numbers gives more debugging. Default is 0. Example: -l 3" << endl;
	Global::debug( 0 ) << all(MUSIC_ARG, NUM_ARGS(MUSIC_ARG)) << " : Turn off music" << endl;
	Global::debug( 0 ) << endl;
}

static void addArgs(vector<const char *> & args, const char * strings[], int num){
    for (int i = 0; i < num; i++){
        args.push_back(strings[i]);
    }
}

int paintown_main( int argc, char ** argv ){
	
	bool music_on = true;
	Collector janitor;

	Global::setDebug( 0 );
        vector<const char *> all_args;
	
#define ADD_ARGS(args) addArgs(all_args, args, NUM_ARGS(args))
        ADD_ARGS(WINDOWED_ARG);
        ADD_ARGS(DATAPATH_ARG);
        ADD_ARGS(DEBUG_ARG);
        ADD_ARGS(MUSIC_ARG);
#undef ADD_ARGS

	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], WINDOWED_ARG, NUM_ARGS(WINDOWED_ARG) ) ){
			gfx = Global::FULLSCREEN;
		} else if ( isArg( argv[ q ], DATAPATH_ARG, NUM_ARGS(DATAPATH_ARG) ) ){
			q += 1;
			if ( q < argc ){
				Util::setDataPath( argv[ q ] );
			}
		} else if ( isArg( argv[ q ], MUSIC_ARG, NUM_ARGS(MUSIC_ARG) ) ){
			music_on = false;
		} else if ( isArg( argv[ q ], DEBUG_ARG, NUM_ARGS(DEBUG_ARG) ) ){
			q += 1;
			if ( q < argc ){
				istringstream i( argv[ q ] );
				int f;
				i >> f;
				Global::setDebug( f );
			}
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

	TimeDifference diff;
	diff.startTime();
	if ( ! Global::init( gfx ) ){
		Global::debug( 0 ) << "Could not initialize system" << endl;
		return -1;
	} else {
		MenuGlobals::setFullscreen((gfx == Global::FULLSCREEN ? true : false));
	}
	diff.endTime();
	diff.printTime( "Init:" );
	
        InputManager input;
	Music music(music_on);
	try{
		Menu game;
		game.load(Util::getDataPath() + "/menu/main.txt");
		game.run();
	} catch (const TokenException & ex){
		Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getReason() << endl;
		return -1;
	} catch (const LoadException & ex){
		Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getReason() << endl;
		return -1;
        } catch (const ReturnException & ex){
        }

	Configuration::saveConfiguration();
	
	return 0;
}
