/*
SAGGUI (Simplified All Gaming Graphical User Interface)
Copyright (c) 2005-2007, Miguel A. Gavidia
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

    * Redistributions of source code must retain the above
       copyright notice, this list of conditions and the following
       disclaimer.
    * Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials
       provided with the distribution.
    * Neither the name of the "SAGGUI" nor the names of its
       contributors may be used to endorse or promote products
       derived from this software without specific prior written
       permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef KEYS_H
#define KEYS_H


class keys
{
	protected:
		int value;

		int scancode;

	public:

		//! Constructor
		keys();
		
		//! Constructor setting it to the given value
		keys(int v, int s);
		
		keys(int v);
		
		//! Destructor
		~keys();
		
		//! Is the value a character
		bool isCharacter() const;
		
		//! Is the value a number
		bool isNumber() const;
		
		//! Is the value a letter
		bool isLetter() const;
		
		//! Get the Raw Value
		int getValue() const;
		
		//! Get scancode Value
		int getScancodeValue() const;
		
		//! Set the unicode value
		void setValue(int v);
		
		//! Set scancode value
		void setScancodeValue(int s);
		
		//! Enumerator of lookup chart for common keys
		enum keyTypes
		{
			SPACE        = ' ',
			TAB          = '\t',
			ENTER        = '\n',
			NUM_0	     = '0',
			NUM_1	     = '1',
			NUM_2	     = '2',
			NUM_3	     = '3',
			NUM_4	     = '4',
			NUM_5	     = '5',
			NUM_6	     = '6',
			NUM_7	     = '7',
			NUM_8	     = '8',
			NUM_9	     = '9',
			PAD_0	     = 1000,
			PAD_1,
			PAD_2,
			PAD_3,
			PAD_4,
			PAD_5,
			PAD_6,
			PAD_7,
			PAD_8,
			PAD_9,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			ESC,
			TILDE,
			MINUS,
			EQUALS,
			BACKSPACE,
			OPENBRACE,
			CLOSEBRACE,
			COLON,
			QUOTE,
			BACKSLASH,
			BACKSLASH2,
			COMMA,
			STOP,
			SLASH,
			INSERT,
			DEL,
			HOME,
			END,
			PGUP,
			PGDN,
			LEFT,
			RIGHT,
			UP,
			DOWN,
			PAD_SLASH,
			ASTERISK,
			PAD_MINUS,
			PAD_PLUS,
			PAD_DEL,
			PAD_ENTER,
			PRTSCR,
			PAUSE,
			ABNT_C1,
			YEN,
			KANA,
			CONVERT,
			NOCONVERT,
			AT,
			CIRCUMFLEX,
			COLON2,
			KANJI,
			PAD_EQUALS,  /* MacOS X */
			BACKQUOTE,  /* MacOS X */
			SEMICOLON,  /* MacOS X */
			COMMAND,  /* MacOS X */
			UNKNOWN1,
			UNKNOWN2,
			UNKNOWN3,
			UNKNOWN4,
			UNKNOWN5,
			UNKNOWN6,
			UNKNOWN7,
			UNKNOWN8,
			LSHIFT,
			RSHIFT,
			LCONTROL,
			RCONTROL,
			ALT,
			ALTGR,
			LWIN,
			RWIN,
			MENU,
			SCRLOCK,
			NUMLOCK,
			CAPSLOCK,
			MAX	     = 1200
		};
};

#endif
