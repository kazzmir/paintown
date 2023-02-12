/*
  HawkThreads condition module
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

enum {
    SIGNAL = 0,
    BROADCAST = 1,
    MAX_EVENTS = 2
};

struct ht_cond_t
{
  HANDLE events_[MAX_EVENTS];
};

#else /* !HT_WIN_THREADS */

#ifdef __FreeBSD__
#include <sys/types.h>
#endif
#include <sys/timeb.h>

#ifdef HL_WINDOWS_APP
#define timeb _timeb
#define ftime _ftime
#else
#include <sys/time.h>
#endif /* HL_WINDOWS_APP */

#include <pthread.h>
struct ht_cond_t
{
  pthread_cond_t    cond;
  pthread_mutex_t   mutex;
};

#endif /* !HT_WIN_THREADS */

HL_EXP int HL_APIENTRY htCondInit(HTcond *cond)
{
	if(cond == NULL)
	{
		return EINVAL;
	}
    else
    {
#ifdef HT_WIN_THREADS
        HTcond cv = NULL;

        cv = (HTcond)malloc(sizeof(struct ht_cond_t));
        if(cv == NULL)
        {
            return ENOMEM;
        }
        cv->events_[SIGNAL] = CreateEvent(NULL, FALSE, FALSE, NULL);
        cv->events_[BROADCAST] = CreateEvent(NULL, TRUE, FALSE, NULL);
#else
        int         result;
        HTcond   cv = NULL;

        cv = (HTcond)malloc(sizeof(struct ht_cond_t));
        if(cv == NULL)
        {
            return ENOMEM;
        }

        result = pthread_cond_init((pthread_cond_t *)&cv->cond, NULL);
        if(result != 0)
        {
            free(cv);
            return result;
        }
        result = pthread_mutex_init((pthread_mutex_t *)&cv->mutex, NULL);
        if(result != 0)
        {
            (void)pthread_cond_destroy((pthread_cond_t *)&cv->cond);
            free(cv);
            return result;
        }
#endif
        *cond = cv;
    }
    return 0;
}

HL_EXP int HL_APIENTRY htCondWait(HTcond *cond, int timeout)
{
    if(cond == NULL)
    {
		return EINVAL;
    }
    if(*cond == NULL)
    {
		return EINVAL;
    }
    if(timeout <= 0)
    {
        HTcond cv = *cond;

#ifdef HT_WIN_THREADS
        DWORD result;

        result = WaitForMultipleObjects (2,cv->events_, FALSE, INFINITE);
        if(result == WAIT_FAILED)
        {
            return EINVAL;
        }
#else
        int result = 0;

        (void)pthread_mutex_lock((pthread_mutex_t *)&cv->mutex);
        result = pthread_cond_wait((pthread_cond_t *)&cv->cond, (pthread_mutex_t *)&cv->mutex);
        if(result != 0)
        {
            return result;
        }
        (void)pthread_mutex_unlock((pthread_mutex_t *)&cv->mutex);
#endif
    }
    else
    {
        HTcond cv = *cond;

#ifdef HT_WIN_THREADS
        DWORD result;

        result = WaitForMultipleObjects (2, cv->events_, FALSE, (DWORD)timeout);
        if(result == WAIT_FAILED)
        {
            return EINVAL;
        }
        else if(result == (DWORD)WAIT_TIMEOUT)
        {
            return (int)ETIMEDOUT;
        }
    }
#else
        int                 result = 0;
        struct timespec     tv;
        struct timeb        t;
        long                ms;

        /* convert timeout to an absolute time */
        ftime(&t);
        ms = t.millitm + timeout;
        tv.tv_sec = t.time + (ms / 1000);
        tv.tv_nsec = (ms % 1000) * 1000000;

        (void)pthread_mutex_lock((pthread_mutex_t *)&cv->mutex);
        result = pthread_cond_timedwait((pthread_cond_t *)&cv->cond,
                                            (pthread_mutex_t *)&cv->mutex, &tv);
        if(result == ETIMEDOUT)
        {
            (void)pthread_mutex_unlock((pthread_mutex_t *)&cv->mutex);
            return ETIMEDOUT;
        }
        else if(result != 0)
        {
            return result;
        }
        (void)pthread_mutex_unlock((pthread_mutex_t *)&cv->mutex);
    }
#endif
    return 0;
}

HL_EXP int HL_APIENTRY htCondSignal(HTcond *cond)
{
    HTcond cv;

    if(cond == NULL)
    {
		return EINVAL;
    }
    if(*cond == NULL)
    {
		return EINVAL;
    }
    else
    {
#ifdef HT_WIN_THREADS
        cv = *cond;
        if(PulseEvent(cv->events_[SIGNAL]) == 0)
        {
            return EINVAL;
        }
#else
        int result;

        cv = *cond;
        result = pthread_cond_signal((pthread_cond_t *)&cv->cond);
        if(result != 0)
        {
            return result;
        }
#endif
    }
    return 0;
}

HL_EXP int HL_APIENTRY htCondBroadcast(HTcond *cond)
{
    if(cond == NULL)
    {
		return EINVAL;
    }
    if(*cond == NULL)
    {
		return EINVAL;
    }
    else
    {
        HTcond  cv = *cond;

#ifdef HT_WIN_THREADS
        if(PulseEvent(cv->events_[BROADCAST]) == 0)
        {
            return EINVAL;
        }
        return 0;
#else
        return pthread_cond_broadcast((pthread_cond_t *)&cv->cond);
#endif
    }
}

HL_EXP int HL_APIENTRY htCondDestroy(HTcond *cond)
{
    if(cond == NULL)
    {
		return EINVAL;
    }
    if(*cond == NULL)
    {
		return EINVAL;
    }
    else
    {
        HTcond cv = *cond;
        int result = 0;

#ifdef HT_WIN_THREADS
        (void)CloseHandle(cv->events_[SIGNAL]);
        (void)CloseHandle(cv->events_[BROADCAST]);
#else
        result = pthread_cond_destroy((pthread_cond_t *)&cv->cond);
#endif
        free(*cond);
        *cond = NULL;
        return result;
    }
}


