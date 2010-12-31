#ifdef USE_ALLEGRO
#include <allegro.h>
#ifdef ALLEGRO_WINDOWS
#include <winalleg.h>
#endif
#endif

#ifdef USE_SDL
#include <SDL.h>
#endif

#ifndef WINDOWS
#include <signal.h>
#include <string.h>
#endif

/* don't be a boring tuna */
// #warning you are ugly

#include "globals.h"
#include "init.h"
#include "network/network.h"
#include "util/thread.h"
#include <time.h>

#include <ostream>
#include "util/dumb/include/dumb.h"
#ifdef USE_ALLEGRO
#include "util/dumb/include/aldumb.h"
#include "util/loadpng/loadpng.h"
#include "util/gif/algif.h"
#endif
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/font.h"
#include "util/sound.h"
#include "configuration.h"
#include "script/script.h"
#include "util/music.h"
#include "util/loading.h"

#ifdef WII
#include <fat.h>
#endif

using namespace std;

volatile int Global::speed_counter = 0;

/* enough seconds for 136 years */
volatile unsigned int Global::second_counter = 0;

/* the original engine was running at 90 ticks per second, but we dont
 * need to render that fast, so TICS_PER_SECOND is really fps and
 * LOGIC_MULTIPLIER will be used to adjust the speed counter to its
 * original value.
 */
const int Global::TICS_PER_SECOND = 40;
const double Global::LOGIC_MULTIPLIER = (double) 90 / (double) Global::TICS_PER_SECOND;
        

#ifdef USE_ALLEGRO
const int Global::WINDOWED = GFX_AUTODETECT_WINDOWED;
const int Global::FULLSCREEN = GFX_AUTODETECT_FULLSCREEN;
#else
/* FIXME: use enums here or something */
const int Global::WINDOWED = 0;
const int Global::FULLSCREEN = 1;
#endif

/* game counter, controls FPS */
static void inc_speed_counter(){
    /* probably put input polling here, InputManager::poll() */
    Global::speed_counter += 1;
}
#ifdef USE_ALLEGRO
END_OF_FUNCTION( inc_speed_counter )
#endif

/* if you need to count seconds for some reason.. */
static void inc_second_counter() {
    Global::second_counter += 1;
}
#ifdef USE_ALLEGRO
END_OF_FUNCTION( inc_second_counter )
#endif

#if !defined(WINDOWS) && !defined(WII) && !defined(MINPSPW) && !defined(NDS)
static void handleSigSegV(int i, siginfo_t * sig, void * data){
    const char * message = "Bug! Caught a memory violation. Shutting down..\n";
    int dont_care = write(1, message, 48);
    dont_care = dont_care;
    // Global::shutdown_message = "Bug! Caught a memory violation. Shutting down..";
    Bitmap::setGfxModeText();
#ifdef USE_ALLEGRO
    allegro_exit();
#endif
    /* write to a log file or something because sigsegv shouldn't
     * normally happen.
     */
    exit(1);
}
#else
#endif

/* catch a socket being closed prematurely on unix */
#if !defined(WINDOWS) && !defined(WII) && !defined(MINPSPW) && !defined(NDS)
static void handleSigPipe( int i, siginfo_t * sig, void * data ){
}

/*
static void handleSigUsr1( int i, siginfo_t * sig, void * data ){
	pthread_exit( NULL );
}
*/
#endif

static void registerSignals(){
#if !defined(WINDOWS) && !defined(WII) && !defined(MINPSPW) && !defined(NDS)
	struct sigaction action;
	memset( &action, 0, sizeof(struct sigaction) );
	action.sa_sigaction = handleSigPipe;
	sigaction( SIGPIPE, &action, NULL );

	memset( &action, 0, sizeof(struct sigaction) );
	action.sa_sigaction = handleSigSegV;
	sigaction( SIGSEGV, &action, NULL );

	/*
	action.sa_sigaction = handleSigUsr1;
	sigaction( SIGUSR1, &action, NULL );
	*/
#endif
}

/* should probably call the janitor here or something */
static void close_paintown(){
    Music::pause();
    Bitmap::setGfxModeText();
#ifdef USE_ALLEGRO
    allegro_exit();
#endif
    exit(0);
}

namespace Global{
    extern int do_shutdown;
}

static void close_window(){
    /* when do_shutdown is 1 the game will attempt to throw ShutdownException
     * wherever it is. If the game is stuck or the code doesn't throw
     * ShutdownException then when the user tries to close the window
     * twice we just forcifully shutdown.
     */
    Global::do_shutdown += 1;
    if (Global::do_shutdown == 2){
        close_paintown();
    }
}
#ifdef USE_ALLEGRO
END_OF_FUNCTION(close_window)
#endif

#ifdef USE_ALLEGRO
static void initSystem(ostream & out){
    out << "Allegro version: " << ALLEGRO_VERSION_STR << endl;
    out << "Allegro init: " <<allegro_init()<<endl;
    out << "Install timer: " <<install_timer()<<endl;

    /* png */
    loadpng_init();
    algif_init();

    out<<"Install keyboard: "<<install_keyboard()<<endl;
    /* do we need the mouse?? */
    // out<<"Install mouse: "<<install_mouse()<<endl;
    out<<"Install joystick: "<<install_joystick(JOY_TYPE_AUTODETECT)<<endl;
    /* 16 bit color depth */
    set_color_depth(16);

    LOCK_VARIABLE( speed_counter );
    LOCK_VARIABLE( second_counter );
    LOCK_FUNCTION( (void *)inc_speed_counter );
    LOCK_FUNCTION( (void *)inc_second_counter );
    /* set up the timers */
    out<<"Install game timer: "<< install_int_ex(inc_speed_counter, BPS_TO_TIMER(Global::TICS_PER_SECOND))<<endl;
    out<<"Install second timer: "<<install_int_ex(inc_second_counter, BPS_TO_TIMER(1))<<endl;

    /* keep running in the background */
    set_display_switch_mode(SWITCH_BACKGROUND);

    /* close window when the X is pressed */
    LOCK_FUNCTION(close_window);
    set_close_button_callback(close_window);
}

#endif
#ifdef USE_SDL
    
// static pthread_t events;

struct TimerInfo{
    TimerInfo(void (*x)(), int y):
        tick(x), frequency(y){}

    void (*tick)();
    int frequency;
};

static void * do_timer(void * arg){
    TimerInfo info = *(TimerInfo *) arg;
    uint32_t delay = (uint32_t)(1000.0 / (double) info.frequency);

    /* assuming SDL_GetTicks() starts at 0, this should last for about 50 days
     * before overflowing. overflow should work out fine. Assuming activate occurs
     * when the difference between now and ticks is at least 6, the following will happen.
     * ticks      now        now-ticks
     * 4294967294 4294967294 0
     * 4294967294 4294967295 1
     * 4294967294 0          2
     * 4294967294 1          3
     * 4294967294 2          4
     * 4294967294 3          5
     * 4294967294 4          6
     * Activate
     * 3          5          2
     * 3          6          3
     * 3          7          4
     * 3          8          5
     * 3          9          6
     * Activate
     *
     * Can 'now' ever be much larger than 'ticks' due to overflow?
     * It doesn't seem like it.
     */
    uint32_t ticks = SDL_GetTicks();

    /* TODO: pass in some variable that tells this loop to quit */
    while (true){
        uint32_t now = SDL_GetTicks();
        while (now - ticks >= delay){
            // Global::debug(0) << "Tick!" << endl;
            info.tick();
            ticks += delay;
        }
        SDL_Delay(1);
    }

    delete (TimerInfo *) arg;

    return NULL;
}

static Util::Thread::Id start_timer(void (*func)(), int frequency){
    TimerInfo * speed = new TimerInfo(func, frequency);
	/*
    speed.tick = func;
    speed.frequency = frequency;
*/
    Util::Thread::Id thread;
    Util::Thread::createThread(&thread, NULL, (Util::Thread::ThreadFunction) do_timer, (void*) speed);
    return thread;
}

#if 0
static void * handleEvents(void * arg){
    bool done = false;
    while (!done){
        SDL_Event event;
        int ok = SDL_WaitEvent(&event);
        if (ok){
            switch (event.type){
                case SDL_QUIT : {
                    done = true;
                    close_window();
                    break;
                }
                default : {
                    // Global::debug(0) << "Ignoring SDL event " << event.type << endl;
                    break;
                }
            }
        }
    }
    return NULL;
}
#endif

/*
static void doSDLQuit(){
    SDL_Event quit;
    quit.type = SDL_QUIT;
    SDL_PushEvent(&quit);
    Global::debug(0) << "Waiting for SDL event handler to finish" << endl;
    pthread_join(events, NULL);
    SDL_Quit();
}
*/
    
static void initSystem(ostream & out){
    out << "SDL Init: ";
    int ok = SDL_Init(SDL_INIT_VIDEO |
                      SDL_INIT_AUDIO |
                      SDL_INIT_TIMER |
                      SDL_INIT_JOYSTICK |
                      SDL_INIT_NOPARACHUTE);
    if (ok == 0){
        out << "Ok" << endl;
    } else {
	out << "Failed (" << ok << ") - " << SDL_GetError() << endl;
	exit(ok);
    }

    /* Just do SDL thread init
#ifdef MINPSPW
    pthread_init();
#endif
*/

    start_timer(inc_speed_counter, Global::TICS_PER_SECOND);
    start_timer(inc_second_counter, 1);

    try{
        SDL_Surface * icon = SDL_LoadBMP(Filesystem::find(Filesystem::RelativePath("menu/icon.bmp")).path().c_str());
        if (icon != NULL){
            SDL_WM_SetIcon(icon, NULL);
        }
    } catch (const Filesystem::NotFound & failed){
        Global::debug(0) << "Could not find window icon: " << failed.getTrace() << endl;
    }

    SDL_WM_SetCaption("Paintown", NULL);

    SDL_EnableUNICODE(1);
    SDL_JoystickEventState(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    atexit(SDL_Quit);
    // atexit(doSDLQuit);
}
#endif

bool Global::init(int gfx){

    ostream & out = Global::debug( 0 );
    out << "-- BEGIN init --" << endl;
    out << "Data path is " << Util::getDataPath2().path() << endl;
    out << "Paintown version " << Global::getVersionString() << endl;
    out << "Build date " << __DATE__ << " " << __TIME__ << endl;

#ifdef WII
    fatInitDefault();
#endif

    /* do implementation specific setup */
    initSystem(out);

    dumb_register_stdfiles();
    
    Sound::initialize();

    Bitmap::SCALE_X = GFX_X;
    Bitmap::SCALE_Y = GFX_Y;

    Configuration::loadConfigurations();
    const int sx = Configuration::getScreenWidth();
    const int sy = Configuration::getScreenHeight();
    if (gfx == -1){
        gfx = Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED;
    } else {
        Configuration::setFullscreen(gfx == Global::FULLSCREEN);
    }
    
    /* set up the screen */
    int gfxCode = Bitmap::setGraphicsMode(gfx, sx, sy);
    if (gfxCode == 0){
        out << "Set graphics mode: Ok" << endl;
    } else {
        out << "Set graphics mode: Failed! (" << gfxCode << ")" << endl;
        return false;
    }
    
    /* music */
    atexit(&dumb_exit);

    out << "Initialize random number generator" << endl;
    /* initialize random number generator */
    srand(time(NULL));

    registerSignals();

#ifdef HAVE_NETWORKING
    out << "Initialize network" << endl;
    Network::init();
    atexit(Network::closeAll);
#endif

    /* this mutex is used to show the loading screen while the game loads */
    // Util::Thread::initializeLock(&Loader::loading_screen_mutex);

    out << "-- END init --" << endl;

    /*
    const Font & font = Font::getDefaultFont();
    // font.setSize(30, 30);
    Bitmap temp(font.textLength("Loading") + 1, font.getHeight("Loading") + 1);
    font.printf(0, 0, Bitmap::makeColor(255, 255, 255), temp, "Loading", 0);
    temp.BlitToScreen(sx / 2, sy / 2);
    */
    Bitmap white(sx, sy);
    white.fill(Bitmap::makeColor(255, 255, 255));
    white.BlitToScreen();

    return true;
}
