#include "r-tech1/graphics/bitmap.h"
#include <math.h>
#include <iostream>

#include "r-tech1/messages.h"
#include "r-tech1/loading.h"
#include "r-tech1/file-system.h"
#include "r-tech1/font.h"
#include "r-tech1/funcs.h"
#include "r-tech1/version.h"
#include "r-tech1/graphics/gradient.h"
#include "r-tech1/parameter.h"
#include "r-tech1/thread.h"
#include <vector>
#include "r-tech1/thread.h"
#include "r-tech1/message-queue.h"
#include "r-tech1/init.h"
#include "r-tech1/events.h"
#include "r-tech1/input/input-map.h"
#include "r-tech1/input/input-manager.h"

using namespace std;

namespace Loader{

volatile bool done_loading = true;

typedef struct pair{
    int x, y;
} ppair;

class MessageInfo{
public:
    MessageInfo(){
        MessageQueue::registerInfo(&messages);
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

    ~MessageInfo(){
        MessageQueue::unregisterInfo(&messages);
    }

private:
    MessageQueue messages;
};

Info::Info(const string & message, const Filesystem::AbsolutePath & background):
x(-1),
y(-1),
_loadingMessage(message),
background(NULL),
_loadingBackground(background){
}

Info::Info(const Info & info){
    this->x = info.x;
    this->y = info.y;
    this->_loadingMessage = info._loadingMessage;
    this->background = info.background;
    this->_loadingBackground = info._loadingBackground;
}
    
Info::~Info(){
}
    
void Info::setBackground(const Graphics::Bitmap * background){
    this->background = background;
}

void Info::setLoadingMessage(const std::string & str){
    this->_loadingMessage = str;
}

void Info::addAuthorInfo(const std::vector<std::string> & info){
    this->_authorInfo = info;
}

void Info::setPosition(int x, int y){
    this->x = x;
    this->y = y;
}

const Graphics::Bitmap * Info::getBackground() const {
    return background;
}

const string & Info::loadingMessage() const {
    return _loadingMessage;
}

const std::vector<std::string> & Info::authorInfo() const {
    return _authorInfo;
}

const Filesystem::AbsolutePath & Info::loadingBackground() const {
    return _loadingBackground;
}

int Info::getPositionX() const {
    return x;
}

int Info::getPositionY() const {
    return y;
}

void * loadingScreenSimple1(void * arg);

/* converts a bitmap with some text on it into a sequence of points */
static vector<ppair> generateFontPixels(const Font & myFont, const string & message, int width, int height){
    Graphics::Bitmap letters(Graphics::Bitmap::createMemoryBitmap(width, height));
    letters.fill(Graphics::MaskColor());
    myFont.printf(0, 0, Graphics::makeColor(255, 255, 255), letters, message.c_str(), 0); 

    vector<ppair> pairs;
    for (int x = 0; x < letters.getWidth(); x++){
        for (int y = 0; y < letters.getHeight(); y++){
            Graphics::Color pixel = letters.getPixel(x, y);
            if (pixel != Graphics::MaskColor()){
                ppair p;
                p.x = x;
                p.y = y;
                pairs.push_back(p);
            }
        }
    }

    // Graphics::resetDisplay();

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
            font.printf(0, 0, Graphics::makeColor(192, 192, 192), work, "Waiting.. %d", 0, last - start);
            work.BlitAreaToScreen(x, y);
        }
    }

    Graphics::Bitmap work;
    unsigned int start;
    unsigned int last;
};

enum LoadingKeys{
    Activate
};

static void loadingScreen1(LoadingContext & context, const Info & levelInfo){
    int load_x = 80;
    int load_y = 220;
    const int infobox_width = 300;
    const int infobox_height = 150;
    const Font & myFont = Font::getDefaultFont(24, 24);

    if (levelInfo.getPositionX() != -1){
        load_x = levelInfo.getPositionX();
    }
    
    if (levelInfo.getPositionY() != -1){
        load_y = levelInfo.getPositionY();
    }

    // const char * the_string = (arg != NULL) ? (const char *) arg : "Loading...";
    int load_width = myFont.textLength(levelInfo.loadingMessage().c_str());
    int load_height = myFont.getHeight(levelInfo.loadingMessage().c_str());

    Global::debug(2) << "loading screen" << endl;

    Messages infobox(infobox_width, infobox_height);

    const int MAX_COLOR = 200;

    /* blend from dark grey to light red */
    Effects::Gradient gradient(MAX_COLOR, Graphics::makeColor(16, 16, 16), Graphics::makeColor(192, 8, 8));

    // TimeCounter counter;

    struct State{
        bool drawInfo;
    };

    class Logic: public Util::Logic {
    public:
        Logic(LoadingContext & context, State & state, Effects::Gradient & gradient, Messages & infoBox):
        context(context),
        state(state),
        gradient(gradient),
        infobox(infoBox),
        active(false){
            input.set(Keyboard::Key_SPACE, 0, true, Activate);
            input.set(Keyboard::Key_ENTER, 0, true, Activate);
            input.set(Joystick::Button1, 0, true, Activate);
        }

        LoadingContext & context;
        State & state;
        Effects::Gradient & gradient;
        MessageInfo info;
        Messages & infobox;
        bool active;
        InputMap<LoadingKeys> input;

        void doInput(){
            class Handler: public InputHandler<LoadingKeys> {
            public:
                Handler(bool & active):
                    active(active){
                    }

                bool & active;

                void press(const LoadingKeys & out, Keyboard::unicode_t unicode){
                    if (out == Activate){
                        /* the info box can't really be turned off because once
                         * its drawn it will remain there. the background would
                         * have to be drawn on top of it to remove the old
                         * info box. maybe do this in the future, if so use
                         *   active = ! active
                         * to toggle it.
                         */
                        active = ! active;
                    }
                }

                void release(const LoadingKeys & out, Keyboard::unicode_t unicode){
                }
            };

            Handler handler(active);
            InputManager::handleEvents(input, InputSource(true), handler);
        }

        void run(){
            for (int i = 0; i < 3; i++){
                gradient.backward();
            }
            doInput();
            info.transferMessages(infobox);
            // state.drawInfo = active && (info.transferMessages(infobox) || state.drawInfo);
            state.drawInfo = active;
        }

        double ticks(double system){
            return system * Global::ticksPerSecond(8);
        }

        bool done(){
            return context.done();
        }
    };

    class Draw: public Util::Draw {
    public:
        Draw(const Info & levelInfo, State & state, Messages & infobox, Effects::Gradient & gradient, int load_width, int load_height, int infobox_width, int infobox_height, int load_x, int load_y):
            levelInfo(levelInfo),
            gradient(gradient),
            state(state),
            infobox(infobox),
            infobox_x(load_x),
            infobox_y(load_y + load_height * 2),
            infobox_width(infobox_width),
            infobox_height(infobox_height),
            load_x(load_x),
            load_y(load_y),
            load_width(load_width),
            load_height(load_height){

            if (levelInfo.loadingBackground() != Filesystem::AbsolutePath("")){
                background = Graphics::Bitmap(levelInfo.loadingBackground().path());
            }

            const Font & myFont = Font::getDefaultFont(24, 24);
            pairs = generateFontPixels(myFont, levelInfo.loadingMessage(), load_width, load_height);
        }

        const Info & levelInfo;
        Effects::Gradient & gradient;
        State & state;
        Messages & infobox;
        Graphics::Bitmap background;
        // Graphics::Bitmap infoWork;
        // Graphics::Bitmap infoBackground;
        vector<ppair> pairs;
        const int infobox_x;
        const int infobox_y;
        const int infobox_width;
        const int infobox_height;
        const int load_x;
        const int load_y;
        const int load_width;
        const int load_height;

        void drawAuthorInfo(const Graphics::Bitmap & screen){
            int startX = screen.getWidth() - Font::getDefaultFont().textLength("R-tech1 version 9.9.9.9");
            int startY = screen.getHeight() - Font::getDefaultFont().getHeight() * (levelInfo.authorInfo().size() + 1);
            int height = Font::getDefaultFont().getHeight();

            Font::getDefaultFont().printf(startX, startY + height * 0, Graphics::makeColor(192, 192, 192), screen, "R-tech1 version %s", 0, Version::getVersionString().c_str());
            int mod = 1;
            for (std::vector<std::string>::const_iterator i = levelInfo.authorInfo().begin(); i != levelInfo.authorInfo().end(); ++i, mod++){
                Font::getDefaultFont().printf(startX, startY + height * mod, Graphics::makeColor(192, 192, 192), screen, (*i), 0);
            }
        }

        void draw(const Graphics::Bitmap & screen){
            if (!background.isEmpty()){
                background.drawStretched(screen);
            }
            Graphics::Bitmap work(screen, load_x, load_y, load_width, load_height);
            // work.lock();
            for (vector< ppair >::iterator it = pairs.begin(); it != pairs.end(); it++){
                Graphics::Color color = gradient.current(it->x);
                work.putPixel(it->x, it->y, color);
            }
            // work.unlock();

            if (state.drawInfo){
                // infoBackground.Blit(infoWork);

                const Font & infoFont = Font::getDefaultFont(24, 24);
                /* cheesy hack to change the font size. the font
                 * should store the size and change it on its own
                 */
                Graphics::Bitmap infoWork(screen, infobox_x, infobox_y, infobox_width, infobox_height);
                Font::getDefaultFont(13, 13);
                infobox.draw(0, 0, infoWork, infoFont);
                Font::getDefaultFont(24, 24);
                infoWork.BlitAreaToScreen(infobox_x, infobox_y);
                // infoWork.BlitToScreen();
                // state.drawInfo = false;
            }

            drawAuthorInfo(screen);
            /* work already contains the correct background */
            // work.Blit( load_x, load_y, *Bitmap::Screen );
            // work.BlitToScreen();
            // work.BlitAreaToScreen(load_x, load_y);
        }
    };

    State state;
    state.drawInfo = false;
    Logic logic(context, state, gradient, infobox);
    Draw draw(levelInfo, state, infobox, gradient, load_width, load_height, infobox_width, infobox_height, load_x, load_y);

    Util::standardLoop(logic, draw);
}

static void loadingScreenSimpleX1(LoadingContext & context, const Info & levelInfo){
    class Logic: public Util::Logic {
    public:
        Logic(LoadingContext & context, int & angle, int speed):
        context(context),
        speed(speed),
        angle(angle){
        }

        LoadingContext & context;
        /* speed of rotation */
        const int speed;
        int & angle;

        double ticks(double system){
            return system * Global::ticksPerSecond(10);
        }

        bool done(){
            return context.done();
        }

        void run(){
            angle += speed * 2;
            angle = angle % 360;
        }
    };

    class Draw: public Util::Draw {
    public:
        Draw(int & angle, const int speed):
        angle(angle),
        speed(speed){
            /*
            if (levelInfo.loadingBackground() != Filesystem::AbsolutePath("")){
                background = Graphics::Bitmap(levelInfo.loadingBackground().path());
            }
            */

            colors[0] = Graphics::makeColor(0x18, 0x52, 0xa0);
            colors[1] = Graphics::makeColor(0x00, 0x99, 0xff);
            colors[2] = Graphics::makeColor(0xff, 0x22, 0x33);
            colors[3] = Graphics::makeColor(0x44, 0x77, 0x33);
            // Graphics::Bitmap::transBlender(0, 0, 0, 64);
        }

        Graphics::Bitmap background;
        int & angle;
        const int speed;

        /* the length of this array is the number of circles to show */
        Graphics::Color colors[4];

        virtual ~Draw(){
        }

        void draw(const Graphics::Bitmap & screen){
            if (!background.isEmpty()){
                background.drawStretched(screen);
            }

            Graphics::Bitmap work(screen, 0, 0, 40, 40);
            int max = Util::array_size(colors);
            double middleX = work.getWidth() / 2;
            double middleY = work.getHeight() / 2;
            int size = 15;
            int radius = 3;
            for (int i = 0; i < max; i++){
                double new_angle = angle + 360.0 / (double) max * i;
                int lag = 17;
                double x = cos(Util::radians(new_angle - lag)) * size;
                double y = sin(Util::radians(new_angle - lag)) * size;
                /* ghost circle */
                work.translucent(0, 0, 0, 128).circleFill(middleX + x, middleY + y, radius, colors[i]);
                x = cos(Util::radians(new_angle)) * size;
                y = sin(Util::radians(new_angle)) * size;
                /* real circle */
                work.circleFill(middleX + x, middleY + y, radius, colors[i]);
            }
            // work.BlitAreaToScreen(0, 0);
        }
    };

    int angle = 0;
    int speed = 10;
    Logic logic(context, angle, speed);
    Draw draw(angle, speed);
    Util::standardLoop(logic, draw);
}

LoadingContext::LoadingContext():
finished(false){
    Util::Thread::initializeLock(&lock);
}

LoadingContext::~LoadingContext(){
    Util::Thread::destroyLock(&lock);
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
    Graphics::Bitmap work(110, 50);
    work.BlitFromScreen(screenX, screenY);
    Graphics::Bitmap top(110, 50);
    top.fill(Graphics::makeColor(0, 0, 0));
    Font::getDefaultFont(25, 25).printf(10, 5, Graphics::makeColor(192, 192, 192), top, "Loading", 0);
    // Graphics::Bitmap::transBlender(0, 0, 0, 200);
    top.translucent(200).draw(0, 0, work);
    work.BlitAreaToScreen(screenX, screenY);
}

void loadScreen(LoadingContext & context, const Info & info, Kind kind){
    std::thread loadingThread;
    loadingThread = std::thread([&](){
        context.doLoad();
    });

    InputManager::deferResizeEvents(true);
    switch (kind){
        case Default: loadingScreen1(context, info); break;
        case SimpleCircle: loadingScreenSimpleX1(context, info); break;
        default: loadingScreen1(context, info); break;
    }

    loadingThread.join();
    InputManager::deferResizeEvents(false);
}

}
