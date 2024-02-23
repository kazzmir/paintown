#ifdef USE_SDL2

#include <iostream>
#include "r-tech1/system/init.h"
#include "r-tech1/debug.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <fcntl.h>

#ifndef CROSS_BUILD
#include <spawn.h>
#include <sys/wait.h>
#endif

namespace System{

SDL_TimerID timer;

#ifndef CROSS_BUILD
/* lame wrapper class for a malloc'd string that free's in the destructor */
class CString {
public:
    CString(const char* str) : str(strdup(str)) {}
    ~CString() { free(str); }
    char* get(){ return str; }
    char* str;
};
#endif

static bool hasGlxInfo(){
    /* FIXME: on windows just return true */
#ifdef CROSS_BUILD
    return true;
#else

    std::string display = std::string("DISPLAY=") + getenv("DISPLAY");

    CString displayString(display.c_str());

    char* const envp[] = {displayString.get(), NULL};

    char glxinfo[] = "glxinfo";
    char* const argv[] = {glxinfo, NULL};

    posix_spawn_file_actions_t files;
    posix_spawn_file_actions_init(&files);
    posix_spawn_file_actions_addopen(&files, 1, "/dev/null", O_WRONLY, 0);
    posix_spawn_file_actions_addopen(&files, 2, "/dev/null", O_WRONLY, 0);

    pid_t pid = 0;
    int ok = posix_spawnp(&pid, "glxinfo", &files, NULL, argv, envp);
    if (ok != 0){
        DebugLog << "SDL2: unable to run glxinfo: " << strerror(errno) << std::endl;
        return true;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) != pid){
        DebugLog << "SDL2: unable to wait for glxinfo" << std::endl;
        return true;
    }

    if (WIFEXITED(status)){
        if (WEXITSTATUS(status) == 0){
            return true;
        } else {
            DebugLog << "SDL2: glxinfo exited with status " << WEXITSTATUS(status) << std::endl;
        }
    }

    return false;
#endif
}

void initSystem(const Global::InitConditions & conditions, Global::stream_type & out){
    DebugLog << "SDL2 Init start" << std::endl;
    /* if glx info fails to run then we probably need to use a software renderer */
    if (conditions.useSoftwareRenderer() || !hasGlxInfo()){
        DebugLog << "SDL2: attempting to use software renderer" << std::endl;
        // set hint to use software renderer
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    }

    // initialize SDL
    int ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    DebugLog << "SDL2 initialization: " << ok << std::endl;
    if (ok != 0){
        DebugLog << "  error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (TTF_Init() != 0){
        DebugLog << "SDL2 TTF initialization failed: " << TTF_GetError() << std::endl;
        exit(1);
    }

    //Initialize SDL_mixer
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        DebugLog << "SDL_mixer initialization failed! SDL_mixer Error: " << Mix_GetError() << std::endl;
        exit(1);
    }
}

static unsigned int timerCallback(unsigned int, void * param){
    Global::speed_counter4 += 1;
    /* zero callback means the timer should cancel. non-zero return value means that the timer should be restarted */
    return 1000 / Global::TICS_PER_SECOND;
}

void startTimers(){
    timer = SDL_AddTimer(1000/Global::TICS_PER_SECOND, timerCallback, NULL);
    if (timer == 0){
        DebugLog << "SDL2 timer initialization failed" << std::endl;
        exit(1);
    }
}

void shutdown(){
    if (timer != 0){
        if (!SDL_RemoveTimer(timer)){
            DebugLog << "SDL2: unable to remove timer" << std::endl;
        }
    }

    DebugLog << "Shutting down SDL2" << std::endl;
    TTF_Quit();
    SDL_Quit();
}

}

#endif
