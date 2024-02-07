#ifndef _rtech1_thread_h
#define _rtech1_thread_h

/* FIXME: NACL should be able to use SDL threads but they are broken
 * for some reason. SDL is implemented in terms of pthreads anyway
 * so for now just use the native pthreads implementation.
 */
#if defined(USE_SDL) && !defined(USE_NACL)
#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_mutex.h>
#elif USE_SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_mutex.h>
#elif USE_ALLEGRO5
#include <allegro5/allegro5.h>
#else
#include <pthread.h>
// #include <semaphore.h>
#endif

#include <thread>
#include "exceptions/exception.h"
/*
#include "exceptions/load_exception.h"
#include "token_exception.h"
#include "mugen/exception.h"
*/
// #include "funcs.h"
#include "debug.h"

/* 9/10/2012: Condition variables have been removed. There are no use-cases in the code
 * where multiple threads are waiting for producer thread to signal them so a simpler
 * solution is just to use a mutex and poll. The main motivation to remove condition
 * variables was in getting the xbox 360 port to work with libxenon which does not
 * yet have support for condition variables, but supposedly mutexes work ok.
 */

namespace Util{

    void rest(int x);

/* Either uses pthreads or SDL_thread */
namespace Thread{
#ifdef USE_SDL
    typedef SDL_mutex* Lock;
    typedef SDL_Thread* Id;
    typedef int (*ThreadFunction)(void*);
    typedef SDL_cond* Condition;
    // typedef SDL_semaphore* Semaphore;
#elif USE_SDL2
    typedef SDL_mutex* Lock;
    typedef SDL_Thread* Id;
    typedef int (*ThreadFunction)(void*);
    typedef SDL_cond* Condition;
#elif USE_ALLEGRO5
    typedef ALLEGRO_MUTEX* Lock;
    typedef ALLEGRO_THREAD* Id;
    typedef void * (*ThreadFunction)(void*);
    typedef ALLEGRO_COND* Condition;
    // typedef SDL_semaphore* Semaphore;
#else
    typedef pthread_mutex_t Lock;
    typedef pthread_t Id;
    typedef pthread_cond_t Condition;
    // typedef sem_t Semaphore;
    typedef void * (*ThreadFunction)(void*);
#endif

    // extern Id uninitializedValue;
    // bool isUninitialized(Id thread);
    bool initializeLock(Lock * lock);

    /*
    void initializeCondition(Condition * condition);
    void destroyCondition(Condition * condition);
    int conditionWait(Condition * condition, Lock * lock);
    int conditionSignal(Condition * condition);
    */

    /*
    void initializeSemaphore(Semaphore * semaphore, unsigned int value);
    void destroySemaphore(Semaphore * semaphore);
    void semaphoreDecrease(Semaphore * semaphore);
    void semaphoreIncrease(Semaphore * semaphore);
    */

    int acquireLock(Lock * lock);
    int releaseLock(Lock * lock);
    void destroyLock(Lock * lock);
    /*
    bool createThread(Id * thread, void * attributes, ThreadFunction function, void * arg);
    void joinThread(Id thread);
    void cancelThread(Id thread);
    */

    /* wraps a Lock in a c++ class */
    class LockObject{
    public:
        LockObject();

        int acquire() const;
        int release() const;

        Lock & getLock(){
            return lock;
        }

        /* wait until check is true.
         * you MUST acquire the lock before calling this function */
        // void wait(volatile bool & check) const;

        /* just until we are signaled
         * you MUST acquire the lock before calling this function */
        // void wait() const;

        /* you MUST acquire the lock before calling this function */
        // void signal() const;

        /* gets the lock, sets the boolean, and signals the condition
         * you MUST NOT acquire the lock before calling this function
         */
        // void lockAndSignal(volatile bool & check, bool what) const;

        virtual ~LockObject();

        Lock lock;
        // Condition condition;
    };

    /* acquires/releases the lock in RAII fashion */
    class ScopedLock{
    public:
        ScopedLock(const LockObject & lock);
        ~ScopedLock();
    private:
        const LockObject & lock;
    };

    /*
    class ThreadObject{
    public:
        ThreadObject(void * data, void * (function)(void * arg));

        / * true if the thread was started, false otherwise * /
        virtual bool start();
        virtual ~ThreadObject();

    protected:
        void * data;
        void * (*function)(void * arg);
        Id thread;
    };
    */
}

class WaitThread{
public:
    /* does not start a new thread yet */
    WaitThread();

    /* starts a thread */
    WaitThread(Thread::ThreadFunction thread, void * arg);

    /* starts a thread */
    void start(Thread::ThreadFunction thread, void * arg);

    bool isRunning();
    void kill();

    virtual ~WaitThread();

public:
    /* actually runs the thread */
    // void doRun();

protected:
    Thread::Lock doneLock;
    std::thread thread;
    volatile bool done;
    void * arg;
    Thread::ThreadFunction function;
};

/* wraps a boolean with lock/unlock while checking/setting it */
class ThreadBoolean{
public:
    ThreadBoolean(volatile bool & what, Thread::Lock & lock);

    bool get();
    void set(bool value);

protected:
    volatile bool & what;
    Thread::Lock & lock;
};

/* Computes stuff in a separate thread and gives it back when you ask for it.
 * Call start() on the future to begin executing it. You can then call `get' on
 * the future object to get the result. If the thread is still executing then
 * `get' will block until the future completes. If the future has already
 * completed then `get' will return immediately with the computed value.  The
 * use case is computing something that has to be used later: Future future;
 * future.start(); // might take a while to compute
 * do_stuff_that_takes_a_while(); // future might finish sometime in here
 * Object o = future.get(); // future is already done
 *
 */
template<class X> class Future{
public:
    Future():
        thing(0),
        done(false),
        exception(NULL),
        ran(false){ }

    virtual ~Future(){
        if (thread.joinable()){
            thread.join();
        }
    }

    virtual X get(){
        /* make sure the future has been started */
        start();

        Exception::Base * failed = NULL;
        bool ok = false;
        while (!ok){
            future.acquire();
            if (!done){
                future.release();
                Util::rest(1);
            } else {
                ok = true;
            }
        }
        if (exception != NULL){
            failed = exception;
        }
        X out = thing;
        future.release();
        if (failed){
            failed->throwSelf();
        }
        return out;
    }

    virtual void start(){
        if (ran){
            return;
        }

        thread = std::thread([this](){
            this->doRun();
        });

        ran = true;
    }

protected:
    bool isDone(){
        Thread::ScopedLock scoped(future);
        return done;
    }

    void doRun(){
        try{
            this->compute();
        /*
        } catch (const LoadException & load){
            me->exception = new LoadException(load);
        } catch (const TokenException & t){
            me->exception = new TokenException(t);
        } catch (const MugenException & m){
            me->exception = new MugenException(m);
        */
        } catch (const Exception::Base & base){
            this->exception = base.copy();
        } catch (...){
            this->exception = new Exception::Base(__FILE__, __LINE__);
        }
        this->future.acquire();
        this->done = true;
        this->future.release();
    }

    virtual void set(X x){
        this->thing = x;
    }

    virtual void compute() = 0;

    X thing;
    std::thread thread;
    Thread::LockObject future;
    volatile bool done;
    /* if any exceptions occur, throw them from `get' */
    Exception::Base * exception;
    /* if the future was already started this is true so it can't be
     * accidentally started twice
     */
    bool ran;
};

}

#endif
