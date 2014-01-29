#if 0

#ifdef USE_SDL
#include <SDL.h>
#endif
#include "util/thread.h"
#include "util/init.h"

/* game counter, controls FPS */
static void inc_speed_counter(){
    /* probably put input polling here, InputManager::poll(). no, don't do that.
     * polling is done in the standardLoop now.
     */
    Global::speed_counter4 += 1;
}
#ifdef USE_ALLEGRO
END_OF_FUNCTION(inc_speed_counter)
#endif

/* if you need to count seconds for some reason.. */
static void inc_second_counter() {
    Global::second_counter += 1;
}
#ifdef USE_ALLEGRO
END_OF_FUNCTION(inc_second_counter)
#endif

#ifdef USE_SDL
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

    /* TODO: pass in some variable that tells this loop to quit */
    // while (run_timer_guard.get()){
    while (true){
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

namespace Common{

void startTimers(){
    start_timer(inc_speed_counter, Global::TICS_PER_SECOND);
    start_timer(inc_second_counter, 1);
}

}
#endif

#endif
