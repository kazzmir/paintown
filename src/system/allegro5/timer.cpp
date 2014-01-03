#include "system/timer.h"
#include "util/init.h"

#ifdef USE_ALLEGRO5
#include <allegro5/allegro.h>

using std::vector;

namespace System{

struct TimerInfo{
    TimerInfo(void (*x)(), ALLEGRO_TIMER * y):
        tick(x), timer(y){}

    void (*tick)();
    ALLEGRO_TIMER * timer;
};

static void * do_timer(void * info){
    TimerInfo * timerInfo = (TimerInfo*) info;

    ALLEGRO_EVENT_SOURCE * source = al_get_timer_event_source(timerInfo->timer);
    ALLEGRO_EVENT_QUEUE * queue = al_create_event_queue();
    al_register_event_source(queue, source);
    while (run_timer_guard.get()){
        ALLEGRO_EVENT event;
        /* Wait a maximum of 50ms in case we need to restart the timers */
        if (al_wait_for_event_timed(queue, &event, 0.05)){
            timerInfo->tick();
        }
    }

    al_destroy_event_queue(queue);
    al_destroy_timer(timerInfo->timer);

    delete timerInfo;
    return NULL;
}

static Util::Thread::Id start_timer(void (*func)(), int frequency){
    ALLEGRO_TIMER * timer = al_create_timer(ALLEGRO_BPS_TO_SECS(frequency));
    if (timer == NULL){
        Global::debug(0) << "Could not create timer" << std::endl;
    }
    al_start_timer(timer);
    TimerInfo * info = new TimerInfo(func, timer);
    Util::Thread::Id thread;
    Util::Thread::createThread(&thread, NULL, (Util::Thread::ThreadFunction) do_timer, (void*) info);
    return thread;
}

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
