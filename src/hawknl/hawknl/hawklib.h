/*
  HawkLib.h version 1.0
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

#ifndef HAWKLIB_H
#define HAWKLIB_H

#if defined (WIN32) || defined (WIN64) || defined (_WIN32_WCE)
#define HL_WINDOWS_APP
#endif

#ifdef HL_WINDOWS_APP
  #ifdef _MSC_VER
    #pragma warning (disable:4514) /* disable "unreferenced inline function has
                                    been removed" warning */
  #endif /* _MSC_VER */
  /* The default build for Windows is as a DLL. */
  /* If you want a static library, define WIN_STATIC_LIB. */
  #ifdef WIN_STATIC_LIB
    #define HL_EXP
  #else
    #if defined (__LCC__)
     #define HL_EXP extern
    #else
     #define HL_EXP __declspec(dllexport)
    #endif /* __LCC__ */
  #endif /* WIN_STATIC_LIB */
  #define HL_APIENTRY __stdcall
  #define HL_CALLBACK __cdecl
  #ifdef __GNUC__
    #define HL_INLINE extern __inline__
  #else
    #define HL_INLINE __inline
  #endif /* __GNUC__ */
#else /* !HL_WINDOWS_APP */
  #define HL_EXP extern
  #define HL_APIENTRY
  #define HL_CALLBACK
  #ifdef __GNUC__
    #define HL_INLINE extern __inline__
  #else
    #define HL_INLINE inline /* assuming C99 compliant compiler */
  #endif /* __GNUC__ */
#endif /* !HL_WINDOWS_APP */


#endif /* HAWKLIB_H */

