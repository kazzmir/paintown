#include "r-tech1/thread.h"

#ifdef WII
/* So we can call ogc's create thread directly */
#include <ogc/lwp.h>
#endif

namespace Util{

namespace Thread{

LockObject::LockObject(){
    initializeLock(&lock);
    // initializeCondition(&condition);
    // Global::debug(0) << "Created lock " << lock << std::endl;
    // Global::debug(0) << "Created condition " << condition << std::endl;
}

int LockObject::acquire() const {
    /* quick hack to get around annoying constness */
    return acquireLock((Lock*) &lock);
}

int LockObject::release() const {
    return releaseLock((Lock*) &lock);
}
        
#if 0
void LockObject::wait() const {
    int ok = 1;
    while (ok != 0){
        /* if conditionWait succeeds then ok will be 0 */
        ok = conditionWait((Condition*) &condition, (Lock*) &lock);
    }
}

void LockObject::wait(volatile bool & check) const {
    int ok = 0;
    /* only wait if check is false. if so then keep waiting until the condition
     * was successful as well.
     */
    while (!check || ok != 0){
        if (ok != 0){
            // Global::debug(0) << "Wait failed: " << SDL_GetError() << std::endl;
        }
        ok = conditionWait((Condition*) &condition, (Lock*) &lock);
    }
}

void LockObject::signal() const {
    conditionSignal((Condition*) &condition);
}
        
void LockObject::lockAndSignal(volatile bool & check, bool what) const {
    acquire();
    check = what;
    signal();
    release();
}
#endif

LockObject::~LockObject(){
    destroyLock(&lock);
    // destroyCondition(&condition);
}

ScopedLock::ScopedLock(const LockObject & lock):
lock(lock){
    lock.acquire();
}

ScopedLock::~ScopedLock(){
    lock.release();
}
    
bool isUninitialized(Id thread){
    return thread == uninitializedValue;
}

ThreadObject::ThreadObject(void * data, void * (function)(void * arg)):
data(data),
function(function),
thread(uninitializedValue){
}

bool ThreadObject::start(){
    if (thread == uninitializedValue){
        return createThread(&thread, NULL, (Thread::ThreadFunction) function, data);
    } else {
        return false;
    }
}

ThreadObject::~ThreadObject(){
    if (thread != uninitializedValue){
        joinThread(thread);
        thread = uninitializedValue;
    }
}

#if defined(USE_SDL) && !defined(USE_NACL)
Id uninitializedValue = NULL;
    
bool initializeLock(Lock * lock){
    *lock = SDL_CreateMutex();
    return *lock != NULL;
}

int acquireLock(Lock * lock){
    return SDL_LockMutex(*lock);
}

int releaseLock(Lock * lock){
    return SDL_UnlockMutex(*lock);
}

void destroyLock(Lock * lock){
    SDL_DestroyMutex(*lock);
}
    
#if 0
void initializeCondition(Condition * condition){
    *condition = SDL_CreateCond();
    if (condition == NULL){
        Global::debug(0) << "Could not create condition" << std::endl;
    }
}

void destroyCondition(Condition * condition){
    SDL_DestroyCond(*condition);
}
    
int conditionWait(Condition * condition, Lock * lock){
    return SDL_CondWait(*condition, *lock);
}

int conditionSignal(Condition * condition){
    return SDL_CondBroadcast(*condition);
}
#endif

/*
void initializeSemaphore(Semaphore * semaphore, unsigned int value){
    *semaphore = SDL_CreateSemaphore(value);
}

void destroySemaphore(Semaphore * semaphore){
    SDL_DestroySemaphore(*semaphore);
}

void semaphoreDecrease(Semaphore * semaphore){
    SDL_SemWait(*semaphore);
}

void semaphoreIncrease(Semaphore * semaphore){
    SDL_SemPost(*semaphore);
}
*/

#ifdef WII
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
#endif

bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
#ifdef WII
    return wiiCreateThread(thread, attributes, function, arg);
#else
    *thread = SDL_CreateThread(function, arg);
    return *thread != NULL;
#endif
}

void joinThread(Id thread){
    if (!isUninitialized(thread)){
#ifdef WII
        wiiJoinThread(thread);
#else
        SDL_WaitThread(thread, NULL);
#endif
    }
}

void cancelThread(Id thread){
#if !SDL_VERSION_ATLEAST(1, 3, 0)
    SDL_KillThread(thread);
#endif
}

#elif USE_ALLEGRO5
Id uninitializedValue = 0;

bool initializeLock(Lock * lock){
    *lock = al_create_mutex();
    return *lock != NULL;
}

int acquireLock(Lock * lock){
    al_lock_mutex(*lock);
    return 0;
}

int releaseLock(Lock * lock){
    al_unlock_mutex(*lock);
    return 0;
}

#if 0
void initializeCondition(Condition * condition){
    *condition = al_create_cond();
}

void destroyCondition(Condition * condition){
    al_destroy_cond(*condition);
}

int conditionWait(Condition * condition, Lock * lock){
    al_wait_cond(*condition, *lock);
    return 0;
}

int conditionSignal(Condition * condition){
    al_broadcast_cond(*condition);
    return 0;
}
#endif

struct AllegroThreadStuff{
    AllegroThreadStuff(const ThreadFunction & function, void * arg):
    function(function),
    arg(arg){
    }

    ThreadFunction function;
    void * arg;
};

static void * allegro_start_thread(ALLEGRO_THREAD * self, void * _stuff){
    AllegroThreadStuff * stuff = (AllegroThreadStuff*) _stuff;
    ThreadFunction function = stuff->function;
    void * arg = stuff->arg;
    delete stuff;
    return function(arg);
}

bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
    AllegroThreadStuff * stuff = new AllegroThreadStuff(function, arg);
    *thread = al_create_thread(allegro_start_thread, stuff);
    if (*thread != NULL){
        al_start_thread(*thread);
        return true;
    } else {
        delete stuff;
        return false;
    }
}

void joinThread(Id thread){
    if (!isUninitialized(thread)){
        al_join_thread(thread, NULL);
    }
}
    
void cancelThread(Id thread){
    al_destroy_thread(thread);
}
    
void destroyLock(Lock * lock){
    al_destroy_mutex(*lock);
}

#else
Id uninitializedValue = 0;

bool initializeLock(Lock * lock){
    return pthread_mutex_init(lock, NULL) == 0;
}

int acquireLock(Lock * lock){
    return pthread_mutex_lock(lock);
}

int releaseLock(Lock * lock){
    return pthread_mutex_unlock(lock);
}

#if 0
void initializeCondition(Condition * condition){
    pthread_cond_init(condition, NULL);
}

void destroyCondition(Condition * condition){
    pthread_cond_destroy(condition);
}

int conditionWait(Condition * condition, Lock * lock){
    return pthread_cond_wait(condition, lock);
}

int conditionSignal(Condition * condition){
    return pthread_cond_broadcast(condition);
}
#endif

#if 0
void initializeSemaphore(Semaphore * semaphore, unsigned int value){
    sem_init(semaphore, 0, value);
}

void destroySemaphore(Semaphore * semaphore){
    /* nothing */
}

void semaphoreDecrease(Semaphore * semaphore){
    sem_wait(semaphore);
}

void semaphoreIncrease(Semaphore * semaphore){
    sem_post(semaphore);
}
#endif

bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
    return pthread_create(thread, (pthread_attr_t*) attributes, function, arg) == 0;
}

void joinThread(Id thread){
    if (!isUninitialized(thread)){
        pthread_join(thread, NULL);
    }
}
    
void cancelThread(Id thread){
    /* FIXME: cancel is not implemented for libogc, find another way.
     * thread suspend/resume is there, though.
     */
#if !defined(WII) && !defined(USE_NACL)
    pthread_cancel(thread);
#endif
}
    
void destroyLock(Lock * lock){
    /* nothing */
}

#endif

}

WaitThread::WaitThread():
done(false){
    Thread::initializeLock(&doneLock);
}

WaitThread::WaitThread(Thread::ThreadFunction thread, void * arg){
    Thread::initializeLock(&doneLock);
    start(thread, arg);
}

static void * do_thread(void * arg){
    WaitThread * thread = (WaitThread *) arg;
    thread->doRun();
    return NULL;
}

void WaitThread::doRun(){
    this->function(this->arg);

    Thread::acquireLock(&doneLock);
    this->done = true;
    Thread::releaseLock(&doneLock);
}

void WaitThread::start(Thread::ThreadFunction thread, void * arg){
    done = false;
    this->arg = arg;
    this->function = thread;
    Thread::createThread(&this->thread, NULL, (Thread::ThreadFunction) do_thread, this);
}

bool WaitThread::isRunning(){
    Thread::acquireLock(&doneLock);
    bool what = done;
    Thread::releaseLock(&doneLock);
    return what;
}

void WaitThread::kill(){
    Thread::cancelThread(thread);
    Thread::joinThread(thread);
}

WaitThread::~WaitThread(){
    /* FIXME: Should we join the thread? */
    /* pthread_join(thread); */
    Thread::joinThread(thread);
}

ThreadBoolean::ThreadBoolean(volatile bool & what, Thread::Lock & lock):
what(what),
lock(lock){
}

bool ThreadBoolean::get(){
    Thread::acquireLock(&lock);
    bool b = what;
    Thread::releaseLock(&lock);
    return b;
}

void ThreadBoolean::set(bool value){
    Thread::acquireLock(&lock);
    what = value;
    Thread::releaseLock(&lock);
}

}
