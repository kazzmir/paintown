#ifndef _paintown_system_timer_h
#define _paintown_system_timer_h

#include "util/thread.h"
#include <vector>

namespace System{

extern volatile bool run_timer;
extern Util::Thread::Lock run_timer_lock;
extern Util::ThreadBoolean run_timer_guard;
extern std::vector<Util::Thread::Id> running_timers;

void closeTimers();
void startTimers();

}

#endif
