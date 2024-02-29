#ifdef USE_SDL
#include <SDL.h>
#endif
#ifdef USE_ALLEGRO5
#include <allegro5/allegro.h>
#endif
#include <vector>
#include <map>
#include <iostream>
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/events.h"
#include "r-tech1/exceptions/shutdown_exception.h"
#include "r-tech1/configuration.h"
#include "r-tech1/debug.h"
#include "r-tech1/funcs.h"
#include "r-tech1/thread.h"
#include "r-tech1/init.h"
#include "r-tech1/parameter.h"
#include "r-tech1/input/keyboard.h"
#include "r-tech1/input/joystick.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/input/input-source.h"

#include "r-tech1/system.h"

using std::map;

namespace Util{

int do_shutdown = 0;

bool shutdown(){
    return do_shutdown > 0;
}

EventManager::EventManager():
bufferKeys(false),
deferResize(false){
    resize.enable = false;
#ifdef USE_ALLEGRO5
    queue = al_create_event_queue();
    if (al_is_keyboard_installed()){
        al_register_event_source(queue, al_get_keyboard_event_source());
    }
    if (al_is_joystick_installed()){
        al_register_event_source(queue, al_get_joystick_event_source());
    }
    if (Graphics::the_display != NULL){
        al_register_event_source(queue, al_get_display_event_source(Graphics::the_display));
    }
#endif
}

#ifdef USE_SDL
static void handleKeyDown(Keyboard & keyboard, const SDL_Event & event){
    keyboard.press(event.key.keysym.sym, event.key.keysym.unicode);
}

static void handleKeyUp(Keyboard & keyboard, const SDL_Event & event){
    keyboard.release(event.key.keysym.sym);
}

static void handleJoystickButtonUp(map<int, ReferenceCount<Joystick> > joysticks, const SDL_Event & event){
    int device = event.jbutton.which;
    int button = event.jbutton.button;
    ReferenceCount<Joystick> joystick = joysticks[device];
    if (joystick != NULL){
        joystick->releaseButton(button);
    }
}

static void handleJoystickHat(map<int, ReferenceCount<Joystick> > joysticks, const SDL_Event & event){
    int device = event.jhat.which;
    int motion = event.jhat.value;
    ReferenceCount<Joystick> joystick = joysticks[device];
    if (joystick != NULL){
        joystick->hatMotion(motion);
    }

#if 0
    /* should up/down control left/right -- flip these values? */
#if DEVKITPRO
    const int axis_up_down = 0;
    const int axis_left_right = 1;
    const int up = 1;
    const int down = -1;
    const int left = -1;
    const int right = 1;
#else
    const int axis_up_down = 1;
    const int axis_left_right = 0;
    const int up = -1;
    const int down = 1;
    const int left = -1;
    const int right = 1;
#endif

    switch (motion){
        case SDL_HAT_CENTERED: break;
        case SDL_HAT_UP: joystick->axisMotion(axis_up_down, up); break;
        case SDL_HAT_DOWN: joystick->axisMotion(axis_up_down, down); break;
        case SDL_HAT_RIGHT: joystick->axisMotion(axis_left_right, right); break;
        case SDL_HAT_LEFT: joystick->axisMotion(axis_left_right, left); break;
        default: break;
    }
#endif
}

static void handleJoystickButtonDown(map<int, ReferenceCount<Joystick> > joysticks, const SDL_Event & event){
    int device = event.jbutton.which;
    int button = event.jbutton.button;
    ReferenceCount<Joystick> joystick = joysticks[device];
    if (joystick != NULL){
        joystick->pressButton(button);
    }
}

static void handleJoystickAxis(map<int, ReferenceCount<Joystick> > joysticks, const SDL_Event & event){
    int device = event.jaxis.which;
    int axis = event.jaxis.axis;
    int value = event.jaxis.value;
    ReferenceCount<Joystick> joystick = joysticks[device];
    if (joystick != NULL){
        joystick->axisMotion(axis, value);
    }
}

void EventManager::runSDL(Keyboard & keyboard, map<int, ReferenceCount<Joystick> > joysticks){
    keyboard.poll();
    for (map<int, ReferenceCount<Joystick> >::iterator it = joysticks.begin(); it != joysticks.end(); it++){
        ReferenceCount<Joystick> joystick = it->second;
        if (joystick != NULL){
            joystick->poll();
        }
    }
    SDL_Event event;
    /* FIXME: android gets into an infinite loop while reading events */
#ifdef ANDROID
    // int good = SDL_PollEvent(&event);
    // for (int check = 0; check < 10 && good; check++, good = SDL_PollEvent(&event)){
    // if (SDL_PollEvent(&event) == 1){
    while (SDL_PollEvent(&event) == 1){
#else
    while (SDL_PollEvent(&event) == 1){
#endif
        switch (event.type){
            case SDL_QUIT : {
                dispatch(CloseWindow);
                break;
            }
            case SDL_KEYDOWN : {
                handleKeyDown(keyboard, event);
                 // dispatch(Key, event.key.keysym.sym);
                 break;
            }
            case SDL_KEYUP : {
                handleKeyUp(keyboard, event);
                break;
            }
            case SDL_JOYBUTTONDOWN: {
                handleJoystickButtonDown(joysticks, event);
                break;
            }
            case SDL_JOYHATMOTION : {
                handleJoystickHat(joysticks, event);
                break;
            }
            case SDL_JOYBUTTONUP: {
                handleJoystickButtonUp(joysticks, event);
                break;
            }
            case SDL_JOYAXISMOTION: {
                handleJoystickAxis(joysticks, event);
                break;
            }
            case SDL_VIDEORESIZE : {
                int width = event.resize.w;
                int height = event.resize.h;
                /* to keep the perspective correct
                 * 640/480 = 1.33333
                 */
                /*
                double ratio = (double) 640 / (double) 480;
                if (width > height){
                    height = (int)((double) width / ratio);
                } else {
                    width = (int)((double) height * ratio);
                }
                */
                dispatch(ResizeScreen, width, height);
                break;
            }
            default : {
                break;
            }
        }
    }
}
#endif

#ifdef USE_SDL2
static void handleKeyDown(Keyboard & keyboard, const SDL_Event & event){
    // keyboard.press(event.key.keysym.sym, event.key.keysym.unicode);
    keyboard.press(event.key.keysym.sym, event.key.keysym.sym);
}

static void handleKeyUp(Keyboard & keyboard, const SDL_Event & event){
    keyboard.release(event.key.keysym.sym);
}

void EventManager::runSDL2(Keyboard & keyboard, map<int, ReferenceCount<Joystick> > joysticks){
    // DebugLog << "EventManager::runSDL2" << std::endl;

    keyboard.poll();
    for (map<int, ReferenceCount<Joystick> >::iterator it = joysticks.begin(); it != joysticks.end(); it++){
        ReferenceCount<Joystick> joystick = it->second;
        if (joystick != NULL){
            joystick->poll();
        }
    }
    SDL_Event event;
    /* FIXME: android gets into an infinite loop while reading events */
#ifdef ANDROID
    // int good = SDL_PollEvent(&event);
    // for (int check = 0; check < 10 && good; check++, good = SDL_PollEvent(&event)){
    // if (SDL_PollEvent(&event) == 1){
    while (SDL_PollEvent(&event) == 1){
#else
    while (SDL_PollEvent(&event) == 1){
#endif
        // DebugLog << "sdl event type: " << event.type << std::endl;
        switch (event.type){
            case SDL_QUIT : {
                dispatch(CloseWindow);
                break;
            }
            case SDL_KEYDOWN : {
                handleKeyDown(keyboard, event);
                 // dispatch(Key, event.key.keysym.sym);
                 break;
            }
            case SDL_KEYUP : {
                handleKeyUp(keyboard, event);
                break;
            }
                            /*
            case SDL_JOYBUTTONDOWN: {
                handleJoystickButtonDown(joysticks, event);
                break;
            }
            case SDL_JOYHATMOTION : {
                handleJoystickHat(joysticks, event);
                break;
            }
            case SDL_JOYBUTTONUP: {
                handleJoystickButtonUp(joysticks, event);
                break;
            }
            case SDL_JOYAXISMOTION: {
                handleJoystickAxis(joysticks, event);
                break;
            }
            */
            case SDL_WINDOWEVENT: {
                switch (event.window.event){
                    case SDL_WINDOWEVENT_RESIZED: {
                        int width = event.window.data1;
                        int height = event.window.data2;
                        /* to keep the perspective correct
                         * 640/480 = 1.33333
                         */
                        /*
                        double ratio = (double) 640 / (double) 480;
                        if (width > height){
                            height = (int)((double) width / ratio);
                        } else {
                            width = (int)((double) height * ratio);
                        }
                        */
                        dispatch(ResizeScreen, width, height);
                        break;
                    }
                    default : {
                        break;
                    }
                }
                break;
            }
            default : {
                break;
            }
        }
    }
}
#endif


#ifdef USE_ALLEGRO
void EventManager::runAllegro(Keyboard & keyboard, map<int, ReferenceCount<Joystick> > joystick){
    keyboard.poll();
}
#endif

#ifdef USE_ALLEGRO5
static void handleKeyDown(Keyboard & keyboard, const ALLEGRO_EVENT & event){
    keyboard.press(event.keyboard.keycode, event.keyboard.unichar);
}

static void handleKeyUp(Keyboard & keyboard, const ALLEGRO_EVENT & event){
    keyboard.release(event.keyboard.keycode);
}

static void handleResize(const ALLEGRO_EVENT & event){
    double width = event.display.width;
    double height = event.display.height;
    /*
    if (width < 640){
        width = 640;
    }
    if (height < 480){
        height = 480;
    }
    */
    /* to keep the perspective correct
     * 640/480 = 1.33333
     */
    /*
    double ratio = (double) 640 / (double) 480;
    if (width > height){
        height = width / ratio;
    } else {
        width = height * ratio;
    }
    */
    
    ALLEGRO_DISPLAY * display = event.display.source;
    al_acknowledge_resize(display);
    Configuration::setScreenWidth((int) width);
    Configuration::setScreenHeight((int) height);
    al_resize_display(display, (int) width, (int) height);
    
    Graphics::getScreenBuffer()->clear();

    /*
    ALLEGRO_TRANSFORM transformation;
    al_identity_transform(&transformation);
    // al_scale_transform(&transformation, (double) al_get_display_width(display) / (double) GFX_X, (double) al_get_display_height(display) / (double) GFX_Y);
    al_scale_transform(&transformation, (double) width / (double) GFX_X, (double) height / (double) GFX_Y);
    al_set_target_bitmap(Graphics::getScreenBuffer()->getData()->getBitmap());
    al_use_transform(&transformation);
    */
}

void EventManager::runAllegro5(Keyboard & keyboard, const map<int, ReferenceCount<Joystick> > & joysticks){
    keyboard.poll();

    for (map<int, ReferenceCount<Joystick> >::const_iterator it = joysticks.begin(); it != joysticks.end(); it++){
        ReferenceCount<Joystick> joystick = it->second;
        if (joystick != NULL){
            joystick->poll();
        }
    }

    ALLEGRO_EVENT event;
    while (al_get_next_event(queue, &event)){
        switch (event.type){
            /*
            case ALLEGRO_EVENT_KEY_DOWN: {
                Global::debug(0) << "Key down " << event.keyboard.keycode << std::endl;
                handleKeyDown(keyboard, event);
                break;
            }
            */
            case ALLEGRO_EVENT_DISPLAY_RESIZE: {
                handleResize(event);
                break;
            }
            case ALLEGRO_EVENT_DISPLAY_CLOSE: {
                throw ShutdownException();
                break;
            }
            case ALLEGRO_EVENT_KEY_UP: {
                handleKeyUp(keyboard, event);
                break;
            }
            case ALLEGRO_EVENT_KEY_CHAR : {
                // Global::debug(0) << "Key char " << event.keyboard.keycode << " unicode " << event.keyboard.unichar << std::endl;
                bool ok = true;
                if (event.keyboard.repeat){
                    ok = Keyboard::getRepeatState();
                }
                if (ok){
                    handleKeyDown(keyboard, event);
                }
                break;
            }
            case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION: {
                /* FIXME: tell input manager to reconfigure joysticks */
                // al_reconfigure_joysticks();
                break;
            }
        }
    }

    /*
    if (joystick){
        joystick->poll();
    }
    SDL_Event event;
    while (SDL_PollEvent(&event) == 1){
        switch (event.type){
            case SDL_QUIT : {
                dispatch(CloseWindow);
                break;
            }
            case SDL_KEYDOWN : {
                handleKeyDown(keyboard, event);
                 // dispatch(Key, event.key.keysym.sym);
                 break;
            }
            case SDL_KEYUP : {
                handleKeyUp(keyboard, event);
                break;
            }
            case SDL_JOYBUTTONDOWN: {
                if (joystick != NULL){
                    handleJoystickButtonDown(joystick, event);
                }
                break;
            }
            case SDL_JOYHATMOTION : {
                if (joystick != NULL){
                    handleJoystickHat(joystick, event);
                }
                break;
            }
            case SDL_JOYBUTTONUP: {
                if (joystick != NULL){
                    handleJoystickButtonUp(joystick, event);
                }
                break;
            }
            case SDL_JOYAXISMOTION: {
                if (joystick != NULL){
                    handleJoystickAxis(joystick, event);
                }
                break;
            }
            case SDL_VIDEORESIZE : {
                int width = event.resize.w;
                int height = event.resize.h;
                / * to keep the perspective correct
                 * 640/480 = 1.33333
                 * /
                if (width > height){
                    height = (int)((double) width / 1.3333333333);
                } else {
                    width = (int)((double) height * 1.3333333333);
                }
                dispatch(ResizeScreen, width, height);
                break;
            }
            default : {
                break;
            }
        }
    }
    */
}
#endif

void EventManager::run(Keyboard & keyboard, std::map<int, ReferenceCount<Joystick> > joysticks){
#ifdef USE_SDL
    runSDL(keyboard, joysticks);
#elif USE_SDL2
    runSDL2(keyboard, joysticks);
#elif USE_ALLEGRO
    runAllegro(keyboard, joysticks);
#elif USE_ALLEGRO5
    runAllegro5(keyboard, joysticks);
#endif
}

/* kill the program if the user requests */
void EventManager::waitForThread(WaitThread & thread){
    // Keyboard dummy;
    while (!thread.isRunning()){
        try{
            /* input manager will run the event manager */
            InputManager::poll();
            // run(dummy);
        } catch (const ShutdownException & death){
            thread.kill();
            throw death;
        }
        Util::rest(10);
    }
}

EventManager::~EventManager(){
#ifdef USE_ALLEGRO5
    al_destroy_event_queue(queue);
#endif
}
    
void EventManager::enableKeyBuffer(){
    bufferKeys = true;
}

void EventManager::disableKeyBuffer(){
    bufferKeys = false;
}

void EventManager::dispatch(Event type, int arg1){
    switch (type){
        case Key : {
            if (bufferKeys){
                keys.push_back(KeyType(arg1));
            }
            break;
        }
        default : {
            break;
        }
    }
}

void EventManager::dispatch(Event type, int arg1, int arg2){
    switch (type){
        case ResizeScreen : {
            if (deferResize){
                resize.type = ResizeScreen;
                resize.width = arg1;
                resize.height = arg2;
                resize.enable = true;
            } else {
                DebugLog1 << "Resizing screen to " << arg1 << ", " << arg2 << std::endl;
                if (Graphics::changeGraphicsMode(0, arg1, arg2) == 0){
                    Configuration::setScreenWidth(arg1);
                    Configuration::setScreenHeight(arg2);
                }
            }
            break;
        }
        default: break;
    }
}

void EventManager::dispatch(Event type){
    switch (type){
        case CloseWindow : {
            throw ShutdownException();
        }
        default : break;
    }
}
    
void EventManager::deferResizeEvents(bool defer){
    deferResize = defer;
    if (!deferResize && resize.enable){
        dispatch(resize.type, resize.width, resize.height);
        resize.enable = false;
    }
}

class LoopDone: public std::exception {
public:
    LoopDone(){
    }

    ~LoopDone() throw () {
    }
};

Logic::~Logic(){
}

Draw::Draw():
frames(0),
second_counter(Global::second_counter),
fps(0){
}

void Draw::drawFirst(const Graphics::Bitmap & screen){
    draw(screen);
}

Draw::~Draw(){
}
    
double Draw::getFps() const {
    return fps;
}

void Draw::updateFrames(){
    if (second_counter != Global::second_counter){
        int difference = Global::second_counter - second_counter;
        double alpha = 0.2;
        /* unlikely, but just in case */
        if (difference == 0){
            difference = 1;
        }
        fps = (alpha * fps) + ((1 - alpha) * (double) frames / difference);
        // fps[fps_index] = (double) frames / (double) difference;
        // fps_index = (fps_index+1) % max_fps_index;
        second_counter = Global::second_counter;
        frames = 0;
    }

    frames += 1;
}

static void changeScreenMode(){
    Configuration::setFullscreen(!Configuration::getFullscreen());
    int gfx = (Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
    Graphics::changeGraphicsMode(gfx, Graphics::Bitmap::getScreenWidth(), Graphics::Bitmap::getScreenHeight());
}

static void checkFullscreen(){
    InputMap<int> input;
    input.set(Keyboard::Key_F11, 0, true, 5);
    std::vector<InputMap<int>::InputEvent> events = InputManager::getEvents(input, InputSource(true));

    for (std::vector<InputMap<int>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
        InputMap<int>::InputEvent event = *it;

        if (!event.enabled){
            continue;
        }

        if (event.out == 5){
            changeScreenMode();
        }
    }
}

/* FIXME: allow the user to customize this setting */
static bool useTouchOverlay(){
#ifdef ANDROID
    return true;
#endif
    return false;
}

static void doStandardLoop(Logic & logic, Draw & draw){
    if (Graphics::screenParameter.current() == NULL){
        throw Exception::Base(__FILE__, __LINE__, "no screen parameter set");
    }
    const Graphics::Bitmap & screen = *Graphics::screenParameter.current();
    screen.clear();
    draw.drawFirst(screen);
    screen.BlitToScreen();
    Global::speed_counter4 = 0;
    double runCounter = 0;
    try{
        const int maxCount = 20;
        int frameCount = 0;
        uint64_t frameTime = 0;
        int logicCount = 0;
        uint64_t logicTime = 0;
        while (!logic.done()){
            if (Global::speed_counter4 > 0){
                // Global::debug(0) << "Speed counter " << Global::speed_counter4 << std::endl;
                runCounter += logic.ticks(Global::speed_counter4);
                Global::speed_counter4 = 0;
                bool need_draw = false;
                // Global::debug(0) << "Run counter: " << runCounter << std::endl;
                while (runCounter >= 1.0){
                    need_draw = true;
                    InputManager::poll();
                    checkFullscreen();
                    runCounter -= 1;
                    logicCount += 1;
                    uint64_t now = System::currentMilliseconds();
                    logic.run();
                    uint64_t later = System::currentMilliseconds();
                    logicTime += (later - now);

                    if (logicCount >= maxCount){
                        // Global::debug(0) << "Logic average " << (logicTime / logicCount) << "ms" << std::endl;
                        logicCount = 0;
                        logicTime = 0;
                    }

                    if (shutdown()){
                        throw ShutdownException();
                    }

                    if (logic.done()){
                        /* quit the loop immediately */
                        throw LoopDone();
                    }
                }

                if (need_draw){
                    frameCount += 1;
                    draw.updateFrames();
                    uint64_t now = System::currentMilliseconds();
                    screen.clear();
                    // draw.draw(screen.aspectRatio(640, 480));
                    draw.draw(screen);
                    if (useTouchOverlay()){
                        InputManager::getTouch()->drawTouchOverlay(screen);
                    }
                    screen.BlitToScreen();
                    uint64_t later = System::currentMilliseconds();
                    frameTime += (later - now);

                    if (frameCount >= maxCount){
                        DebugLog2 << "Draw average " << (frameTime / frameCount) << "ms" << std::endl;
                        frameCount = 0;
                        frameTime = 0;
                    }
                }
            }

            while (Global::speed_counter4 == 0){
                if (shutdown()){
                    throw ShutdownException();
                }

                /* if the fps is limited then don't keep redrawing */
                if (Global::rateLimit){
                    rest(1);
                } else {
                    draw.updateFrames();
                    screen.clear();
                    // draw.draw(screen.aspectRatio(640, 480));
                    draw.draw(screen);
                    if (useTouchOverlay()){
                        InputManager::getTouch()->drawTouchOverlay(screen);
                    }
                    screen.BlitToScreen();
                }
            }
        }
    } catch (const LoopDone & done){
    }
}

void standardLoop(Logic & logic, Draw & draw){
    /* if a screen already exists (because we have nested standardLoops) then
     * leave this parameter alone, otherwise set a new parameter.
     */
    /*
    if (Parameter<Graphics::Bitmap*>::current() == NULL){
        doStandardLoop(logic, draw);
    } else {
        doStandardLoop(logic, draw);
    }
    */
    doStandardLoop(logic, draw);
}

void standardLoop(std::function<bool()> logic, std::function<double(double)> ticks, std::function<void(const Graphics::Bitmap &)> draw){
    class LocalLogic: public Logic {
    public:
        std::function<bool()> logic;
        std::function<double(double)> ticks_;
        bool is_done;

        LocalLogic(std::function<bool()> logic, std::function<double(double)> ticks):
        logic(logic),
        ticks_(ticks),
        is_done(false){
        }

        virtual bool done(){
            return is_done;
        }

        virtual void run(){
            is_done = logic() || is_done;
        }

        virtual double ticks(double systemTicks){
            return ticks_(systemTicks);
        }

    };

    class LocalDraw: public Draw {
    public:
        std::function<void(const Graphics::Bitmap &)> draw_;

        LocalDraw(std::function<void(const Graphics::Bitmap &)> draw):
        draw_(draw){
        }

        virtual void drawFirst(const Graphics::Bitmap & screen){
            draw_(screen);
        }

        virtual void draw(const Graphics::Bitmap & screen){
            draw_(screen);
        }
    };

    LocalLogic logicObject(logic, ticks);
    LocalDraw drawObject(draw);

    standardLoop(logicObject, drawObject);
}

}
