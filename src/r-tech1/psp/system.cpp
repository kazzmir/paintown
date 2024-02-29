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

#include <cstring>
#include <cstdlib>



/**
 * ref
 * https://pspdev.github.io/pspsdk/
*/
#include <psptypes.h>
#include <pspkernel.h>
#include <pspctrl.h> // For PSP control functions
#include <pspdebug.h> // For PSP debug functions

/* psp doesn't have an implementation of access() yet. if it gets one this function
 * can be removed.
 */
int access(const char * path, int mode){
    struct stat information;
    int ok = stat(path, &information);
    // printf("stat of '%s' is %d\n", path.c_str(), ok);
    if (ok == 0){
        if (mode == R_OK){
            if (((information.st_mode & S_IRUSR) == S_IRUSR) ||
                ((information.st_mode & S_IRGRP) == S_IRGRP) ||
                ((information.st_mode & S_IROTH) == S_IROTH)){
                return 0;
            } else {
            /* handle other modes if they become useful to us */
                return -1;
            }
       } else {
           return -1;
       }
    } else {
        // perror("stat");
        return -1;
    }
}

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
