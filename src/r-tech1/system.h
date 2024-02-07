#ifndef _rtech1_system_h
#define _rtech1_system_h

/* system utilities */

#include <string>
#include <stdint.h>

namespace System{
    bool isDirectory(const std::string & path);
    void makeDirectory(const std::string & path);
    void makeAllDirectory(const std::string & path);
    bool readableFile(const std::string & path);
    bool readable(const std::string & path);
    uint64_t getModificationTime(const std::string & path);
    // uint64_t currentMicroseconds();
    uint64_t currentMilliseconds();
    uint64_t currentSeconds();
    unsigned long memoryUsage();

    /* call startMemoryUsage once at the very beginning of the program */
    void startMemoryUsage();

    void shutdown();
}

#endif
