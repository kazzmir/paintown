/*
 * sffextract.c
 * ============
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#include <direct.h>
#elif defined(__BORLANDC__)  /* DS modification 1 */
#include <dir.h>
#else
#include <unistd.h>  /* ifdef __GCC__, __MWERKS__, maybe others */
#endif
#if defined(__MWERKS__)
#include <stat.h>
#include <extras.h>
#endif

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "defs.h"
#include "sffextract.h"


/*#if !defined(__MSDOS) && !defined(_WIN32) */
#if !defined(_MSC_VER) && !defined(__BORLANDC__)
#if !defined(__MWERKS__) || !defined(_WIN32)
/* Splits the input filename into a drive (ignored), a dir (ignored), a fname
   and an ext. */
/* Not robust code... */
/* Also only tries to replicate the aspects of Microsoft's version which are
   actually used by Sffextract.  That is, it doesn't store anything in drive
   nor dir, since Sffextract doesn't need those. */
void _splitpath (const char *filename, char *drive, char *dir, char *fname, char *ext)
{
    signed int length = strlen (filename);
    signed int fpos, flength;
    signed int epos = length;
    signed char ext_found = 0;

    if( length < 1 ) return;

    /* seeks the beginning of the extension */
    while( (epos >= 0) && !ext_found ){
        if(filename[epos] == '.')
            ext_found = 1;
        else if(filename[epos] == DIR_SEPARATOR)
            ext_found = -1;
        else epos--;
    }

    /* seeks the beginning of the filename */
    fpos = epos;
    if( ext_found == 1 )
        while( (fpos >= 0) && (filename[fpos] != DIR_SEPARATOR) )
            fpos--;
    /* fpos < 0 => fpos == -1 => filename starts at fpos=0
       filename[pos] == DIR_SEPARATOR => filename starts at fpos+1 */
    fpos++;

    if( ext_found == 1 ){
        flength = epos - fpos;
        strncpy( ext, filename+epos, _MAX_EXT );
        ext[_MAX_EXT-1] = '\0';
    }
    else flength = length - fpos;
    if( flength >= _MAX_FNAME ) return;
    strncpy( fname, filename+fpos, flength );
    fname[flength] = '\0';
}
#endif
#endif


/* ------------------------------------------------------------------------- */
/* Variables definition */

/* Some variables that control the extractor's behaviour */
char force = FALSE;
char create_pcx = FALSE;
char display_header = FALSE;
char display_options = TRUE;
char display_contents = FALSE;
char display_outfile = FALSE; /* Little note: This refers to Sprmaker's
                output SFF filename, not any of Sffextract's output files. */
char debug = FALSE;
char subdirectory = FALSE;
char absolute_paths = FALSE;
char use_act = FALSE;
/* new , by [E] 13/08/2003 */
char hex_output = FALSE;
/* ends, by [E] 13/08/2003 */
char short_output = FALSE;
char invert_shared = FALSE;
char linux_kyara = FALSE; /* Indicator of whether to treat the SFF as being 
                             for a character in-game in versions of Mugen
                             after 2001.04.14. (In case you're wondering,
                             kyara is the shortened romaji form of the
                             Japanese borrowed word for "character". ^^) */
FILE * of = NULL;
FILE * ef = NULL;
/*char output_dir[_MAX_DRIVE + _MAX_DIR] = "";*/
char act_path[FILENAME_MAX] = "\0";
/*char no_overwrite = FALSE;*/

/* Delimiter characters for strtok calls: */
char seps[] = "\a\b\n\v\f\r\x11\x12\x13\x14\x1a\x1b\x7f";


/* ------------------------------------------------------------------------- */
/* The functions corresponding to the variables above */
void set_display_header( char value ) { display_header = value; }
void set_display_options( char value ) { display_options = value; }
void set_display_contents( char value ) { display_contents = value; }
void set_display_outfile( char value ) { display_outfile = value; }
void set_force( char value ) { force = value; }
void set_create_pcx( char value ) { create_pcx = value; }
void set_debug( char value ) { debug = value; }
void set_subdirectory( char value ) { subdirectory = value; }
void set_absolute_paths( char value ) { absolute_paths = value; }
void set_output_file( FILE * f ) { of = f; }
void set_error_file( FILE * erf ) { ef = erf; }
/* new , by [E] 13/08/2003 */
void set_hex_output( char value ) { hex_output = value; }
/* ends, by [E] 13/08/2003 */
void set_short_output( char value ) { short_output = value; }
void set_invert_shared( char value ) { invert_shared = value; }
void set_linux_kyara( char value ) { linux_kyara = value; }
/*void set_output_dir( char * dir ) { strcpy( output_dir, dir ); }*/
void set_act_file( char * act ) { strcpy( act_path, act ); }
void set_use_act( char value ) { use_act = value; }
/*void set_no_overwrite( char value ) { no_overwrite = value; }*/


/* ------------------------------------------------------------------------- */
/* Extracts header from "sff_file" into "header".
 * Returns 1 on failure, 0 on success.
 */
int extract_header( FILE * sff_file, sff_header * header ) {
    char * token;
    char comment[477] = "\0"; /* Winane added this because the string needs to
                        be null-terminated, and direct access to the header
                        doesn't guarantee a null character. But perhaps there's
                        a better solution.*/

    /* Reads header info */
    if( !fread( header, sizeof(sff_header), 1, sff_file ) ){
        fprintf( ef, "Error: Couldn't read complete header.\n" );
        return 1;
    }

    /* Checks if the file signature is OK */
    if( strcmp( header -> signature, SFF_SIGNATURE ) ) {
        if( !force ) {
            fprintf( ef, "Error: Specified file is not a SFF file.\n" );
            fclose(sff_file);
            return 1;
        }
        fprintf( ef, "Error: Specified file is not a SFF file.\nAttempting to go on with extraction anyway.\n" );
    }

    /* Display header */
    if( display_header ) {
    fprintf( of, "; %s %d.%d.%d.%d\n; %li groups\n; %li images\n",
        header -> signature, header -> verhi, header -> verlo,
        header -> verlo2, header -> verlo3, header -> nb_groups,
        header -> nb_imgs );

    strncpy(comment, header->comments, 476);
    token = strtok( comment, seps );
    if(token != NULL) fprintf( of, "; %s\n", token );
    while( (token = strtok(NULL, seps)) != NULL ){
        fprintf( of, "; %s\n", token );
        }
    }

    if( !header->nb_groups || (header->nb_groups > 65536) || (header->nb_groups > header->nb_imgs) )
        fprintf( ef, "Warning: Invalid number of groups specified in header. Ignoring.\n" );
                   /* Both M.C.M. and Z-CharCAD are frequently guilty of causing this.
                      Doesn't really affect anything, though. */

    if( !header->nb_imgs || header->nb_imgs > 134217683){ /* 2147483647 is impossible */
        fprintf( ef, "Error: Invalid number of images specified in header.\n" );
        return 1;
    }

    if( header->sub_size !=32 )
        fprintf( ef,
         "Warning: Nonstandard subheader size specified in header. Ignoring.\n" );
         /* It's probably just M.C.M.'s fault.  Mugen ignores it, so we'll just ignore it too.*/


    if( (header->palette_type != SPRPALTYPE_INDIV) && (header->palette_type != SPRPALTYPE_SHARED) ){
        /*if( force )*/
            fprintf( ef, "Warning: Unknown palette type: %d. Treating as if individual type.\n",
                header->palette_type );
          /* If it doesn't abort Mugen, perhaps it shouldn't abort Sffextract either. */
        /*else{
            fprintf( ef, "Error: Unknown palette type: %d.\n",
                 header -> palette_type );
            fclose(sff_file);
            return 1;
        }*/
    }

    /* Display palette option */
    if( display_options ) {
        if( header -> palette_type == invert_shared )
            fprintf( of, ";\n; Options\n#\n; Palette\n1\n; Individual palette\n1\n" );
        else if( header -> palette_type == !invert_shared )
            fprintf( of, ";\n; Options\n#\n; Palette\n1\n; Shared palette\n2\n" );
        else if( force )
            fprintf( of, ";\n; Options\n; Palette\n; (Palette type had originally been specified as unknown type %d.)\n", header->palette_type );

        if( invert_shared )
            fprintf( of, "; (Palette type reversed from original.)\n" );

        fprintf( of, "; End of options\n;\n" );
    }

    return 0;
}



/* ------------------------------------------------------------------------- */
/* Does the extraction job */
int extract( char * filename ) {
    FILE *sff_file, *act_file;
    FILE *pcxfile, *linked_pcx;
    sff_header header;
    sff_sub sub, sub2;
    pcx_header pcxhead; /* Used by both the -a option code and by the PCX
                           extraction code. */
    char *token = NULL; /* For strtok function. */
    char *pcxdata = NULL;
    char *palselect = NULL;
    unsigned long *offset = NULL; /* offset table for reading images data */
    unsigned char *dupcheck = NULL; /* table for indicating duplicate group+image numbers */
    unsigned long *imglist = NULL; /* list of group+image numbers */
    char comment[14] = "\0"; /* Winane added this because the string needs to
                        be null-terminated, and direct access to the subheader
                        doesn't guarantee a null character. But perhaps there's
                        a better solution.*/
    char pcxname[FILENAME_MAX] = "\0";
    char linked_name[FILENAME_MAX] = "\0";
    unsigned char colorsave[3] = "\0"; /* For RGB palette entry storage. */
    unsigned char palsaveD[768] = "\0"; /* PCX 256-color palette (default one to use) */
    unsigned char palsave1[768] = "\0"; /* PCX 256-color palette of 1st image (Linux
                                           mode) or of specified palette file */
    char dirprefix[_MAX_DRIVE + _MAX_DIR - 13] = "\0"; /* current directory */
    char fname[_MAX_FNAME] = "\0"; /* filename without directory */
/*  char drive[_MAX_DRIVE] = "\0";
    char dir[_MAX_DIR] = "\0";
 */
    char ext[_MAX_EXT] = "\0";
    unsigned long i, j; /* "For" loop indices. */
    unsigned long sff_length; /* Length of SFF file. */
    unsigned long reallength; /* Because sub.length is unreliable. -Winane */
    unsigned long imgnum; /* For storing group+image number. */
    signed char is_8bitpal;
    signed char is_linked;
    char found_1st = 0; /* Indicator of whether we've found a 768-byte palette yet. */
    /* char file_exists = 0; // Not currently used, but might be later. */
    /* Also not currently used:
    unsigned char small_pal[768] = "\0"; // Palette of small portrait.
    unsigned char large_pal[768] = "\0"; // Palette of large portrait.
    unsigned long small_portrait = 0; // Index of first small portrait, if any.
    unsigned long large_portrait = 0; // Index of first large portrait, if any.
    signed char smallP_found = FALSE;
    signed char largeP_found = FALSE;
    */


    sff_file = fopen( filename, "rb" );
    if( !sff_file ) {
        fprintf( ef, "File %s does not exist.\n", filename );
        return 1;
    }
    fseek( sff_file, 0, SEEK_END );
    sff_length = ftell( sff_file );
    fseek( sff_file, 0, SEEK_SET );

    if(use_act){
        act_file = fopen( act_path, "rb" );
        if( !act_file ){
            set_use_act( FALSE );
            fprintf( ef, "Palette file %s does not exist.\n", act_path );
            return 1;
        }
        fseek( act_file, 0, SEEK_END );
        if( ftell(act_file) == 768 ){ /* then it must be an ACT file. */
            for( i=0; i<256; i++ ){
                fseek( act_file, -3*(i+1), SEEK_END );
                fread( colorsave, 3, 1, act_file );
                palsave1[3*i]=colorsave[0];
                palsave1[3*i+1]=colorsave[1];
                palsave1[3*i+2]=colorsave[2];
            }
            fprintf( ef, "Applying palette from ACT file: %s\n", act_path );
            fclose( act_file );
        }
        else if( ftell(act_file) < 897 ){ /* 128-byte header + 768-byte palette + 0x0C byte = minimum 8-bit PCX file size. */
            set_use_act( FALSE );
            fprintf( ef, "File %s is not a valid palette file.\n", act_path );
            return 1;
        }
        else{ /* we'll assume it's a PCX file, for now. */
            fseek( act_file, 0, SEEK_SET );
            fread( &pcxhead, sizeof(pcx_header), 1, act_file );
            /* Check to see if the PCX file uses an 8-bit palette. */
            if( (pcxhead.manufacturer == 10) && (pcxhead.version >= 5) && (pcxhead.BitsPerPixel == 8) && (pcxhead.NPlanes == 1) ){
                fseek( act_file, -769, SEEK_END);
                fread( colorsave, 1, 1, act_file ); /* No need to define another variable; colorsave will do just fine. */
                if( colorsave[0] == 12 ){
                    fseek( act_file, -768, SEEK_END );
                    fread( palsave1, 768, 1, act_file );
                    fprintf( ef, "Applying palette from PCX file: %s\n", act_path );
                    fclose( act_file );
                }
                else{
                    set_use_act( FALSE );
                    fprintf( ef, "File %s is not a valid palette file. (Must be ACT or 8-bit PCX.)\n", act_path );
                    return 1;
                }
            }
            /* Add support for JASC and RIFF palette files later... */
            /* Minimum JASC PAL size = 1,813 bytes (carriage returns are necessary). */
            else{
                set_use_act( FALSE );
                fprintf( ef, "File %s is not a valid palette file. (Must be ACT or 8-bit PCX.)\n", act_path );
                return 1;
            }
        }
    }


    /* Sets directory prefix for filenames */
    if( absolute_paths ) {
      if( !getcwd(dirprefix, _MAX_DRIVE + _MAX_DIR - 16) ) {
        fprintf( ef, "sffextract.c: fixme: not enough memory for dirprefix\n" );
        return 1;
      }
#if defined(__MSDOS) || defined(_WIN32)
#if !defined(_MSC_VER) && !defined(__MINGW32__) && !defined(__BORLANDC__)
      /* For some reason, DJGPP's getcwd uses / instead of \. */
      /* Ugly fix. Have a look at this later. */
      for (i=0 ; i<strlen(dirprefix) ; i++)
        if (dirprefix[i] == '/') dirprefix[i] = '\\';
#endif
#endif
      /* Append directories separator character, unless it's already there
         (as would be the case if the current directory is root). */
      if( dirprefix[strlen(dirprefix)-1] != DIR_SEPARATOR ){
          dirprefix[strlen(dirprefix)+1] = '\0';
          dirprefix[strlen(dirprefix)] = DIR_SEPARATOR;
      }
    }
    else {
      dirprefix[0] = '\0'; /* no prefix => current directory */
    }

#if defined(__MSDOS) || defined(_WIN32)
    for (i=0 ; i<strlen(filename) ; i++)
        if (filename[i] == '/') filename[i] = '\\';
#endif

/*  _splitpath( filename, drive, dir, fname, ext ); */
    _splitpath( filename, NULL, NULL, fname, ext );
    if( fname[0] == '\0' ){
        fprintf( ef, "Error splitting SFF file name from extension.\nEither the file name starts with a period, or the file name is too long.\n" );
        /* I haven't overlooked any other possible explanations for such an error, have I?  -Winane */
        return 1;
    }

    if( subdirectory ) {
    /* Create a subdirectory whose name is the same as the input file, but without
       the extension .sff */
      if( ext[0] == '\0' ){
          fprintf( ef, "Error: Sorry, the -d option is not designed to work with SFF files lacking a\nfile extension.  Please rename the file and try again.\n" );
          return 1;
      }
      if( strlen(dirprefix) + strlen(fname) > _MAX_DRIVE + _MAX_DIR - 16 ){
          fprintf( ef, "Error: Specified destination path is too long.\n" );
          return 1;
      }
      strcat( dirprefix, fname );

      /* create subdirectory, with drwxr-xr-x permissions if the file system supports that. */
#if (defined(__MSDOS) || defined(_WIN32)) && !defined(__DJGPP__)
      if( (mkdir(dirprefix) == -1) && (errno != EEXIST) ) {
#else
      if( (mkdir(dirprefix,493) == -1) && (errno != EEXIST) ) {
#endif
        fprintf( ef, "Error: Unable to create %s directory.\n", dirprefix );
        return 1;
      }
      dirprefix[strlen(dirprefix)+1] = '\0';
      dirprefix[strlen(dirprefix)] = DIR_SEPARATOR;
    }

    if( debug ) fprintf( ef, "Directory prefix : %s\n", dirprefix );

    /* Extracts header */
    if( extract_header(sff_file, &header) ) {
        fprintf( ef, "Error while extracting header. Aborting.\n" );
        return 1;
    }
    if( header.palette_type && header.palette_type != 1 ) header.palette_type = 0;

    /* Displays output SFF file, same name as input SFF file */
    if( display_outfile ) {
    fprintf( of, "; Output file\n%s%s.sff\n", dirprefix, fname );
    }

    /* Seeks on first subheader */
    if( header.sub_offset < 512 || header.sub_offset > 2147482717 ){
        if( !force ){
            fprintf( ef, "Error: Header specifies invalid initial offset. Aborting.\n" );
            return 1;
        }
        fprintf( ef, "Error: Header specifies invalid initial offset.\nUsing default offset instead.\n" );
        sub.next_offset = 512;
    }
    else sub.next_offset = header.sub_offset;

    /* Creates offset table */
    offset = (unsigned long *) malloc( (header.nb_imgs+1)*sizeof(unsigned long) );
    if( offset == NULL){
        fprintf( ef, "Error: Can't allocate memory for offset table.\n" );
        return 1;
    }

    /* Create table for checking whether linked images are linked to duplicately numbered images. */
    dupcheck = (unsigned char *) calloc( (header.nb_imgs+1), sizeof(char) );
    if( dupcheck == NULL){
        fprintf( ef, "Error: Can't allocate memory for duplicity table.\n" );
        free( offset );
        return 1;
    }

    /* Create table for concatenated group+image numbers. */
    imglist = (unsigned long *) malloc( (header.nb_imgs+1)*sizeof(unsigned long) );
    if( imglist == NULL){
        fprintf( ef, "Error: Can't allocate memory for image number table.\n" );
        free( offset );
        free( dupcheck );
        return 1;
    }

    /* Create table for new-Mugen mode palette selection. */
    palselect = (char *) calloc( (header.nb_imgs+1), sizeof(char) );
    if( palselect == NULL){
        fprintf( ef, "Error: Can't allocate memory for palette selection table.\n" );
        free( offset );
        free( dupcheck );
        free( imglist );
        return 1;
    }






    if( linux_kyara ){

      /* Values in the final palselect table have these meanings:
         0 = keep own palette
         1 = use previous palette
         2 = use first palette or specified ACT file
      */
      for( i = 0 ; i < header.nb_imgs ; i++ ) {
        /* get subfile header */
        offset[i] = sub.next_offset;
        if( sub.next_offset > sff_length ) break;
        fseek( sff_file, sub.next_offset, SEEK_SET );
        if( !fread( &sub, sizeof(sff_sub), 1, sff_file ) ) break;


        if( sub.same_pal ){
            if( i > 0 && palselect[i-1] == 2 ) palselect[i] = 2;
            else palselect[i] = 1;
        }

        if( (sub.group == 0 || sub.group == 9000) && sub.img == 0 ){
            if( sub.same_pal ){
                for( j = i; j > 0 && palselect[j] == 1; j-- ){
                    palselect[j] = 2;
                    if( palselect[j-1] == 0 ) palselect[j-1] = 2;
                }
            }
            else palselect[i] = 2;
        }

        /* Not currently used:
        if( sub.group == 9000 ){
            if( sub.img == 0 && !smallP_found ){
                small_portrait = i;
                smallP_found = 1;
                if( sub.length > 0 ){
                    // Read in palette data.
                }
            }
            if( sub.img == 1 && !largeP_found ){
                large_portrait = i;
                largeP_found = 1;
                if( sub.length > 0 ){
                    // Read in palette data.
                }
            }
        }
        */

        if( sub.next_offset == 0 ){ /* Just in case any SFFs actually conform to formats.txt. */
          break;
        }
        else if( sub.next_offset < (offset[i]+32) || sub.next_offset > 2147483615 ){
          break;
        }
      }

      /* Seek back to first subheader */
      if( header.sub_offset < 512 || header.sub_offset > 2147482717 ){
          sub.next_offset = 512;
      }
      else sub.next_offset = header.sub_offset;

    }






    /* Read and display file contents, except image data which is saved into a
       file */
    for( i = 0 ; i < header.nb_imgs ; i++ ) {
        /* get subfile header */
        offset[i] = sub.next_offset;
        if( sub.next_offset > sff_length ){
            fprintf( ef,
                "Error: Offset of image %lu is beyond the end of the file.\nSFF file appears to be truncated. Aborting.\n", i );
            return 1;
        }
        fseek( sff_file, sub.next_offset, SEEK_SET );
        if( !fread( &sub, sizeof(sff_sub), 1, sff_file ) ){
            fprintf( ef, "Error: SFF file seems to be truncated. Aborting.\n" );
            return 1;
        }

        /* modified to allow hexadecimal output, by [E] 13/08/2003 */
        if( hex_output )
            sprintf( pcxname, "%s%0*X%0*X.pcx", dirprefix, 4, sub.group, 4, sub.img );
        /* (and to allow selection of decimal output width, by Winane 16/08/2003) */
        else if( short_output )
            sprintf( pcxname, "%s%0*d%0*d.pcx", dirprefix, 4, sub.group, 4, sub.img );
        else
            sprintf( pcxname, "%s%0*d%0*d.pcx", dirprefix, 5, sub.group, 5, sub.img );
        /* ends, by [E] 13/08/2003 */

        /* This should be a more reliable way to determine the length of the PCX data. -Winane */
        reallength = sub.next_offset-offset[i]-32;

        /* This might often not be sufficient, but is it necessary?
           Actual SFFs seem to disagree with formats.txt. -Winane */
        if( sub.next_offset == 0 ) {
            if( sub.same_pal ) reallength = sub.length-768;
            else reallength = sub.length;
        }



        /* Check to see if previous image had same group + image number. */
        /* This part is nice and simple, but makes the complexity of the program O(n^2).
           We'd best implement a better algorithm in a future version,
           in case any SFFs have very large numbers of images. */

        imgnum = sub.group;
        imgnum = ( imgnum << 16 ) + sub.img;
        imglist[i] = imgnum;

        for( j = 0; j < i; j++ ){
            if( imglist[j] == imgnum ){
                if( dupcheck[i] < 255 ) ++dupcheck[i];
            }
        }

        if( dupcheck[i] ){
            sprintf( pcxname, "%sx%05lu.pcx", dirprefix, i );

            if( !create_pcx ){
                if( sub.group == 9000 && (sub.img == 0 || sub.img == 1) )
                    fprintf( ef, "Warning:" );
                else fprintf( ef, "Notice:" );
                fprintf( ef, " Extra image %d,%d named as: %s\n", sub.group, sub.img, pcxname+strlen(dirprefix) );
            }
        }





        if( sub.length == 0 ){
          is_linked = 1;
          if( sub.prev >= i ){
            if( !force ){
                fprintf( ef, "Error in image %lu (%d,%d): Invalid linked sprite index. Aborting.\n", i, sub.group, sub.img );
                return 1;
            }
            fprintf( ef, "Error: Invalid linked sprite index. Skipping image %lu (%d,%d).\n", i, sub.group, sub.img );
            continue;
          }
          if( sub.prev > 32767 ){
            if( !force ){
                fprintf( ef, "Error: Linked sprite index too large in image %lu: %d,%d. Aborting.\n", i, sub.group, sub.img );
                return 1;
            }
            fprintf( ef, "Error: Linked sprite index too large in image %lu: %d,%d. Ignoring error.\n", i, sub.group, sub.img );
          }

          if( debug ) fprintf( ef, "Image %lu ", i );

          while( sub.length == 0 ) {
            if( debug ) fprintf( ef, "-> %d ", sub.prev );

            fseek( sff_file, offset[sub.prev], SEEK_SET );
            fread( &sub2, sizeof(sff_sub), 1, sff_file );
            sub.length = sub2.next_offset-offset[sub.prev]-32;
            if( (sub.prev <= sub2.prev) && ((sub.prev != 0) || (i == 0)) && sub2.length==0 ) {
                if( !force ) {
                    fprintf( ef, "Image %lu (%d,%d) : circular definition or forward linking. Aborting.\n", i, sub.group, sub.img );
                    return 1;
                }
                fprintf( ef, "Circular definition or forward linking error. Skipping image %lu (%d,%d).\n", i, sub.group, sub.img );
                is_linked = -1;
                break; /* Hmm, better take a closer look at this part later. */
            }
            else{
                if(sub.length == 0) sub.prev = sub2.prev;
                else reallength = sub2.next_offset-offset[sub.prev]-32;
            }
          }
          if( is_linked == -1 ) continue;
          else if( debug ) fprintf( ef, "\n" );



          /* Check whether image is linked to a duplicately numbered image: */
          if( dupcheck[sub.prev] ){
              sprintf( linked_name, "%sx%05hu.pcx", dirprefix, sub.prev );
#ifdef _DEBUG
          /* Temporary addition, to check whether errors would have occurred in pre-0.8 versions of Sffextract: */
          /* (Hm, can't decide whether to leave this enabled in the release.)  - Winane */
              fprintf( ef, "%lu (%d,%d) linked to %s\n", i, sub.group, sub.img, linked_name );
#endif
          }
          else if( hex_output )
              sprintf( linked_name, "%s%0*X%0*X.pcx", dirprefix, 4, sub2.group, 4, sub2.img );
          else if( short_output )
              sprintf( linked_name, "%s%0*d%0*d.pcx", dirprefix, 4, sub2.group, 4, sub2.img );
          else
              sprintf( linked_name, "%s%0*d%0*d.pcx", dirprefix, 5, sub2.group, 5, sub2.img );

          if( sub.group == 9000 && (sub.img == 0 || sub.img == 1) )
              fprintf( ef, "Warning: Image %d,%d (at index %lu) is a linked sprite.\n", sub.group, sub.img, i );
        }
        else is_linked = 0;





        /* Read in PCX header data. */
        j = fread( &pcxhead, 1, sizeof(pcx_header), sff_file ); /* May as well re-use j for this. */
        fseek( sff_file, 0-j, SEEK_CUR );

        /* Get image data. */
        pcxdata = (char *) realloc( pcxdata, reallength );
        if( pcxdata == NULL ){
            fprintf( ef, "Error: Can't reallocate memory for PCX data, or PCX data is zero-length. Aborting.\n" );
            free( offset );
            free( dupcheck );
            free( imglist );
            free( palselect );
            return 1;
        }
        if( !fread( pcxdata, reallength, 1, sff_file ) ){
            fprintf( ef, "Error: Couldn't read entire PCX data of image %lu.\nSFF file seems to be truncated. Aborting.\n", i );
            return 1;
        }

        /* Check to see if the PCX file uses an 8-bit palette. */
        if( (reallength < 129) || (pcxhead.version < 5) || (pcxhead.BitsPerPixel != 8) || (pcxhead.NPlanes != 1) )
            is_8bitpal = 0;
        else if( (!is_linked && !sub.same_pal) || (is_linked && !sub2.same_pal) ){
            if( (reallength < 897) || *(pcxdata+reallength-769) != 12 ){
              /*
                If a palettized 8-bit image's subheader claims it's got an individual
                palette, but in fact its palette was removed, then Mugen will just use
                the next 768 bytes in the SFF file instead (filling in with 0xFF if the
                image is at the end of the file (only tested once)), if not a character
                in DOS Mugen.  Emulating that interpretation isn't entirely desirable,
                so let's just give such images the first or user-specified palette
                instead, as I assume would happen if it's a DOS character (too lazy to
                verify =P ).  We'll indicate such images with is_8bitpal = 2.  -Winane
              */
                if( *(pcxdata+reallength-1) != 12 ) is_8bitpal = 0;
                else is_8bitpal = 2;
            }
            else is_8bitpal = 1;
        }
        else if( *(pcxdata+reallength-1) != 12 ){
            if( (reallength < 897) || *(pcxdata+reallength-769) != 12 )
                is_8bitpal = 0;
            else is_8bitpal = -1; /* Means that the subheader indicates shared palette,
                                     but the palette apparently wasn't removed.
                                     Probably caused by CharSffDtoW, or Sprmaker if i==0. */
        }
        else is_8bitpal = 1;

        /* The language here isn't entirely accurate when the image being processed is a linked image.
           But we'll worry about that later, if ever.  Doesn't seem too significant to me (Winane).*/
        if( !is_8bitpal )
            fprintf( ef, "Notice: Image %lu (%d,%d) is not in 256-color PCX format: %s\n", i, sub.group, sub.img, pcxname+strlen(dirprefix) );
                       /* Assuming there's no other way for a PCX file to be 256-color.  I'm not 100% sure about that.  -Winane */

        else if( is_8bitpal==-1 && (!is_linked || sub.same_pal) ){
            fprintf( ef, "Notice: Image %lu (%d,%d) kept own palette despite subheader: %s\n", i, sub.group, sub.img, pcxname+strlen(dirprefix) );

            if( !(!is_linked && sub.group==9000 && sub.img==1) && !(is_linked && sub2.group==9000 && sub2.group==1) ){
                if( use_act && (!linux_kyara || palselect[i]==2) ){
                    fprintf( ef, "But sffextract will now replace its palette with the specified ACT palette.\n" );
                }
                else if( create_pcx && palselect[i]==2 && i!=0 ){
                    fprintf( ef, "But sffextract will now replace its palette with the 1st palette in the SFF.\n" );
                }
            }
        }

        else if( is_8bitpal==2 && !(is_linked && sub.same_pal) ){
            fprintf( ef, "Notice: Image %lu (%d,%d) has no palette despite subheader: %s\n", i, sub.group, sub.img, pcxname+strlen(dirprefix) );
            if( use_act )
                fprintf( ef, "Sffextract will now replace its missing palette with the palette you specified.\n" );
            else if( create_pcx ){
                if( found_1st )
                    fprintf( ef, "Sffextract will now give it the first palette in the SFF file.\n" );
                else fprintf( ef, "Sffextract will have to give it a blank palette.\n" );
            }
        }





     if( create_pcx ) {

        if( is_linked == 1 ){

          linked_pcx = fopen( linked_name, "rb" );
          if( linked_pcx == NULL ) {
              if( !force ){
                fprintf( ef, "Error: Failed to open linked-to PCX file %s,\nlinked to from image %lu (%d,%d). Aborting.\n",
                    linked_name, i, sub.group, sub.img );
                return 1;
              }
              fprintf( ef, "Error: Failed to open linked-to PCX file %s\nSkipping image %lu (%d,%d).\n", linked_name, i, sub.group, sub.img );
          }
          else {
              /* Copy previous PCX file and save as the new one: */
              fseek( linked_pcx, 0, SEEK_END );
              reallength = ftell( linked_pcx );
              pcxdata = (char *) realloc( pcxdata, reallength );
              if( pcxdata == NULL ){
                  fprintf( ef, "Error: Can't reallocate memory for PCX data, or PCX data is zero-length. Aborting.\n" );
                  free( offset );
                  free( dupcheck );
                  free( imglist );
                  free( palselect );
                  return 1;
              }
              fseek( linked_pcx, 0, SEEK_SET );
              fread( pcxdata, reallength, 1, linked_pcx );
              fclose( linked_pcx );
              if( (pcxfile = fopen( pcxname, "wb" )) == NULL ){
                  fprintf( ef, "Error: Couldn't create PCX file %s for writing. Aborting.\n", pcxname );
                  return 1;
              }
              fwrite( pcxdata, reallength, 1, pcxfile );
              fclose( pcxfile );
          }
        }

        else {

          if( debug ) fprintf( ef, "Image %lu: ", i );

          /* Save image data to a file */
          if( (pcxfile = fopen( pcxname, "wb" )) == NULL ){
              fprintf( ef, "Error: Couldn't create PCX file %s for writing. Aborting.\n", pcxname );
              return 1;
          }
          /* Skip last 768 bytes if we're about to replace the palette. */
          if( is_8bitpal%2 && (!sub.same_pal || is_8bitpal==-1) && !(sub.group==9000 && sub.img==1) && ((use_act && !linux_kyara) || (palselect[i]==2 && i!=0)) ){
              fwrite( pcxdata, reallength-768, 1, pcxfile );
          }
          /* Otherwise, save the whole thing. */
          else fwrite( pcxdata, reallength, 1, pcxfile );

          /* (Yes, much of this code was written while using a 1600x1200 LCD screen.
             My apologies for not making it more readable on smaller displays.  -Winane) */


          /* I'll explain the logic used here later, if I'm feeling ambitious. =P  -Winane */
          if( is_8bitpal ){
              if( !found_1st && !use_act && is_8bitpal!=2 ){
                  memcpy( palsaveD, pcxdata+reallength-768, 768 );
                  memcpy( palsave1, palsaveD, 768 );
                  found_1st = 1;
                  if( debug ) fprintf( ef, "First palette. " );
              }

              else if( palselect[i]==2 || (use_act && !linux_kyara) || is_8bitpal==2 ){
                  if( !(sub.group==9000 && sub.img==1 && (!sub.same_pal || is_8bitpal==-1)) || is_8bitpal==2 )
                      fwrite( palsave1, 768, 1, pcxfile );
              }

              else if( palselect[i]==1 && found_1st ){
                  if( is_8bitpal==1 || (use_act && !(sub.group==9000 && sub.img==1)) )
                      fwrite( palsaveD, 768, 1, pcxfile );
              }

              else if( linux_kyara ){
                  if( is_8bitpal==1 || !found_1st ){
                      memcpy( palsaveD, pcxdata+reallength-768, 768 );
                      if( debug ){
                          if( !found_1st ) fprintf( ef, "First palette. " );
                          else fprintf( ef, "Individual palette. " );
                      }
                      found_1st = 1;
                  }
              }

              else if( !use_act && is_8bitpal==1 ){
                  if( sub.same_pal )
                      fwrite( palsaveD, 768, 1, pcxfile );

                  else if( header.palette_type == invert_shared ){
                      memcpy( palsaveD, pcxdata+reallength-768, 768 );
                      if( debug ) fprintf( ef, "Individual palette. " );
                  }
              }
          }

          fclose( pcxfile );
        }

        if( dupcheck[i] ){
            if( sub.group == 9000 && (sub.img == 0 || sub.img == 1) )
                fprintf( ef, "Warning:" );
            else fprintf( ef, "Notice:" );
            fprintf( ef, " Extra image %d,%d saved as: %s\n", sub.group, sub.img, pcxname+strlen(dirprefix) );
        }
        else if( debug ) fprintf( ef, "Saved as %s\n", pcxname+strlen(dirprefix) );

     }

    /* Output to stdout. */
     if( display_contents ) {

        /* comments (if any) */
        if( strcmp("", sub.comments) ) {
            strncpy(comment, sub.comments, 13);
            token = strtok( comment, seps );
            if(token != NULL) fprintf( of, "; %s\n", token );
            while( (token = strtok(NULL, seps)) != NULL ){
                fprintf( of, "; %s\n", token );
            }
        }

        /* individual palette while default is shared */
        if( header.palette_type == !invert_shared && sub.same_pal == FALSE && display_options )
            fprintf( of, "; Individual palette\n#\n1\n1\n" );
        /* shared palette while default is individual */
        if( header.palette_type == invert_shared && sub.same_pal == TRUE && display_options )
            fprintf( of, "; Shared palette\n#\n1\n2\n" );

        /* pcx filename, group, image & coords */
        fprintf( of, "%s\n%d\n%d\n%d\n%d\n", pcxname,
            sub.group, sub.img, sub.xcoord, sub.ycoord );
     }

     if( sub.next_offset == 0 ){ /* Just in case any SFFs actually conform to formats.txt. */
        if (i<header.nb_imgs-1)
            fprintf( ef, "Warning: Subheader indicates End Of File but not all images (as indicated by SFF header) were processed.\n" );
        break;
     }
     else if( sub.next_offset < (offset[i]+32) || sub.next_offset > 2147483615 ){
        fprintf( ef, "Error: Subheader of image %lu (%d,%d) specifies invalid next offset.\nAborting.\n", i, sub.group, sub.img );
        return 1;
     }
    }



    fclose( sff_file );
    free( offset );
    free( dupcheck );
    free( imglist );
    free( palselect );
    free( pcxdata );

    return 0;
}
