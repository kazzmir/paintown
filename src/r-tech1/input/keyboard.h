#ifndef _rtech1_keyboard_h
#define _rtech1_keyboard_h

#include <map>
#include <vector>
#include <set>
#include <stdint.h>

/* FIXME: use a namespace here. Input maybe? */

class KeyboardListener;

/* handles allegro key[] array better than keypressed()
 * and readkey()
 */
class Keyboard{
private:
    Keyboard();
public:
    friend class InputManager;

    typedef int KeyType;
    typedef uint32_t unicode_t;

    struct KeyData{
        KeyData():
        key(-1),
        unicode(-1),
        enabled(false){
        }

        KeyData(int key, unicode_t unicode, bool enabled):
        key(key),
        unicode(unicode),
        enabled(enabled){
        }

        int key;
        /* Converted to a unicode character in UTF-32 */
        unicode_t unicode;
        /* whether the key is being pressed */
        bool enabled;
    };

    /*
    typedef void (*ObserverCallback)(const KeyData & data, void * extra);
    struct Observer{
        Observer():
            callback(NULL),
            extra(NULL){
            }

        Observer(ObserverCallback callback, void * extra):
        callback(callback),
        extra(extra){
        }

        ObserverCallback callback;
        void * extra;
    };
    */

    /* poll:
     * Put the keys in Allegro's key[] array into our map of int -> bool
     */
    void poll();

    /* wait for all keys to be released */
    void wait();

    /* []:
     * Extract a boolean value given a key number
     */
#if 0
    inline bool operator[] ( const int i ){

        /* if the key has been pressed for the first time return true */
        if ( my_keys[ i ] < 0 ){
            my_keys[ i ] = 1;
            return true;
        }

        bool b = my_keys[ i ] > key_delay[ i ];
        if ( b ){
            my_keys[ i ] = 1;
        }
        return b;
    }
#endif

    /* keypressed:
     * Returns true if a key is pressed
     */
    bool keypressed();

    /* readKeys:
     * Store all pressed keys in a user supplied vector
     */
    void readKeys(std::vector< int > & all_keys);
    void readBufferedKeys(std::vector<int> & keys);
    std::vector<KeyData> readData();
    std::vector<unicode_t> readText();

    // int readKey();
    void clear();
    
    /* sets the latest repeat state. popping the state restores the previous state */
    static void pushRepeatState(bool enabled);
    static void popRepeatState();
    static bool getRepeatState();

    /* true on systems that probably have a keyboard, like pc.
     * false on ps3, wii, etc.
     */
    static bool haveKeyboard();

    void setDelay( const int key, const int delay );
    void setAllDelay( const int delay );

    inline const std::vector<KeyData> & getBufferedKeys() const {
        return buffer;
    }

    static const char * keyToName( int key );
    static bool isNumber( int key );
    static bool isChar( int key );
    static bool isAlpha( int key );

    static const KeyType Key_A;
    static const KeyType Key_B;
    static const KeyType Key_C;
    static const KeyType Key_D;
    static const KeyType Key_E;
    static const KeyType Key_F;
    static const KeyType Key_G;
    static const KeyType Key_H;
    static const KeyType Key_I;
    static const KeyType Key_J;
    static const KeyType Key_K;
    static const KeyType Key_L;
    static const KeyType Key_M;
    static const KeyType Key_N;
    static const KeyType Key_O;
    static const KeyType Key_P;
    static const KeyType Key_Q;
    static const KeyType Key_R;
    static const KeyType Key_S;
    static const KeyType Key_T;
    static const KeyType Key_U;
    static const KeyType Key_V;
    static const KeyType Key_W;
    static const KeyType Key_X;
    static const KeyType Key_Y;
    static const KeyType Key_Z;
    static const KeyType Key_0;
    static const KeyType Key_1;
    static const KeyType Key_2;
    static const KeyType Key_3;
    static const KeyType Key_4;
    static const KeyType Key_5;
    static const KeyType Key_6;
    static const KeyType Key_7;
    static const KeyType Key_8;
    static const KeyType Key_9;
    static const KeyType Key_0_PAD;
    static const KeyType Key_1_PAD;
    static const KeyType Key_2_PAD;
    static const KeyType Key_3_PAD;
    static const KeyType Key_4_PAD;
    static const KeyType Key_5_PAD;
    static const KeyType Key_6_PAD;
    static const KeyType Key_7_PAD;
    static const KeyType Key_8_PAD;
    static const KeyType Key_9_PAD;
    static const KeyType Key_F1;
    static const KeyType Key_F2;
    static const KeyType Key_F3;
    static const KeyType Key_F4;
    static const KeyType Key_F5;
    static const KeyType Key_F6;
    static const KeyType Key_F7;
    static const KeyType Key_F8;
    static const KeyType Key_F9;
    static const KeyType Key_F10;
    static const KeyType Key_F11;
    static const KeyType Key_F12;
    static const KeyType Key_ESC;
    static const KeyType Key_TILDE;
    static const KeyType Key_MINUS;
    static const KeyType Key_EQUALS;
    static const KeyType Key_BACKSPACE;
    static const KeyType Key_TAB;
    static const KeyType Key_OPENBRACE;
    static const KeyType Key_CLOSEBRACE;
    static const KeyType Key_ENTER;
    static const KeyType Key_COLON;
    static const KeyType Key_QUOTE;
    static const KeyType Key_BACKSLASH;
    // static const KeyType Key_BACKSLASH2;
    static const KeyType Key_COMMA;
    static const KeyType Key_STOP;
    static const KeyType Key_SLASH;
    static const KeyType Key_SPACE;
    static const KeyType Key_INSERT;
    static const KeyType Key_DEL;
    static const KeyType Key_HOME;
    static const KeyType Key_END;
    static const KeyType Key_PGUP;
    static const KeyType Key_PGDN;
    static const KeyType Key_LEFT;
    static const KeyType Key_RIGHT;
    static const KeyType Key_UP;
    static const KeyType Key_DOWN;
    static const KeyType Key_SLASH_PAD;
    static const KeyType Key_ASTERISK;
    static const KeyType Key_MINUS_PAD;
    static const KeyType Key_PLUS_PAD;
    static const KeyType Key_DEL_PAD;
    static const KeyType Key_ENTER_PAD;
    static const KeyType Key_PRTSCR;
    static const KeyType Key_PAUSE;
    static const KeyType Key_ABNT_C1;
    static const KeyType Key_YEN;
    static const KeyType Key_KANA;
    static const KeyType Key_CONVERT;
    static const KeyType Key_NOCONVERT;
    static const KeyType Key_AT;
    static const KeyType Key_CIRCUMFLEX;
    static const KeyType Key_COLON2;
    static const KeyType Key_KANJI;
    static const KeyType Key_EQUALS_PAD;
    static const KeyType Key_BACKQUOTE;
    static const KeyType Key_SEMICOLON;
    static const KeyType Key_COMMAND;
    /*
       static KeyType Key_UNKNOWN1;
       static KeyType Key_UNKNOWN2;
       static KeyType Key_UNKNOWN3;
       static KeyType Key_UNKNOWN4;
       static KeyType Key_UNKNOWN5;
       static KeyType Key_UNKNOWN6;
       static KeyType Key_UNKNOWN7;
       static KeyType Key_UNKNOWN8;
       */
    static const KeyType Key_MODIFIERS;
    static const KeyType Key_LSHIFT;
    static const KeyType Key_RSHIFT;
    static const KeyType Key_LCONTROL;
    static const KeyType Key_RCONTROL;
    static const KeyType Key_ALT;
    static const KeyType Key_ALTGR;
    static const KeyType Key_LWIN;
    static const KeyType Key_RWIN;
    static const KeyType Key_MENU;
    static const KeyType Key_SCRLOCK;
    static const KeyType Key_NUMLOCK;
    static const KeyType Key_CAPSLOCK;

    void press(KeyType key, unicode_t unicode);
    void release(KeyType key);
    /*
    virtual void addObserver(ObserverCallback observer, void * extra);
    virtual void removeObserver(ObserverCallback observer, void * extra);
    */
    
    static void addListener(KeyboardListener * listener);
    static void removeListener(KeyboardListener * listener);

    const std::set<KeyboardListener*> & getListeners();

protected:
    static void disableKeyRepeat();
    static void enableKeyRepeat();

    static std::set<KeyboardListener*> listeners;

    // std::map<int,int> my_keys;
    std::map<int,int> key_delay;
    // std::vector<Observer> observers;

    std::map<KeyType, KeyData> keyState;
    std::vector<KeyData> buffer;
    bool enableBuffer;
    static std::vector<bool> repeatState;
};

class KeyboardListener{
public:
    KeyboardListener();
    virtual ~KeyboardListener();
    virtual void press(Keyboard * from, Keyboard::KeyType key, Keyboard::unicode_t unicode) = 0;
    virtual void release(Keyboard * from, Keyboard::KeyType key) = 0;
};



#endif
