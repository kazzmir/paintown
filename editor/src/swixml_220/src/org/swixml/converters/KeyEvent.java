/*--
 $Id: KeyEvent.java,v 1.2 2005/05/31 04:51:43 wolfpaulus Exp $

 Copyright (C) 2003-2007 Wolf Paulus.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions, and the disclaimer that follows
 these conditions in the documentation and/or other materials provided
 with the distribution.

 3. The end-user documentation included with the redistribution,
 if any, must include the following acknowledgment:
        "This product includes software developed by the
         SWIXML Project (http://www.swixml.org/)."
 Alternately, this acknowledgment may appear in the software itself,
 if and wherever such third-party acknowledgments normally appear.

 4. The name "Swixml" must not be used to endorse or promote products
 derived from this software without prior written permission. For
 written permission, please contact <info_AT_swixml_DOT_org>

 5. Products derived from this software may not be called "Swixml",
 nor may "Swixml" appear in their name, without prior written
 permission from the Swixml Project Management.

 THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED.  IN NO EVENT SHALL THE SWIXML PROJECT OR ITS
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
 ====================================================================

 This software consists of voluntary contributions made by many
 individuals on behalf of the Swixml Project and was originally
 created by Wolf Paulus <wolf_AT_swixml_DOT_org>. For more information
 on the Swixml Project, please see <http://www.swixml.org/>.
*/

package org.swixml.converters;

/**
 * The KeyEvent Interface makes the KeyEvents available to the converter objects
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.2 $
 * @since
 */

public interface KeyEvent {
  /**
   * The first number in the range of ids used for key events.
   */
  int KEY_FIRST = 400;
  /**
   * The last number in the range of ids used for key events.
   */
  int KEY_LAST = 402;
  /**
   * The "key typed" event.  This event is generated when a character is
   * entered.  In the simplest case, it is produced by a single key press.
   * Often, however, characters are produced by series of key presses, and
   * the mapping from key pressed events to key typed events may be
   * many-to-one or many-to-many.
   */
  int KEY_TYPED = KEY_FIRST;
  /**
   * The "key pressed" event. This event is generated when a key
   * is pushed down.
   */
  int KEY_PRESSED = 1 + KEY_FIRST; //Event.KEY_PRESS
  /**
   * The "key released" event. This event is generated when a key
   * is let up.
   */
  int KEY_RELEASED = 2 + KEY_FIRST; //Event.KEY_RELEASE
  int VK_ENTER = '\n';
  int VK_BACK_SPACE = '\b';
  int VK_TAB = '\t';
  int VK_CANCEL = 0x03;
  int VK_CLEAR = 0x0C;
  int VK_SHIFT = 0x10;
  int VK_CONTROL = 0x11;
  int VK_ALT = 0x12;
  int VK_PAUSE = 0x13;
  int VK_CAPS_LOCK = 0x14;
  int VK_ESCAPE = 0x1B;
  int VK_SPACE = 0x20;
  int VK_PAGE_UP = 0x21;
  int VK_PAGE_DOWN = 0x22;
  int VK_END = 0x23;
  int VK_HOME = 0x24;
  /**
   * Constant for the non-numpad <b>left</b> arrow key.
   * @see #VK_KP_LEFT
   */
  int VK_LEFT = 0x25;
  /**
   * Constant for the non-numpad <b>up</b> arrow key.
   * @see #VK_KP_UP
   */
  int VK_UP = 0x26;
  /**
   * Constant for the non-numpad <b>right</b> arrow key.
   * @see #VK_KP_RIGHT
   */
  int VK_RIGHT = 0x27;
  /**
   * Constant for the non-numpad <b>down</b> arrow key.
   * @see #VK_KP_DOWN
   */
  int VK_DOWN = 0x28;
  int VK_COMMA = 0x2C;
  /**
   * Constant for the "-" key.
   * @since 1.2
   */
  int VK_MINUS = 0x2D;
  int VK_PERIOD = 0x2E;
  int VK_SLASH = 0x2F;
  /** VK_0 thru VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39) */
  int VK_0 = 0x30;
  int VK_1 = 0x31;
  int VK_2 = 0x32;
  int VK_3 = 0x33;
  int VK_4 = 0x34;
  int VK_5 = 0x35;
  int VK_6 = 0x36;
  int VK_7 = 0x37;
  int VK_8 = 0x38;
  int VK_9 = 0x39;
  int VK_SEMICOLON = 0x3B;
  int VK_EQUALS = 0x3D;
  /** VK_A thru VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A) */
  int VK_A = 0x41;
  int VK_B = 0x42;
  int VK_C = 0x43;
  int VK_D = 0x44;
  int VK_E = 0x45;
  int VK_F = 0x46;
  int VK_G = 0x47;
  int VK_H = 0x48;
  int VK_I = 0x49;
  int VK_J = 0x4A;
  int VK_K = 0x4B;
  int VK_L = 0x4C;
  int VK_M = 0x4D;
  int VK_N = 0x4E;
  int VK_O = 0x4F;
  int VK_P = 0x50;
  int VK_Q = 0x51;
  int VK_R = 0x52;
  int VK_S = 0x53;
  int VK_T = 0x54;
  int VK_U = 0x55;
  int VK_V = 0x56;
  int VK_W = 0x57;
  int VK_X = 0x58;
  int VK_Y = 0x59;
  int VK_Z = 0x5A;
  int VK_OPEN_BRACKET = 0x5B;
  int VK_BACK_SLASH = 0x5C;
  int VK_CLOSE_BRACKET = 0x5D;
  int VK_NUMPAD0 = 0x60;
  int VK_NUMPAD1 = 0x61;
  int VK_NUMPAD2 = 0x62;
  int VK_NUMPAD3 = 0x63;
  int VK_NUMPAD4 = 0x64;
  int VK_NUMPAD5 = 0x65;
  int VK_NUMPAD6 = 0x66;
  int VK_NUMPAD7 = 0x67;
  int VK_NUMPAD8 = 0x68;
  int VK_NUMPAD9 = 0x69;
  int VK_MULTIPLY = 0x6A;
  int VK_ADD = 0x6B;
  /**
   * This constant is obsolete, and is included only for backwards compatibility.
   */
  int VK_SEPARATER = 0x6C;
  /**
   * Constant for the Numpad Separator key.
   * @since 1.4
   */
  int VK_SEPARATOR = VK_SEPARATER;
  int VK_SUBTRACT = 0x6D;
  int VK_DECIMAL = 0x6E;
  int VK_DIVIDE = 0x6F;
  int VK_DELETE = 0x7F; /* ASCII DEL */
  int VK_NUM_LOCK = 0x90;
  int VK_SCROLL_LOCK = 0x91;
  /** Constant for the F1 function key. */
  int VK_F1 = 0x70;
  /** Constant for the F2 function key. */
  int VK_F2 = 0x71;
  /** Constant for the F3 function key. */
  int VK_F3 = 0x72;
  /** Constant for the F4 function key. */
  int VK_F4 = 0x73;
  /** Constant for the F5 function key. */
  int VK_F5 = 0x74;
  /** Constant for the F6 function key. */
  int VK_F6 = 0x75;
  /** Constant for the F7 function key. */
  int VK_F7 = 0x76;
  /** Constant for the F8 function key. */
  int VK_F8 = 0x77;
  /** Constant for the F9 function key. */
  int VK_F9 = 0x78;
  /** Constant for the F10 function key. */
  int VK_F10 = 0x79;
  /** Constant for the F11 function key. */
  int VK_F11 = 0x7A;
  /** Constant for the F12 function key. */
  int VK_F12 = 0x7B;
  /* F13 - F24 are used on IBM 3270 keyboard; use random range for constants. */
  int VK_F13 = 0xF000;
  /**
   * Constant for the F14 function key.
   * @since 1.2
   */
  int VK_F14 = 0xF001;
  /**
   * Constant for the F15 function key.
   * @since 1.2
   */
  int VK_F15 = 0xF002;
  /**
   * Constant for the F16 function key.
   * @since 1.2
   */
  int VK_F16 = 0xF003;
  /**
   * Constant for the F17 function key.
   * @since 1.2
   */
  int VK_F17 = 0xF004;
  /**
   * Constant for the F18 function key.
   * @since 1.2
   */
  int VK_F18 = 0xF005;
  /**
   * Constant for the F19 function key.
   * @since 1.2
   */
  int VK_F19 = 0xF006;
  /**
   * Constant for the F20 function key.
   * @since 1.2
   */
  int VK_F20 = 0xF007;
  /**
   * Constant for the F21 function key.
   * @since 1.2
   */
  int VK_F21 = 0xF008;
  /**
   * Constant for the F22 function key.
   * @since 1.2
   */
  int VK_F22 = 0xF009;
  /**
   * Constant for the F23 function key.
   * @since 1.2
   */
  int VK_F23 = 0xF00A;
  /**
   * Constant for the F24 function key.
   * @since 1.2
   */
  int VK_F24 = 0xF00B;
  int VK_PRINTSCREEN = 0x9A;
  int VK_INSERT = 0x9B;
  int VK_HELP = 0x9C;
  int VK_META = 0x9D;
  int VK_BACK_QUOTE = 0xC0;
  int VK_QUOTE = 0xDE;
  /**
   * Constant for the numeric keypad <b>up</b> arrow key.
   * @see #VK_UP
   * @since 1.2
   */
  int VK_KP_UP = 0xE0;
  /**
   * Constant for the numeric keypad <b>down</b> arrow key.
   * @see #VK_DOWN
   * @since 1.2
   */
  int VK_KP_DOWN = 0xE1;
  /**
   * Constant for the numeric keypad <b>left</b> arrow key.
   * @see #VK_LEFT
   * @since 1.2
   */
  int VK_KP_LEFT = 0xE2;
  /**
   * Constant for the numeric keypad <b>right</b> arrow key.
   * @see #VK_RIGHT
   * @since 1.2
   */
  int VK_KP_RIGHT = 0xE3;
  /** @since 1.2 */
  int VK_DEAD_GRAVE = 0x80;
  /** @since 1.2 */
  int VK_DEAD_ACUTE = 0x81;
  /** @since 1.2 */
  int VK_DEAD_CIRCUMFLEX = 0x82;
  /** @since 1.2 */
  int VK_DEAD_TILDE = 0x83;
  /** @since 1.2 */
  int VK_DEAD_MACRON = 0x84;
  /** @since 1.2 */
  int VK_DEAD_BREVE = 0x85;
  /** @since 1.2 */
  int VK_DEAD_ABOVEDOT = 0x86;
  /** @since 1.2 */
  int VK_DEAD_DIAERESIS = 0x87;
  /** @since 1.2 */
  int VK_DEAD_ABOVERING = 0x88;
  /** @since 1.2 */
  int VK_DEAD_DOUBLEACUTE = 0x89;
  /** @since 1.2 */
  int VK_DEAD_CARON = 0x8a;
  /** @since 1.2 */
  int VK_DEAD_CEDILLA = 0x8b;
  /** @since 1.2 */
  int VK_DEAD_OGONEK = 0x8c;
  /** @since 1.2 */
  int VK_DEAD_IOTA = 0x8d;
  /** @since 1.2 */
  int VK_DEAD_VOICED_SOUND = 0x8e;
  /** @since 1.2 */
  int VK_DEAD_SEMIVOICED_SOUND = 0x8f;
  /** @since 1.2 */
  int VK_AMPERSAND = 0x96;
  /** @since 1.2 */
  int VK_ASTERISK = 0x97;
  /** @since 1.2 */
  int VK_QUOTEDBL = 0x98;
  /** @since 1.2 */
  int VK_LESS = 0x99;
  /** @since 1.2 */
  int VK_GREATER = 0xa0;
  /** @since 1.2 */
  int VK_BRACELEFT = 0xa1;
  /** @since 1.2 */
  int VK_BRACERIGHT = 0xa2;
  /**
   * Constant for the "@" key.
   * @since 1.2
   */
  int VK_AT = 0x0200;
  /**
   * Constant for the ":" key.
   * @since 1.2
   */
  int VK_COLON = 0x0201;
  /**
   * Constant for the "^" key.
   * @since 1.2
   */
  int VK_CIRCUMFLEX = 0x0202;
  /**
   * Constant for the "$" key.
   * @since 1.2
   */
  int VK_DOLLAR = 0x0203;
  /**
   * Constant for the Euro currency sign key.
   * @since 1.2
   */
  int VK_EURO_SIGN = 0x0204;
  /**
   * Constant for the "!" key.
   * @since 1.2
   */
  int VK_EXCLAMATION_MARK = 0x0205;
  /**
   * Constant for the inverted exclamation mark key.
   * @since 1.2
   */
  int VK_INVERTED_EXCLAMATION_MARK = 0x0206;
  /**
   * Constant for the "(" key.
   * @since 1.2
   */
  int VK_LEFT_PARENTHESIS = 0x0207;
  /**
   * Constant for the "#" key.
   * @since 1.2
   */
  int VK_NUMBER_SIGN = 0x0208;
  /**
   * Constant for the "+" key.
   * @since 1.2
   */
  int VK_PLUS = 0x0209;
  /**
   * Constant for the ")" key.
   * @since 1.2
   */
  int VK_RIGHT_PARENTHESIS = 0x020A;
  /**
   * Constant for the "_" key.
   * @since 1.2
   */
  int VK_UNDERSCORE = 0x020B;
  /* not clear what this means - listed in Microsoft Windows API */
  int VK_FINAL = 0x0018;
  /* Japanese PC 106 keyboard, Japanese Solaris keyboard: henkan */
  int VK_CONVERT = 0x001C;
  /* Japanese PC 106 keyboard: muhenkan */
  int VK_NONCONVERT = 0x001D;
  /* Japanese Solaris keyboard: kakutei */
  int VK_ACCEPT = 0x001E;
  /* not clear what this means - listed in Microsoft Windows API */
  int VK_MODECHANGE = 0x001F;
  /* replaced by VK_KANA_LOCK for Microsoft Windows and Solaris;
       might still be used on other platforms */
  int VK_KANA = 0x0015;
  /* replaced by VK_INPUT_METHOD_ON_OFF for Microsoft Windows and Solaris;
       might still be used for other platforms */
  int VK_KANJI = 0x0019;
  /* Japanese PC 106 keyboard: eisuu */
  int VK_ALPHANUMERIC = 0x00F0;
  /* Japanese PC 106 keyboard: katakana */
  int VK_KATAKANA = 0x00F1;
  /* Japanese PC 106 keyboard: hiragana */
  int VK_HIRAGANA = 0x00F2;
  /* Japanese PC 106 keyboard: zenkaku */
  int VK_FULL_WIDTH = 0x00F3;
  /* Japanese PC 106 keyboard: hankaku */
  int VK_HALF_WIDTH = 0x00F4;
  /* Japanese PC 106 keyboard: roumaji */
  int VK_ROMAN_CHARACTERS = 0x00F5;
  /* Japanese PC 106 keyboard - VK_CONVERT + ALT: zenkouho */
  int VK_ALL_CANDIDATES = 0x0100;
  /* Japanese PC 106 keyboard - VK_CONVERT + SHIFT: maekouho */
  int VK_PREVIOUS_CANDIDATE = 0x0101;
  /* Japanese PC 106 keyboard - VK_ALPHANUMERIC + ALT: kanji bangou */
  int VK_CODE_INPUT = 0x0102;
  /* Japanese Macintosh keyboard - VK_JAPANESE_HIRAGANA + SHIFT */
  int VK_JAPANESE_KATAKANA = 0x0103;
  /* Japanese Macintosh keyboard */
  int VK_JAPANESE_HIRAGANA = 0x0104;
  /* Japanese Macintosh keyboard */
  int VK_JAPANESE_ROMAN = 0x0105;
  /* Japanese PC 106 keyboard with special Windows driver - eisuu + Control; Japanese Solaris keyboard: kana */
  int VK_KANA_LOCK = 0x0106;
  /* Japanese PC 106 keyboard: kanji. Japanese Solaris keyboard: nihongo */
  int VK_INPUT_METHOD_ON_OFF = 0x0107;
  /** @since 1.2 */
  int VK_CUT = 0xFFD1;
  /** @since 1.2 */
  int VK_COPY = 0xFFCD;
  /** @since 1.2 */
  int VK_PASTE = 0xFFCF;
  /** @since 1.2 */
  int VK_UNDO = 0xFFCB;
  /** @since 1.2 */
  int VK_AGAIN = 0xFFC9;
  /** @since 1.2 */
  int VK_FIND = 0xFFD0;
  /** @since 1.2 */
  int VK_PROPS = 0xFFCA;
  /** @since 1.2 */
  int VK_STOP = 0xFFC8;
  /**
   * Constant for the Compose function key.
   * @since 1.2
   */
  int VK_COMPOSE = 0xFF20;
  /**
   * Constant for the AltGraph function key.
   * @since 1.2
   */
  int VK_ALT_GRAPH = 0xFF7E;
  /**
   * This value is used to indicate that the keyCode is unknown.
   * KEY_TYPED events do not have a keyCode value; this value
   * is used instead.
   */
  int VK_UNDEFINED = 0x0;
  /**
   * KEY_PRESSED and KEY_RELEASED events which do not map to a
   * valid Unicode character use this for the keyChar value.
   */
  char CHAR_UNDEFINED = 0xFFFF;
  /**
   * A constant indicating that the keyLocation is indeterminate
   * or not relevant.
   * KEY_TYPED events do not have a keyLocation; this value
   * is used instead.
   * @since 1.4
   */
  int KEY_LOCATION_UNKNOWN = 0;
  /**
   * A constant indicating that the key pressed or released
   * is not distinguished as the left or right version of a key,
   * and did not originate on the numeric keypad (or did not
   * originate with a virtual key corresponding to the numeric
   * keypad).
   * @since 1.4
   */
  int KEY_LOCATION_STANDARD = 1;
  /**
   * A constant indicating that the key pressed or released is in
   * the left key location (there is more than one possible location
   * for this key).  Example: the left shift key.
   * @since 1.4
   */
  int KEY_LOCATION_LEFT = 2;
  /**
   * A constant indicating that the key pressed or released is in
   * the right key location (there is more than one possible location
   * for this key).  Example: the right shift key.
   * @since 1.4
   */
  int KEY_LOCATION_RIGHT = 3;
  /**
   * A constant indicating that the key event originated on the
   * numeric keypad or with a virtual key corresponding to the
   * numeric keypad.
   * @since 1.4
   */
  int KEY_LOCATION_NUMPAD = 4;
}
