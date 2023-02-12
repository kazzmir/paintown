#include "r-tech1/graphics/bitmap.h"
#include <string.h>

namespace Graphics{

static bool isPng(const unsigned char * data, int length){
    return length > 3 &&
           data[0] == 0x89 &&
           data[1] == 'P' &&
           data[2] == 'N' &&
           data[3] == 'G';
}

static bool isBmp(const unsigned char * data, int length){
    return length > 2 &&
           data[0] == 'B' &&
           data[1] == 'M';
}

static bool isJpg(const unsigned char * data, int length){
    /* TODO */
    return false;
#if 0
int start;
    int is_JPG;
    int in_scan;
    Uint8 magic[4];

    /* This detection code is by Steaphan Greene <stea@cs.binghamton.edu> */
    /* Blame me, not Sam, if this doesn't work right. */
    /* And don't forget to report the problem to the the sdl list too! */

    if ( !src )
        return 0;
    start = SDL_RWtell(src);
    is_JPG = 0;
    in_scan = 0;
    if ( SDL_RWread(src, magic, 2, 1) ) {
        if ( (magic[0] == 0xFF) && (magic[1] == 0xD8) ) {
            is_JPG = 1;
            while (is_JPG == 1) {
                if(SDL_RWread(src, magic, 1, 2) != 2) {
                    is_JPG = 0;
                } else if( (magic[0] != 0xFF) && (in_scan == 0) ) {
                    is_JPG = 0;
                } else if( (magic[0] != 0xFF) || (magic[1] == 0xFF) ) {
                    /* Extra padding in JPEG (legal) */
                    /* or this is data and we are scanning */
                    SDL_RWseek(src, -1, RW_SEEK_CUR);
                } else if(magic[1] == 0xD9) {
                    /* Got to end of good JPEG */
                    break;
                } else if( (in_scan == 1) && (magic[1] == 0x00) ) {
                    /* This is an encoded 0xFF within the data */
                } else if( (magic[1] >= 0xD0) && (magic[1] < 0xD9) ) {
                    /* These have nothing else */
                } else if(SDL_RWread(src, magic+2, 1, 2) != 2) {
                    is_JPG = 0;
                } else {
                    /* Yes, it's big-endian */
                    Uint32 start;
                    Uint32 size;
                    Uint32 end;
                    start = SDL_RWtell(src);
                    size = (magic[2] << 8) + magic[3];
                    end = SDL_RWseek(src, size-2, RW_SEEK_CUR);
                    if ( end != start + size - 2 ) is_JPG = 0;
                    if ( magic[1] == 0xDA ) {
                        /* Now comes the actual JPEG meat */
#ifdef  FAST_IS_JPEG
                        /* Ok, I'm convinced.  It is a JPEG. */
                        break;
#else
                        /* I'm not convinced.  Prove it! */
                        in_scan = 1;
#endif
                    }
                }
            }
        }
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return(is_JPG);
#endif
}

static bool isPcx(const unsigned char * data, int length){
    const int ZSoft_Manufacturer = 10;
    const int PC_Paintbrush_Version = 5;
    const int PCX_Uncompressed_Encoding = 0;
    const int PCX_RunLength_Encoding = 1;

    return length > 3 &&
           data[0] == ZSoft_Manufacturer &&
           data[1] == PC_Paintbrush_Version &&
           (data[2] == PCX_RunLength_Encoding ||
            data[2] == PCX_Uncompressed_Encoding);
}

static bool isTga(const unsigned char * data, int length){
    /* No magic for tga files.. come up with some way to detect them */
    return false;
}

static bool isTif(const unsigned char * data, int length){
    return length > 4 &&
           ((data[0] == 'I' &&
            data[1] == 'I' &&
            data[2] == 0x2a &&
            data[3] == 0x00) ||
           (data[0] == 'M' &&
            data[1] == 'M' &&
            data[2] == 0x00 &&
            data[3] == 0x2a));
}

static bool isXpm(const unsigned char * data, int length){
    return strncmp((const char *) data, "/* XPM */", length) == 0;
}

static bool isGif(const unsigned char * data, int length){
    return length > 7 &&
           (strncmp((const char *) data, "GIF", 3) == 0) &&
           (strncmp((const char *) data + 3, "87a", 3) == 0 ||
            strncmp((const char *) data + 3, "89a", 3) == 0);
}

ImageFormat identifyImage(const unsigned char * data, int length){
    if (isPng(data, length)){
        return FormatPNG;
    }
    if (isBmp(data, length)){
        return FormatBMP;
    }
    if (isJpg(data, length)){
        return FormatJPG;
    }
    if (isPcx(data, length)){
        return FormatPCX;
    }
    if (isTga(data, length)){
        return FormatTGA;
    }
    if (isTif(data, length)){
        return FormatTIF;
    }
    if (isXpm(data, length)){
        return FormatXPM;
    }
    if (isGif(data, length)){
        return FormatGIF;
    }
    return FormatUnknown;
}

}
