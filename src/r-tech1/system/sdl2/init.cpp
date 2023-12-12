#ifdef USE_SDL2

#include <iostream>
#include "r-tech1/system/init.h"
#include "r-tech1/debug.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <spawn.h>
#include <sys/wait.h>

namespace System{

SDL_TimerID timer;

static bool hasGlxInfo(){
    /* FIXME: on windows just return true */

    char glxinfo[] = "glxinfo";
    char* const argv[] = {glxinfo, NULL};
    pid_t pid = 0;
    int ok = posix_spawnp(&pid, "glxinfo", NULL, NULL, argv, NULL);
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
}

void initSystem(const Global::InitConditions & conditions, Global::stream_type & out){
    /* if glx info fails to run then we probably need to use a software renderer */
    if (!hasGlxInfo()){
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
}

static unsigned int timerCallback(unsigned int, void * param){
    Global::speed_counter4 += 1;
    /* zero callback means the timer should cancel. non-zero return value means that the timer should be restarted */
    return 1;
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
