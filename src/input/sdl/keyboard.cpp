#include "../keyboard.h"

Keyboard::Keyboard(){
    /* TODO */
}

void Keyboard::poll(){
    /* TODO */
}

void Keyboard::wait(){
    /* TODO */
}

bool Keyboard::keypressed(){
    /* TODO */
    return false;
}
	
void Keyboard::readKeys( std::vector< int > & all_keys ){
    /* TODO */
}

int Keyboard::readKey(){
    /* TODO */
    return 0;
}

void Keyboard::clear(){
    /* TODO */
}

void Keyboard::setDelay( const int key, const int delay ){
    /* TODO */
}

void Keyboard::setAllDelay( const int delay ){
    /* TODO */
}

/* TODO: fix the keys */
Keyboard::KeyType Keyboard::Key_A = 1;
Keyboard::KeyType Keyboard::Key_B = 2;
Keyboard::KeyType Keyboard::Key_C = 3;
Keyboard::KeyType Keyboard::Key_D = 4;
Keyboard::KeyType Keyboard::Key_E = 5;
Keyboard::KeyType Keyboard::Key_F = 6;
Keyboard::KeyType Keyboard::Key_G = 7;
Keyboard::KeyType Keyboard::Key_H = 8;
Keyboard::KeyType Keyboard::Key_I = 9;
Keyboard::KeyType Keyboard::Key_J = 10;
Keyboard::KeyType Keyboard::Key_K = 11;
Keyboard::KeyType Keyboard::Key_L = 12;
Keyboard::KeyType Keyboard::Key_M = 13;
Keyboard::KeyType Keyboard::Key_N = 14;
Keyboard::KeyType Keyboard::Key_O = 15;
Keyboard::KeyType Keyboard::Key_P = 16;
Keyboard::KeyType Keyboard::Key_Q = 17;
Keyboard::KeyType Keyboard::Key_R = 18;
Keyboard::KeyType Keyboard::Key_S = 19;
Keyboard::KeyType Keyboard::Key_T = 20;
Keyboard::KeyType Keyboard::Key_U = 21;
Keyboard::KeyType Keyboard::Key_V = 22;
Keyboard::KeyType Keyboard::Key_W = 23;
Keyboard::KeyType Keyboard::Key_X = 24;
Keyboard::KeyType Keyboard::Key_Y = 25;
Keyboard::KeyType Keyboard::Key_Z = 26;
Keyboard::KeyType Keyboard::Key_0 = 27;
Keyboard::KeyType Keyboard::Key_1 = 28;
Keyboard::KeyType Keyboard::Key_2 = 29;
Keyboard::KeyType Keyboard::Key_3 = 30;
Keyboard::KeyType Keyboard::Key_4 = 31;
Keyboard::KeyType Keyboard::Key_5 = 32;
Keyboard::KeyType Keyboard::Key_6 = 33;
Keyboard::KeyType Keyboard::Key_7 = 34;
Keyboard::KeyType Keyboard::Key_8 = 35;
Keyboard::KeyType Keyboard::Key_9 = 36;
Keyboard::KeyType Keyboard::Key_0_PAD = 37;
Keyboard::KeyType Keyboard::Key_1_PAD = 38;
Keyboard::KeyType Keyboard::Key_2_PAD = 39;
Keyboard::KeyType Keyboard::Key_3_PAD = 40;
Keyboard::KeyType Keyboard::Key_4_PAD = 41;
Keyboard::KeyType Keyboard::Key_5_PAD = 42;
Keyboard::KeyType Keyboard::Key_6_PAD = 43;
Keyboard::KeyType Keyboard::Key_7_PAD = 44;
Keyboard::KeyType Keyboard::Key_8_PAD = 45;
Keyboard::KeyType Keyboard::Key_9_PAD = 46;
Keyboard::KeyType Keyboard::Key_F1 = 47;
Keyboard::KeyType Keyboard::Key_F2 = 48;
Keyboard::KeyType Keyboard::Key_F3 = 49;
Keyboard::KeyType Keyboard::Key_F4 = 50;
Keyboard::KeyType Keyboard::Key_F5 = 51;
Keyboard::KeyType Keyboard::Key_F6 = 52;
Keyboard::KeyType Keyboard::Key_F7 = 53;
Keyboard::KeyType Keyboard::Key_F8 = 54;
Keyboard::KeyType Keyboard::Key_F9 = 55;
Keyboard::KeyType Keyboard::Key_F10 = 56;
Keyboard::KeyType Keyboard::Key_F11 = 57;
Keyboard::KeyType Keyboard::Key_F12 = 58;
Keyboard::KeyType Keyboard::Key_ESC = 59;
Keyboard::KeyType Keyboard::Key_TILDE = 60;
Keyboard::KeyType Keyboard::Key_MINUS = 61;
Keyboard::KeyType Keyboard::Key_EQUALS = 62;
Keyboard::KeyType Keyboard::Key_BACKSPACE = 63;
Keyboard::KeyType Keyboard::Key_TAB = 64;
Keyboard::KeyType Keyboard::Key_OPENBRACE = 65;
Keyboard::KeyType Keyboard::Key_CLOSEBRACE = 66;
Keyboard::KeyType Keyboard::Key_ENTER = 67;
Keyboard::KeyType Keyboard::Key_COLON = 68;
Keyboard::KeyType Keyboard::Key_QUOTE = 69;
Keyboard::KeyType Keyboard::Key_BACKSLASH = 70;
Keyboard::KeyType Keyboard::Key_BACKSLASH2 = 71;
Keyboard::KeyType Keyboard::Key_COMMA = 72;
Keyboard::KeyType Keyboard::Key_STOP = 73;
Keyboard::KeyType Keyboard::Key_SLASH = 74;
Keyboard::KeyType Keyboard::Key_SPACE = 75;
Keyboard::KeyType Keyboard::Key_INSERT = 76;
Keyboard::KeyType Keyboard::Key_DEL = 77;
Keyboard::KeyType Keyboard::Key_HOME = 78;
Keyboard::KeyType Keyboard::Key_END = 79;
Keyboard::KeyType Keyboard::Key_PGUP = 80;
Keyboard::KeyType Keyboard::Key_PGDN = 81;
Keyboard::KeyType Keyboard::Key_LEFT = 82;
Keyboard::KeyType Keyboard::Key_RIGHT = 83;
Keyboard::KeyType Keyboard::Key_UP = 84;
Keyboard::KeyType Keyboard::Key_DOWN = 85;
Keyboard::KeyType Keyboard::Key_SLASH_PAD = 86;
Keyboard::KeyType Keyboard::Key_ASTERISK = 87;
Keyboard::KeyType Keyboard::Key_MINUS_PAD = 88;
Keyboard::KeyType Keyboard::Key_PLUS_PAD = 89;
Keyboard::KeyType Keyboard::Key_DEL_PAD = 90;
Keyboard::KeyType Keyboard::Key_ENTER_PAD = 91;
Keyboard::KeyType Keyboard::Key_PRTSCR = 92;
Keyboard::KeyType Keyboard::Key_PAUSE = 93;
Keyboard::KeyType Keyboard::Key_ABNT_C1 = 94;
Keyboard::KeyType Keyboard::Key_YEN = 95;
Keyboard::KeyType Keyboard::Key_KANA = 96;
Keyboard::KeyType Keyboard::Key_CONVERT = 97;
Keyboard::KeyType Keyboard::Key_NOCONVERT = 98;
Keyboard::KeyType Keyboard::Key_AT = 99;
Keyboard::KeyType Keyboard::Key_CIRCUMFLEX = 100;
Keyboard::KeyType Keyboard::Key_COLON2 = 101;
Keyboard::KeyType Keyboard::Key_KANJI = 102;
Keyboard::KeyType Keyboard::Key_EQUALS_PAD = 103;
Keyboard::KeyType Keyboard::Key_BACKQUOTE = 104;
Keyboard::KeyType Keyboard::Key_SEMICOLON = 105;
Keyboard::KeyType Keyboard::Key_COMMAND = 106;

Keyboard::KeyType Keyboard::Key_MODIFIERS = 107;
Keyboard::KeyType Keyboard::Key_LSHIFT = 108;
Keyboard::KeyType Keyboard::Key_RSHIFT = 109;
Keyboard::KeyType Keyboard::Key_LCONTROL = 110;
Keyboard::KeyType Keyboard::Key_RCONTROL = 111;
Keyboard::KeyType Keyboard::Key_ALT = 112;
Keyboard::KeyType Keyboard::Key_ALTGR = 113;
Keyboard::KeyType Keyboard::Key_LWIN = 114;
Keyboard::KeyType Keyboard::Key_RWIN = 115;
Keyboard::KeyType Keyboard::Key_MENU = 116;
Keyboard::KeyType Keyboard::Key_SCRLOCK = 117;
Keyboard::KeyType Keyboard::Key_NUMLOCK = 118;
Keyboard::KeyType Keyboard::Key_CAPSLOCK = 119;
