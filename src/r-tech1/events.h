#ifndef _rtech1_events_h
#define _rtech1_events_h

/* handles global events from the system such as
 *   window manager events (press X button)
 *   keyboard/mouse/joystick input (for some backends like SDL)
 */

#include <vector>
#include <map>
#include "pointer.h"

#ifdef USE_ALLEGRO5
struct ALLEGRO_EVENT_SOURCE;
struct ALLEGRO_EVENT_QUEUE;
#endif

#ifdef USE_SDL
#include <SDL.h>
#endif

class Keyboard;
class Joystick;

namespace Graphics{
    class Bitmap;
}

namespace Util{

class WaitThread;

class EventManager{
public:
    EventManager();
    virtual void run(Keyboard & keyboard, std::map<int, ReferenceCount<Joystick> > joysticks);
    virtual void waitForThread(WaitThread & thread);
    virtual ~EventManager();

#ifdef USE_SDL
    typedef SDLKey KeyType;
#else
    typedef int KeyType;
#endif

    inline const std::vector<KeyType> & getBufferedKeys() const {
        return keys;
    }
    
    void deferResizeEvents(bool defer);

    void enableKeyBuffer();
    void disableKeyBuffer();

private:
    enum Event{
        CloseWindow,
        ResizeScreen,
        Key
    };

    struct ResizeEvent{
        Event type;
        int width;
        int height;
        bool enable;
    };

    virtual void dispatch(Event type, int arg1, int arg2);
    virtual void dispatch(Event type, int arg1);
    virtual void dispatch(Event type);

#ifdef USE_SDL
    virtual void runSDL(Keyboard &, std::map<int, ReferenceCount<Joystick> >);
#endif
#ifdef USE_ALLEGRO
    virtual void runAllegro(Keyboard & keyboard, std::map<int, ReferenceCount<Joystick> >);
#endif
#ifdef USE_ALLEGRO5
    virtual void runAllegro5(Keyboard & keyboard, const std::map<int, ReferenceCount<Joystick> > &);
    ALLEGRO_EVENT_QUEUE * queue;
#endif

    std::vector<KeyType> keys;
    bool bufferKeys;
    bool deferResize;
    /* Only need the last event */
    ResizeEvent resize;
};

/* implement these classes to get the standard run loop */
class Logic{
public:
    /* run a cycle of logic */
    virtual void run() = 0;
    /* the run loop should finish */
    virtual bool done() = 0;

    /* return a number of logic ticks for a given number of ticks on
     * a real system.
     */
    virtual double ticks(double systemTicks) = 0;

    virtual ~Logic();
};

class Draw{
public:
    Draw();
    /* give the drawer a chance to draw stuff to the screen before any logic occurs.
     * default implementation is to do nothing.
     */
    virtual void drawFirst(const Graphics::Bitmap & screen);
    /* standard draw method after logic has run */
    virtual void draw(const Graphics::Bitmap & screen) = 0;
    virtual ~Draw();
    /* called by the standardLoop */
    virtual void updateFrames();
    virtual double getFps() const;
protected:
    int frames;
    unsigned int second_counter;
    double fps;
};

void standardLoop(Logic & logic, Draw & draw);
/* true if the game should shutdown immediately */
bool shutdown();
extern int do_shutdown;

}

#endif
