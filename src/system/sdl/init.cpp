#ifdef USE_SDL

#include <SDL.h>
#include "../init.h"
#include "util/debug.h"
#include <stdlib.h>
#include "util/file-system.h"

namespace System{

void initSystem(const Global::InitConditions & conditions, Global::stream_type & out){
#ifdef ANDROID
    /* opengles2 is the default renderer but it doesn't work */
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles");
#endif

    out << "SDL Init: ";

    int ok = SDL_Init(SDL_INIT_VIDEO |
                      SDL_INIT_AUDIO |
                      SDL_INIT_TIMER |
                      SDL_INIT_JOYSTICK |
                      SDL_INIT_NOPARACHUTE);
    if (ok == 0){
        out << "Ok" << std::endl;
    } else {
	out << "Failed (" << ok << ") - " << SDL_GetError() << std::endl;
	exit(ok);
    }

    /* Just do SDL thread init
#ifdef MINPSPW
    pthread_init();
#endif
*/

    if (conditions.graphics != Global::InitConditions::Disabled){
        try{
            SDL_Surface * icon = SDL_LoadBMP(Storage::instance().find(Filesystem::RelativePath("menu/icon.bmp")).path().c_str());
            if (icon != NULL){
                SDL_WM_SetIcon(icon, NULL);
            }
        } catch (const Filesystem::NotFound & failed){
            Global::debug(0) << "Could not find window icon: " << failed.getTrace() << std::endl;
        }

        SDL_WM_SetCaption("Paintown", NULL);
    }

    SDL_EnableUNICODE(1);
    SDL_JoystickEventState(1);

    atexit(SDL_Quit);
    // atexit(doSDLQuit);
}

}

#endif
