#ifndef GME_TYPES_H
#define GME_TYPES_H

/* CMake will either define the following to 1, or #undef it,
 * depending on the options passed to CMake.  This is used to
 * conditionally compile in the various emulator types.
 *
 * See gme_type_list() in gme.cpp
 */

#define USE_GME_AY 1
#define USE_GME_GBS 1
#define USE_GME_GYM 1
#define USE_GME_HES 1
#define USE_GME_KSS 1
#define USE_GME_NSF 1
#define USE_GME_NSFE 1
#define USE_GME_SAP 1
#define USE_GME_SPC 1
/* VGM and VGZ are a package deal */
#define USE_GME_VGM 1

#endif /* GME_TYPES_H */
