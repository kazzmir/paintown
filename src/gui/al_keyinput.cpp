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

#ifdef USE_ALLEGRO

#include <allegro.h>
#include "al_keyinput.h"

// constructor
	allegroKeyInput::allegroKeyInput()
	{
		// Nothing
	}
	
	// destructor
	allegroKeyInput::~allegroKeyInput()
	{
		// Nothing
	}
	
	keys allegroKeyInput::convertKey(int unicode, int scancode)
	{
		int value;

		switch(scancode)
		{
			case KEY_ESC:
				value = keys::ESC;
				break;
                
			case KEY_ALT:
				value = keys::ALT;
				break;

			case KEY_ALTGR:
				value = keys::ALTGR;
				break;

			case KEY_LSHIFT:
				value = keys::LSHIFT;
				break;

			case KEY_RSHIFT:
				value = keys::RSHIFT;
				break;
                
			case KEY_LCONTROL:
				value = keys::LCONTROL;
				break;
                
			case KEY_RCONTROL:
				value = keys::RCONTROL;
				break;

			case KEY_LWIN:
				value = keys::LWIN;
				break;
                
			case KEY_RWIN:
				value = keys::RWIN;
				break;

			case KEY_INSERT:
				value = keys::INSERT;
				break;

			case KEY_HOME:
				value = keys::HOME;
				break;

			case KEY_PGUP:
				value = keys::PGUP;
				break;

			case KEY_PGDN:
				value = keys::PGDN;
				break;

			case KEY_DEL:
				value = keys::DEL;
				break;
                
			case KEY_DEL_PAD:
				value = keys::PAD_DEL;
				break;

			case KEY_END:
				value = keys::END;
				break;

			case KEY_CAPSLOCK:
				value = keys::CAPSLOCK;
				break;

			case KEY_BACKSPACE:
				value = keys::BACKSPACE;
				break;

			case KEY_F1:
				value = keys::F1;
				break;

			case KEY_F2:
				value = keys::F2;
				break;

			case KEY_F3:
				value = keys::F3;
				break;

			case KEY_F4:
				value = keys::F4;
				break;

			case KEY_F5:
				value = keys::F5;
				break;

			case KEY_F6:
				value = keys::F6;
				break;

			case KEY_F7:
				value = keys::F7;
				break;
                
			case KEY_F8:
				value = keys::F8;
				break;

			case KEY_F9:
				value = keys::F9;
				break;

			case KEY_F10:
				value = keys::F10;
				break;

			case KEY_F11:
				value = keys::F11;
				break;

			case KEY_F12:
				value = keys::F12;
				break;

			case KEY_PRTSCR:
				value = keys::PRTSCR;
				break;
                
			case KEY_PAUSE:
				value = keys::PAUSE;
				break;

			case KEY_SCRLOCK:
				value = keys::SCRLOCK;
				break;

			case KEY_NUMLOCK:
				value = keys::NUMLOCK;
				break;

			case KEY_LEFT:
				value = keys::LEFT;
				break;

			case KEY_RIGHT:
				value = keys::RIGHT;
				break;

			case KEY_UP:
				value = keys::UP;
				break;

			case KEY_DOWN:
				value = keys::DOWN;
				break;

			case KEY_ENTER_PAD:
				value = keys::PAD_ENTER;
				break;
			case KEY_ENTER:
				value = keys::ENTER;
				break;

			case KEY_0_PAD:
				value = keys::PAD_0;
				break;
			case KEY_1_PAD:
				value = keys::PAD_1;
				break;
			case KEY_2_PAD:
				value = keys::PAD_2;
				break;
			case KEY_3_PAD:
				value = keys::PAD_3;
				break;
			case KEY_4_PAD:
				value = keys::PAD_4;
				break;
			case KEY_5_PAD:
				value = keys::PAD_5;
				break;
			case KEY_6_PAD:
				value = keys::PAD_6;
				break;
			case KEY_7_PAD:
				value = keys::PAD_7;
				break;
			case KEY_8_PAD:
				value = keys::PAD_8;
				break;
			case KEY_9_PAD:
				value = keys::PAD_9;
				break;
			case KEY_SLASH_PAD:
				value = keys::PAD_SLASH;
				break;
			case KEY_MINUS_PAD:
				value = keys::PAD_MINUS;
				break;
			case KEY_PLUS_PAD:
				value = keys::PAD_PLUS;
				break;

			default:
				value = unicode;
		}
		
		keys newkey(value, scancode);
		
        	return newkey;

	}
	
	//init
	void allegroKeyInput::update()
	{
		// Poll allegros keyboard and set the keys
		while(keypressed())
		{
			int unicode, scancode;
			 unicode = ureadkey(&scancode);
			keys pressed = convertKey(unicode,scancode);
			queuePressed(pressed);
		}
		
		// Check these other keys since allegro doesn't do it just right :|
		if(key[KEY_LSHIFT])queuePressed(keys(keys::LSHIFT,KEY_LSHIFT));
		if(key[KEY_RSHIFT])queuePressed(keys(keys::RSHIFT,KEY_RSHIFT));
		if(key[KEY_LCONTROL])queuePressed(keys(keys::LCONTROL,KEY_LCONTROL));
		if(key[KEY_RCONTROL])queuePressed(keys(keys::RCONTROL,KEY_RCONTROL));
		if(key[KEY_ALT])queuePressed(keys(keys::ALT,KEY_ALT));
		if(key[KEY_ALTGR])queuePressed(keys(keys::ALTGR,KEY_ALTGR));
		if(key[KEY_LWIN])queuePressed(keys(keys::LWIN,KEY_LWIN));
		if(key[KEY_RWIN])queuePressed(keys(keys::RWIN,KEY_RWIN));
		if(key[KEY_MENU])queuePressed(keys(keys::MENU,KEY_MENU));
		if(key[KEY_SCRLOCK])queuePressed(keys(keys::SCRLOCK,KEY_SCRLOCK));
		if(key[KEY_NUMLOCK])queuePressed(keys(keys::NUMLOCK,KEY_NUMLOCK));
		if(key[KEY_CAPSLOCK])queuePressed(keys(keys::CAPSLOCK,KEY_CAPSLOCK));
		
		for(std::map<int, keys>::iterator i = pressedKeys.begin();i!=pressedKeys.end();++i)
		{
			if(!key[(i->second).getScancodeValue()])
			{
				queueReleased(i->second);
			}
		}
		for(std::list< std::map<int, keys>::iterator >::iterator i = deleteList.begin();i!=deleteList.end();++i)
		{
			pressedKeys.erase((*i));
		}
		deleteList.clear();
	}

#endif
