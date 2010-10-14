#ifndef _paintown_loading_h
#define _paintown_loading_h

#include "util/thread.h"

namespace Loader{

/* Kind of loader to show */
enum Kind{
    Default,
    SimpleCircle
};

extern volatile bool done_loading;
extern Util::Thread::Lock loading_screen_mutex;

void startLoading(Util::Thread::Id * thread, void * arg = 0, Kind kind = Default);
void stopLoading(Util::Thread::Id thread);

void * loadingScreen(void *);

}

#endif
