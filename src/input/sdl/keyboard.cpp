#include <SDL.h>
#include "../keyboard.h"
#include "../input-manager.h"
#include "util/funcs.h"

Keyboard::Keyboard():
enableBuffer(false){
}

void Keyboard::poll(){
    buffer.clear();
    SDL_PumpEvents();
}

void Keyboard::wait(){
    while (keypressed()){
        Util::rest(1);
        poll();
    }
}

bool Keyboard::keypressed(){
    int keys = 0;
    Uint8 * state = SDL_GetKeyState(&keys);
    for (int i = 0; i < keys; i++){
        if (i != SDLK_NUMLOCK && state[i] == 1){
            return true;
        }
    }

    return false;
}
	
#if 0
void Keyboard::readKeys( std::vector<int> & all_keys ){
    int keys = 0;
    Uint8 * state = SDL_GetKeyState(&keys);
    /* FIXME: the mapping between SDLK_* and our keyboard values are not 1-1 so
     * use a function here that returns the right mapping.
     */
    for (int i = 0; i < keys; i++){
        if (i != SDLK_NUMLOCK && state[i] == 1){
            all_keys.push_back(i);
        }
    }
}
#endif

void Keyboard::readKeys(std::vector<int> & all_keys){
    for (std::map<KeyType, KeyData>::iterator it = keyState.begin(); it != keyState.end(); it++){
        KeyType key = it->first;
        const KeyData & data = it->second;
        if (data.enabled){
            all_keys.push_back(key);
        }
    }
}

/*
int Keyboard::readKey(){
    std::vector<int> keys;
    do{
        readKeys(keys);
        Util::rest(1);
        InputManager::poll();
        poll();
    } while (keys.size() == 0);
    return keys.front();
}
*/

void Keyboard::clear(){
    buffer.clear();
    keyState.clear();
}

Keyboard::KeyType Keyboard::Key_A = SDLK_a;
Keyboard::KeyType Keyboard::Key_B = SDLK_b;
Keyboard::KeyType Keyboard::Key_C = SDLK_c;
Keyboard::KeyType Keyboard::Key_D = SDLK_d;
Keyboard::KeyType Keyboard::Key_E = SDLK_e;
Keyboard::KeyType Keyboard::Key_F = SDLK_f;
Keyboard::KeyType Keyboard::Key_G = SDLK_g;
Keyboard::KeyType Keyboard::Key_H = SDLK_h;
Keyboard::KeyType Keyboard::Key_I = SDLK_i;
Keyboard::KeyType Keyboard::Key_J = SDLK_j;
Keyboard::KeyType Keyboard::Key_K = SDLK_k;
Keyboard::KeyType Keyboard::Key_L = SDLK_l;
Keyboard::KeyType Keyboard::Key_M = SDLK_m;
Keyboard::KeyType Keyboard::Key_N = SDLK_n;
Keyboard::KeyType Keyboard::Key_O = SDLK_o;
Keyboard::KeyType Keyboard::Key_P = SDLK_p;
Keyboard::KeyType Keyboard::Key_Q = SDLK_q;
Keyboard::KeyType Keyboard::Key_R = SDLK_r;
Keyboard::KeyType Keyboard::Key_S = SDLK_s;
Keyboard::KeyType Keyboard::Key_T = SDLK_t;
Keyboard::KeyType Keyboard::Key_U = SDLK_u;
Keyboard::KeyType Keyboard::Key_V = SDLK_v;
Keyboard::KeyType Keyboard::Key_W = SDLK_w;
Keyboard::KeyType Keyboard::Key_X = SDLK_x;
Keyboard::KeyType Keyboard::Key_Y = SDLK_y;
Keyboard::KeyType Keyboard::Key_Z = SDLK_z;
Keyboard::KeyType Keyboard::Key_0 = SDLK_0;
Keyboard::KeyType Keyboard::Key_1 = SDLK_1;
Keyboard::KeyType Keyboard::Key_2 = SDLK_2;
Keyboard::KeyType Keyboard::Key_3 = SDLK_3;
Keyboard::KeyType Keyboard::Key_4 = SDLK_4;
Keyboard::KeyType Keyboard::Key_5 = SDLK_5;
Keyboard::KeyType Keyboard::Key_6 = SDLK_6;
Keyboard::KeyType Keyboard::Key_7 = SDLK_7;
Keyboard::KeyType Keyboard::Key_8 = SDLK_8;
Keyboard::KeyType Keyboard::Key_9 = SDLK_9;
Keyboard::KeyType Keyboard::Key_0_PAD = SDLK_KP0;
Keyboard::KeyType Keyboard::Key_1_PAD = SDLK_KP1;
Keyboard::KeyType Keyboard::Key_2_PAD = SDLK_KP2;
Keyboard::KeyType Keyboard::Key_3_PAD = SDLK_KP3;
Keyboard::KeyType Keyboard::Key_4_PAD = SDLK_KP4;
Keyboard::KeyType Keyboard::Key_5_PAD = SDLK_KP5;
Keyboard::KeyType Keyboard::Key_6_PAD = SDLK_KP6;
Keyboard::KeyType Keyboard::Key_7_PAD = SDLK_KP7;
Keyboard::KeyType Keyboard::Key_8_PAD = SDLK_KP8;
Keyboard::KeyType Keyboard::Key_9_PAD = SDLK_KP9;
Keyboard::KeyType Keyboard::Key_F1 = SDLK_F1;
Keyboard::KeyType Keyboard::Key_F2 = SDLK_F2;
Keyboard::KeyType Keyboard::Key_F3 = SDLK_F3;
Keyboard::KeyType Keyboard::Key_F4 = SDLK_F4;
Keyboard::KeyType Keyboard::Key_F5 = SDLK_F5;
Keyboard::KeyType Keyboard::Key_F6 = SDLK_F6;
Keyboard::KeyType Keyboard::Key_F7 = SDLK_F7;
Keyboard::KeyType Keyboard::Key_F8 = SDLK_F8;
Keyboard::KeyType Keyboard::Key_F9 = SDLK_F9;
Keyboard::KeyType Keyboard::Key_F10 = SDLK_F10;
Keyboard::KeyType Keyboard::Key_F11 = SDLK_F11;
Keyboard::KeyType Keyboard::Key_F12 = SDLK_F12;
Keyboard::KeyType Keyboard::Key_ESC = SDLK_ESCAPE;
Keyboard::KeyType Keyboard::Key_TILDE = SDLK_BACKQUOTE;
Keyboard::KeyType Keyboard::Key_MINUS = SDLK_MINUS;
Keyboard::KeyType Keyboard::Key_EQUALS = SDLK_EQUALS;
Keyboard::KeyType Keyboard::Key_BACKSPACE = SDLK_BACKSPACE;
Keyboard::KeyType Keyboard::Key_TAB = SDLK_TAB;
Keyboard::KeyType Keyboard::Key_OPENBRACE = SDLK_LEFTBRACKET;
Keyboard::KeyType Keyboard::Key_CLOSEBRACE = SDLK_RIGHTBRACKET;
Keyboard::KeyType Keyboard::Key_ENTER = SDLK_RETURN;
Keyboard::KeyType Keyboard::Key_COLON = SDLK_COLON;
Keyboard::KeyType Keyboard::Key_QUOTE = SDLK_QUOTEDBL;
Keyboard::KeyType Keyboard::Key_BACKSLASH = SDLK_BACKSLASH;
Keyboard::KeyType Keyboard::Key_BACKSLASH2 = 999; /* FIXME */
Keyboard::KeyType Keyboard::Key_COMMA = SDLK_COMMA;
Keyboard::KeyType Keyboard::Key_STOP = SDLK_PERIOD;
Keyboard::KeyType Keyboard::Key_SLASH = SDLK_SLASH;
Keyboard::KeyType Keyboard::Key_SPACE = SDLK_SPACE;
Keyboard::KeyType Keyboard::Key_INSERT = SDLK_INSERT;
Keyboard::KeyType Keyboard::Key_DEL = SDLK_DELETE;
Keyboard::KeyType Keyboard::Key_HOME = SDLK_HOME;
Keyboard::KeyType Keyboard::Key_END = SDLK_END;
Keyboard::KeyType Keyboard::Key_PGUP = SDLK_PAGEUP;
Keyboard::KeyType Keyboard::Key_PGDN = SDLK_PAGEDOWN;
Keyboard::KeyType Keyboard::Key_LEFT = SDLK_LEFT;
Keyboard::KeyType Keyboard::Key_RIGHT = SDLK_RIGHT;
Keyboard::KeyType Keyboard::Key_UP = SDLK_UP;
Keyboard::KeyType Keyboard::Key_DOWN = SDLK_DOWN;
Keyboard::KeyType Keyboard::Key_SLASH_PAD = SDLK_KP_DIVIDE;
Keyboard::KeyType Keyboard::Key_ASTERISK = SDLK_ASTERISK;
Keyboard::KeyType Keyboard::Key_MINUS_PAD = SDLK_KP_MINUS;
Keyboard::KeyType Keyboard::Key_PLUS_PAD = SDLK_KP_PLUS;
Keyboard::KeyType Keyboard::Key_DEL_PAD = 1000; /* FIXME */
Keyboard::KeyType Keyboard::Key_ENTER_PAD = SDLK_KP_ENTER;
Keyboard::KeyType Keyboard::Key_PRTSCR = SDLK_PRINT;
Keyboard::KeyType Keyboard::Key_PAUSE = SDLK_PAUSE;
Keyboard::KeyType Keyboard::Key_ABNT_C1 = 1001; /* FIXME */
Keyboard::KeyType Keyboard::Key_YEN = 1002;
Keyboard::KeyType Keyboard::Key_KANA = 1003;
Keyboard::KeyType Keyboard::Key_CONVERT = 1004;
Keyboard::KeyType Keyboard::Key_NOCONVERT = 1005;
Keyboard::KeyType Keyboard::Key_AT = SDLK_AT;
Keyboard::KeyType Keyboard::Key_CIRCUMFLEX = SDLK_CARET;
Keyboard::KeyType Keyboard::Key_COLON2 = 1006;
Keyboard::KeyType Keyboard::Key_KANJI = 1007;
Keyboard::KeyType Keyboard::Key_EQUALS_PAD = SDLK_KP_EQUALS;
Keyboard::KeyType Keyboard::Key_BACKQUOTE = 1008;
Keyboard::KeyType Keyboard::Key_SEMICOLON = SDLK_SEMICOLON;
Keyboard::KeyType Keyboard::Key_COMMAND = 1009;

Keyboard::KeyType Keyboard::Key_MODIFIERS = 1010;
Keyboard::KeyType Keyboard::Key_LSHIFT = SDLK_LSHIFT;
Keyboard::KeyType Keyboard::Key_RSHIFT = SDLK_RSHIFT;
Keyboard::KeyType Keyboard::Key_LCONTROL = SDLK_LCTRL;
Keyboard::KeyType Keyboard::Key_RCONTROL = SDLK_RCTRL;
Keyboard::KeyType Keyboard::Key_ALT = SDLK_LALT;
Keyboard::KeyType Keyboard::Key_ALTGR = SDLK_RALT;
Keyboard::KeyType Keyboard::Key_LWIN = SDLK_LMETA;
Keyboard::KeyType Keyboard::Key_RWIN = SDLK_RMETA;
Keyboard::KeyType Keyboard::Key_MENU = SDLK_HELP;
Keyboard::KeyType Keyboard::Key_SCRLOCK = SDLK_SCROLLOCK;
Keyboard::KeyType Keyboard::Key_NUMLOCK = SDLK_NUMLOCK;
Keyboard::KeyType Keyboard::Key_CAPSLOCK = SDLK_CAPSLOCK;
