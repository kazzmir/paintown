#include "thread.h"

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
    
/*
bool isUninitialized(Id thread){
    return thread == uninitializedValue;
}
*/

/*
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
*/

#if defined(USE_SDL) && !defined(USE_NACL)
// Id uninitializedValue = NULL;
    
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

#if 0
#ifndef CROSS_BUILD
bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
    *thread = SDL_CreateThread(function, arg);
    return *thread != NULL;
}

void joinThread(Id thread){
    if (!isUninitialized(thread)){
        SDL_WaitThread(thread, NULL);
    }
}

void cancelThread(Id thread){
#if !SDL_VERSION_ATLEAST(1, 3, 0)
    SDL_KillThread(thread);
#endif
}
#endif
#endif

#elif USE_SDL2

#if 0
bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg){
    // FIXME: name
    *thread = SDL_CreateThread(function, "thread", arg);
    return *thread != NULL;
}

void cancelThread(Id thread){
    // FIXME
    // SDL_KillThread(thread);
}

#ifndef CROSS_BUILD
void joinThread(Id thread){
    if (!isUninitialized(thread)){
        SDL_WaitThread(thread, NULL);
    }
}
#endif

Id uninitializedValue = NULL;
#endif
    
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
#ifndef CROSS_BUILD
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

/*
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
*/

void WaitThread::start(Thread::ThreadFunction thread, void * arg){
    done = false;
    this->arg = arg;
    this->function = thread;
    this->thread = std::thread([this](){
        this->function(this->arg);

        Thread::acquireLock(&doneLock);
        this->done = true;
        Thread::releaseLock(&doneLock);
    });
}

bool WaitThread::isRunning(){
    Thread::acquireLock(&doneLock);
    bool what = done;
    Thread::releaseLock(&doneLock);
    return what;
}

void WaitThread::kill(){
    // FIXME: handle kill somehow
    // Thread::cancelThread(thread);
    thread.join();
}

WaitThread::~WaitThread(){
    /* FIXME: Should we join the thread? */
    /* pthread_join(thread); */
    thread.join();
    // Thread::joinThread(thread);
    Thread::destroyLock(&doneLock);
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
