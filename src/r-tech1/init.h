#ifndef _rtech1_init_h
#define _rtech1_init_h

/* global vars */

/*
#define GFX_X 640
#define GFX_Y 480
*/

namespace Global{
    extern volatile int speed_counter4;
    extern volatile unsigned int second_counter;

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

        WindowMode graphics;
        bool sound;
        bool fullscreen;
        bool networking;
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
}

/*
#define GFX_X 320
#define GFX_Y 240
*/

#endif
