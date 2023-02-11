/*
  HawkThreads cross platform thread library
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

#ifndef HTINTERNAL_H
#define HTINTERNAL_H

// #include "hawkthreads.h"

/* for malloc and free */
#include <stdlib.h>
#if !defined(MACOSX) && !defined(__FreeBSD__)
#include <malloc.h>
#endif
#ifdef HL_WINDOWS_APP
/* Windows systems */
#ifdef _MSC_VER
#pragma warning (disable:4201)
#pragma warning (disable:4214)
#endif /* _MSC_VER */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>

#ifdef _MSC_VER
#pragma warning (default:4201)
#pragma warning (default:4214)
#endif /* _MSC_VER */

#define HT_WIN_THREADS
#define ETIMEDOUT		WSAETIMEDOUT

#endif /* HL_WINDOWS_APP */

/* Windows CE has no errno.h file */
#if defined (_WIN32_WCE)
#define EPERM           1
#define ENOMEM          12
#define EINVAL          22
#define EDEADLK         36
#else
#include <errno.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* HTINTERNAL_H */
