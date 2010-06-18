#include "util/bitmap.h"
#include <math.h>
#include <iostream>

#include "level/utils.h"
#include "network/messages.h"
#include "loading.h"
#include "util/file-system.h"
#include "util/font.h"
#include "util/funcs.h"
#include "util/gradient.h"
#include "util/thread.h"
#include "globals.h"
#include <vector>
#include "util/thread.h"
#include "util/message-queue.h"
#include "init.h"

using namespace std;

namespace Loader{

Util::Thread::Lock loading_screen_mutex;
volatile bool done_loading = false;

typedef struct pair{
	int x, y;
} ppair;

class Info{
public:
    Info(){
        Global::registerInfo(&messages);
    }

    bool transferMessages(Messages & box){
        bool did = false;
        while (messages.hasAny()){
            const string & str = messages.get();
            box.addMessage(str);
            did = true;
        }
        return did;
    }

    ~Info(){
        Global::unregisterInfo(&messages);
    }

private:
    MessageQueue messages;
};

void startLoading(Util::Thread::Id * thread, void * arg){
    Util::Thread::acquireLock(&loading_screen_mutex);
    done_loading = false;
    Util::Thread::releaseLock(&loading_screen_mutex);
    Util::Thread::createThread(thread, NULL, loadingScreen, arg);
}

void stopLoading(Util::Thread::Id thread){
    Util::Thread::acquireLock( &loading_screen_mutex );
    done_loading = true;
    Util::Thread::releaseLock( &loading_screen_mutex );
    Util::Thread::joinThread(thread);
}

static void setupBackground(const Bitmap & background, int load_x, int load_y, int load_width, int load_height, int infobox_x, int infobox_y, int infoWidth, int infoHeight, const Bitmap & infoBackground, const Bitmap & work){
    background.Blit(load_x, load_y, load_width, load_height, 0, 0, work);
    Font::getDefaultFont().printf( 400, 480 - Font::getDefaultFont().getHeight() * 5 / 2 - Font::getDefaultFont().getHeight(), Bitmap::makeColor( 192, 192, 192 ), background, "Paintown version %s", 0, Global::getVersionString().c_str());
    Font::getDefaultFont().printf( 400, 480 - Font::getDefaultFont().getHeight() * 5 / 2, Bitmap::makeColor( 192, 192, 192 ), background, "Made by Jon Rafkind", 0 );
    background.BlitToScreen();
    background.Blit(infobox_x, infobox_y, infoWidth, infoHeight, 0, 0, infoBackground);
}

static vector<ppair> generateFontPixels(const Font & myFont, const string & message, int width, int height){
    Bitmap letters(width, height);
    letters.fill( Bitmap::MaskColor() );
    myFont.printf( 0, 0, Bitmap::makeColor(255, 255, 255), letters, message.c_str(), 0 ); 

    vector< ppair > pairs;
    /* store every pixel we need to draw */
    for ( int x = 0; x < letters.getWidth(); x++ ){
        for ( int y = 0; y < letters.getHeight(); y++ ){
            int pixel = letters.getPixel(x, y);
            if (pixel != Bitmap::MaskColor()){
                ppair p;
                p.x = x;
                p.y = y;
                pairs.push_back( p );
            }
        }
    }

    return pairs;
}

/* shows time elapsed */
class TimeCounter{
public:
    TimeCounter():
    work(200, 40){
        start = Global::second_counter;
        last = 0;
    }

    void draw(int x, int y){
        const Font & font = Font::getDefaultFont(24, 24);
        if (Global::second_counter != last){
            work.clear();
            last = Global::second_counter;
            font.printf(0, 0, Bitmap::makeColor(192, 192, 192), work, "Waiting.. %d", 0, last - start);
            work.BlitAreaToScreen(x, y);
        }
    }

    Bitmap work;
    unsigned int start;
    unsigned int last;
};

void * loadingScreen( void * arg ){
    int load_x = 80;
    int load_y = 220;
    const int infobox_width = 300;
    const int infobox_height = 150;
    Info info;
    const Font & myFont = Font::getFont(Global::DEFAULT_FONT, 24, 24);
    const Font & infoFont = Font::getFont(Global::DEFAULT_FONT, 24, 24);
    Level::LevelInfo levelInfo;
    if (arg != NULL){
        levelInfo = *(Level::LevelInfo*) arg;
    }

    if (levelInfo.getPositionX() != -1){
        load_x = levelInfo.getPositionX();
    }
    
    if (levelInfo.getPositionY() != -1){
        load_y = levelInfo.getPositionY();
    }

    // const char * the_string = (arg != NULL) ? (const char *) arg : "Loading...";
    int load_width = myFont.textLength(levelInfo.loadingMessage().c_str());
    int load_height = myFont.getHeight(levelInfo.loadingMessage().c_str());

    const int infobox_x = load_x;
    const int infobox_y = load_y + load_height * 2;

    Global::debug( 2 ) << "loading screen" << endl;

    Bitmap work( load_width, load_height );

    vector<ppair> pairs = generateFontPixels(myFont, levelInfo.loadingMessage(), load_width, load_height);

    Messages infobox(infobox_width, infobox_height);
    Bitmap infoWork(infobox_width, infobox_height);
    Bitmap infoBackground(infobox_width, infobox_height);

    const int MAX_COLOR = 200;

    /* blend from dark grey to light red */
    Effects::Gradient gradient(MAX_COLOR, Bitmap::makeColor(16, 16, 16), Bitmap::makeColor(192, 8, 8));

    Global::speed_counter = 0;

    if (levelInfo.getBackground() != 0){
        setupBackground(*levelInfo.getBackground(), load_x, load_y, load_width, load_height, infobox_x, infobox_y, infoBackground.getWidth(), infoBackground.getHeight(), infoBackground, work);
    } else {
        setupBackground(Bitmap(levelInfo.loadingBackground().path()), load_x, load_y, load_width, load_height, infobox_x, infobox_y, infoBackground.getWidth(), infoBackground.getHeight(), infoBackground, work);
    }

    Util::ThreadBoolean quit(done_loading, loading_screen_mutex);

    TimeCounter counter;

    bool firstDraw = true;

    while ( ! quit.get() ){

        /* true if a logic loop has passed */
        bool draw = firstDraw;

        /* will be true if any new info messages appeared */
        bool drawInfo = firstDraw;
        firstDraw = false;
        if ( Global::speed_counter > 0 ){
            double think = Global::speed_counter;	
            Global::speed_counter = 0;
            draw = true;

            while ( think > 0 ){
                gradient.backward();
                think -= 1;
            }

            /* if no new messages appeared this will be false */
            drawInfo = info.transferMessages(infobox);
        } else {
            Util::rest( 1 );
        }

        if (draw){
            for ( vector< ppair >::iterator it = pairs.begin(); it != pairs.end(); it++ ){
                int color = gradient.current(it->x);
                work.putPixel(it->x, it->y, color);
            }

            // counter.draw(200, 100);

            /* we might not have to draw the whole info box again if no new
             * messages appeared.
             */
            if (drawInfo){
                infoBackground.Blit(infoWork);

                /* cheesy hack to change the font size. the font
                 * should store the size and change it on its own
                 */
                Font::getFont(Global::DEFAULT_FONT, 13, 13);
                infobox.draw(0, 0, infoWork, infoFont);
                Font::getFont(Global::DEFAULT_FONT, 24, 24);
                infoWork.BlitAreaToScreen(infobox_x, infobox_y);
            }
            /* work already contains the correct background */
            // work.Blit( load_x, load_y, *Bitmap::Screen );
            work.BlitAreaToScreen(load_x, load_y);
        }

        /*
        pthread_mutex_lock( &loading_screen_mutex );
        quit = done_loading;
        pthread_mutex_unlock( &loading_screen_mutex );
        */
    }

    return NULL;
}

}
