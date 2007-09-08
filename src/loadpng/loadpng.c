/* loadpng, Allegro wrapper routines for libpng
 * by Peter Wang (tjaden@users.sf.net).
 *
 * Because all I really did was copy and paste copy out of libpng's example.c,
 * the loadpng is being released under the same licence as libpng itself.
 *
 * COPYRIGHT NOTICE:
 *
 * The loadpng Library is supplied "AS IS".  The Contributing Authors
 * disclaim all warranties, expressed or implied, including, without
 * limitation, the warranties of merchantability and of fitness for any
 * purpose.  The Contributing Authors no liability for direct, indirect,
 * incidental, special, exemplary, or consequential damages, which may
 * result from the use of the loadpng Library, even if advised of the
 * possibility of such damage.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject
 * to the following restrictions:
 * 1. The origin of this source code must not be misrepresented.
 * 2. Altered versions must be plainly marked as such and must not be
 *    misrepresented as being the original source.
 * 3. This Copyright notice may not be removed or altered from any source or
 *    altered source distribution.
 *
 * The Contributing Authors specifically permit, without fee, and encourage
 * the use of this source code as a component to supporting the PNG file
 * format in commercial products.  If you use this source code in a product,
 * acknowledgment is not required but would be appreciated.
 */


#include <png.h>
#include <allegro.h>
#include <allegro/internal/aintern.h>
#include "loadpng.h"

/* We need internals _color_load_depth and _fixup_loaded_bitmap.  The
 * first can be replaced by the new get_color_depth() function which
 * is in Allegro 4.1 branch.  But it's not worth it to break 4.0
 * compatibility.
 */



double _png_screen_gamma = -1;
int _png_compression_level = Z_BEST_COMPRESSION;



/* get_gamma:
 *  Get screen gamma value one of three ways.
 */
static double get_gamma(void)
{
    char *gamma_str;

    if (_png_screen_gamma != -1)
	return _png_screen_gamma;

    /* Use the environment variable if available.
     * 2.2 is a good guess for PC monitors.
     * 1.1 is good for my laptop.
     */
    gamma_str = getenv("SCREEN_GAMMA");
    return (gamma_str) ? atof(gamma_str) : 2.2;
}



/*
 *----------------------------------------------------------------------
 *			Loading
 *----------------------------------------------------------------------
 */



/* read_data, write_data:
 *  Custom read and write functions to use Allegro packfile routines,
 *  rather than C streams (so we can read from datafiles!)
 */
static void read_data(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
    PACKFILE *f = png_get_io_ptr(png_ptr);
    if ((png_uint_32)pack_fread(data, length, f) != length)
	png_error(png_ptr, "read error (loadpng calling pack_fread)");
}

static void write_data(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
    PACKFILE *f = png_get_io_ptr(png_ptr);
    if ((png_uint_32)pack_fwrite(data, length, f) != length)
	png_error(png_ptr, "write error (loadpng calling pack_fwrite)");
}

/* Don't think Allegro has any problem with buffering
 * (rather, Allegro provides no way to flush packfiles).
 */
static void flush_data(png_structp png_ptr) { (void)png_ptr; }



/* check_if_png:
 *  Check if input file is really PNG format.
 */
#define PNG_BYTES_TO_CHECK 4

static int check_if_png(PACKFILE *fp)
{
    unsigned char buf[PNG_BYTES_TO_CHECK];

    if (pack_fread(buf, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
	return 0;

    return (png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK) == 0);
}

/* really_load_png:
 *  Worker routine, used by load_png and load_memory_png.
 */
static BITMAP *really_load_png(png_structp png_ptr, png_infop info_ptr, RGB *pal)
{
    BITMAP *bmp;
    PALETTE tmppal;
    png_uint_32 width, height, rowbytes;
    int bit_depth, color_type, interlace_type;
    double image_gamma, screen_gamma;
    int intent;
    int bpp, dest_bpp;
    int number_passes, pass;
    png_uint_32 y;

    /* The call to png_read_info() gives us all of the information from the
     * PNG file before the first IDAT (image data chunk).
     */
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		 &interlace_type, NULL, NULL);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
     * byte into separate bytes (useful for paletted and grayscale images).
     */
    png_set_packing(png_ptr);

    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
    if ((color_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8))
	png_set_expand(png_ptr);

    /* Adds a full alpha channel if there is transparency information
     * in a tRNS chunk. */
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	png_set_tRNS_to_alpha(png_ptr);

    /* Convert 16-bit to 8-bit. */
    if (bit_depth == 16)
	png_set_strip_16(png_ptr);

    /* Convert grayscale to RGB triplets */
    if ((color_type == PNG_COLOR_TYPE_GRAY) ||
	(color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
	png_set_gray_to_rgb(png_ptr);

    /* Tell libpng to handle the gamma conversion for us. */
    screen_gamma = get_gamma();

    if (png_get_sRGB(png_ptr, info_ptr, &intent)) {
	png_set_sRGB(png_ptr, info_ptr, intent);
    }
    else {
	if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
	    png_set_gamma(png_ptr, screen_gamma, image_gamma);
	else
	    png_set_gamma(png_ptr, screen_gamma, 0.50);
    }

    /* Even if the user doesn't supply space for a palette, we want
     * one for the load process.
     */
    if (!pal)
	pal = tmppal;

    /* Dither RGB files down to 8 bit palette or reduce palettes
     * to the number of colors available on your screen.
     */
    if (color_type & PNG_COLOR_MASK_COLOR) {
	int num_palette, i;
	png_colorp palette;

	if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
	    /* We don't actually dither, we just copy the palette. */
	    for (i = 0; ((i < num_palette) && (i < 256)); i++) {
		pal[i].r = palette[i].red >> 2;		/* 256 -> 64 */
		pal[i].g = palette[i].green >> 2;
		pal[i].b = palette[i].blue >> 2;
	    }

	    for (; i < 256; i++)
		pal[i].r = pal[i].g = pal[i].b = 0;
	}
    }
    else {
	generate_332_palette(pal);
    }

    /* Flip RGB pixels to BGR. */
    /*if (makecol24(255, 0, 0) > makecol24(0, 0, 255))
      png_set_bgr(png_ptr);*/

    /* Turn on interlace handling. */
    number_passes = png_set_interlace_handling(png_ptr);

    /* Call to gamma correct and add the background to the palette
     * and update info structure.
     */
    png_read_update_info(png_ptr, info_ptr);

    rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    /* Allocate the memory to hold the image using the fields of info_ptr. */
    if (color_type == PNG_COLOR_TYPE_GRAY)
	bpp = 24;
    else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	bpp = 32;
    else
	bpp = rowbytes * 8 / width;

    /* Allegro cannot handle less than 8 bpp. */
    if (bpp < 8)
	bpp = 8;

    dest_bpp = _color_load_depth(bpp, (bpp == 32));
    bmp = create_bitmap_ex(bpp, width, height);

    /* Read the image, one line at a line (easier to debug!) */
    for (pass = 0; pass < number_passes; pass++) {
	for (y = 0; y < height; y++)
	    png_read_rows(png_ptr, &bmp->line[y], NULL, 1);
    }

    /* Hack: I guess libpng and Allegro disagree on RGBA images.
     * Without this, sometimes things come out in BGR.  Using only
     * png_set_bgr() causes BGR images in *other* situations.  So we
     * manually decompose colours, then ask Allegro to rebuild them.
     * Hopefully this will be the end of it (crosses fingers).
     */
    if (bitmap_color_depth(bmp) == 32) {
	int x, y, c;

	for (y = 0; y < bmp->h; y++) {
	    for (x = 0; x < bmp->w; x++) {
		c = getpixel(bmp, x, y);
#ifdef ALLEGRO_BIG_ENDIAN
		putpixel(bmp, x, y, makeacol32((c>>24) & 0xff,
					       (c>>16) & 0xff,
					       (c>>8) & 0xff,
					       (c) & 0xff));
#else
		putpixel(bmp, x, y, makeacol32(c & 0xff,
					       (c>>8) & 0xff,
					       (c>>16) & 0xff,
					       (c>>24) & 0xff));
#endif
	    }
	}
    }

    /* Hack: libpng and Allegro sometimes disagree about this too.
     * It's probably a mistake of mine somewhere.
     */
    else if (bitmap_color_depth(bmp) == 24) {
	int x, y, c;

	for (y = 0; y < bmp->h; y++) {
	    for (x = 0; x < bmp->w; x++) {
		c = getpixel(bmp, x, y);
#ifdef ALLEGRO_BIG_ENDIAN
		putpixel(bmp, x, y, makecol24((c>>16) & 0xff,
					      (c>>8) & 0xff,
					      (c) & 0xff));
#else
		putpixel(bmp, x, y, makecol24(c & 0xff,
					      (c>>8) & 0xff,
					      (c>>16) & 0xff));
#endif
	    }
	}
    } else if ( bitmap_color_depth(bmp) == 16 ){
#ifdef ALLEGRO_BIG_ENDIAN
	int x, y, c;
	for (y = 0; y < bmp->h; y++) {
	    for (x = 0; x < bmp->w; x++) {
		c = getpixel(bmp, x, y);
		putpixel(bmp, x, y, makecol16( (c>>16) & 0xff,
					      (c>>8) & 0xff,
					      (c) & 0xff));
	    }
	}
#endif
    }

    if (dest_bpp != bpp)
	bmp = _fixup_loaded_bitmap(bmp, pal, dest_bpp);

    /* Read rest of file, and get additional chunks in info_ptr. */
    png_read_end(png_ptr, info_ptr);

    return bmp;
}



/* load_png:
 *  Load a PNG file from disk, doing colour coversion if required.
 */
BITMAP *load_png(AL_CONST char *filename, RGB *pal)
{
    PACKFILE *fp;
    BITMAP *bmp;
    png_structp png_ptr;
    png_infop info_ptr;

    fp = pack_fopen(filename, "r");
    if (!fp)
	return NULL;

    if (!check_if_png(fp)) {
	pack_fclose(fp);
	return NULL;
    }

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				     (void *)NULL, NULL, NULL);
    if (!png_ptr) {
	pack_fclose(fp);
	return NULL;
    }

    /* Allocate/initialize the memory for image information. */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
	png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	pack_fclose(fp);
	return NULL;
    }

    /* Set error handling if you are using the setjmp/longjmp method (this is
     * the normal method of doing things with libpng).  REQUIRED unless you
     * set up your own error handlers in the png_create_read_struct() earlier.
     */
    if (setjmp(png_ptr->jmpbuf)) {
	/* Free all of the memory associated with the png_ptr and info_ptr */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	pack_fclose(fp);
	/* If we get here, we had a problem reading the file */
	return NULL;
    }

    /* Use Allegro packfile routines. */
    png_set_read_fn(png_ptr, fp, (png_rw_ptr)read_data);

    /* We have already read some of the signature. */
    png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

    /* Really load the image now. */
    bmp = really_load_png(png_ptr, info_ptr, pal);

    /* Clean up after the read, and free any memory allocated. */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    /* Done. */
    pack_fclose(fp);

    return bmp;
}



/* read_data_memory:
 *  Custom reader function to read a PNG file from a memory buffer.
 */

typedef struct {
    AL_CONST void *buffer;
    png_uint_32 bufsize;
    png_uint_32 current_pos;
} MEMORY_READER_STATE;

static void read_data_memory(png_structp png_ptr, png_bytep data, png_uint_32 length)
{
    MEMORY_READER_STATE *f = png_get_io_ptr(png_ptr);

    if (length > (f->bufsize - f->current_pos))
	png_error(png_ptr, "read error in read_data_memory (loadpng)");

    memcpy(data, f->buffer + f->current_pos, length);
    f->current_pos += length;
}



/* check_if_png_memory:
 *  Check if input buffer is really PNG format.
 */
static int check_if_png_memory(AL_CONST void *buffer)
{
    return (png_sig_cmp((void *)buffer, (png_size_t)0, PNG_BYTES_TO_CHECK) == 0);
}



/* load_memory_png:
 *  Load a PNG file from memory, doing colour coversion if required.
 */
BITMAP *load_memory_png(AL_CONST void *buffer, int bufsize, RGB *pal)
{
    MEMORY_READER_STATE memory_reader_state;
    BITMAP *bmp;
    png_structp png_ptr;
    png_infop info_ptr;

    if (!buffer || (bufsize <= 0))
    	return NULL;

    if (!check_if_png_memory(buffer))
	return NULL;

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				     (void *)NULL, NULL, NULL);
    if (!png_ptr)
	return NULL;

    /* Allocate/initialize the memory for image information. */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
	png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	return NULL;
    }

    /* Set error handling if you are using the setjmp/longjmp method (this is
     * the normal method of doing things with libpng).  REQUIRED unless you
     * set up your own error handlers in the png_create_read_struct() earlier.
     */
    if (setjmp(png_ptr->jmpbuf)) {
	/* Free all of the memory associated with the png_ptr and info_ptr */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	/* If we get here, we had a problem reading the file */
	return NULL;
    }

    /* Set up the reader state. */
    memory_reader_state.buffer = buffer;
    memory_reader_state.bufsize = bufsize;
    memory_reader_state.current_pos = PNG_BYTES_TO_CHECK;

    /* Tell libpng to use our custom reader. */
    png_set_read_fn(png_ptr, &memory_reader_state, (png_rw_ptr)read_data_memory);

    /* We have already read some of the signature. */
    png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);

    /* Really load the image now. */
    bmp = really_load_png(png_ptr, info_ptr, pal);

    /* Clean up after the read, and free any memory allocated. */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    
    return bmp;
}



/*
 *----------------------------------------------------------------------
 *			Saving
 *----------------------------------------------------------------------
 */



/* save_hicolour:
 *  Core save routine for 15/16 bpp images, by Martijn Versteegh.
 */
static int save_hicolour(png_structp png_ptr, BITMAP *bmp, int depth)
{
    unsigned char *row, *p;
    int i, j, c;

    row = malloc(bmp->w * 3);
    if (!row)
	return 0;

    for (i=0; i<bmp->h; i++) {
	p = row;
        for (j = 0; j < bmp->w; j++) {
            c = getpixel(bmp, j, i);
            if (depth == 15) {
		*p++ = getr15(c);
		*p++ = getg15(c);
		*p++ = getb15(c);
            }
	    else {
		*p++ = getr16(c);
		*p++ = getg16(c);
		*p++ = getb16(c);
            }
        }

        png_write_row(png_ptr, row);
    }

    free(row);

    return 1;
}



/* save_truecolour:
 *  Core save routine for 32 bpp images.
 */
static int save_truecolour(png_structp png_ptr, BITMAP *bmp)
{
    unsigned char *row, *p;
    int i, j, c;

    row = malloc(bmp->w * 4);
    if (!row)
	return 0;

    for (i=0; i<bmp->h; i++) {
	p = row;
        for (j = 0; j < bmp->w; j++) {
            c = getpixel(bmp, j, i);
	    *p++ = getr32(c);
	    *p++ = getg32(c);
	    *p++ = getb32(c);
	    *p++ = geta32(c);
        }

        png_write_row(png_ptr, row);
    }

    free(row);

    return 1;
}



/* save_png:
 *  Writes a non-interlaced, no-frills PNG, taking the usual save_xyz
 *  parameters.  Returns non-zero on error.
 */
int save_png(AL_CONST char *filename, BITMAP *bmp, AL_CONST RGB *pal)
{
    PACKFILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int depth, colour_type;

    depth = bitmap_color_depth(bmp);
    if ((!bmp) || ((depth == 8) && (!pal)))
	return -1;

    fp = pack_fopen(filename, "w");
    if (!fp)
	return -1;

    /* Create and initialize the png_struct with the
     * desired error handler functions.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				      (void *)NULL, NULL, NULL);
    if (!png_ptr) {
	pack_fclose(fp);
	return -1;
    }

    /* Allocate/initialize the image information data. */
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
	pack_fclose(fp);
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	return -1;
    }

    /* Set error handling. */
    if (setjmp(png_ptr->jmpbuf)) {
	/* If we get here, we had a problem reading the file. */
	pack_fclose(fp);
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	return -1;
    }

    /* Use packfile routines. */
    png_set_write_fn(png_ptr, fp, (png_rw_ptr)write_data, flush_data);

    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
     * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE.
     */
    if (depth == 8)
	colour_type = PNG_COLOR_TYPE_PALETTE;
    else if (depth == 32)
	colour_type = PNG_COLOR_TYPE_RGB_ALPHA;
    else
	colour_type = PNG_COLOR_TYPE_RGB;

    png_set_IHDR(png_ptr, info_ptr, bmp->w, bmp->h, 8, colour_type,
		 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
		 PNG_FILTER_TYPE_BASE);

    /* Set the palette if there is one.  Required for indexed-color images. */
    if (colour_type == PNG_COLOR_TYPE_PALETTE) {
	png_color palette[256];
	int i;

	for (i = 0; i < 256; i++) {
	    palette[i].red = pal[i].r << 2;   /* 64 -> 256 */
	    palette[i].green = pal[i].g << 2;
	    palette[i].blue = pal[i].b << 2;
	}

	/* Set palette colors. */
	png_set_PLTE(png_ptr, info_ptr, palette, 256);
    }

    /* Optionally write comments into the image ... Nah. */

    /* Write the file header information. */
    png_write_info(png_ptr, info_ptr);

    /* Once we write out the header, the compression type on the text
     * chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
     * PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
     * at the end.
     */

    /* Flip BGR pixels to RGB. */
    /*if (depth >= 24) {
	if (makecol24(255, 0, 0) > makecol24(0, 0, 255))
	    png_set_bgr(png_ptr);
    }*/

    /* Set compression level. */
    png_set_compression_level(png_ptr, _png_compression_level);

    /* Save the data. */
    if ((depth == 15) || (depth == 16)) {
	if (!save_hicolour(png_ptr, bmp, depth)) {
	    pack_fclose(fp);
	    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	    return -1;
	}
    }
    else if (depth == 32) {
	if (!save_truecolour(png_ptr, bmp)) {
	    pack_fclose(fp);
	    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	    return -1;
	}
    }
    else
	png_write_image(png_ptr, bmp->line);   /* Dirt cheap! */

    png_write_end(png_ptr, info_ptr);

    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    pack_fclose(fp);

    return 0;
}



/*
 *----------------------------------------------------------------------
 *			Miscellaneous
 *----------------------------------------------------------------------
 */



/* register_png_file_type:
 */
void register_png_file_type(void)
{
    register_bitmap_file_type("png", load_png, save_png);
}



/* register_png_datafile_object:
 */

static void *load_datafile_png(PACKFILE *f, long size)
{
    BITMAP *bmp;
    char *buffer;

    buffer = malloc(size);
    if (!buffer)
	return NULL;

    if (pack_fread(buffer, size, f) != size) {
	free(buffer);
	return NULL;
    }

    bmp = load_memory_png(buffer, size, NULL);

    free(buffer);

    return bmp;
}

static void destroy_datafile_png(void *data)
{
    if (data)
	destroy_bitmap(data);
}

void register_png_datafile_object(int id)
{
    register_datafile_object(id, load_datafile_png, destroy_datafile_png);
}



/* loadpng_init:
 *  This is supposed to resemble jpgalleg_init in JPGalleg 2.0.
 */
int loadpng_init(void)
{
    register_png_datafile_object(DAT_ID('P','N','G',' '));
    register_png_file_type();
    return 0;
}
