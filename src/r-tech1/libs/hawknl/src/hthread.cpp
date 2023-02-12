/*
  HawkThreads thread module
  Copyright (C) 2003-2004 Phil Frisbie, Jr. (phil@hawksoft.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.
    
  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA  02111-1307, USA.
      
  Or go to http://www.gnu.org/copyleft/lgpl.html
*/

#include "hawknl/htinternal.h"

#ifdef HT_WIN_THREADS

typedef struct {
    void *(*func) (void *);
    void *arg;
} ThreadParms;

static unsigned  __stdcall threadfunc(void *arg)
{
    void *(*func) (void *);
    void *args;
    
    func = ((ThreadParms *)arg)->func;
    args = ((ThreadParms *)arg)->arg;
    free(arg);
    
    return (unsigned)((*func)(args));
}
#if defined (_WIN32_WCE)
#define _beginthreadex(security, \
		       stack_size, \
		       start_proc, \
		       arg, \
		       flags, \
		       pid) \
	CreateThread(security, \
		     stack_size, \
		     (LPTHREAD_START_ROUTINE) start_proc, \
		     arg, \
		     flags, \
		     pid)
#else /* !(_WIN32_WCE) */
#include <process.h>
#endif /* !(_WIN32_WCE) */

#else /* !HT_WIN_THREADS */
/* POSIX systems */
#include <pthread.h>
#include <sched.h>
#endif /* !HT_WIN_THREADS */

#ifndef HL_WINDOWS_APP
#include <unistd.h>
#endif /* HL_WINDOWS_APP */

HL_EXP HThreadID HL_APIENTRY htThreadCreate(HThreadFunc func, void *data, int joinable)
{
    /* Windows threads */
#ifdef HT_WIN_THREADS
    HANDLE          h;
    unsigned        tid;
    ThreadParms     *p;

    p = (ThreadParms *)malloc(sizeof(ThreadParms));
    if(p == NULL)
    {
        SetLastError((DWORD)ENOMEM);
        return (HThreadID)HT_INVALID;
    }
    p->func = func;
    p->arg = data;
    h = (HANDLE)_beginthreadex(NULL, 0, threadfunc, p, 0, &tid);
    if(h == (HANDLE)(0))
    {
        return (HThreadID)HT_INVALID;
    }
    if(joinable == HT_FALSE)
    {
        (void)CloseHandle(h);
        return NULL;
    }
    return (HThreadID)h;
    
    /* POSIX systems */
#else
    pthread_attr_t  attr;
    pthread_t       tid;
    int             result;
    
    (void)pthread_attr_init(&attr);
    if(joinable == HT_FALSE)
    {
        (void)pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }
    else
    {
        (void)pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    }
    result = pthread_create(&tid, &attr, func, data);
    (void)pthread_attr_destroy(&attr);
    if(result != 0)
    {
#ifdef HL_WINDOWS_APP
            SetLastError((DWORD)result);
#endif
            return (HThreadID)HT_INVALID;
    }
    if(joinable == HT_FALSE)
    {
        return NULL;
    }
    return (HThreadID)tid;
#endif
}

HL_EXP void HL_APIENTRY htThreadYield(void)
{
    /* Windows threads */
#ifdef HT_WIN_THREADS
    Sleep((DWORD)0);

    /* POSIX systems */
#else
    (void)sched_yield();
    
#endif
}

HL_EXP int HL_APIENTRY htThreadJoin(HThreadID threadID, void **status)
{
#ifdef HT_WIN_THREADS
    /* Windows threads */
    if(WaitForSingleObject((HANDLE)threadID, INFINITE) == WAIT_FAILED)
    {
        return EINVAL;
    }
    if(status != NULL)
    {
        (void)GetExitCodeThread((HANDLE)threadID, (LPDWORD)status);
    }
    (void)CloseHandle((HANDLE)threadID);
    return 0;

#else
    /* POSIX systems */
    return pthread_join((pthread_t)threadID, status);
#endif
}

HL_EXP void HL_APIENTRY htThreadSleep(int mseconds)
{
#ifdef HL_WINDOWS_APP
    Sleep((DWORD)mseconds);
#else /* !HL_WINDOWS_APP */
    struct timespec     tv;

    tv.tv_sec = mseconds / 1000;
    tv.tv_nsec = (mseconds % 1000) * 1000;

    (void)nanosleep(&tv, NULL);
#endif /* !HL_WINDOWS_APP */
    /* can use usleep if nanosleep is not supported on your platform */
/*    (void)usleep(mseconds*1000); */
}


