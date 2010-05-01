#ifdef USE_ALLEGRO
#include "allegro/keyboard.cpp"
#endif
#ifdef USE_SDL
#include "sdl/keyboard.cpp"
#endif

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
		/*
		case Keyboard::Key_UNKNOWN1 : return "UNKNOWN1";
		case Keyboard::Key_UNKNOWN2 : return "UNKNOWN2";
		case Keyboard::Key_UNKNOWN3 : return "UNKNOWN3";
		case Keyboard::Key_UNKNOWN4 : return "UNKNOWN4";
		case Keyboard::Key_UNKNOWN5 : return "UNKNOWN5";
		case Keyboard::Key_UNKNOWN6 : return "UNKNOWN6";
		case Keyboard::Key_UNKNOWN7 : return "UNKNOWN7";
		case Keyboard::Key_UNKNOWN8 : return "UNKNOWN8";
		*/
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
