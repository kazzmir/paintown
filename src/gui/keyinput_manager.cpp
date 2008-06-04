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

#include "keyinput_manager.h"
#include "al_keyinput.h"
	
	allegroKeyInput keyInputManager::input;
	
	// Mouse Manager Constructor
	keyInputManager::keyInputManager()
	{
		dTimer.reset();
		delay = 0;
		keyHolder[keys::SPACE]=false;
		keyHolder[keys::TAB]=false;
		keyHolder[keys::ENTER]=false;
		keyBlocker[keys::SPACE]=false;
		keyBlocker[keys::TAB]=false;
		keyBlocker[keys::ENTER]=false;
		for(int i=0;i<keys::MAX;++i)
		{
			keyHolder[i]=false;
			keyBlocker[i]=false;
		}
	}
	
	// Mouse Manager Destructor
	keyInputManager::~keyInputManager()
	{
		// nothing
	}
	
	// Check mouse for changes and fire events
	void keyInputManager::update()
	{
		if(delay<=dTimer.msecs())dTimer.reset();
		
		// update the keyboard
		input.update();
		if(dTimer.msecs()<=delay)
		{
			// Do pressed queue
			while(!input.pressedEmpty())
			{
				keys k = input.dequeuePressed();
				keyHolder[k.getValue()] = true;
				pressed.emit(k);
			}
			
			// Do released queue
			while(!input.releasedEmpty())
			{
				keys k = input.dequeueReleased();
				keyHolder[k.getValue()] = false;
				released.emit(k);
			}
		}
	}
	
	// Key state
	bool keyInputManager::keyState(int unicode, bool blocking)
	{
		//if(unicode>255 || unicode<0)return false;
		
		if(blocking)
		{
			
			if(keyHolder[unicode] && !(keyBlocker[unicode]))
			{
				keyBlocker[unicode]=true;
				return true;
			}
			else if(!keyHolder[unicode] && keyBlocker[unicode])
			{
				keyBlocker[unicode]=false;
				return false;
			}
			return false;
		}
		return keyHolder[unicode];
	}
	
	//! Set delay in milleseconds (doesn't effect the keyStates)
	void keyInputManager::setDelay(unsigned int msecs)
	{
		delay = msecs;
	}

