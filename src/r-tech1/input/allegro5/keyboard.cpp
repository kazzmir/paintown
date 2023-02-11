#include "r-tech1/input/keyboard.h"
#include <allegro5/keycodes.h>

const Keyboard::KeyType Keyboard::Key_A = ALLEGRO_KEY_A;
const Keyboard::KeyType Keyboard::Key_B = ALLEGRO_KEY_B;
const Keyboard::KeyType Keyboard::Key_C = ALLEGRO_KEY_C;
const Keyboard::KeyType Keyboard::Key_D = ALLEGRO_KEY_D;
const Keyboard::KeyType Keyboard::Key_E = ALLEGRO_KEY_E;
const Keyboard::KeyType Keyboard::Key_F = ALLEGRO_KEY_F;
const Keyboard::KeyType Keyboard::Key_G = ALLEGRO_KEY_G;
const Keyboard::KeyType Keyboard::Key_H = ALLEGRO_KEY_H;
const Keyboard::KeyType Keyboard::Key_I = ALLEGRO_KEY_I;
const Keyboard::KeyType Keyboard::Key_J = ALLEGRO_KEY_J;
const Keyboard::KeyType Keyboard::Key_K = ALLEGRO_KEY_K;
const Keyboard::KeyType Keyboard::Key_L = ALLEGRO_KEY_L;
const Keyboard::KeyType Keyboard::Key_M = ALLEGRO_KEY_M;
const Keyboard::KeyType Keyboard::Key_N = ALLEGRO_KEY_N;
const Keyboard::KeyType Keyboard::Key_O = ALLEGRO_KEY_O;
const Keyboard::KeyType Keyboard::Key_P = ALLEGRO_KEY_P;
const Keyboard::KeyType Keyboard::Key_Q = ALLEGRO_KEY_Q;
const Keyboard::KeyType Keyboard::Key_R = ALLEGRO_KEY_R;
const Keyboard::KeyType Keyboard::Key_S = ALLEGRO_KEY_S;
const Keyboard::KeyType Keyboard::Key_T = ALLEGRO_KEY_T;
const Keyboard::KeyType Keyboard::Key_U = ALLEGRO_KEY_U;
const Keyboard::KeyType Keyboard::Key_V = ALLEGRO_KEY_V;
const Keyboard::KeyType Keyboard::Key_W = ALLEGRO_KEY_W;
const Keyboard::KeyType Keyboard::Key_X = ALLEGRO_KEY_X;
const Keyboard::KeyType Keyboard::Key_Y = ALLEGRO_KEY_Y;
const Keyboard::KeyType Keyboard::Key_Z = ALLEGRO_KEY_Z;
const Keyboard::KeyType Keyboard::Key_0 = ALLEGRO_KEY_0;
const Keyboard::KeyType Keyboard::Key_1 = ALLEGRO_KEY_1;
const Keyboard::KeyType Keyboard::Key_2 = ALLEGRO_KEY_2;
const Keyboard::KeyType Keyboard::Key_3 = ALLEGRO_KEY_3;
const Keyboard::KeyType Keyboard::Key_4 = ALLEGRO_KEY_4;
const Keyboard::KeyType Keyboard::Key_5 = ALLEGRO_KEY_5;
const Keyboard::KeyType Keyboard::Key_6 = ALLEGRO_KEY_6;
const Keyboard::KeyType Keyboard::Key_7 = ALLEGRO_KEY_7;
const Keyboard::KeyType Keyboard::Key_8 = ALLEGRO_KEY_8;
const Keyboard::KeyType Keyboard::Key_9 = ALLEGRO_KEY_9;
const Keyboard::KeyType Keyboard::Key_0_PAD = ALLEGRO_KEY_PAD_0;
const Keyboard::KeyType Keyboard::Key_1_PAD = ALLEGRO_KEY_PAD_1;
const Keyboard::KeyType Keyboard::Key_2_PAD = ALLEGRO_KEY_PAD_2;
const Keyboard::KeyType Keyboard::Key_3_PAD = ALLEGRO_KEY_PAD_3;
const Keyboard::KeyType Keyboard::Key_4_PAD = ALLEGRO_KEY_PAD_4;
const Keyboard::KeyType Keyboard::Key_5_PAD = ALLEGRO_KEY_PAD_5;
const Keyboard::KeyType Keyboard::Key_6_PAD = ALLEGRO_KEY_PAD_6;
const Keyboard::KeyType Keyboard::Key_7_PAD = ALLEGRO_KEY_PAD_7;
const Keyboard::KeyType Keyboard::Key_8_PAD = ALLEGRO_KEY_PAD_8;
const Keyboard::KeyType Keyboard::Key_9_PAD = ALLEGRO_KEY_PAD_9;
const Keyboard::KeyType Keyboard::Key_F1 = ALLEGRO_KEY_F1;
const Keyboard::KeyType Keyboard::Key_F2 = ALLEGRO_KEY_F2;
const Keyboard::KeyType Keyboard::Key_F3 = ALLEGRO_KEY_F3;
const Keyboard::KeyType Keyboard::Key_F4 = ALLEGRO_KEY_F4;
const Keyboard::KeyType Keyboard::Key_F5 = ALLEGRO_KEY_F5;
const Keyboard::KeyType Keyboard::Key_F6 = ALLEGRO_KEY_F6;
const Keyboard::KeyType Keyboard::Key_F7 = ALLEGRO_KEY_F7;
const Keyboard::KeyType Keyboard::Key_F8 = ALLEGRO_KEY_F8;
const Keyboard::KeyType Keyboard::Key_F9 = ALLEGRO_KEY_F9;
const Keyboard::KeyType Keyboard::Key_F10 = ALLEGRO_KEY_F10;
const Keyboard::KeyType Keyboard::Key_F11 = ALLEGRO_KEY_F11;
const Keyboard::KeyType Keyboard::Key_F12 = ALLEGRO_KEY_F12;
const Keyboard::KeyType Keyboard::Key_ESC = ALLEGRO_KEY_ESCAPE;
const Keyboard::KeyType Keyboard::Key_TILDE = ALLEGRO_KEY_TILDE;
const Keyboard::KeyType Keyboard::Key_MINUS = ALLEGRO_KEY_MINUS;
const Keyboard::KeyType Keyboard::Key_EQUALS = ALLEGRO_KEY_EQUALS;
const Keyboard::KeyType Keyboard::Key_BACKSPACE = ALLEGRO_KEY_BACKSPACE;
const Keyboard::KeyType Keyboard::Key_TAB = ALLEGRO_KEY_TAB;
const Keyboard::KeyType Keyboard::Key_OPENBRACE = ALLEGRO_KEY_OPENBRACE;
const Keyboard::KeyType Keyboard::Key_CLOSEBRACE = ALLEGRO_KEY_CLOSEBRACE;
const Keyboard::KeyType Keyboard::Key_ENTER = ALLEGRO_KEY_ENTER;
const Keyboard::KeyType Keyboard::Key_COLON = ALLEGRO_KEY_SEMICOLON2;
const Keyboard::KeyType Keyboard::Key_QUOTE = ALLEGRO_KEY_QUOTE;
const Keyboard::KeyType Keyboard::Key_BACKSLASH = ALLEGRO_KEY_BACKSLASH;
const Keyboard::KeyType Keyboard::Key_BACKSLASH2 = ALLEGRO_KEY_BACKSLASH2;
const Keyboard::KeyType Keyboard::Key_COMMA = ALLEGRO_KEY_COMMA;
const Keyboard::KeyType Keyboard::Key_STOP = ALLEGRO_KEY_FULLSTOP;
const Keyboard::KeyType Keyboard::Key_SLASH = ALLEGRO_KEY_SLASH;
const Keyboard::KeyType Keyboard::Key_SPACE = ALLEGRO_KEY_SPACE;
const Keyboard::KeyType Keyboard::Key_INSERT = ALLEGRO_KEY_INSERT;
const Keyboard::KeyType Keyboard::Key_DEL = ALLEGRO_KEY_DELETE;
const Keyboard::KeyType Keyboard::Key_HOME = ALLEGRO_KEY_HOME;
const Keyboard::KeyType Keyboard::Key_END = ALLEGRO_KEY_END;
const Keyboard::KeyType Keyboard::Key_PGUP = ALLEGRO_KEY_PGUP;
const Keyboard::KeyType Keyboard::Key_PGDN = ALLEGRO_KEY_PGDN;
const Keyboard::KeyType Keyboard::Key_LEFT = ALLEGRO_KEY_LEFT;
const Keyboard::KeyType Keyboard::Key_RIGHT = ALLEGRO_KEY_RIGHT;
const Keyboard::KeyType Keyboard::Key_UP = ALLEGRO_KEY_UP;
const Keyboard::KeyType Keyboard::Key_DOWN = ALLEGRO_KEY_DOWN;
const Keyboard::KeyType Keyboard::Key_SLASH_PAD = ALLEGRO_KEY_PAD_SLASH;
const Keyboard::KeyType Keyboard::Key_ASTERISK = ALLEGRO_KEY_PAD_ASTERISK;
const Keyboard::KeyType Keyboard::Key_MINUS_PAD = ALLEGRO_KEY_PAD_MINUS;
const Keyboard::KeyType Keyboard::Key_PLUS_PAD = ALLEGRO_KEY_PAD_PLUS;
const Keyboard::KeyType Keyboard::Key_DEL_PAD = ALLEGRO_KEY_PAD_DELETE;
const Keyboard::KeyType Keyboard::Key_ENTER_PAD = ALLEGRO_KEY_PAD_ENTER;
const Keyboard::KeyType Keyboard::Key_PRTSCR = ALLEGRO_KEY_PRINTSCREEN;
const Keyboard::KeyType Keyboard::Key_PAUSE = ALLEGRO_KEY_PAUSE;
const Keyboard::KeyType Keyboard::Key_ABNT_C1 = ALLEGRO_KEY_ABNT_C1;
const Keyboard::KeyType Keyboard::Key_YEN = ALLEGRO_KEY_YEN;
const Keyboard::KeyType Keyboard::Key_KANA = ALLEGRO_KEY_KANA;
const Keyboard::KeyType Keyboard::Key_CONVERT = ALLEGRO_KEY_CONVERT;
const Keyboard::KeyType Keyboard::Key_NOCONVERT = ALLEGRO_KEY_NOCONVERT;
const Keyboard::KeyType Keyboard::Key_AT = ALLEGRO_KEY_AT;
const Keyboard::KeyType Keyboard::Key_CIRCUMFLEX = ALLEGRO_KEY_CIRCUMFLEX;
const Keyboard::KeyType Keyboard::Key_COLON2 = ALLEGRO_KEY_COLON2;
const Keyboard::KeyType Keyboard::Key_KANJI = ALLEGRO_KEY_KANJI;
const Keyboard::KeyType Keyboard::Key_EQUALS_PAD = ALLEGRO_KEY_PAD_EQUALS;
const Keyboard::KeyType Keyboard::Key_BACKQUOTE = ALLEGRO_KEY_BACKQUOTE;
const Keyboard::KeyType Keyboard::Key_SEMICOLON = ALLEGRO_KEY_SEMICOLON;
const Keyboard::KeyType Keyboard::Key_COMMAND = ALLEGRO_KEY_COMMAND;

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

const Keyboard::KeyType Keyboard::Key_MODIFIERS = ALLEGRO_KEY_MODIFIERS;
const Keyboard::KeyType Keyboard::Key_LSHIFT = ALLEGRO_KEY_LSHIFT;
const Keyboard::KeyType Keyboard::Key_RSHIFT = ALLEGRO_KEY_RSHIFT;
const Keyboard::KeyType Keyboard::Key_LCONTROL = ALLEGRO_KEY_LCTRL;
const Keyboard::KeyType Keyboard::Key_RCONTROL = ALLEGRO_KEY_RCTRL;
const Keyboard::KeyType Keyboard::Key_ALT = ALLEGRO_KEY_ALT;
const Keyboard::KeyType Keyboard::Key_ALTGR = ALLEGRO_KEY_ALTGR;
const Keyboard::KeyType Keyboard::Key_LWIN = ALLEGRO_KEY_LWIN;
const Keyboard::KeyType Keyboard::Key_RWIN = ALLEGRO_KEY_RWIN;
const Keyboard::KeyType Keyboard::Key_MENU = ALLEGRO_KEY_MENU;
const Keyboard::KeyType Keyboard::Key_SCRLOCK = ALLEGRO_KEY_SCROLLLOCK;
const Keyboard::KeyType Keyboard::Key_NUMLOCK = ALLEGRO_KEY_NUMLOCK;
const Keyboard::KeyType Keyboard::Key_CAPSLOCK = ALLEGRO_KEY_CAPSLOCK;
    
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
