#ifdef USE_SDL2

#include <SDL2/SDL_keyboard.h>
#include "r-tech1/input/keyboard.h"

const Keyboard::KeyType Keyboard::Key_A = SDLK_a;
const Keyboard::KeyType Keyboard::Key_B = SDLK_b;
const Keyboard::KeyType Keyboard::Key_C = SDLK_c;
const Keyboard::KeyType Keyboard::Key_D = SDLK_d;
const Keyboard::KeyType Keyboard::Key_E = SDLK_e;
const Keyboard::KeyType Keyboard::Key_F = SDLK_f;
const Keyboard::KeyType Keyboard::Key_G = SDLK_g;
const Keyboard::KeyType Keyboard::Key_H = SDLK_h;
const Keyboard::KeyType Keyboard::Key_I = SDLK_i;
const Keyboard::KeyType Keyboard::Key_J = SDLK_j;
const Keyboard::KeyType Keyboard::Key_K = SDLK_k;
const Keyboard::KeyType Keyboard::Key_L = SDLK_l;
const Keyboard::KeyType Keyboard::Key_M = SDLK_m;
const Keyboard::KeyType Keyboard::Key_N = SDLK_n;
const Keyboard::KeyType Keyboard::Key_O = SDLK_o;
const Keyboard::KeyType Keyboard::Key_P = SDLK_p;
const Keyboard::KeyType Keyboard::Key_Q = SDLK_q;
const Keyboard::KeyType Keyboard::Key_R = SDLK_r;
const Keyboard::KeyType Keyboard::Key_S = SDLK_s;
const Keyboard::KeyType Keyboard::Key_T = SDLK_t;
const Keyboard::KeyType Keyboard::Key_U = SDLK_u;
const Keyboard::KeyType Keyboard::Key_V = SDLK_v;
const Keyboard::KeyType Keyboard::Key_W = SDLK_w;
const Keyboard::KeyType Keyboard::Key_X = SDLK_x;
const Keyboard::KeyType Keyboard::Key_Y = SDLK_y;
const Keyboard::KeyType Keyboard::Key_Z = SDLK_z;
const Keyboard::KeyType Keyboard::Key_0 = SDLK_0;
const Keyboard::KeyType Keyboard::Key_1 = SDLK_1;
const Keyboard::KeyType Keyboard::Key_2 = SDLK_2;
const Keyboard::KeyType Keyboard::Key_3 = SDLK_3;
const Keyboard::KeyType Keyboard::Key_4 = SDLK_4;
const Keyboard::KeyType Keyboard::Key_5 = SDLK_5;
const Keyboard::KeyType Keyboard::Key_6 = SDLK_6;
const Keyboard::KeyType Keyboard::Key_7 = SDLK_7;
const Keyboard::KeyType Keyboard::Key_8 = SDLK_8;
const Keyboard::KeyType Keyboard::Key_9 = SDLK_9;
const Keyboard::KeyType Keyboard::Key_0_PAD = SDLK_KP_0;
const Keyboard::KeyType Keyboard::Key_1_PAD = SDLK_KP_1;
const Keyboard::KeyType Keyboard::Key_2_PAD = SDLK_KP_2;
const Keyboard::KeyType Keyboard::Key_3_PAD = SDLK_KP_3;
const Keyboard::KeyType Keyboard::Key_4_PAD = SDLK_KP_4;
const Keyboard::KeyType Keyboard::Key_5_PAD = SDLK_KP_5;
const Keyboard::KeyType Keyboard::Key_6_PAD = SDLK_KP_6;
const Keyboard::KeyType Keyboard::Key_7_PAD = SDLK_KP_7;
const Keyboard::KeyType Keyboard::Key_8_PAD = SDLK_KP_8;
const Keyboard::KeyType Keyboard::Key_9_PAD = SDLK_KP_9;
const Keyboard::KeyType Keyboard::Key_F1 = SDLK_F1;
const Keyboard::KeyType Keyboard::Key_F2 = SDLK_F2;
const Keyboard::KeyType Keyboard::Key_F3 = SDLK_F3;
const Keyboard::KeyType Keyboard::Key_F4 = SDLK_F4;
const Keyboard::KeyType Keyboard::Key_F5 = SDLK_F5;
const Keyboard::KeyType Keyboard::Key_F6 = SDLK_F6;
const Keyboard::KeyType Keyboard::Key_F7 = SDLK_F7;
const Keyboard::KeyType Keyboard::Key_F8 = SDLK_F8;
const Keyboard::KeyType Keyboard::Key_F9 = SDLK_F9;
const Keyboard::KeyType Keyboard::Key_F10 = SDLK_F10;
const Keyboard::KeyType Keyboard::Key_F11 = SDLK_F11;
const Keyboard::KeyType Keyboard::Key_F12 = SDLK_F12;
const Keyboard::KeyType Keyboard::Key_ESC = SDLK_ESCAPE;
const Keyboard::KeyType Keyboard::Key_TILDE = SDLK_BACKQUOTE;
const Keyboard::KeyType Keyboard::Key_MINUS = SDLK_MINUS;
const Keyboard::KeyType Keyboard::Key_EQUALS = SDLK_EQUALS;
const Keyboard::KeyType Keyboard::Key_BACKSPACE = SDLK_BACKSPACE;
const Keyboard::KeyType Keyboard::Key_TAB = SDLK_TAB;
const Keyboard::KeyType Keyboard::Key_OPENBRACE = SDLK_LEFTBRACKET;
const Keyboard::KeyType Keyboard::Key_CLOSEBRACE = SDLK_RIGHTBRACKET;
const Keyboard::KeyType Keyboard::Key_ENTER = SDLK_RETURN;
const Keyboard::KeyType Keyboard::Key_COLON = SDLK_COLON;
const Keyboard::KeyType Keyboard::Key_QUOTE = SDLK_QUOTE;
const Keyboard::KeyType Keyboard::Key_BACKSLASH = SDLK_BACKSLASH;
// const Keyboard::KeyType Keyboard::Key_BACKSLASH2 = 0;
const Keyboard::KeyType Keyboard::Key_COMMA = SDLK_COMMA;
const Keyboard::KeyType Keyboard::Key_STOP = SDLK_STOP;
const Keyboard::KeyType Keyboard::Key_SLASH = SDLK_SLASH;
const Keyboard::KeyType Keyboard::Key_SPACE = SDLK_SPACE;
const Keyboard::KeyType Keyboard::Key_INSERT = SDLK_INSERT;
const Keyboard::KeyType Keyboard::Key_DEL = SDLK_DELETE;
const Keyboard::KeyType Keyboard::Key_HOME = SDLK_HOME;
const Keyboard::KeyType Keyboard::Key_END = SDLK_END;
const Keyboard::KeyType Keyboard::Key_PGUP = SDLK_PAGEUP;
const Keyboard::KeyType Keyboard::Key_PGDN = SDLK_PAGEDOWN;
const Keyboard::KeyType Keyboard::Key_LEFT = SDLK_LEFT;
const Keyboard::KeyType Keyboard::Key_RIGHT = SDLK_RIGHT;
const Keyboard::KeyType Keyboard::Key_UP = SDLK_UP;
const Keyboard::KeyType Keyboard::Key_DOWN = SDLK_DOWN;
const Keyboard::KeyType Keyboard::Key_SLASH_PAD = 0; // FIXME
const Keyboard::KeyType Keyboard::Key_ASTERISK = 0; // FIXME
const Keyboard::KeyType Keyboard::Key_MINUS_PAD = SDLK_KP_MINUS;
const Keyboard::KeyType Keyboard::Key_PLUS_PAD = SDLK_KP_PLUS;
const Keyboard::KeyType Keyboard::Key_DEL_PAD = 0; // FIXME
const Keyboard::KeyType Keyboard::Key_ENTER_PAD = SDLK_KP_ENTER;
const Keyboard::KeyType Keyboard::Key_PRTSCR = SDLK_PRINTSCREEN;
const Keyboard::KeyType Keyboard::Key_PAUSE = SDLK_PAUSE;
const Keyboard::KeyType Keyboard::Key_ABNT_C1 = 0;
const Keyboard::KeyType Keyboard::Key_YEN = 0;
const Keyboard::KeyType Keyboard::Key_KANA = 0;
const Keyboard::KeyType Keyboard::Key_CONVERT = 0;
const Keyboard::KeyType Keyboard::Key_NOCONVERT = 0;
const Keyboard::KeyType Keyboard::Key_AT = 0;
const Keyboard::KeyType Keyboard::Key_CIRCUMFLEX = 0;
const Keyboard::KeyType Keyboard::Key_COLON2 = 0;
const Keyboard::KeyType Keyboard::Key_KANJI = 0;
const Keyboard::KeyType Keyboard::Key_EQUALS_PAD = SDLK_EQUALS;
const Keyboard::KeyType Keyboard::Key_BACKQUOTE = SDLK_BACKQUOTE;
const Keyboard::KeyType Keyboard::Key_SEMICOLON = SDLK_SEMICOLON;
const Keyboard::KeyType Keyboard::Key_COMMAND = 0;

/*
const int Keyboard::Key_UNKNOWN1 = ALLEGRO_KEY_UNKNOWN1;
const int Keyboard::Key_UNKNOWN2 = ALLEGRO_KEY_UNKNOWN2;
const int Keyboard::Key_UNKNOWN3 = ALLEGRO_KEY_UNKNOWN3;
const int Keyboard::Key_UNKNOWN4 = ALLEGRO_KEY_UNKNOWN4;
const int Keyboard::Key_UNKNOWN5 = ALLEGRO_KEY_UNKNOWN5;
const int Keyboard::Key_UNKNOWN6 = ALLEGRO_KEY_UNKNOWN6;
const int Keyboard::Key_UNKNOWN7 = ALLEGRO_KEY_UNKNOWN7;
const int Keyboard::Key_UNKNOWN8 = ALLEGRO_KEY_UNKNOWN8;
*/

const Keyboard::KeyType Keyboard::Key_MODIFIERS = 0;
const Keyboard::KeyType Keyboard::Key_LSHIFT = 0;
const Keyboard::KeyType Keyboard::Key_RSHIFT = 0;
const Keyboard::KeyType Keyboard::Key_LCONTROL = 0;
const Keyboard::KeyType Keyboard::Key_RCONTROL = 0;
const Keyboard::KeyType Keyboard::Key_ALT = 0;
const Keyboard::KeyType Keyboard::Key_ALTGR = 0;
const Keyboard::KeyType Keyboard::Key_LWIN = 0;
const Keyboard::KeyType Keyboard::Key_RWIN = 0;
const Keyboard::KeyType Keyboard::Key_MENU = 0;
const Keyboard::KeyType Keyboard::Key_SCRLOCK = 0;
const Keyboard::KeyType Keyboard::Key_NUMLOCK = 0;
const Keyboard::KeyType Keyboard::Key_CAPSLOCK = 0;
    
Keyboard::Keyboard():
enableBuffer(false){
}

void Keyboard::disableKeyRepeat(){
    /* TODO */
}

void Keyboard::enableKeyRepeat(){
    /* TODO */
}

void Keyboard::readKeys(std::vector<int> & all_keys){
    for (std::map<KeyType, KeyData>::iterator it = keyState.begin(); it != keyState.end(); it++){
        KeyType key = it->first;
        const KeyData & data = it->second;
        if (data.enabled){
            all_keys.push_back(key);
        }
    }
}

bool Keyboard::keypressed(){
    /* TODO */
    return false;
}
    
void Keyboard::poll(){
    buffer.clear();
}

void Keyboard::clear(){
    buffer.clear();
    keyState.clear();
}

#endif
