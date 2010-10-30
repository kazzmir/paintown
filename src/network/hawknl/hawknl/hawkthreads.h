/*
  HawkThreads cross platform network library
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

#ifndef HAWKTHREADS_H
#define HAWKTHREADS_H

#include "hawklib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HT_VERSION_STRING "HawkThreads 1.0"

typedef void *(*HThreadFunc)(void *data);
typedef void *HThreadID;
typedef struct ht_mutex_t *HTmutex;
typedef struct ht_cond_t *HTcond;

#define HT_INVALID              (-1)
#define HT_FALSE                (0)
#define HT_TRUE                 (1)

/*

  Multithreading API

*/

/* returns HThreadID on success, or NULL on error */
HL_EXP /*@null@*/ HThreadID HL_APIENTRY htThreadCreate(HThreadFunc func, void *data, int joinable);

HL_EXP void      HL_APIENTRY htThreadYield(void);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htThreadJoin(HThreadID threadID, /*@out@*/ void **status);

HL_EXP void      HL_APIENTRY htThreadSleep(int mseconds);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htMutexInit(/*@out@*/ HTmutex *mutex);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htMutexLock(HTmutex *mutex);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htMutexUnlock(HTmutex *mutex);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htMutexDestroy(/*@null@*/ HTmutex *mutex);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htCondInit(/*@out@*/ HTcond *cond);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htCondWait(HTcond *cond, int timeout);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htCondSignal(HTcond *cond);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htCondBroadcast(HTcond *cond);

/* returns 0 on success, or an error number */
HL_EXP int       HL_APIENTRY htCondDestroy(/*@null@*/ HTcond *cond);


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* HAWKTHREADS_H */

