#ifndef _paintown_ogc_pthread
#define _paintown_ogc_pthread

#include <ogc/cond.h>
#include <ogcsys.h>
#include <gccore.h>
#define STACKSIZE 8*1024

typedef enum {
    PTHREAD_MUTEX_ERRORCHECK_NP
} pthread_mutexattr_set;

typedef enum {
    PTHREAD_CREATE_DETACHED,
    PTHREAD_CREATE_JOINABLE
} pthread_threadcreate_set;

typedef unsigned int pthread_key_t;
typedef lwp_t pthread_t;
typedef mutex_t pthread_mutex_t;
typedef void* pthread_mutexattr_t;
typedef int pthread_attr_t;
 
inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
//int pthread_cancel(pthread_t thread);
inline int pthread_join(pthread_t thread, void ** value);
 
inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
inline int pthread_mutex_destroy(pthread_mutex_t *mutex);
inline int pthread_mutex_lock(pthread_mutex_t *mutex);
inline int pthread_mutex_unlock(pthread_mutex_t *mutex);

int pthread_key_create(pthread_key_t *key, void (*destr_function) (void *));
int pthread_setspecific(pthread_key_t key, const void *pointer);
void * pthread_getspecific(pthread_key_t key);
 
//imp
inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
*thread = 0;
return LWP_CreateThread(thread, start_routine, arg, 0, STACKSIZE, 64);
}

inline void sched_yield(){
    LWP_YieldThread();
}

inline int pthread_join(pthread_t thread, void ** value){
    return LWP_JoinThread(thread, value);
}

inline int pthread_attr_init(pthread_attr_t * attr){
    return 0;
}

inline int pthread_attr_setdetachstate(pthread_attr_t * attr, pthread_threadcreate_set f){
    return 0;
}

inline int pthread_attr_destroy(pthread_attr_t * attr){
    return 0;
}
 
//int pthread_cancel(pthread_t thread);
 
inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
return LWP_MutexInit(mutex, 0);
}
inline int pthread_mutex_destroy(pthread_mutex_t *mutex){ return LWP_MutexDestroy(*mutex);}

inline int pthread_mutexattr_init(pthread_mutexattr_t * attr){
    return 0;
}

inline int pthread_mutexattr_settype(pthread_mutexattr_t * attr, pthread_mutexattr_set s){
    return 0;
}

inline int pthread_mutexattr_destroy(pthread_mutexattr_t * attr){
    return 0;
}
 
inline int pthread_mutex_lock(pthread_mutex_t *mutex) { return LWP_MutexLock(*mutex); }
inline int pthread_mutex_trylock(pthread_mutex_t *mutex){ return LWP_MutexTryLock(*mutex);}
inline int pthread_mutex_unlock(pthread_mutex_t *mutex) { return LWP_MutexUnlock(*mutex); }

/* condition variable stuff */

typedef cond_t pthread_cond_t;

/* end condition variable stuff */

#endif
