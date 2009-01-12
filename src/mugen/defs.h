/*
 * defs.h
 * ======
 *
 * This file holds common definitions.
 *
 * Copyright (C) 2003, 2004, 2005 Osuna Richert Christophe
 *
 *  This file is part of Sffextract.
 *
 *  Sffextract is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
*/

#ifndef __DEFS_H
#define __DEFS_H

#define byte  unsigned char
#define int16 short int

#define FALSE 0
#define TRUE  1

#if defined(__MSDOS) || defined(_WIN32)
#define DIR_SEPARATOR '\\'
#elif defined(_MAC)
/* Not at all sure about this: */
#define DIR_SEPARATOR ':'
#else
#define DIR_SEPARATOR '/'
#endif

#if !defined(_MSC_VER) && !defined(__BORLANDC__)
 /* Maximum filename length */
 /* Not very nice code, but it will do for now... */
 /* These values should probably be decreased when compiling for old Microsoft
    systems lacking long filename support.
    These values are unnecessarily limiting for Linux, but let's keep things
    consistent between systems as much as possible. */
  #ifndef _MAX_DRIVE
  #define _MAX_DRIVE 3
  #endif

  #ifndef _MAX_DIR
  #define _MAX_DIR 256
  #endif

  #ifndef _MAX_FNAME
  #define _MAX_FNAME 256
  #endif

  #ifndef _MAX_EXT
  #define _MAX_EXT 256
  #endif
#endif

#endif /* __DEFS_H */
