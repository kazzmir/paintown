#ifndef _rtech1_system_timer_h
#define _rtech1_system_timer_h

#include "r-tech1/thread.h"
#include <thread>
#include <vector>

namespace System{

extern volatile bool run_timer;
extern Util::Thread::Lock run_timer_lock;
extern Util::ThreadBoolean run_timer_guard;
extern std::vector<std::thread> running_timers;

void closeTimers();
void startTimers();

}

#endif
