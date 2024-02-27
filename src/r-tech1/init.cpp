#ifdef USE_ALLEGRO
#include <allegro.h>
#ifdef ALLEGRO_WINDOWS
#include <winalleg.h>
#endif
#endif

/*
#ifdef USE_ALLEGRO5
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#endif
*/

#ifdef USE_SDL
#include <SDL.h>
#endif

#ifndef WINDOWS
#include <signal.h>
#include <string.h>
#include <unistd.h>
#endif

#if defined(LINUX) && !defined(UCLIBC)
#include <execinfo.h>
#endif

/* don't be a boring tuna */
// #warning you are ugly

/**
 * 
 * FIXME No System, this outside util directory
 * 
 */

#include "r-tech1/system/init.h"
#include "r-tech1/init.h"
#include "r-tech1/network/network.h"
#include "r-tech1/thread.h"
#include <time.h>
#include "r-tech1/system/timer.h"

#include <ostream>
#include "libs/dumb/include/dumb.h"

/*
#ifdef USE_ALLEGRO
#include "sound/dumb/include/aldumb.h"
#include "graphics/allegro/loadpng/loadpng.h"
#include "graphics/allegro/gif/algif.h"
#endif
*/

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include "r-tech1/font.h"
#include "r-tech1/events.h"
#include "r-tech1/sound/sound.h"
#include "r-tech1/configuration.h"
#include "r-tech1/sound/music.h"
#include "r-tech1/resource.h"
#include "r-tech1/loading.h"
#include "r-tech1/input/keyboard.h"
#include "r-tech1/message-queue.h"

#if defined(GAMECUBE) || defined(WII)
#include <fat.h>
#elif defined(SWITCH)
#include <switch.h>
#endif

#include "r-tech1/xenon/xenon.h"

using namespace std;

Atomic::atomic<uint64_t> Global::speed_counter4(0);
bool Global::rateLimit = true;

/* enough seconds for 5.8 * 10^11 years */
Atomic::atomic<uint64_t> Global::second_counter(0);

/* the original engine was running at 90 ticks per second, but we dont
 * need to render that fast, so TICS_PER_SECOND is really fps and
 * LOGIC_MULTIPLIER will be used to adjust the speed counter to its
 * original value.
 */
int Global::TICS_PER_SECOND = 40;
// const double Global::LOGIC_MULTIPLIER = (double) 90 / (double) Global::TICS_PER_SECOND;

double Global::ticksPerSecond(int ticks){
    return (double) ticks / (double) TICS_PER_SECOND;
}
    
#ifdef USE_ALLEGRO
const int Global::WINDOWED = GFX_AUTODETECT_WINDOWED;
const int Global::FULLSCREEN = GFX_AUTODETECT_FULLSCREEN;
#else
/* FIXME: use enums here or something */
const int Global::WINDOWED = 0;
const int Global::FULLSCREEN = 1;
#endif

#ifndef CROSS_BUILD
#ifdef LINUX
static void print_stack_trace(){
    /* use addr2line on these addresses to get a filename and line number */
    void *trace[128];
    int frames = backtrace(trace, 128);
    printf("Stack trace\n");
    for (int i = 0; i < frames; i++){
        printf(" %p\n", trace[i]);
    }
}
#endif

static void handleSigSegV(int i, siginfo_t * sig, void * data){
    const char * message = "Bug! Caught a memory violation. Shutting down..\n";
    int dont_care = write(1, message, 48);
    dont_care = dont_care;
#if defined(LINUX) && !defined(UCLIBC)
    print_stack_trace();
#endif
    // Global::shutdown_message = "Bug! Caught a memory violation. Shutting down..";
    Graphics::setGfxModeText();
#ifdef USE_ALLEGRO
    allegro_exit();
#endif
#ifdef USE_SDL
    SDL_Quit();
#endif
    /* write to a log file or something because sigsegv shouldn't
     * normally happen.
     */
    exit(1);
}
#elif defined(WINDOWS)
// Define an empty struct for siginfo_t on windows since the functions don't use the context anyways
struct siginfo_t {
  void * nothing;  
};
#else
#endif

/* catch a socket being closed prematurely on unix */
#ifndef CROSS_BUILD
static void handleSigPipe( int i, siginfo_t * sig, void * data ){
}

/*
static void handleSigUsr1( int i, siginfo_t * sig, void * data ){
    pthread_exit( NULL );
}
*/

static void handleSigInt(int signal, siginfo_t* info, void* context){
    DebugLog << "Shut down due to ctrl-c" << endl;
    Util::do_shutdown += 1;
}

#endif

static void registerSignals(){
#ifndef CROSS_BUILD
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_sigaction = handleSigPipe;
    sigaction( SIGPIPE, &action, NULL );

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_sigaction = handleSigSegV;
    sigaction( SIGSEGV, &action, NULL );

    action.sa_sigaction = handleSigInt;
    sigaction(SIGINT, &action, NULL);

    /*
    action.sa_sigaction = handleSigUsr1;
    sigaction( SIGUSR1, &action, NULL );
    */
#endif
}

/* mostly used for testing purposes */
#if 0
bool Global::initNoGraphics(){
    /* copy/pasting the init code isn't ideal, maybe fix it later */
    Global::stream_type & out = Global::debug(0);
    out << "-- BEGIN init --" << endl;
    out << "Data path is " << Util::getDataPath2().path() << endl;
    out << "Build date " << __DATE__ << " " << __TIME__ << endl;

#if defined(GAMECUBE) || defined(WII)
    /* <WinterMute> fatInitDefault will set working dir to argv[0] passed by launcher,
     * or root of first device mounted
     */
    out << "Fat init " << (fatInitDefault() == true ? "Ok" : "Failed") << endl;
#elif defined(SWITCH)
    // Init text console on switch (don't use for now)
    // consoleInit(NULL);
    // consoleUpdate(NULL) <- need to call this to update the console
    // (Should call consoleExit(NULL)) on exit
    //DebugLog1 << "Switch console init..." << std::endl;
#endif
    /*
    char buffer[512];
    if (getcwd(buffer, 512) != 0){
        printf("Working directory '%s'\n", buffer);
    }
    */

    if (!Storage::instance().exists(Util::getDataPath2())){
        Global::debug(0) << "Cannot find data path '" << Util::getDataPath2().path() << "'! Either use the -d switch to specify the data directory or find the data directory and move it to that path" << endl;
        return false;
    }

    /* do implementation specific setup */
    System::initSystem(out);

    dumb_register_stdfiles();
    
    // Sound::initialize();

    // Filesystem::initialize();

    /*
    Graphics::SCALE_X = GFX_X;
    Graphics::SCALE_Y = GFX_Y;
    */

    Configuration::loadConfigurations();
    const int sx = Configuration::getScreenWidth();
    const int sy = Configuration::getScreenHeight();
    Graphics::Bitmap::setFakeGraphicsMode(sx, sy);
       
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

    return true;
}
#endif

#if defined(USE_SDL) && defined(MACOSX)
#include <CoreFoundation/CoreFoundation.h>
#endif

static void maybeSetWorkingDirectory(){
#if defined(USE_SDL) && defined(MACOSX)
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if (mainBundle != NULL){
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        char path[PATH_MAX];
        if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)){
            chdir(path);
        } else {
            Global::debug(0) << "Could not set working directory to Resources" << std::endl;
        }
        CFRelease(resourcesURL);
    }
#endif
}

Global::InitConditions::InitConditions():
graphics(Default),
sound(true),
fullscreen(false),
networking(true),
softwareRenderer(false){
}

std::thread::id Global::mainThreadId;
bool Global::isMainThread(){
    return mainThreadId == std::this_thread::get_id();
}

bool Global::init(const InitConditions & conditions){

    mainThreadId = std::this_thread::get_id();

    /* Can xenon_init be moved lower? Probably.. */
#ifdef XENON
    xenon_init();
#endif

    Global::stream_type & out = Global::debug(0);
    out << "-- BEGIN init --" << endl;
    out << "Data path is " << Util::getDataPath2().path() << endl;
    out << "Build date " << __DATE__ << " " << __TIME__ << endl;

    maybeSetWorkingDirectory();


#if defined(GAMECUBE) || defined(WII)
    /* <WinterMute> fatInitDefault will set working dir to argv[0] passed by launcher,
     * or root of first device mounted
     */
    out << "Fat init " << (fatInitDefault() == 0 ? "Ok" : "Failed") << endl;
#endif
    /*
    char buffer[512];
    if (getcwd(buffer, 512) != 0){
        printf("Working directory '%s'\n", buffer);
    }
    */

#ifndef NACL
    /* do implementation specific setup */
    System::initSystem(conditions, out);
#endif

    dumb_register_stdfiles();
    
    if (conditions.sound){
        Sound::initialize();
    }

    // Filesystem::initialize();

    /*
    Graphics::SCALE_X = GFX_X;
    Graphics::SCALE_Y = GFX_Y;
    */

    Configuration::loadConfigurations();

    if (conditions.graphics != InitConditions::Disabled){
        const int sx = Configuration::getScreenWidth();
        const int sy = Configuration::getScreenHeight();
        InitConditions::WindowMode gfx = conditions.graphics; 
        if (conditions.graphics == InitConditions::Default){
            gfx = Configuration::getFullscreen() ? InitConditions::Fullscreen : InitConditions::Window;
        } else {
            Configuration::setFullscreen(conditions.graphics == InitConditions::Fullscreen);
        }

        /* set up the screen */
        int mode = WINDOWED;
        switch (gfx){
            case InitConditions::Disabled: break; /* won't happen */
            case InitConditions::Default: break; /* already handled */
            case InitConditions::Window: mode = WINDOWED; break;
            case InitConditions::Fullscreen: mode = FULLSCREEN; break;
        }

        int gfxCode = Graphics::setGraphicsMode(mode, sx, sy);
        if (gfxCode == 0){
            out << "Set graphics mode: Ok" << endl;
        } else {
            out << "Set graphics mode: Failed! (" << gfxCode << ")" << endl;
            return false;
        }
    }
    
    /* music */
    atexit(&dumb_exit);

    out << "Initialize random number generator" << endl;
    /* initialize random number generator */
    srand(time(NULL));

    registerSignals();

#ifdef HAVE_NETWORKING
    if (conditions.networking){
        out << "Initialize network" << endl;
        Network::init();
        atexit(Network::closeAll);
    }
#endif

    /* this mutex is used to show the loading screen while the game loads */
    Util::Thread::initializeLock(&MessageQueue::messageLock);
    Resource::initialize();

    Util::Thread::initializeLock(&System::run_timer_lock);
    System::run_timer = true;
    Global::TICS_PER_SECOND = Configuration::getFps();
    System::startTimers();

    out << "-- END init --" << endl;

    /*
    const Font & font = Font::getDefaultFont();
    // font.setSize(30, 30);
    Bitmap temp(font.textLength("Loading") + 1, font.getHeight("Loading") + 1);
    font.printf(0, 0, Bitmap::makeColor(255, 255, 255), temp, "Loading", 0);
    temp.BlitToScreen(sx / 2, sy / 2);
    */

#ifdef PS3
    // ps3JoystickHack();
#endif

    return true;
}

/* Checks for the data path and blinks the screen a specific color based on its findings:
 *   grey: still looking
 *   red: failed
 *   white: found it
 */
bool Global::dataCheck(){
    Graphics::Bitmap white(*Graphics::getScreenBuffer());
    /* for nacl which takes a while to run exists(), we just want
     * to show some progress
     */
    white.fill(Graphics::makeColor(128, 128, 128));
    white.BlitToScreen();
    if (!Storage::instance().exists(Util::getDataPath2())){
        white.fill(Graphics::makeColor(255, 0, 0));
        white.BlitToScreen();
        Global::debug(0) << "Cannot find data path '" << Util::getDataPath2().path() << "'! Either use the -d switch to specify the data directory or find the data directory and move it to that path" << endl;
        /* The program is probably going to exit immediately, so allow the user some time
         * to see that the red screen occured.
         */
        Util::restSeconds(1);
        return false;
    } else {
        white.fill(Graphics::makeColor(255, 255, 255));
        white.BlitToScreen();
    }

    return true;
}

/* Restarts the timers */
void Global::setTicksPerSecond(int ticks){
    if (ticks < 1){
        ticks = 1;
    }
    if (ticks > 90){
        ticks = 90;
    }
    if (ticks != TICS_PER_SECOND){
        TICS_PER_SECOND = ticks;
        System::closeTimers();
        System::startTimers();
    }
}

void Global::close(){
    System::closeTimers();
}
