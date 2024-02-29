#ifndef _rtech1_init_h
#define _rtech1_init_h

/* global vars */

/*
#define GFX_X 640
#define GFX_Y 480
*/

#include "atomic.h"
#include <thread>

namespace Global{
    extern Atomic::atomic<uint64_t> speed_counter4;
    extern Atomic::atomic<uint64_t> second_counter;

    // extern const double LOGIC_MULTIPLIER;
    extern int TICS_PER_SECOND;

    extern const int WINDOWED;
    extern const int FULLSCREEN;

    extern bool rateLimit;

    class InitConditions{
    public:
        InitConditions();

        enum WindowMode{
            Disabled,
            Default,
            Window,
            Fullscreen
        };

        bool useSoftwareRenderer() const {
            return softwareRenderer;
        }

        WindowMode graphics;
        bool sound;
        bool fullscreen;
        bool networking;
        bool softwareRenderer;
    };

    bool init(const InitConditions & conditions);
    void close();
    // bool initNoGraphics();

    /* Check for the data path */
    bool dataCheck();

    /* Returns a consistent number of ticks per second regardless of what
     * TICS_PER_SECOND is.
     */
    double ticksPerSecond(int ticks);

    /* Updates TICS_PER_SECOND */
    void setTicksPerSecond(int ticks);

    extern std::thread::id mainThreadId;

    bool isMainThread();
}

/*
#define GFX_X 320
#define GFX_Y 240
*/

#endif
