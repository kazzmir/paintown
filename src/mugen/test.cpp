#include <allegro.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <map>

#include "globals.h"
#include "mugen_reader.h"
#include "mugen_section.h"
#include "mugen_item_content.h"
#include "mugen_item.h"
#include "mugen_character.h"
#include "mugen_animation.h"
#include "mugen_sprite.h"

#include "util/bitmap.h"
#include "util/funcs.h"

#include "factory/font_render.h"

#include "gui/keyinput_manager.h"

using namespace std;

/* testing spellcasters code 
static int sff_memloadInt(unsigned char *data, int *ofs) {
    int a,b;
    a = data[*ofs];
    (*ofs)++;
    b = data[*ofs];
    (*ofs)++;
    return a + (b <<8);
}

static RGB* sffLoadPaletteFromMemory(unsigned char *data) {
    RGB* pal = (RGB*) malloc(sizeof(RGB) * 256);
    int  a, ofs;
    ofs = 0;
    for (a=0; a < 256; ++a) {
        pal[a].r = data[ofs++] / 4;
        pal[a].g = data[ofs++] / 4;
        pal[a].b = data[ofs++] / 4;
    }
    return pal;
}

static BITMAP* sffLoadPcxFromMemory(unsigned char* data) {
    BITMAP *bmp = NULL;
    
    // skip the first 3 bytes 
    int     ofs = 3;
    //char    planes = data[ofs++];
    int     width  = -sff_memloadInt(data, &ofs);
    int     height = -sff_memloadInt(data, &ofs);
    int     bpp  = 0;
    int     bytesPerLine = 0;
    int     x,y;
    int     value;
    int     count;
    
    width += sff_memloadInt(data, &ofs) +1;
    height+= sff_memloadInt(data, &ofs) +1;
    
    // skip 4 bytes (dpi) 
    ofs += 4;
    
    // skip 16 color palette 
    ofs += 48;
    
    /// padding 
    ofs++;
        
        
    bpp  = data[ofs++] *8;
    if (bpp != 8) { // || bpp != 24) {
        return NULL;
    }
    
    bytesPerLine = sff_memloadInt(data, &ofs);
    
    // image data starts at ofs 128
    ofs = 128;
    bmp = create_bitmap_ex(bpp, width, height);
    for (y=0; y < height; ++y) {
        x = 0;
        while (x < bytesPerLine) {
            value = data[ofs++];
            
            // check if upper 2 bit are set 
            if ((value & 0xc0) == 0xc0) {
                // bits are set, that means the
                 //  lower 6 bit contain the repeat count,
                 //  and the color is stored in the next byte
                
                count = value & 0x3f;
                value = data[ofs++];
            } else {
                // value contains the color already 
                count = 1;
            }
            if (bpp == 8) {
                while (count > 0) {
                    if (x < bmp->w) {
                        bmp->line[y][x] = value;                        
                    }
                    ++x;
                    --count;
                }
            }
        }
    }
    
    
    return bmp;
}
*/

static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static void showOptions(){
    Global::debug(0) << "M.U.G.E.N. Config Reader:" << endl;
    Global::debug(0) << "-f <file>: Load a M.U.G.E.N. config file and output to screen." << endl;
    Global::debug(0) << "-c <name>: Load a M.U.G.E.N. character and output some data about it.\n         ie: 'data/mugen/chars/name' only pass name." << endl;
    Global::debug(0) << endl;
}

/* testing testing 1 2 3 */
void testPCX(){
    unsigned char data[1 << 18];
    FILE * f = fopen("x.pcx", "r");
    int length;
    length = fread(data, sizeof(char), 1<<18, f);
    Global::debug(0) << "Size is " << length << endl;
    fclose(f);
    Bitmap b = Bitmap::memoryPCX(data, length);
    // Bitmap b("x.pcx");
    if (b.getError()){
        Global::debug(0) << "what the hell" << endl;
    }
    Bitmap work(640, 480);
    work.circleFill(40, 40, 100, Bitmap::makeColor(255, 0, 0));
    b.draw(0, 0, work);
    Global::debug(0) << "Width " << b.getWidth() << " Height " << b.getHeight() << endl;
    work.BlitToScreen();
    Util::rest(3000);
}

int main( int argc, char ** argv ){
	
	if(argc <= 1){
	    showOptions();
	    return 0;
	}

	const char * FILE_ARG = "-f";
	const char * LOC_ARG = "-c";
        const char * DEBUG_ARG = "-l";
	std::string ourFile;
	bool configLoaded = false;

        allegro_init();
        install_timer();
	install_keyboard();
        set_color_depth(16);
        Bitmap::setGfxModeWindowed(640, 480);

	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], FILE_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
				configLoaded = !configLoaded;
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if ( isArg( argv[ q ], LOC_ARG ) ){
			q += 1;
			if ( q < argc ){
				ourFile = std::string( argv[ q ] );
			}
			else{
                            Global::debug(0) << "Error no file given!" << endl;
			  showOptions();
			  return 0;
			}
		} else if (isArg(argv[q], DEBUG_ARG)){
                    q += 1;
                    if (q < argc){
                        istringstream i( argv[ q ] );
                        int f;
                        i >> f;
                        Global::setDebug( f );
                    } else {
                        Global::debug(0) << "No number given for " << DEBUG_ARG << endl;
                    }
                } else {
		    // WHAT?
		    showOptions();
		    return 0;
		}
	}
	
	if( configLoaded ){
	    MugenReader reader( ourFile );
	    
	    std::vector< MugenSection * > collection;
	    
	    try{
		collection = reader.getCollection();
                Global::debug(0) << endl << "---------------------------------------------------------" << endl;
		for( unsigned int i = 0; i < collection.size(); ++i ){
                    Global::debug(0) << collection[i]->getHeader() << endl;
                    Global::debug(0) << "---------------------------------------------------------" << endl;
		    while( collection[i]->hasItems() ){
			MugenItemContent *content = collection[i]->getNext();
                        while( content->hasItems() ){
                            Global::debug(0) << content->getNext()->query();
                            if( content->hasItems() ) Global::debug(0) << ",";
                        }
                        Global::debug(0) << endl;
		    }
                    Global::debug(0) << "---------------------------------------------------------" << endl;
		}
                Global::debug(0) << endl;
	    }
	    catch( MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    }
	}
	else{
	    try{
                Global::debug(0) << "Trying to load character: " << ourFile << "..." << endl;
		MugenCharacter character( ourFile );
		character.load();
		/*
                for (map<int,MugenAnimation*>::const_iterator it = character.getAnimations().begin(); it != character.getAnimations().end(); it++){
                    Global::debug(0) << "Animation " << it->first << endl;
                    const vector<MugenFrame*> & frames = it->second->getFrames();
                    for (vector<MugenFrame*>::const_iterator frame = frames.begin(); frame != frames.end(); frame++){
                        MugenSprite * sprite = (*frame)->sprite;
                        if (sprite == 0){
                            continue;
                        }
                        Bitmap b = new Bitmap(sffLoadPcxFromMemory( (char*) sprite->pcx ));;//Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
                        b.BlitToScreen();
                        Util::rest(1000);
                    }
                }
		*/
		Global::debug(0) << "Loaded character: \"" << character.getName() << "\" successfully." << endl;
		bool quit = false;
		bool animate = false;
		int ticks = 0;
		int loop = 0;
		
		map<int,MugenAnimation*>::const_iterator it = character.getAnimations().begin();
		unsigned int currentAnim = 0;
		unsigned int lastAnim = character.getAnimations().size() -1;
		unsigned int currentFrame = 0;
		unsigned int lastFrame = it->second->getFrames().size() -1;
		
		MugenSprite * sprite = it->second->getFrames()[currentFrame]->sprite;
		
		//BITMAP *b_buff = sffLoadPcxFromMemory( (unsigned char*) sprite->pcx );
		//Bitmap b = Bitmap(b_buff, true);//Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
		//destroy_bitmap(b_buff);
		Bitmap b = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
		
		Bitmap work( 640, 480 );
		
		while( !quit ){
		    work.clear();
		    keyInputManager::update();
		    ++ticks;
		    // Since -1 is to stop the animation completely, we'll give it a pause of 150 ticks, because we want to see the loop
		    const int time = it->second->getFrames()[currentFrame]->time == -1 ? 150 : it->second->getFrames()[currentFrame]->time;
		    if(animate && ticks >= 15 + time){
			ticks = 0;
			if( it->second->getFrames()[currentFrame]->loopstart ) loop = currentFrame;
			if( currentFrame < lastFrame )currentFrame++;
			else currentFrame = loop;
			sprite = it->second->getFrames()[currentFrame]->sprite;
			if (sprite != 0){
			   /*b_buff = sffLoadPcxFromMemory( (unsigned char*) sprite->pcx );
			   b = Bitmap(b_buff, true);
			   destroy_bitmap(b_buff);*/
			   b = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
			}
		    }
		    
		    if( keyInputManager::keyState(keys::UP, true) ){
			if( currentAnim < lastAnim ){
			    currentAnim++;
			    it++;
			}
			loop = 0;
			currentFrame = 0;
			lastFrame = it->second->getFrames().size()-1;
			sprite = it->second->getFrames()[currentFrame]->sprite;
			if (sprite != 0){
			  /* b_buff = sffLoadPcxFromMemory( (unsigned char*) sprite->pcx );
			   b = Bitmap(b_buff, true);
			   destroy_bitmap(b_buff);*/
			  b = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
			}
		    }
		    else if( keyInputManager::keyState(keys::DOWN, true) ){
			if( currentAnim > 0 ){
			    currentAnim--;
			    it--;
			}
			loop = 0;
			currentFrame = 0;
			lastFrame = it->second->getFrames().size()-1;
			sprite = it->second->getFrames()[currentFrame]->sprite;
			if (sprite != 0){
			   /*b_buff = sffLoadPcxFromMemory( (unsigned char*) sprite->pcx );
			   b = Bitmap(b_buff, true);
			   destroy_bitmap(b_buff);*/
			   b = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
			}
		    }
		    else if( keyInputManager::keyState(keys::LEFT, true) && !animate){
			if( currentFrame > 0 )currentFrame--;
			else currentFrame = lastFrame;
			sprite = it->second->getFrames()[currentFrame]->sprite;
			if (sprite != 0){
			  /*  b_buff = sffLoadPcxFromMemory( (unsigned char*) sprite->pcx );
			   b = Bitmap(b_buff, true);
			   destroy_bitmap(b_buff); */
			  b = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
			}
		    }
		    else if( keyInputManager::keyState(keys::RIGHT, true) && !animate){
			if( currentFrame < lastFrame )currentFrame++;
			else currentFrame = 0;
			sprite = it->second->getFrames()[currentFrame]->sprite;
			if (sprite != 0){
			   /*  b_buff = sffLoadPcxFromMemory( (unsigned char*) sprite->pcx );
			   b = Bitmap(b_buff, true);
			   destroy_bitmap(b_buff); */
			  b = Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->reallength);
			}
		    }
		    else if( keyInputManager::keyState(keys::SPACE, true) ){
			animate = !animate;
		    }
		    quit |= keyInputManager::keyState(keys::ESC, true );
		    
		    b.Blit(240 + it->second->getFrames()[currentFrame]->xoffset, 100 + it->second->getFrames()[currentFrame]->yoffset, work);
		    Font::getDefaultFont().printf( 15, 250, Bitmap::makeColor( 255, 255, 255 ), work, "Current Animation: %i, Current Frame: %i", 0, currentAnim, currentFrame );
		    if(sprite!=0)Font::getDefaultFont().printf( 15, 270, Bitmap::makeColor( 255, 255, 255 ), work, "Length: %d | x: %d | y: %d | Group: %d | Image: %d", sprite->length, sprite->x, sprite->y, sprite->groupNumber, sprite->imageNumber);
		    work.BlitToScreen();
		    Util::rest(1);
		}
	    }
	    catch( MugenException &ex){
                Global::debug(0) << "Problem loading file, error was: " << ex.getReason() << endl;
		return 1;
	    }
	}
	
	return 0;
}
