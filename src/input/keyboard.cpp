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
    /*
       setDelay( Key_UNKNOWN1, delay );
       setDelay( Key_UNKNOWN2, delay );
       setDelay( Key_UNKNOWN3, delay );
       setDelay( Key_UNKNOWN4, delay );
       setDelay( Key_UNKNOWN5, delay );
       setDelay( Key_UNKNOWN6, delay );
       setDelay( Key_UNKNOWN7, delay );
       setDelay( Key_UNKNOWN8, delay );
       */
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
    
void Keyboard::press(KeyType key, unicode_t unicode){
    keyState[key].key = key;
    keyState[key].unicode = unicode;
    keyState[key].enabled = true;
    // if (enableBuffer){
        buffer.push_back(keyState[key]);
    // }
    /*
    KeyData data(key, unicode, true);
    for (std::vector<Observer>::iterator it = observers.begin(); it != observers.end(); it++){
        Observer observer = (*it);
        observer.callback(data, observer.extra);
    }
    */
}

void Keyboard::release(KeyType key){
    keyState[key].enabled = false;
    buffer.push_back(keyState[key]);
    /*
    KeyData data(key, 0, false);
    for (std::vector<Observer>::iterator it = observers.begin(); it != observers.end(); it++){
        Observer observer = (*it);
        observer.callback(data, observer.extra);
    }
    */
}
    
void Keyboard::readBufferedKeys(std::vector<int> & keys){
    for (std::vector<KeyData>::iterator it = buffer.begin(); it != buffer.end(); it++){
        const KeyData & data = *it;
        keys.push_back(data.key);
    }
}

std::vector<Keyboard::KeyData> Keyboard::readData(){
    std::vector<Keyboard::KeyData> out;
    for (std::map<KeyType, KeyData>::iterator it = keyState.begin(); it != keyState.end(); it++){
        KeyType key = it->first;
        const KeyData & data = it->second;
        if (data.enabled){
            out.push_back(data);
        }
    }
    return out;
}

std::vector<Keyboard::unicode_t> Keyboard::readText(){
    std::vector<Keyboard::unicode_t> out;
    for (std::map<KeyType, KeyData>::iterator it = keyState.begin(); it != keyState.end(); it++){
        KeyType key = it->first;
        const KeyData & data = it->second;
        if (data.enabled){
            out.push_back(data.unicode);
        }
    }
    return out;
}

/*
void Keyboard::addObserver(ObserverCallback observer, void * data){
    observers.push_back(Observer(observer, data));
}

void Keyboard::removeObserver(ObserverCallback observer, void * data){
    for (std::vector<Observer>::iterator it = observers.begin(); it != observers.end(); it++){
        const Observer & what = *it;
        if (what.callback == observer && what.extra == data){
            observers.erase(it);
            break;
        }
    }
}
*/
