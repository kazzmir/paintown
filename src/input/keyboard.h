#ifndef _keyboard_h
#define _keyboard_h

#include <map>
#include <vector>

/* handles allegro key[] array better than keypressed()
 * and readkey()
 */
class Keyboard{
public:

    typedef const int KeyType;

	Keyboard();

	/* poll:
	 * Put the keys in Allegro's key[] array into our map of int -> bool
	 */
	void poll();

	/* wait for all keys to be released */
	void wait();

	/* []:
	 * Extract a boolean value given a key number
	 */
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

	/* keypressed:
	 * Returns true if a key is pressed
	 */
	bool keypressed();
	
	/* readKeys:
	 * Store all pressed keys in a user supplied vector
	 */
	void readKeys( std::vector< int > & all_keys );
	int readKey();
	void clear();

	void setDelay( const int key, const int delay );
	void setAllDelay( const int delay );

	static const char * keyToName( int key );
	static bool isNumber( int key );
	static bool isChar( int key );
	static bool isAlpha( int key );

	static KeyType Key_A;
	static KeyType Key_B;
	static KeyType Key_C;
	static KeyType Key_D;
	static KeyType Key_E;
	static KeyType Key_F;
	static KeyType Key_G;
	static KeyType Key_H;
	static KeyType Key_I;
	static KeyType Key_J;
	static KeyType Key_K;
	static KeyType Key_L;
	static KeyType Key_M;
	static KeyType Key_N;
	static KeyType Key_O;
	static KeyType Key_P;
	static KeyType Key_Q;
	static KeyType Key_R;
	static KeyType Key_S;
	static KeyType Key_T;
	static KeyType Key_U;
	static KeyType Key_V;
	static KeyType Key_W;
	static KeyType Key_X;
	static KeyType Key_Y;
	static KeyType Key_Z;
	static KeyType Key_0;
	static KeyType Key_1;
	static KeyType Key_2;
	static KeyType Key_3;
	static KeyType Key_4;
	static KeyType Key_5;
	static KeyType Key_6;
	static KeyType Key_7;
	static KeyType Key_8;
	static KeyType Key_9;
	static KeyType Key_0_PAD;
	static KeyType Key_1_PAD;
	static KeyType Key_2_PAD;
	static KeyType Key_3_PAD;
	static KeyType Key_4_PAD;
	static KeyType Key_5_PAD;
	static KeyType Key_6_PAD;
	static KeyType Key_7_PAD;
	static KeyType Key_8_PAD;
	static KeyType Key_9_PAD;
	static KeyType Key_F1;
	static KeyType Key_F2;
	static KeyType Key_F3;
	static KeyType Key_F4;
	static KeyType Key_F5;
	static KeyType Key_F6;
	static KeyType Key_F7;
	static KeyType Key_F8;
	static KeyType Key_F9;
	static KeyType Key_F10;
	static KeyType Key_F11;
	static KeyType Key_F12;
	static KeyType Key_ESC;
	static KeyType Key_TILDE;
	static KeyType Key_MINUS;
	static KeyType Key_EQUALS;
	static KeyType Key_BACKSPACE;
	static KeyType Key_TAB;
	static KeyType Key_OPENBRACE;
	static KeyType Key_CLOSEBRACE;
	static KeyType Key_ENTER;
	static KeyType Key_COLON;
	static KeyType Key_QUOTE;
	static KeyType Key_BACKSLASH;
	static KeyType Key_BACKSLASH2;
	static KeyType Key_COMMA;
	static KeyType Key_STOP;
	static KeyType Key_SLASH;
	static KeyType Key_SPACE;
	static KeyType Key_INSERT;
	static KeyType Key_DEL;
	static KeyType Key_HOME;
	static KeyType Key_END;
	static KeyType Key_PGUP;
	static KeyType Key_PGDN;
	static KeyType Key_LEFT;
	static KeyType Key_RIGHT;
	static KeyType Key_UP;
	static KeyType Key_DOWN;
	static KeyType Key_SLASH_PAD;
	static KeyType Key_ASTERISK;
	static KeyType Key_MINUS_PAD;
	static KeyType Key_PLUS_PAD;
	static KeyType Key_DEL_PAD;
	static KeyType Key_ENTER_PAD;
	static KeyType Key_PRTSCR;
	static KeyType Key_PAUSE;
	static KeyType Key_ABNT_C1;
	static KeyType Key_YEN;
	static KeyType Key_KANA;
	static KeyType Key_CONVERT;
	static KeyType Key_NOCONVERT;
	static KeyType Key_AT;
	static KeyType Key_CIRCUMFLEX;
	static KeyType Key_COLON2;
	static KeyType Key_KANJI;
	static KeyType Key_EQUALS_PAD;
	static KeyType Key_BACKQUOTE;
	static KeyType Key_SEMICOLON;
	static KeyType Key_COMMAND;
	static KeyType Key_UNKNOWN1;
	static KeyType Key_UNKNOWN2;
	static KeyType Key_UNKNOWN3;
	static KeyType Key_UNKNOWN4;
	static KeyType Key_UNKNOWN5;
	static KeyType Key_UNKNOWN6;
	static KeyType Key_UNKNOWN7;
	static KeyType Key_UNKNOWN8;
	static KeyType Key_MODIFIERS;
	static KeyType Key_LSHIFT;
	static KeyType Key_RSHIFT;
	static KeyType Key_LCONTROL;
	static KeyType Key_RCONTROL;
	static KeyType Key_ALT;
	static KeyType Key_ALTGR;
	static KeyType Key_LWIN;
	static KeyType Key_RWIN;
	static KeyType Key_MENU;
	static KeyType Key_SCRLOCK;
	static KeyType Key_NUMLOCK;
	static KeyType Key_CAPSLOCK;

protected:

	std::map<int,int> my_keys;
	std::map<int,int> key_delay;

};

#endif
