/*
 * sffextract.h
 * ============
 *
 * This file defines the SFF file format according to "formats.txt" which comes
 * along with the Mugen development tools.
 *
 * This file also defines some functions in order to extract data from a SFF
 * file.
 *
 * The SFF structure is rather simple : it begins with a header which contains
 * some general information (signature, number of images, ...), and then there
 * is a sequence of sprites. Sprites are composed of a header which defines its
 * characteristics, and then image data stored as PCX data. If the sprite
 * contains no data (data length is zero), then it is considered to be a linked
 * sprite, which means its data is the same as another image which is indicated
 * in the sprite header.
 *
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
 *
 * Contributors:
 * _ Winane (important bugfixes and enhancements)
 * _ [E] (hexadecimal output option)
 * _ Dark Saviour (Borland compatibility)
 */

#ifndef __SFFEXTRACT_H
#define __SFFEXTRACT_H

#include <stdio.h>
#include "defs.h"

#define SPRPALTYPE_INDIV  0
#define SPRPALTYPE_SHARED 1

#define SFF_SIGNATURE "ElecbyteSpr"


/* -------------------------------------------------------------------------- */
/* Types'definitions */

/* HEADER */
typedef struct {
    char signature [12];      /* "ElecbyteSpr\0" signature */
    byte verhi;
    byte verlo;
    byte verlo2;
    byte verlo3;
    unsigned long nb_groups;  /* Number of groups */
    unsigned long nb_imgs;    /* Number of images */
    unsigned long sub_offset; /* File offset where first subfile is located */
    unsigned long sub_size;   /* Size of subheader in bytes */
    char palette_type;        /* 1 = SPRPALTPYE_SHARED or
                                 0 = SPRPALTYPE_INDIV */
    char blank [3];           /* Blank, set to zero */
    char comments [476];      /* Blank, can be used for comments */
} sff_header;


/* SUBFILEHEADER */
typedef struct {
    unsigned long next_offset; /* File offset where next subfile in the "linked
                                  list" is located. Null if last subfile. */
    unsigned long length;      /* Subfile length (not including header).
                                  Length is zero if it is a linked sprite. */
    int16 xcoord;        /* Image axis X coordinate */
    int16 ycoord;        /* Image axis Y coordinate */
    unsigned short group;         /* Group number */
    unsigned short img;           /* Image number (in the group) */
    unsigned short prev; /* Index of previous copy of sprite (linked
                            sprites only) */
    byte same_pal;       /* True if palette is same as previous images. */
    char comments [13];  /* Blank, can be used for comments.
                            !! formats.txt says it's 14 bytes but it is
                            13 bytes in order to have a 32-bytes long
                            subheader !! */
} sff_sub;


/* PCX HEADER */
typedef struct {
    unsigned char manufacturer; /* Constant Flag, 10 = ZSoft .pcx */
    unsigned char version;      /* Version information */
    unsigned char encoding;     /* 1 = .PCX run length encoding */
    unsigned char BitsPerPixel; /* Number of bits to represent a pixel */
    short Xmin;                 /* Image Dimensions */
    short Ymin;
    short Xmax;
    short Ymax;
    unsigned short HDpi;         /* Horizontal Resolution of image in DPI */
    unsigned short VDpi;         /* Vertical Resolution of image in DPI */
    unsigned char colormap[48];  /* Color palette setting */
    unsigned char reserved;      /* Should be set to 0. */
    unsigned char NPlanes;       /* Number of color planes */
    unsigned short BytesPerLine; /* Number of bytes to allocate for a scanline plane. */
    unsigned short PaletteInfo;  /* How to interpret palette: 1 = Color/BW,
                                    2 = Grayscale (ignored in PB IV/ IV +) */
    unsigned short HscreenSize;  /* Horizontal screen size in pixels. */
    unsigned short VscreenSize;  /* Vertical screen size in pixels. */
    char filler[54];             /* Blank to fill out 128 byte header. */
} pcx_header;
/* Source: ZSoft Corporation's PCX File Format Technical Reference Manual, Revision 5. */


/* -------------------------------------------------------------------------- */
/* Functions'prototypes */

/* Program behaviour */

/* new , by [E] 13/08/2003 */
void set_hex_output( char value );
/* ends, by [E] 13/08/2003 */
void set_short_output( char value );
void set_display_header( char value );
void set_display_options( char value );
void set_display_contents( char value );
void set_display_outfile( char value );
void set_force( char value );
void set_create_pcx( char value );
void set_debug( char value );
void set_subdirectory( char value );
void set_absolute_paths( char value );
void set_invert_shared( char value );
void set_linux_kyara( char value );
void set_output_file( FILE * file );
void set_error_file( FILE * file );
/*void set_output_dir( char * dir );*/
void set_act_file( char * act );
void set_use_act( char value );
/*void set_no_overwrite( char value );*/

/* ------------------------------------------------------------------------- */
/* Extracts header from "sff_file" into "header".
 * Returns 1 on failure, 0 on success.
 */
int extract_header( FILE * sff_file, sff_header * header );

/* Does the extraction job */
int extract( char * filename );



#endif /* __SFFEXTRACT_H */
