#ifdef DEVKITPRO
#include "thread.h"

namespace Util{

namespace Thread{

/* So we can call ogc's create thread directly */
#if defined(GAMECUBE) || defined(WII)
#include <ogc/lwp.h>


struct LwpThread{
    lwp_t thread;
    char * stack;
};

/* We use libogc's LWP_CreateThread directly because SDL's thread api doesnt let us
 * explicitly specify the stack size.
 * MUGEN in particular needs a relatively large stack (somewhere around 32kb-64kb)
 * to run the parsers. We allocate a little more than that just to be safe (128kb).
 * It is easier to use libogc directly than hack SDL each time.
 */
bool wiiCreateThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
    LwpThread * lwp = new LwpThread();
    int size = 128 * 1024;
    lwp->stack = new char[size];
    if (LWP_CreateThread(&lwp->thread, (void * (*)(void*)) function, arg, lwp->stack, size, 80) != 0){
        delete[] lwp->stack;
        delete lwp;
        return false;
    }
    *thread = (Id) lwp;
    return true;
}

void wiiJoinThread(Id thread){
    LwpThread * lwp = (LwpThread*) thread;
    if (LWP_JoinThread(lwp->thread, NULL) == 0){
        delete[] lwp->stack;
        delete lwp;
    } else {
        /* Now what?? */
    }
}

#if 0
bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
    return wiiCreateThread(thread, attributes, function, arg);
}

void joinThread(Id thread){
    if (!isUninitialized(thread)){
        wiiJoinThread(thread);
    }
}
#endif

#elif defined(WIIU)
// Do something for WIIU
#endif


}

}


#endif