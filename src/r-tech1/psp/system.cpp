#ifdef MINPSPW

#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "r-tech1/system.h"
#include "r-tech1/funcs.h"

#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#ifdef USE_SDL2
#include <SDL2/SDL.h>
#endif

/**
 * https://pspdev.github.io/pspsdk/
*/
#include <psptypes.h>
#include <pspkernel.h>

bool System::isDirectory(const std::string & path){
    SceIoStat stat;
    int result = sceIoGetstat(path.c_str(), &stat);
    if (result < 0) {
        DebugLog2 << "Error getting file stat: " <<  result << std::endl;
        return false; // Error getting file stat
    }

    return FIO_S_ISDIR(stat.st_mode);
}

void System::makeDirectory(const std::string & path){
    int result = sceIoMkdir(path.c_str(), 0777); // Create directory with full permissions
    if (result < 0) {
        // Handle error if mkdir fails
        DebugLog2 << "Error creating directory: " << result << std::endl;
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
        DebugLog2 << "Error creating directory: " << result << std::endl;

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
    // TODO - FIXME
    return 0;
#endif
}

static void * start_memory = 0;
unsigned long System::memoryUsage(){
    // TODO - FIXME
    return 0;
 }

/* call startMemoryUsage once at the very beginning of the program */
void System::startMemoryUsage(){
    // TODO - FIXME
}

#endif
