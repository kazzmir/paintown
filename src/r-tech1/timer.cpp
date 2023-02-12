#include <time.h>
#include "r-tech1/timer.h"
#include "r-tech1/init.h"
#include "r-tech1/funcs.h"
#include "r-tech1/thread.h"

namespace Util{

void * do_wait(void * timer_){
    Timer * timer = (Timer *) timer_;
    unsigned int now = Global::second_counter;
    /* add 1 because the current time is in between X and X+1 */
    while (Global::second_counter - now < timer->wait+1){
        Util::rest(50);
        bool do_stop = false;
        Thread::acquireLock(&timer->lock);
        do_stop = timer->stopped;
        Thread::releaseLock(&timer->lock);
        if (do_stop){
            return NULL;
        }
    }
    timer->func(timer->arg);
    return NULL;
}

Timer::Timer(unsigned int seconds_to_wait, timeout func, void * arg):
wait(seconds_to_wait),
func(func),
arg(arg),
stopped(false){
    Thread::initializeLock(&lock);
    Thread::createThread(&timer, NULL, (Util::Thread::ThreadFunction) do_wait, this);
}

void Timer::stop(){
    Thread::acquireLock(&lock);
    stopped = true;
    Thread::releaseLock(&lock);
    Thread::joinThread(timer);
}

}
