#include <stdio.h>
#include "debug.h"
#include <iostream>
#include <string.h>

using namespace std;

typedef unsigned char palette[768];

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

bool readPalette(const char * path, palette pal){
    unsigned char colorsave[3]; // rgb pal save
    FILE *act_file = fopen(path, "rb");
    if (!act_file){
	Global::debug(1) << "Unable to open ACT file: " << path << endl;
	if (act_file){
            fclose(act_file);
        }
	pal = 0;
	return false;
    } else {
	fseek(act_file, 0, SEEK_END);
	// Then it must be an act file
	if (ftell(act_file) == 768){ 
	    for (int i = 0; i < 256; i++){
		fseek(act_file, -3*(i+1), SEEK_END);
		size_t bleh = fread(colorsave, 3, 1, act_file);
		bleh = bleh;
		pal[3*i]=colorsave[0];
		pal[3*i+1]=colorsave[1];
		pal[3*i+2]=colorsave[2];
	    }
	    Global::debug(1) << "Applying palette from ACT file: " << path << endl;
	    if (act_file){
                fclose(act_file);
            }

	    return true;
	} else if (ftell(act_file) < 897){ 
            // 128-byte header + 768-byte palette + 0x0C byte = minimum 8-bit PCX file size.
            Global::debug(1) << "File " << path << " is not a valid palette file." << endl;
	    if (act_file){
                fclose(act_file);
            }
	    pal = 0;
	    return false;
        } else {
            // we'll assume it's a PCX file, for now.
            fseek(act_file, 0, SEEK_SET);
	    pcx_header pcxhead;
            size_t bleh = fread(&pcxhead, sizeof(pcx_header), 1, act_file);
            // Check to see if the PCX file uses an 8-bit palette.
            if ((pcxhead.manufacturer == 10) && (pcxhead.version >= 5) && (pcxhead.BitsPerPixel == 8) && (pcxhead.NPlanes == 1)){
                fseek( act_file, -769, SEEK_END);
		// No need to define another variable; colorsave will do just fine.
                bleh = fread( colorsave, 1, 1, act_file ); 
                if (colorsave[0] == 12){
                    fseek(act_file, -768, SEEK_END);
                    bleh = fread(pal, 768, 1, act_file);
                    Global::debug(1) << "Applying palette from PCX file: " << path << endl;
		    return true;
                } else {
                    Global::debug(1) << "File " << path << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
		    if (act_file){
                        fclose(act_file);
                    }
		    pal = 0;
		    return false;
                }
            } else {
            // Add support for JASC and RIFF palette files later... 
            // Minimum JASC PAL size = 1,813 bytes (carriage returns are necessary). 
		Global::debug(1) << "File " << path << " is not a valid palette file. (Must be ACT or 8-bit PCX.)";
		if (act_file){
                    fclose( act_file );
                }
		pal = 0;
		return false;
            }
        }
    }

    if (act_file){
        fclose(act_file);
    }

    pal = 0;
    return false;
}

void newReadPalette(const char * path, palette out){
}

void load(const char * path){
    palette palette1;
    palette palette2;
    memset(palette1, 0, sizeof(palette));
    memset(palette2, 0, sizeof(palette));

    readPalette(path, palette1);
    newReadPalette(path, palette2);
    if (memcmp(palette1, palette2, sizeof(palette)) != 0){
        Global::debug(0) << "Fail!" << endl;
    } else {
        Global::debug(1) << "Fail!" << endl;
    }
}

int main(){
    load("data/mugen/chars/kfm/kfm.act");
    load("data/mugen/chars/kfm/kfm2.act");
    load("data/mugen/chars/kfm/kfm3.act");
    load("data/mugen/chars/kfm/kfm4.act");
    load("data/mugen/chars/kfm/kfm5.act");
    load("data/mugen/chars/kfm/kfm6.act");
}
