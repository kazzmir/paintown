#ifndef _rtech1_input_manager
#define _rtech1_input_manager

#include <vector>
#include <algorithm>
#include "input.h"
#include "input-map.h"
#include "input-source.h"
#include "touch.h"
#include "r-tech1/funcs.h"
#include "r-tech1/pointer.h"
#include "r-tech1/events.h"
#include "keyboard.h"
#include "joystick.h"
#include "touch.h"
#include "r-tech1/exceptions/exception.h"

class Configuration;
class InputSource;

template <class Output>
class InputHandler{
public:
    InputHandler(){
    }

    virtual ~InputHandler(){
    }

    virtual void press(const Output & out, Keyboard::unicode_t unicode) = 0;
    virtual void release(const Output & out, Keyboard::unicode_t unicode) = 0;
};

/* handles keyboard/joystick/whatever input during the game */
class InputManager{
public:
    /* main has one instance of this and thats it.
     * should the janitor have the reference instead?
     */
    friend int rtech_main(int, char**);
    friend int main(int, char **);

    /* returns true if any input device is activated (keys pressed, joystick button */
    static bool anyInput();

    static const Util::ReferenceCount<DeviceInput::Touch> & getTouch();

    // static std::vector<Input::PaintownInput> getInput(const Configuration & configuration, const int facing);
    static void poll();
    /*
    static void enableBufferInput();
    static void disableBufferInput();
    */
    static void waitForKeys(int key1, int key2, const InputSource & source);
    static void waitForKeys(int key1, const InputSource & source);
    static int readKey();
    static void waitForClear();

    static void deferResizeEvents(bool defer);
    
    static const std::map<int, Util::ReferenceCount<Joystick> > & getJoysticks();

    /*
    template <class X>
    static void observeKeyboard(InputMap<X> & input){
        manager->keyboard.addObserver(InputMap<X>::observeKey, &input);
    }
    
    template <class X>
    static void unObserveKeyboard(InputMap<X> & input){
        manager->keyboard.removeObserver(InputMap<X>::observeKey, &input);
    }
    */

    static std::vector<Keyboard::unicode_t> readText(){
        return manager->keyboard.readText();
    }

    template <typename X>
    static std::vector<Keyboard::unicode_t> readText(InputMap<X> & input, typename InputMap<X>::Output & output){
        return manager->_readText(input, output);
    }

#if 0
    template <typename X>
    static typename InputMap<X>::Output getMap(InputMap<X> & input){
        if (manager){
            return manager->_getMap(input);
        }
        /* just crash hard.. who cares */
        throw Exception::Base(__FILE__, __LINE__);
        /* make the compiler happy about returning something */
        return *(typename InputMap<X>::Output*)1;
    }
#endif

    template <typename X>
    static typename std::vector<typename InputMap<X>::InputEvent> getEvents(InputMap<X> & input, const InputSource & source){
        return manager->_getEvents(input, source);
    }

    template <typename X>
    static void handleEvents(InputMap<X> & input, const InputSource & source, InputHandler<X> & handler){
        typename std::vector<typename InputMap<X>::InputEvent> events = getEvents(input, source);
        for (typename std::vector<typename InputMap<X>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
            const typename InputMap<X>::InputEvent & event = *it;
            if (event.enabled){
                handler.press(event.out, event.unicode);
            } else {
                handler.release(event.out, event.unicode);
            }
        }
    }

    template <typename X>
    static bool pressed(InputMap<X> & input, const InputSource & source, X out){
        if (manager){
            return manager->_pressed(input, source, out);
        }
        return false;
    }

    /* wait for a key to be released
     * really this waits for all inputs that would result in `out'
     * being generated to stop.
     */
    template <typename X>
    static void waitForRelease(InputMap<X> & input, const InputSource & source, X out){
        while (InputManager::pressed(input, source, out)){
            Util::rest(1);
            InputManager::poll();
        }
    }

    template <typename X>
    static void waitForPress(InputMap<X> & input, const InputSource & source, X out){
        while (!InputManager::pressed(input, source, out)){
            Util::rest(1);
            InputManager::poll();
        }
    }

    template <typename X>
    static void captureInput(InputMap<X> & input){
        manager->_captureInput(input);
    }

    template <typename X>
    static void releaseInput(InputMap<X> & input){
        manager->_releaseInput(input);
    }

protected:
    InputManager();
    virtual ~InputManager();
    
    virtual bool _anyInput();

    virtual int _readKey();
    
    // virtual std::vector<Input::PaintownInput> _getInput(const Configuration & configuration, const int facing);

    template <typename X>
    void _captureInput(InputMap<X> & input){
        capture = (void*) &input;
    }

    template <typename X>
    void _releaseInput(InputMap<X> & input){
        if (capture == (void*) &input){
            capture = 0;
        }
    }

    template <typename X>
    std::vector<Keyboard::unicode_t> _readText(InputMap<X> & input, typename InputMap<X>::Output & output){
        std::vector<Keyboard::unicode_t> text;
        std::vector<Keyboard::KeyData> all = keyboard.readData();
        for (std::vector<Keyboard::KeyData>::iterator it = all.begin(); it != all.end(); it++){
            const Keyboard::KeyData & data = *it;
            KeyState<X> * state = input.getState(data.key);
            if (state != NULL && output[state->out]){
                text.push_back(data.unicode);
            }
        }

        return text;
    }

    std::vector<DeviceInput::Touch::Event> getTouchEvents();

    void removeDuplicates(std::vector<int> & storage){
        std::vector<int> output;
        int last = -1;
        for (std::vector<int>::iterator it = storage.begin(); it != storage.end(); it++){
            if (*it != last){
                output.push_back(*it);
                last = *it;
            }
        }
        storage = output;
    }

    template <typename X>
    typename std::vector<typename InputMap<X>::InputEvent> _getEvents(InputMap<X> & input, const InputSource & source){
        /* FIXME: get events from the source */
        std::vector<typename InputMap<X>::InputEvent> events;
        if (capture != NULL && capture != &input){
            return events;
        }

        if (source.useKeyboard()){
            const std::vector<typename Keyboard::KeyData> & buffer = keyboard.getBufferedKeys();
            for (std::vector<Keyboard::KeyData>::const_iterator it = buffer.begin(); it != buffer.end(); it++){
                const Keyboard::KeyData & data = *it;
                Util::ReferenceCount<KeyState<X> > state = input.getState(data.key);
                if (state != NULL){
                    events.push_back(typename InputMap<X>::InputEvent(state->out, data.unicode, data.enabled));
                }
            }
        }

        std::vector<DeviceInput::Touch::Event> touchEvents = getTouchEvents();
        for (std::vector<DeviceInput::Touch::Event>::iterator it = touchEvents.begin(); it != touchEvents.end(); it++){
            DeviceInput::Touch::Event event = *it;
            Util::ReferenceCount<TouchState<X> > state = input.getTouchState(event.key);
            if (state != NULL){
                events.push_back(typename InputMap<X>::InputEvent(state->out, -1, event.enabled));
            }
        }

        for (std::vector<int>::const_iterator it = source.getJoystick().begin(); it != source.getJoystick().end(); it++){
            int config = *it;
            if (config >= 0 && config < (int) joysticks.size()){
                Util::ReferenceCount<Joystick> joystick = joysticks[config];
                if (joystick != NULL){
                    const std::vector<typename Joystick::Event> & joystickEvents = joystick->getEvents();
                    for (std::vector<Joystick::Event>::const_iterator it = joystickEvents.begin(); it != joystickEvents.end(); it++){
                        Joystick::Event event = *it;
                        Util::ReferenceCount<JoystickState<X> > state = input.getJoystickState(event.key);
                        if (state != NULL){
                            events.push_back(typename InputMap<X>::InputEvent(state->out, -1, event.enabled));
                        }
                    }
                }
            }
        }

        return events;
    }

#if 0
    template <typename X>
    typename InputMap<X>::Output _getMap(InputMap<X> & input){
        typename InputMap<X>::Output output;

        if (capture != 0 && capture != &input){
            return output;
        }

        std::vector<int> all_keys;
        keyboard.readKeys(all_keys);
        keyboard.readBufferedKeys(all_keys);
        // all_keys.insert(all_keys.end(), bufferedKeys.begin(), bufferedKeys.end());
        // all_keys.insert(all_keys.end(), keyboard.currentKeys().begin(), keyboard.currentKeys.end());
        std::sort(all_keys.begin(), all_keys.end());
        removeDuplicates(all_keys);
        // std::unique(all_keys.begin(), all_keys.end());
        // bufferedKeys.clear();

        input.read(all_keys, &output);

        /*
        if (joystick != NULL){
            JoystickInput all_joystick = joystick->readAll();
            input.read(all_joystick, &output);
        }
        */

        /* just bumps an internal counter */
        input.update();

        return output;
    }
#endif

    template <typename X>
    bool _pressed(InputMap<X> & input, const InputSource & source, X result){
        /* FIXME: use the input source, luke */

        if (capture != 0 && capture != &input){
            return false;
        }

        std::vector<int> all_keys;
        keyboard.readKeys(all_keys);
        // all_keys.insert(all_keys.end(), bufferedKeys.begin(), bufferedKeys.end());
        std::sort(all_keys.begin(), all_keys.end());
        removeDuplicates(all_keys);
        // bufferedKeys.clear();
        bool out = false;

        out = input.pressed(all_keys, result);

        /*
        if (joystick != NULL){
            JoystickInput all_joystick = joystick->readAll();
            out |= input.pressed(all_joystick, result);
        }
        */

        return out;
    }

    virtual void _poll();

protected:
    void installJoysticks();
    void checkJoysticks();

private:
    static InputManager * manager;
    void * capture;
    std::map<int, Util::ReferenceCount<Joystick> > joysticks;
    Keyboard keyboard;
    Util::ReferenceCount<DeviceInput::Touch> touch;
    Util::EventManager eventManager;
    // std::vector<int> bufferedKeys;
    // bool bufferKeys;
};

#endif
