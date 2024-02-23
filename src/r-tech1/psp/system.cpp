#ifdef MINPSPW

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "r-tech1/system.h"

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif

#include <cstring>
#include <cstdlib>
/**
 * ref
 * https://pspdev.github.io/pspsdk/
*/
#include <psptypes.h>
#include <pspkernel.h>
#include <pspdisplay.h> // For PSP display functions
#include <pspctrl.h> // For PSP control functions
#include <pspdebug.h> // For PSP debug functions
#include <pspctrl.h>

bool System::isDirectory(const std::string & path){
    SceIoStat stat;
    int result = sceIoGetstat(path.c_str(), &stat);
    if (result < 0) {
        pspDebugScreenPrintf("Error getting file stat: %d\n", result);
        return false; // Error getting file stat
    }

    return FIO_S_ISDIR(stat.st_mode);
}

void System::makeDirectory(const std::string & path){
    int result = sceIoMkdir(path.c_str(), 0777); // Create directory with full permissions
    if (result < 0) {
        // Handle error if mkdir fails
        pspDebugScreenPrintf("Error creating directory: %d\n", result);
    }
}

bool System::readableFile(const std::string & path){
    return !isDirectory(path) && readable(path);
}

bool System::readable(const std::string & path){
    SceUID fd = sceIoOpen(path.c_str(), PSP_O_RDONLY, 0777);
    if (fd >= 0) {
        sceIoClose(fd);
        return true;
    }
}

uint64_t System::getModificationTime(const std::string & path){
    SceIoStat stat;
    int result = sceIoGetstat(path.c_str(), &stat);
    if (result < 0) {
        return 0; // Return 0 if there's an error
    }

    return stat.sce_st_mtime.microsecond;
}

uint64_t System::currentMilliseconds(){
#ifdef USE_SDL
    return SDL_GetTicks();
#elif USE_SDL2
    return SDL_GetTicks();
#else

    // Get the current system time in 64-bit ticks
    SceKernelWideTime currentTime;
    sceKernelGetSystemTimeWide(&currentTime);

    // Convert ticks to milliseconds (1 tick = 1000000 nanoseconds)
    return (currentTime.low / 1000) + (currentTime.hi * 1000000ull);
#endif
}

static void * start_memory = 0;
unsigned long System::memoryUsage(){
    void * here = malloc(0);
    /* hopefully the heap is growing up */
    return (char*) here - (char*) start_memory;
 }

/* call startMemoryUsage once at the very beginning of the program */
void System::startMemoryUsage(){
    start_memory = malloc(0);
}

#endif
