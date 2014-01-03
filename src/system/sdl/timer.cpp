#include "../timer.h"
#include "util/init.h"

#ifdef USE_SDL
#include <SDL.h>
    
// static pthread_t events;

namespace System{

struct TimerInfo{
    TimerInfo(void (*x)(), int y):
        tick(x), frequency(y){}

    void (*tick)();
    int frequency;
};

static void * do_timer(void * arg){
    TimerInfo info = *(TimerInfo *) arg;
    uint32_t delay = (uint32_t)(1000.0 / (double) info.frequency);

    /* assuming SDL_GetTicks() starts at 0, this should last for about 50 days
     * before overflowing. overflow should work out fine. Assuming activate occurs
     * when the difference between now and ticks is at least 6, the following will happen.
     * ticks      now        now-ticks
     * 4294967294 4294967294 0
     * 4294967294 4294967295 1
     * 4294967294 0          2
     * 4294967294 1          3
     * 4294967294 2          4
     * 4294967294 3          5
     * 4294967294 4          6
     * Activate
     * 3          5          2
     * 3          6          3
     * 3          7          4
     * 3          8          5
     * 3          9          6
     * Activate
     *
     * Can 'now' ever be much larger than 'ticks' due to overflow?
     * It doesn't seem like it.
     */
    uint32_t ticks = SDL_GetTicks();

    while (run_timer_guard.get()){
        uint32_t now = SDL_GetTicks();
        while (now - ticks >= delay){
            // Global::debug(0) << "Tick!" << endl;
            info.tick();
            ticks += delay;
        }
        SDL_Delay(1);
    }

    delete (TimerInfo *) arg;

    return NULL;
}

static Util::Thread::Id start_timer(void (*func)(), int frequency){
    TimerInfo * speed = new TimerInfo(func, frequency);
	/*
    speed.tick = func;
    speed.frequency = frequency;
*/
    Util::Thread::Id thread;
    Util::Thread::createThread(&thread, NULL, (Util::Thread::ThreadFunction) do_timer, (void*) speed);
    return thread;
}

/*
static void doSDLQuit(){
    SDL_Event quit;
    quit.type = SDL_QUIT;
    SDL_PushEvent(&quit);
    Global::debug(0) << "Waiting for SDL event handler to finish" << endl;
    pthread_join(events, NULL);
    SDL_Quit();
}
*/

/* game counter, controls FPS */
static void inc_speed_counter(){
    /* probably put input polling here, InputManager::poll(). no, don't do that.
     * polling is done in the standardLoop now.
     */
    Global::speed_counter4 += 1;
}

/* if you need to count seconds for some reason.. */
static void inc_second_counter() {
    Global::second_counter += 1;
}

void startTimers(){
    run_timer_guard.set(true);
    running_timers.push_back(start_timer(inc_speed_counter, Global::TICS_PER_SECOND));
    running_timers.push_back(start_timer(inc_second_counter, 1));
}

}

#endif
