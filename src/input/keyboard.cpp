#include "keyboard.h"
#include "allegro.h"
#include "funcs.h"
#include "globals.h"
#include <iostream>
#include <vector>
#include <map>

using namespace std;

const int Keyboard::Key_A = ::KEY_A;
const int Keyboard::Key_B = ::KEY_B;
const int Keyboard::Key_C = ::KEY_C;
const int Keyboard::Key_D = ::KEY_D;
const int Keyboard::Key_E = ::KEY_E;
const int Keyboard::Key_F = ::KEY_F;
const int Keyboard::Key_G = ::KEY_G;
const int Keyboard::Key_H = ::KEY_H;
const int Keyboard::Key_I = ::KEY_I;
const int Keyboard::Key_J = ::KEY_J;
const int Keyboard::Key_K = ::KEY_K;
const int Keyboard::Key_L = ::KEY_L;
const int Keyboard::Key_M = ::KEY_M;
const int Keyboard::Key_N = ::KEY_N;
const int Keyboard::Key_O = ::KEY_O;
const int Keyboard::Key_P = ::KEY_P;
const int Keyboard::Key_Q = ::KEY_Q;
const int Keyboard::Key_R = ::KEY_R;
const int Keyboard::Key_S = ::KEY_S;
const int Keyboard::Key_T = ::KEY_T;
const int Keyboard::Key_U = ::KEY_U;
const int Keyboard::Key_V = ::KEY_V;
const int Keyboard::Key_W = ::KEY_W;
const int Keyboard::Key_X = ::KEY_X;
const int Keyboard::Key_Y = ::KEY_Y;
const int Keyboard::Key_Z = ::KEY_Z;
const int Keyboard::Key_0 = ::KEY_0;
const int Keyboard::Key_1 = ::KEY_1;
const int Keyboard::Key_2 = ::KEY_2;
const int Keyboard::Key_3 = ::KEY_3;
const int Keyboard::Key_4 = ::KEY_4;
const int Keyboard::Key_5 = ::KEY_5;
const int Keyboard::Key_6 = ::KEY_6;
const int Keyboard::Key_7 = ::KEY_7;
const int Keyboard::Key_8 = ::KEY_8;
const int Keyboard::Key_9 = ::KEY_9;
const int Keyboard::Key_0_PAD = ::KEY_0_PAD;
const int Keyboard::Key_1_PAD = ::KEY_1_PAD;
const int Keyboard::Key_2_PAD = ::KEY_2_PAD;
const int Keyboard::Key_3_PAD = ::KEY_3_PAD;
const int Keyboard::Key_4_PAD = ::KEY_4_PAD;
const int Keyboard::Key_5_PAD = ::KEY_5_PAD;
const int Keyboard::Key_6_PAD = ::KEY_6_PAD;
const int Keyboard::Key_7_PAD = ::KEY_7_PAD;
const int Keyboard::Key_8_PAD = ::KEY_8_PAD;
const int Keyboard::Key_9_PAD = ::KEY_9_PAD;
const int Keyboard::Key_F1 = ::KEY_F1;
const int Keyboard::Key_F2 = ::KEY_F2;
const int Keyboard::Key_F3 = ::KEY_F3;
const int Keyboard::Key_F4 = ::KEY_F4;
const int Keyboard::Key_F5 = ::KEY_F5;
const int Keyboard::Key_F6 = ::KEY_F6;
const int Keyboard::Key_F7 = ::KEY_F7;
const int Keyboard::Key_F8 = ::KEY_F8;
const int Keyboard::Key_F9 = ::KEY_F9;
const int Keyboard::Key_F10 = ::KEY_F10;
const int Keyboard::Key_F11 = ::KEY_F11;
const int Keyboard::Key_F12 = ::KEY_F12;
const int Keyboard::Key_ESC = ::KEY_ESC;
const int Keyboard::Key_TILDE = ::KEY_TILDE;
const int Keyboard::Key_MINUS = ::KEY_MINUS;
const int Keyboard::Key_EQUALS = ::KEY_EQUALS;
const int Keyboard::Key_BACKSPACE = ::KEY_BACKSPACE;
const int Keyboard::Key_TAB = ::KEY_TAB;
const int Keyboard::Key_OPENBRACE = ::KEY_OPENBRACE;
const int Keyboard::Key_CLOSEBRACE = ::KEY_CLOSEBRACE;
const int Keyboard::Key_ENTER = ::KEY_ENTER;
const int Keyboard::Key_COLON = ::KEY_COLON;
const int Keyboard::Key_QUOTE = ::KEY_QUOTE;
const int Keyboard::Key_BACKSLASH = ::KEY_BACKSLASH;
const int Keyboard::Key_BACKSLASH2 = ::KEY_BACKSLASH2;
const int Keyboard::Key_COMMA = ::KEY_COMMA;
const int Keyboard::Key_STOP = ::KEY_STOP;
const int Keyboard::Key_SLASH = ::KEY_SLASH;
const int Keyboard::Key_SPACE = ::KEY_SPACE;
const int Keyboard::Key_INSERT = ::KEY_INSERT;
const int Keyboard::Key_DEL = ::KEY_DEL;
const int Keyboard::Key_HOME = ::KEY_HOME;
const int Keyboard::Key_END = ::KEY_END;
const int Keyboard::Key_PGUP = ::KEY_PGUP;
const int Keyboard::Key_PGDN = ::KEY_PGDN;
const int Keyboard::Key_LEFT = ::KEY_LEFT;
const int Keyboard::Key_RIGHT = ::KEY_RIGHT;
const int Keyboard::Key_UP = ::KEY_UP;
const int Keyboard::Key_DOWN = ::KEY_DOWN;
const int Keyboard::Key_SLASH_PAD = ::KEY_SLASH_PAD;
const int Keyboard::Key_ASTERISK = ::KEY_ASTERISK;
const int Keyboard::Key_MINUS_PAD = ::KEY_MINUS_PAD;
const int Keyboard::Key_PLUS_PAD = ::KEY_PLUS_PAD;
const int Keyboard::Key_DEL_PAD = ::KEY_DEL_PAD;
const int Keyboard::Key_ENTER_PAD = ::KEY_ENTER_PAD;
const int Keyboard::Key_PRTSCR = ::KEY_PRTSCR;
const int Keyboard::Key_PAUSE = ::KEY_PAUSE;
const int Keyboard::Key_ABNT_C1 = ::KEY_ABNT_C1;
const int Keyboard::Key_YEN = ::KEY_YEN;
const int Keyboard::Key_KANA = ::KEY_KANA;
const int Keyboard::Key_CONVERT = ::KEY_CONVERT;
const int Keyboard::Key_NOCONVERT = ::KEY_NOCONVERT;
const int Keyboard::Key_AT = ::KEY_AT;
const int Keyboard::Key_CIRCUMFLEX = ::KEY_CIRCUMFLEX;
const int Keyboard::Key_COLON2 = ::KEY_COLON2;
const int Keyboard::Key_KANJI = ::KEY_KANJI;
const int Keyboard::Key_EQUALS_PAD = ::KEY_EQUALS_PAD;
const int Keyboard::Key_BACKQUOTE = ::KEY_BACKQUOTE;
const int Keyboard::Key_SEMICOLON = ::KEY_SEMICOLON;
const int Keyboard::Key_COMMAND = ::KEY_COMMAND;
const int Keyboard::Key_UNKNOWN1 = ::KEY_UNKNOWN1;
const int Keyboard::Key_UNKNOWN2 = ::KEY_UNKNOWN2;
const int Keyboard::Key_UNKNOWN3 = ::KEY_UNKNOWN3;
const int Keyboard::Key_UNKNOWN4 = ::KEY_UNKNOWN4;
const int Keyboard::Key_UNKNOWN5 = ::KEY_UNKNOWN5;
const int Keyboard::Key_UNKNOWN6 = ::KEY_UNKNOWN6;
const int Keyboard::Key_UNKNOWN7 = ::KEY_UNKNOWN7;
const int Keyboard::Key_UNKNOWN8 = ::KEY_UNKNOWN8;
const int Keyboard::Key_MODIFIERS = ::KEY_MODIFIERS;
const int Keyboard::Key_LSHIFT = ::KEY_LSHIFT;
const int Keyboard::Key_RSHIFT = ::KEY_RSHIFT;
const int Keyboard::Key_LCONTROL = ::KEY_LCONTROL;
const int Keyboard::Key_RCONTROL = ::KEY_RCONTROL;
const int Keyboard::Key_ALT = ::KEY_ALT;
const int Keyboard::Key_ALTGR = ::KEY_ALTGR;
const int Keyboard::Key_LWIN = ::KEY_LWIN;
const int Keyboard::Key_RWIN = ::KEY_RWIN;
const int Keyboard::Key_MENU = ::KEY_MENU;
const int Keyboard::Key_SCRLOCK = ::KEY_SCRLOCK;
const int Keyboard::Key_NUMLOCK = ::KEY_NUMLOCK;
const int Keyboard::Key_CAPSLOCK = ::KEY_CAPSLOCK;

Keyboard::Keyboard(){
	for ( int q = 0; q < KEY_MAX; q++ ){
		my_keys[ q ] = 0;
	}
	setAllDelay( 0 );
}

/* KEY_MAX is defined in allegro at
 * allegro/include/allegro/keyboard.h
 */
void Keyboard::poll(){

	for ( int q = 0; q < KEY_MAX; q++ ){
		// my_keys[ q ] = key[ q ];
		if ( key[ q ] ){
			/* my_keys[ q ] becomes negative so that the key
			 * can be pressed the first time without having
			 * to wait for a delay
			 */
			if ( my_keys[ q ] <= 0 ){
				my_keys[ q ] -= 1;
			} else {
				my_keys[ q ] += 1;
			}
			// printf( "Key %d = %d\n", q, my_keys[ q ] );
		} else {
			my_keys[ q ] = 0;
		}
	}
}

void Keyboard::readKeys( vector< int > & all_keys ){
	for ( map<int,int>::const_iterator it = my_keys.begin(); it != my_keys.end(); it++ ){
		const int & key = it->first;
		const int & delay = it->second;
		if ( delay != 0 ){
			Global::debug( 6 ) << "Key " << key << " delay " << delay << " key_delay " << key_delay[ key ] << endl;
		}
		if ( delay < 0 ){
			all_keys.push_back( key );
			my_keys[ key ] = 1;
		} else if ( delay > key_delay[ key ] ){
			all_keys.push_back( key );
			my_keys[ key ] = 1;
		}
	}
}
	
int Keyboard::readKey(){
	return ::readkey() >> 8;
}
	
void Keyboard::setDelay( const int key, const int delay ){
	key_delay[ key ] = delay;	
}
	
void Keyboard::setAllDelay( const int delay ){
	setDelay( Key_A, delay );
	setDelay( Key_B, delay );
	setDelay( Key_C, delay );
	setDelay( Key_D, delay );
	setDelay( Key_E, delay );
	setDelay( Key_F, delay );
	setDelay( Key_G, delay );
	setDelay( Key_H, delay );
	setDelay( Key_I, delay );
	setDelay( Key_J, delay );
	setDelay( Key_K, delay );
	setDelay( Key_L, delay );
	setDelay( Key_M, delay );
	setDelay( Key_N, delay );
	setDelay( Key_O, delay );
	setDelay( Key_P, delay );
	setDelay( Key_Q, delay );
	setDelay( Key_R, delay );
	setDelay( Key_S, delay );
	setDelay( Key_T, delay );
	setDelay( Key_U, delay );
	setDelay( Key_V, delay );
	setDelay( Key_W, delay );
	setDelay( Key_X, delay );
	setDelay( Key_Y, delay );
	setDelay( Key_Z, delay );
	setDelay( Key_0, delay );
	setDelay( Key_1, delay );
	setDelay( Key_2, delay );
	setDelay( Key_3, delay );
	setDelay( Key_4, delay );
	setDelay( Key_5, delay );
	setDelay( Key_6, delay );
	setDelay( Key_7, delay );
	setDelay( Key_8, delay );
	setDelay( Key_9, delay );
	setDelay( Key_0_PAD, delay );
	setDelay( Key_1_PAD, delay );
	setDelay( Key_2_PAD, delay );
	setDelay( Key_3_PAD, delay );
	setDelay( Key_4_PAD, delay );
	setDelay( Key_5_PAD, delay );
	setDelay( Key_6_PAD, delay );
	setDelay( Key_7_PAD, delay );
	setDelay( Key_8_PAD, delay );
	setDelay( Key_9_PAD, delay );
	setDelay( Key_F1, delay );
	setDelay( Key_F2, delay );
	setDelay( Key_F3, delay );
	setDelay( Key_F4, delay );
	setDelay( Key_F5, delay );
	setDelay( Key_F6, delay );
	setDelay( Key_F7, delay );
	setDelay( Key_F8, delay );
	setDelay( Key_F9, delay );
	setDelay( Key_F10, delay );
	setDelay( Key_F11, delay );
	setDelay( Key_F12, delay );
	setDelay( Key_ESC, delay );
	setDelay( Key_TILDE, delay );
	setDelay( Key_MINUS, delay );
	setDelay( Key_EQUALS, delay );
	setDelay( Key_BACKSPACE, delay );
	setDelay( Key_TAB, delay );
	setDelay( Key_OPENBRACE, delay );
	setDelay( Key_CLOSEBRACE, delay );
	setDelay( Key_ENTER, delay );
	setDelay( Key_COLON, delay );
	setDelay( Key_QUOTE, delay );
	setDelay( Key_BACKSLASH, delay );
	setDelay( Key_BACKSLASH2, delay );
	setDelay( Key_COMMA, delay );
	setDelay( Key_STOP, delay );
	setDelay( Key_SLASH, delay );
	setDelay( Key_SPACE, delay );
	setDelay( Key_INSERT, delay );
	setDelay( Key_DEL, delay );
	setDelay( Key_HOME, delay );
	setDelay( Key_END, delay );
	setDelay( Key_PGUP, delay );
	setDelay( Key_PGDN, delay );
	setDelay( Key_LEFT, delay );
	setDelay( Key_RIGHT, delay );
	setDelay( Key_UP, delay );
	setDelay( Key_DOWN, delay );
	setDelay( Key_SLASH_PAD, delay );
	setDelay( Key_ASTERISK, delay );
	setDelay( Key_MINUS_PAD, delay );
	setDelay( Key_PLUS_PAD, delay );
	setDelay( Key_DEL_PAD, delay );
	setDelay( Key_ENTER_PAD, delay );
	setDelay( Key_PRTSCR, delay );
	setDelay( Key_PAUSE, delay );
	setDelay( Key_ABNT_C1, delay );
	setDelay( Key_YEN, delay );
	setDelay( Key_KANA, delay );
	setDelay( Key_CONVERT, delay );
	setDelay( Key_NOCONVERT, delay );
	setDelay( Key_AT, delay );
	setDelay( Key_CIRCUMFLEX, delay );
	setDelay( Key_COLON2, delay );
	setDelay( Key_KANJI, delay );
	setDelay( Key_EQUALS_PAD, delay );
	setDelay( Key_BACKQUOTE, delay );
	setDelay( Key_SEMICOLON, delay );
	setDelay( Key_COMMAND, delay );
	setDelay( Key_UNKNOWN1, delay );
	setDelay( Key_UNKNOWN2, delay );
	setDelay( Key_UNKNOWN3, delay );
	setDelay( Key_UNKNOWN4, delay );
	setDelay( Key_UNKNOWN5, delay );
	setDelay( Key_UNKNOWN6, delay );
	setDelay( Key_UNKNOWN7, delay );
	setDelay( Key_UNKNOWN8, delay );
	setDelay( Key_MODIFIERS, delay );
	setDelay( Key_LSHIFT, delay );
	setDelay( Key_RSHIFT, delay );
	setDelay( Key_LCONTROL, delay );
	setDelay( Key_RCONTROL, delay );
	setDelay( Key_ALT, delay );
	setDelay( Key_ALTGR, delay );
	setDelay( Key_LWIN, delay );
	setDelay( Key_RWIN, delay );
	setDelay( Key_MENU, delay );
	setDelay( Key_SCRLOCK, delay );
	setDelay( Key_NUMLOCK, delay );
	setDelay( Key_CAPSLOCK, delay );
}

void Keyboard::wait(){
	clear();
	poll();
	while ( keypressed() ){
		poll();
		Util::rest( 1 );
	}
}

bool Keyboard::keypressed(){
	for ( map<int,int>::const_iterator it = my_keys.begin(); it != my_keys.end(); it++ ){
		const int & n = (*it).second;
		if ( n < 0 || n > key_delay[ it->first ] ){
			return true;
		}
	}
	return false;
}
	
bool Keyboard::isNumber( int key ){
	return key == Key_0 ||
	       key == Key_1 ||
	       key == Key_2 ||
	       key == Key_3 ||
	       key == Key_4 ||
	       key == Key_5 ||
	       key == Key_6 ||
	       key == Key_7 ||
	       key == Key_8 ||
	       key == Key_9;
}

bool Keyboard::isChar( int key ){
	return key == Key_A ||
	       key == Key_B ||
			 key == Key_C ||
			 key == Key_D ||
			 key == Key_E ||
			 key == Key_F ||
			 key == Key_G ||
			 key == Key_H ||
			 key == Key_I ||
			 key == Key_J ||
			 key == Key_K ||
			 key == Key_L ||
			 key == Key_M ||
			 key == Key_N ||
			 key == Key_O ||
			 key == Key_P ||
			 key == Key_Q ||
			 key == Key_R ||
			 key == Key_S ||
			 key == Key_T ||
			 key == Key_U ||
			 key == Key_V ||
			 key == Key_W ||
			 key == Key_X ||
			 key == Key_Y ||
			 key == Key_Z ||
                         key == Key_MINUS;
}

bool Keyboard::isAlpha( int key ){
		  return isNumber( key ) || isChar( key );
}

void Keyboard::clear(){
	::clear_keybuf();
	my_keys.clear();
}

const char * Keyboard::keyToName( int key ){
	switch ( key ){
		case Keyboard::Key_A : return "A";
		case Keyboard::Key_B : return "B";
		case Keyboard::Key_C : return "C";
		case Keyboard::Key_D : return "D";
		case Keyboard::Key_E : return "E";
		case Keyboard::Key_F : return "F";
		case Keyboard::Key_G : return "G";
		case Keyboard::Key_H : return "H";
		case Keyboard::Key_I : return "I";
		case Keyboard::Key_J : return "J";
		case Keyboard::Key_K : return "K";
		case Keyboard::Key_L : return "L";
		case Keyboard::Key_M : return "M";
		case Keyboard::Key_N : return "N";
		case Keyboard::Key_O : return "O";
		case Keyboard::Key_P : return "P";
		case Keyboard::Key_Q : return "Q";
		case Keyboard::Key_R : return "R";
		case Keyboard::Key_S : return "S";
		case Keyboard::Key_T : return "T";
		case Keyboard::Key_U : return "U";
		case Keyboard::Key_V : return "V";
		case Keyboard::Key_W : return "W";
		case Keyboard::Key_X : return "X";
		case Keyboard::Key_Y : return "Y";
		case Keyboard::Key_Z : return "Z";
		case Keyboard::Key_0 : return "0";
		case Keyboard::Key_1 : return "1";
		case Keyboard::Key_2 : return "2";
		case Keyboard::Key_3 : return "3";
		case Keyboard::Key_4 : return "4";
		case Keyboard::Key_5 : return "5";
		case Keyboard::Key_6 : return "6";
		case Keyboard::Key_7 : return "7";
		case Keyboard::Key_8 : return "8";
		case Keyboard::Key_9 : return "9";
		case Keyboard::Key_0_PAD : return "0_PAD";
		case Keyboard::Key_1_PAD : return "1_PAD";
		case Keyboard::Key_2_PAD : return "2_PAD";
		case Keyboard::Key_3_PAD : return "3_PAD";
		case Keyboard::Key_4_PAD : return "4_PAD";
		case Keyboard::Key_5_PAD : return "5_PAD";
		case Keyboard::Key_6_PAD : return "6_PAD";
		case Keyboard::Key_7_PAD : return "7_PAD";
		case Keyboard::Key_8_PAD : return "8_PAD";
		case Keyboard::Key_9_PAD : return "9_PAD";
		case Keyboard::Key_F1 : return "F1";
		case Keyboard::Key_F2 : return "F2";
		case Keyboard::Key_F3 : return "F3";
		case Keyboard::Key_F4 : return "F4";
		case Keyboard::Key_F5 : return "F5";
		case Keyboard::Key_F6 : return "F6";
		case Keyboard::Key_F7 : return "F7";
		case Keyboard::Key_F8 : return "F8";
		case Keyboard::Key_F9 : return "F9";
		case Keyboard::Key_F10 : return "F10";
		case Keyboard::Key_F11 : return "F11";
		case Keyboard::Key_F12 : return "F12";
		case Keyboard::Key_ESC : return "ESC";
		case Keyboard::Key_TILDE : return "~";
		case Keyboard::Key_MINUS : return "-";
		case Keyboard::Key_EQUALS : return "=";
		case Keyboard::Key_BACKSPACE : return "BACKSPACE";
		case Keyboard::Key_TAB : return "TAB";
		case Keyboard::Key_OPENBRACE : return "OPENBRACE";
		case Keyboard::Key_CLOSEBRACE : return "CLOSEBRACE";
		case Keyboard::Key_ENTER : return "ENTER";
		case Keyboard::Key_COLON : return "COLON";
		case Keyboard::Key_QUOTE : return "QUOTE";
		case Keyboard::Key_BACKSLASH : return "BACKSLASH";
		case Keyboard::Key_BACKSLASH2 : return "BACKSLASH2";
		case Keyboard::Key_COMMA : return "COMMA";
		case Keyboard::Key_STOP : return ".";
		case Keyboard::Key_SLASH : return "SLASH";
		case Keyboard::Key_SPACE : return "SPACE";
		case Keyboard::Key_INSERT : return "INSERT";
		case Keyboard::Key_DEL : return "DEL";
		case Keyboard::Key_HOME : return "HOME";
		case Keyboard::Key_END : return "END";
		case Keyboard::Key_PGUP : return "PGUP";
		case Keyboard::Key_PGDN : return "PGDN";
		case Keyboard::Key_LEFT : return "LEFT";
		case Keyboard::Key_RIGHT : return "RIGHT";
		case Keyboard::Key_UP : return "UP";
		case Keyboard::Key_DOWN : return "DOWN";
		case Keyboard::Key_SLASH_PAD : return "SLASH_PAD";
		case Keyboard::Key_ASTERISK : return "ASTERISK";
		case Keyboard::Key_MINUS_PAD : return "MINUS_PAD";
		case Keyboard::Key_PLUS_PAD : return "PLUS_PAD";
		case Keyboard::Key_DEL_PAD : return "DEL_PAD";
		case Keyboard::Key_ENTER_PAD : return "ENTER_PAD";
		case Keyboard::Key_PRTSCR : return "PRTSCR";
		case Keyboard::Key_PAUSE : return "PAUSE";
		case Keyboard::Key_ABNT_C1 : return "ABNT_C1";
		case Keyboard::Key_YEN : return "YEN";
		case Keyboard::Key_KANA : return "KANA";
		case Keyboard::Key_CONVERT : return "CONVERT";
		case Keyboard::Key_NOCONVERT : return "NOCONVERT";
		case Keyboard::Key_AT : return "AT";
		case Keyboard::Key_CIRCUMFLEX : return "CIRCUMFLEX";
		case Keyboard::Key_COLON2 : return "COLON2";
		case Keyboard::Key_KANJI : return "KANJI";
		case Keyboard::Key_EQUALS_PAD : return "EQUALS_PAD";
		case Keyboard::Key_BACKQUOTE : return "BACKQUOTE";
		case Keyboard::Key_SEMICOLON : return "SEMICOLON";
		case Keyboard::Key_COMMAND : return "COMMAND";
		case Keyboard::Key_UNKNOWN1 : return "UNKNOWN1";
		case Keyboard::Key_UNKNOWN2 : return "UNKNOWN2";
		case Keyboard::Key_UNKNOWN3 : return "UNKNOWN3";
		case Keyboard::Key_UNKNOWN4 : return "UNKNOWN4";
		case Keyboard::Key_UNKNOWN5 : return "UNKNOWN5";
		case Keyboard::Key_UNKNOWN6 : return "UNKNOWN6";
		case Keyboard::Key_UNKNOWN7 : return "UNKNOWN7";
		case Keyboard::Key_UNKNOWN8 : return "UNKNOWN8";
		// case Keyboard::Key_MODIFIERS : return "MODIFIERS";
		case Keyboard::Key_LSHIFT : return "LSHIFT";
		case Keyboard::Key_RSHIFT : return "RSHIFT";
		case Keyboard::Key_LCONTROL : return "LCONTROL";
		case Keyboard::Key_RCONTROL : return "RCONTROL";
		case Keyboard::Key_ALT : return "ALT";
		case Keyboard::Key_ALTGR : return "ALTGR";
		case Keyboard::Key_LWIN : return "LWIN";
		case Keyboard::Key_RWIN : return "RWIN";
		case Keyboard::Key_MENU : return "MENU";
		case Keyboard::Key_SCRLOCK : return "SCRLOCK";
		case Keyboard::Key_NUMLOCK : return "NUMLOCK";
		case Keyboard::Key_CAPSLOCK : return "CAPSLOCK";
		default : return "Unknown key";
	}
}
