#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include <math.h>
#include <iostream>

#include "level/utils.h"
#include "util/messages.h"
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

// Util::Thread::Lock loading_screen_mutex;
volatile bool done_loading = true;

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

void * loadingScreenSimple1(void * arg);

#if 0
void startLoading(Util::Thread::Id * thread, void * arg, Kind kind){
    bool create = false;
    Util::Thread::acquireLock(&loading_screen_mutex);
    create = done_loading;
    done_loading = false;
    Util::Thread::releaseLock(&loading_screen_mutex);

    /* prevent multiple loading threads from being made */
    if (create){
        switch (kind){
            case Default: Util::Thread::createThread(thread, NULL, (Util::Thread::ThreadFunction) loadingScreen, arg); break;
            case SimpleCircle: Util::Thread::createThread(thread, NULL, (Util::Thread::ThreadFunction) loadingScreenSimple1, arg); break;
        }
    }
}

void stopLoading(Util::Thread::Id thread){
    bool needJoin = false;
    Util::Thread::acquireLock( &loading_screen_mutex );
    needJoin = ! done_loading;
    done_loading = true;
    Util::Thread::releaseLock( &loading_screen_mutex );
    if (needJoin){
        Util::Thread::joinThread(thread);
    }
}
#endif

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

/* FIXME: get rid of this method */
#if 0
void * loadingScreen(void * arg){
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
    }

    return NULL;
}
#endif

static void loadingScreen1(LoadingContext & context, const Level::LevelInfo & levelInfo){
    int load_x = 80;
    int load_y = 220;
    const int infobox_width = 300;
    const int infobox_height = 150;
    Info info;
    const Font & myFont = Font::getFont(Global::DEFAULT_FONT, 24, 24);
    const Font & infoFont = Font::getFont(Global::DEFAULT_FONT, 24, 24);

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

    Global::debug(2) << "loading screen" << endl;

    Bitmap work(load_width, load_height);

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

    TimeCounter counter;

    bool firstDraw = true;

    while (! context.done()){

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
    }
}

/* shows some circles rotating around a center point */
/* FIXME: get rid of this method */
#if 0
void * loadingScreenSimple1(void * arg){
    Bitmap work(40, 40);
    Bitmap original(40, 40);
    original.BlitFromScreen(0, 0);
    Util::ThreadBoolean quit(done_loading, loading_screen_mutex);
    Global::speed_counter = 0;
    int angle = 0;
    int color1 = Bitmap::makeColor(0, 0, 0);
    int color2 = Bitmap::makeColor(0x00, 0x99, 0xff);
    int color3 = Bitmap::makeColor(0xff, 0x22, 0x33);
    int color4 = Bitmap::makeColor(0x44, 0x77, 0x33);
    /* the length of this array is the number of circles to show */
    int colors[4] = {color1, color2, color3, color4};
    Bitmap::transBlender(0, 0, 0, 64);
    /* speed of rotation */
    int speed = 7;
    while (! quit.get()){
        bool draw = false;

        if (Global::speed_counter > 0){
            double think = Global::speed_counter;	
            Global::speed_counter = 0;
            draw = true;

            while (think > 0){
                angle += speed;
                think -= 1;
            }
        } else {
            Util::rest(1);
        }

        if (draw){
            int max = sizeof(colors) / sizeof(int);
            double middleX = work.getWidth() / 2;
            double middleY = work.getHeight() / 2;
            original.Blit(work);
            for (int i = 0; i < max; i++){
                double x = cos(Util::radians(angle + 360 / max * i)) * 15;
                double y = sin(Util::radians(angle + 360 / max * i)) * 15;
                /* ghost circle */
                work.translucent().circleFill(middleX + x, middleY + y, 2, colors[i]);
                x = cos(Util::radians(angle + speed + 360 / max * i)) * 15;
                y = sin(Util::radians(angle + speed + 360 / max * i)) * 15;
                /* real circle */
                work.circleFill(middleX + x, middleY + y, 2, colors[i]);
            }
            work.BlitAreaToScreen(0, 0);
        }
    }

    return NULL;
}
#endif

static void loadingScreenSimpleX1(LoadingContext & context, const Level::LevelInfo & levelInfo){
    Bitmap work(40, 40);
    Bitmap original(40, 40);
    original.BlitFromScreen(0, 0);
    Global::speed_counter = 0;
    int angle = 0;
    int color1 = Bitmap::makeColor(0, 0, 0);
    int color2 = Bitmap::makeColor(0x00, 0x99, 0xff);
    int color3 = Bitmap::makeColor(0xff, 0x22, 0x33);
    int color4 = Bitmap::makeColor(0x44, 0x77, 0x33);
    /* the length of this array is the number of circles to show */
    int colors[4] = {color1, color2, color3, color4};
    Bitmap::transBlender(0, 0, 0, 64);
    /* speed of rotation */
    int speed = 7;
    while (! context.done()){
        bool draw = false;

        if (Global::speed_counter > 0){
            double think = Global::speed_counter;	
            Global::speed_counter = 0;
            draw = true;

            while (think > 0){
                angle += speed;
                think -= 1;
            }
        } else {
            Util::rest(1);
        }

        if (draw){
            int max = sizeof(colors) / sizeof(int);
            double middleX = work.getWidth() / 2;
            double middleY = work.getHeight() / 2;
            original.Blit(work);
            for (int i = 0; i < max; i++){
                double x = cos(Util::radians(angle + 360 / max * i)) * 15;
                double y = sin(Util::radians(angle + 360 / max * i)) * 15;
                /* ghost circle */
                work.translucent().circleFill(middleX + x, middleY + y, 2, colors[i]);
                x = cos(Util::radians(angle + speed + 360 / max * i)) * 15;
                y = sin(Util::radians(angle + speed + 360 / max * i)) * 15;
                /* real circle */
                work.circleFill(middleX + x, middleY + y, 2, colors[i]);
            }
            work.BlitAreaToScreen(0, 0);
        }
    }
}

LoadingContext::LoadingContext():
finished(false){
    Util::Thread::initializeLock(&lock);
}

LoadingContext::~LoadingContext(){
}

void LoadingContext::doLoad(){
    this->load();
    Util::Thread::acquireLock(&lock);
    finished = true;
    Util::Thread::releaseLock(&lock);
}

bool LoadingContext::done(){
    bool ok = false;
    Util::Thread::acquireLock(&lock);
    ok = this->finished;
    Util::Thread::releaseLock(&lock);
    return ok;
}
    
void * LoadingContext::load_it(void * arg){
    LoadingContext * context = (LoadingContext*) arg;
    context->doLoad();
    return NULL;
}

static void showLoadMessage(){
    int screenX = 80;
    int screenY = 50;
    Bitmap work(110, 50);
    work.BlitFromScreen(screenX, screenY);
    Bitmap top(110, 50);
    top.fill(Bitmap::makeColor(0, 0, 0));
    Font::getDefaultFont(25, 25).printf(10, 5, Bitmap::makeColor(192, 192, 192), top, "Loading", 0);
    Bitmap::transBlender(0, 0, 0, 200);
    top.drawTrans(0, 0, work);
    work.BlitAreaToScreen(screenX, screenY);
}

void loadScreen(LoadingContext & context, const Level::LevelInfo & info, Kind kind){
    Util::Thread::Id loadingThread;
    bool created = Util::Thread::createThread(&loadingThread, NULL, (Util::Thread::ThreadFunction) LoadingContext::load_it, &context);
    if (!created){
        Global::debug(0) << "Could not create loading thread. Loading will occur in the main thread" << endl;
        showLoadMessage();
        LoadingContext::load_it(&context);
        // throw LoadException(__FILE__, __LINE__, "Could not create loader thread");
    } else {
        switch (kind){
            case Default: loadingScreen1(context, info); break;
            case SimpleCircle: loadingScreenSimpleX1(context, info); break;
            default: loadingScreen1(context, info); break;
        }

        Util::Thread::joinThread(loadingThread);
    }
}

}
