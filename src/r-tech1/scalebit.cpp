/*
 * This file is part of the Scale2x project.
 *
 * Copyright (C) 2003 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This file contains an example implementation of the Scale effect
 * applyed to a generic bitmap.
 *
 * You can find an high level description of the effect at :
 *
 * http://scale2x.sourceforge.net/
 *
 * Alternatively at the previous license terms, you are allowed to use this
 * code in your program with these conditions:
 * - the program is not used in commercial activities.
 * - the whole source code of the program is released with the binary.
 * - derivative works of the program are allowed.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <allegro.h>

#include "scale2x.h"
#include "scale.h"
// #include "scale3x.h"

#define HAVE_ALLOCA_H
#if defined(HAVE_ALLOCA_H)
#include <alloca.h>
#endif

#include <assert.h>
#include <stdlib.h>

typedef scale2x_uint8* conv8;
typedef scale2x_uint16* conv16;
typedef scale2x_uint32* conv32;

/**
 * Apply the Scale2x effect on a group of rows. Used internally.
 */
static inline void stage_scale2x(void* dst0, void* dst1, const void* src0, const void* src1, const void* src2, unsigned pixel, unsigned pixel_per_row)
{

	switch (pixel) {
		case 1 : scale2x_8_mmx((conv8)dst0, (conv8)dst1, (conv8)src0, (conv8)src1, (conv8)src2, pixel_per_row); break;
		case 2 : scale2x_16_mmx((conv16)dst0, (conv16)dst1, (conv16)src0, (conv16)src1, (conv16)src2, pixel_per_row); break;
		case 4 : scale2x_32_mmx((conv32)dst0, (conv32)dst1, (conv32)src0, (conv32)src1, (conv32)src2, pixel_per_row); break;
	}


	/*
	switch (pixel) {
#if defined(__GNUC__) && defined(__i386__)
		case 1 : scale2x_8_mmx((conv8)dst0, (conv8)dst1, (conv8)src0, (conv8)src1, (conv8)src2, pixel_per_row); break;
		case 2 : scale2x_16_mmx((conv16)dst0, (conv16)dst1, (conv16)src0, (conv16)src1, (conv16)src2, pixel_per_row); break;
		case 4 : scale2x_32_mmx((conv32)dst0, (conv32)dst1, (conv32)src0, (conv32)src1, (conv32)src2, pixel_per_row); break;
#else
		case 1 : scale2x_8_def((conv8)dst0, (conv8)dst1, (conv8)src0, (conv8)src1, (conv8)src2, pixel_per_row); break;
		case 2 : scale2x_16_def((conv16)dst0, (conv16)dst1, (conv16)src0, (conv16)src1, (conv16)src2, pixel_per_row); break;
		case 4 : scale2x_32_def((conv32)dst0, (conv32)dst1, (conv32)src0, (conv32)src1, (conv32)src2, pixel_per_row); break;
#endif
	}
	*/
}

/**
 * Apply the Scale3x effect on a group of rows. Used internally.
 */
 /*
static inline void stage_scale3x(void* dst0, void* dst1, void* dst2, const void* src0, const void* src1, const void* src2, unsigned pixel, unsigned pixel_per_row)
{
	switch (pixel) {
		case 1 : scale3x_8_def(dst0, dst1, dst2, src0, src1, src2, pixel_per_row); break;
		case 2 : scale3x_16_def(dst0, dst1, dst2, src0, src1, src2, pixel_per_row); break;
		case 4 : scale3x_32_def(dst0, dst1, dst2, src0, src1, src2, pixel_per_row); break;
	}
}
*/

/**
 * Apply the Scale4x effect on a group of rows. Used internally.
 */
static inline void stage_scale4x(void* dst0, void* dst1, void* dst2, void* dst3, const void* src0, const void* src1, const void* src2, const void* src3, unsigned pixel, unsigned pixel_per_row)
{
	stage_scale2x(dst0, dst1, src0, src1, src2, pixel, 2 * pixel_per_row);
	stage_scale2x(dst2, dst3, src1, src2, src3, pixel, 2 * pixel_per_row);
}

#define SCDST(i) (dst+(i)*dst_slice)
#define SCSRC(i) (src+(i)*src_slice)
#define SCMID(i) (mid[(i)])

/**
 * Apply the Scale2x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 2x2 times the size of the source bitmap.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */

void scale2x_allegro( BITMAP * src_bitmap, BITMAP * dest_bitmap, unsigned int pixel ){
	// unsigned char * dst = dest_bitmap->line[0];
	// unsigned char * src = src_bitmap->line[0];
	unsigned int dst_line = 0;
	unsigned int src_line = 1;
	unsigned int count;
	unsigned int width;

	unsigned char * srcprev;
	unsigned char * srccurr;
	unsigned char * srcnext;
	unsigned char * destcurr;
	unsigned char * destnext;
	
	width = src_bitmap->w;
	count = src_bitmap->h;
	count -= 2;
	// count = count >> 4;
	
	while (count) {
		 srcprev = src_bitmap->line[src_line-1];
		 srccurr = src_bitmap->line[src_line];
		 srcnext = src_bitmap->line[src_line+1];
		 destcurr = dest_bitmap->line[dst_line];
		 destnext = dest_bitmap->line[dst_line+1];
		// stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(1), SCSRC(2), pixel, width);
		stage_scale2x( destcurr, destnext, srcprev, srccurr, srcnext, pixel, width );

		// dst = SCDST(2);
		// src = SCSRC(1);
		src_line++;
		dst_line += 2;

		--count;
	}
	srcprev = src_bitmap->line[src_line-1];
	srccurr = src_bitmap->line[src_line];
	srcnext = src_bitmap->line[src_line];
	destcurr = dest_bitmap->line[dst_line];
	destnext = dest_bitmap->line[dst_line+1];
	stage_scale2x( destcurr, destnext, srcprev, srccurr, srcnext, pixel, width );

	// stage_scale2x(SCDST(0), SCDST(1), SCSRC(1-1), SCSRC(2-1), SCSRC(2-1), pixel, width);
	
	scale2x_mmx_emms();


}

static void scale2x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	unsigned char* dst = (unsigned char*)void_dst;
	const unsigned char* src = (unsigned char*)void_src;
	unsigned count;

	assert(height >= 2);

	count = height;

	stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(0), SCSRC(1), pixel, width);

	dst = SCDST(2);

	count -= 2;
	while (count) {
		stage_scale2x(SCDST(0), SCDST(1), SCSRC(0), SCSRC(1), SCSRC(2), pixel, width);

		dst = SCDST(2);
		src = SCSRC(1);

		--count;
	}

	stage_scale2x(SCDST(0), SCDST(1), SCSRC(1-1), SCSRC(2-1), SCSRC(2-1), pixel, width);

#if defined(__GNUC__) && defined(__i386__)
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale32x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 3x3 times the size of the source bitmap.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
 /*
static void scale3x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	unsigned char* dst = (unsigned char*)void_dst;
	const unsigned char* src = (unsigned char*)void_src;
	unsigned count;

	assert(height >= 2);

	count = height;

	stage_scale3x(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(0), SCSRC(1), pixel, width);

	dst = SCDST(3);

	count -= 2;
	while (count) {
		stage_scale3x(SCDST(0), SCDST(1), SCDST(2), SCSRC(0), SCSRC(1), SCSRC(2), pixel, width);

		dst = SCDST(3);
		src = SCSRC(1);

		--count;
	}

	stage_scale3x(SCDST(0), SCDST(1), SCDST(2), SCSRC(1-1), SCSRC(2-1), SCSRC(2-1), pixel, width);
}
*/

/**
 * Apply the Scale4x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 4x4 times the size of the source bitmap.
 * \note This function requires also a small buffer bitmap used internally to store
 * intermediate results. This bitmap must have at least an horizontal size in bytes of 2*width*pixel,
 * and a vertical size of 6 rows. The memory of this buffer must not be allocated
 * in video memory because it's also read and not only written. Generally
 * a heap (malloc) or a stack (alloca) buffer is the best choices.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_mid Pointer at the first pixel of the buffer bitmap.
 * \param mid_slice Size in bytes of a buffer bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
static void scale4x_buf(void* void_dst, unsigned dst_slice, void* void_mid, unsigned mid_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	unsigned char* dst = (unsigned char*)void_dst;
	const unsigned char* src = (unsigned char*)void_src;
	unsigned count;
	unsigned char* mid[6];

	assert(height >= 4);

	count = height;

	/* set the 6 buffer pointers */
	mid[0] = (unsigned char*)void_mid;
	mid[1] = mid[0] + mid_slice;
	mid[2] = mid[1] + mid_slice;
	mid[3] = mid[2] + mid_slice;
	mid[4] = mid[3] + mid_slice;
	mid[5] = mid[4] + mid_slice;

	stage_scale2x(SCMID(-2+6), SCMID(-1+6), SCSRC(0), SCSRC(0), SCSRC(1), pixel, width);
	stage_scale2x(SCMID(0), SCMID(1), SCSRC(0), SCSRC(1), SCSRC(2), pixel, width);
	stage_scale2x(SCMID(2), SCMID(3), SCSRC(1), SCSRC(2), SCSRC(3), pixel, width);
	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(-2+6), SCMID(-2+6), SCMID(-1+6), SCMID(0), pixel, width);

	dst = SCDST(4);

	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(-1+6), SCMID(0), SCMID(1), SCMID(2), pixel, width);

	dst = SCDST(4);

	count -= 4;
	while (count) {
		unsigned char* tmp;

		stage_scale2x(SCMID(4), SCMID(5), SCSRC(2), SCSRC(3), SCSRC(4), pixel, width);
		stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(1), SCMID(2), SCMID(3), SCMID(4), pixel, width);

		dst = SCDST(4);
		src = SCSRC(1);

		tmp = SCMID(0); /* shift by 2 position */
		SCMID(0) = SCMID(2);
		SCMID(2) = SCMID(4);
		SCMID(4) = tmp;
		tmp = SCMID(1);
		SCMID(1) = SCMID(3);
		SCMID(3) = SCMID(5);
		SCMID(5) = tmp;

		--count;
	}

	stage_scale2x(SCMID(4), SCMID(5), SCSRC(2), SCSRC(3), SCSRC(3), pixel, width);
	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(1), SCMID(2), SCMID(3), SCMID(4), pixel, width);

	dst = SCDST(4);

	stage_scale4x(SCDST(0), SCDST(1), SCDST(2), SCDST(3), SCMID(3), SCMID(4), SCMID(5), SCMID(5), pixel, width);

#if defined(__GNUC__) && defined(__i386__)
	scale2x_mmx_emms();
#endif
}

/**
 * Apply the Scale4x effect on a bitmap.
 * The destination bitmap is filled with the scaled version of the source bitmap.
 * The source bitmap isn't modified.
 * The destination bitmap must be manually allocated before calling the function,
 * note that the resulting size is exactly 4x4 times the size of the source bitmap.
 * \note This function operates like ::scale4x_buf() but the intermediate buffer is
 * automatically allocated in the stack.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
 
void scale4x_allegro( BITMAP * src_bitmap, BITMAP * dest_bitmap, unsigned int pixel ){
	unsigned int mid_slice;
	void * mid;
	unsigned int width;
	unsigned char * src[ 4 ];
	unsigned char * dest[ 4 ];
	unsigned int src_line = 0;
	unsigned int dest_line = 0;
	unsigned int count;
	
	width = src_bitmap->w;
	count = src_bitmap->h - 4;

	while ( count ){
		int section = 0;

		for ( section = 0; section < 4; section++ ){

			unsigned int xx = width * section;
			unsigned int xz = xx * 2;
			src[0] = src_bitmap->line[0+src_line] + xx;
			src[1] = src_bitmap->line[1+src_line] + xx;
			src[2] = src_bitmap->line[2+src_line] + xx;
			src[3] = src_bitmap->line[3+src_line] + xx;
			dest[0] = dest_bitmap->line[0+dest_line] + xz;
			dest[1] = dest_bitmap->line[1+dest_line] + xz;
			dest[2] = dest_bitmap->line[2+dest_line] + xz;
			dest[3] = dest_bitmap->line[3+dest_line] + xz;
		
			stage_scale4x( dest[0], dest[1], dest[2], dest[3], src[0], src[1], src[2], src[3], pixel, width / 4 );

		}

		/*
		src[0] += width;
		src[1] += width;
		src[2] += width;
		src[3] += width;
		dest[0] += width*2;
		dest[1] += width*2;
		dest[2] += width*2;
		dest[3] += width*2;
		
		stage_scale4x( dest[0], dest[1], dest[2], dest[3], src[0], src[1], src[2], src[3], pixel, width / 4 );
		*/

		src_line++;
		dest_line += 4;

		--count;
	}
	

}
 /*
static void scale4x(void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	unsigned mid_slice;
	void* mid;

	mid_slice = 2 * pixel * width; / * required space for 1 row buffer * /

	mid_slice = (mid_slice + 0x7) & ~0x7; / * align to 8 bytes * /

#if HAVE_ALLOCA
	mid = alloca(6 * mid_slice); / * allocate space for 6 row buffers * /

	assert(mid != 0); / * alloca should never fails * /
#else
	mid = malloc(6 * mid_slice); / * allocate space for 6 row buffers * /

	if (!mid)
		return;
#endif

	scale4x_buf(void_dst, dst_slice, mid, mid_slice, void_src, src_slice, pixel, width, height);

#if !HAVE_ALLOCA
	free(mid);
#endif
}
*/

/**
 * Check if the scale implementation is applicable at the given arguments.
 * \param scale Scale factor. 2, 3 or 4.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 * \return
 *   - -1 on precondition violated.
 *   - 0 on success.
 */
int scale_precondition(unsigned scale, unsigned pixel, unsigned width, unsigned height)
{
	if (scale != 2 && scale != 3 && scale != 4)
		return -1;

	if (pixel != 1 && pixel != 2 && pixel != 4)
		return -1;

	switch (scale) {
	case 2 :
	case 3 :
		if (height < 2)
			return -1;
		break;
	case 4 :
		if (height < 4)
			return -1;
		break;
	}

#if defined(__GNUC__) && defined(__i386__)
	switch (scale) {
	case 2 :
	case 4 :
		if (width < (16 / pixel))
			return -1;
		if (width % (8 / pixel) != 0)
			return -1;
		break;
	case 3 :
		if (width < 2)
			return -1;
		break;
	}
#else
	if (width < 2)
		return -1;
#endif

	return 0;
}

/**
 * Apply the Scale effect on a bitmap.
 * This function is simply a common interface for ::scale2x(), ::scale3x() and ::scale4x().
 * \param scale Scale factor. 2, 3 or 4.
 * \param void_dst Pointer at the first pixel of the destination bitmap.
 * \param dst_slice Size in bytes of a destination bitmap row.
 * \param void_src Pointer at the first pixel of the source bitmap.
 * \param src_slice Size in bytes of a source bitmap row.
 * \param pixel Bytes per pixel of the source and destination bitmap.
 * \param width Horizontal size in pixels of the source bitmap.
 * \param height Vertical size in pixels of the source bitmap.
 */
void scale(unsigned scale, void* void_dst, unsigned dst_slice, const void* void_src, unsigned src_slice, unsigned pixel, unsigned width, unsigned height)
{
	switch (scale) {
	case 2 :
		scale2x(void_dst, dst_slice, void_src, src_slice, pixel, width, height);
		break;
	case 3 :
		// scale3x(void_dst, dst_slice, void_src, src_slice, pixel, width, height);
		break;
	case 4 :
		// scale4x(void_dst, dst_slice, void_src, src_slice, pixel, width, height);
		break;
	}
}
