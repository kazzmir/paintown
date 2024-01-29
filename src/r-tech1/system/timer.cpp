#include "r-tech1/system/timer.h"

using std::vector;

namespace System{

volatile bool run_timer;
Util::Thread::Lock run_timer_lock;
Util::ThreadBoolean run_timer_guard(run_timer, run_timer_lock);
std::vector<std::thread> running_timers;

void closeTimers(){
    run_timer_guard.set(false);
    for (std::thread & thread: running_timers){
        thread.join();
    }
    running_timers.clear();
}

}
