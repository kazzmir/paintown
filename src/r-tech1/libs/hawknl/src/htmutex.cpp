/*
  HawkThreads mutex module
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

struct ht_mutex_t
{
  CRITICAL_SECTION  mutex;
  DWORD             thread;
};
#else /* !HT_WIN_THREADS */
#include <pthread.h>
#ifndef HL_WINDOWS_APP
#include <sys/errno.h>
#endif /* HL_WINDOWS_APP */


struct ht_mutex_t
{
  pthread_mutex_t   mutex;
};
#endif /* HT_WIN_THREADS */

HL_EXP int HL_APIENTRY htMutexInit(HTmutex *mutex)
{
	if(mutex == NULL)
	{
		return EINVAL;
	}
    else
    {
        HTmutex mx;
#ifdef HT_WIN_THREADS
        /* native Windows */

        mx = (HTmutex)malloc(sizeof(struct ht_mutex_t));
	    if(mx == NULL)
	    {
            return ENOMEM;
	    }
        InitializeCriticalSection(&mx->mutex);
        mx->thread = 0;
#else
        /* POSIX */
        pthread_mutexattr_t attr;
        int                 result;

        mx = (HTmutex)malloc(sizeof(struct ht_mutex_t));
	    if(mx == NULL)
	    {
            return ENOMEM;
	    }
        (void)pthread_mutexattr_init(&attr);
#if defined MACOSX || defined __FreeBSD__
        /* GUSI is not fully POSIX compliant, and does not
         * define PTHREAD_MUTEX_ERRORCHECK
         * http://lists.apple.com/archives/unix-porting/2005/May/msg00021.html
         */
        (void)pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
#else
        (void)pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
#endif
        result = pthread_mutex_init((pthread_mutex_t *)&mx->mutex, &attr);
        (void)pthread_mutexattr_destroy(&attr);
        if(result != 0)
        {
            return result;
        }
#endif
        *mutex = mx;
    }
    return 0;
}

HL_EXP int HL_APIENTRY htMutexLock(HTmutex *mutex)
{
	if(mutex == NULL)
	{
		return EINVAL;
	}
	if(*mutex == NULL)
	{
		return EINVAL;
	}
    else
    {
        HTmutex mx = *mutex;
#ifdef HT_WIN_THREADS
        DWORD threadid = GetCurrentThreadId();

        /* native Windows */
        /* this call will not stop recursion on a single thread */
        EnterCriticalSection(&mx->mutex);
        /* check for recursion */
        if(mx->thread == threadid)
        {
            /* must call LeaveCriticalSection for each EnterCriticalSection */
            /* so this nullifies the above call to EnterCriticalSection*/
            LeaveCriticalSection(&mx->mutex);
		    return EDEADLK;
        }
        else
        {
            mx->thread = threadid;
        }
        return 0;
#else
        /* POSIX */
        return pthread_mutex_lock((pthread_mutex_t *)&mx->mutex);
#endif
    }
}

HL_EXP int HL_APIENTRY htMutexUnlock(HTmutex *mutex)
{
	if(mutex == NULL)
	{
		return EINVAL;
	}
	if(*mutex == NULL)
	{
		return EINVAL;
	}
    else
    {
        HTmutex mx = *mutex;
#ifdef HT_WIN_THREADS
        DWORD threadid = GetCurrentThreadId();

        /* native Windows */
        if((mx->thread == 0) ||(mx->thread != threadid))
        {
		    return EPERM;
        }
        mx->thread = 0;
        LeaveCriticalSection(&mx->mutex);
        return 0;
#else
        /* POSIX */
        return pthread_mutex_unlock((pthread_mutex_t *)&mx->mutex);
#endif
    }
}

HL_EXP int HL_APIENTRY htMutexDestroy(HTmutex *mutex)
{
	if(mutex == NULL)
	{
		return EINVAL;
	}
	if(*mutex == NULL)
	{
		return EINVAL;
	}
    else
    {
        HTmutex mx = *mutex;
        int     result = 0;
#ifdef HT_WIN_THREADS
        /* native Windows */
        DeleteCriticalSection(&mx->mutex);
#else
        /* POSIX */
        result = pthread_mutex_destroy((pthread_mutex_t *)&mx->mutex);
#endif
        free(*mutex);
        *mutex = NULL;
        return result;
    }
}

