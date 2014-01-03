#include "timer.h"

using std::vector;

namespace System{

volatile bool run_timer;
Util::Thread::Lock run_timer_lock;
Util::ThreadBoolean run_timer_guard(run_timer, run_timer_lock);
std::vector<Util::Thread::Id> running_timers;

void closeTimers(){
    run_timer_guard.set(false);
    for (vector<Util::Thread::Id>::iterator it = running_timers.begin(); it != running_timers.end(); it++){
        Util::Thread::Id timer = *it;
        Util::Thread::joinThread(timer);
    }
    running_timers.clear();
}

}
