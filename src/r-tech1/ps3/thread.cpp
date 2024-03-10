#ifdef PS3

#include "thread.h"

// So we can call ps3 directly to create thread
#include <sys/thread.h>
#include "pthread.h"

namespace Util{

namespace Thread{

#if 0
   bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
        // TODO: FIXME
       return 0;
   }

   void joinThread(Id * thread){
        // TODO: FIXME
   }

   void cancelThread(Id thread){
        // TODO: FIXME
   }
#endif

}

}

#endif