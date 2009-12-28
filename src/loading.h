#ifndef _paintown_loading_h
#define _paintown_loading_h

#include <pthread.h>

namespace Loader{

extern bool done_loading;
extern pthread_mutex_t loading_screen_mutex;

void startLoading(pthread_t * thread, void * arg = 0);
void stopLoading(pthread_t thread);

void * loadingScreen( void * arg );

}

#endif
