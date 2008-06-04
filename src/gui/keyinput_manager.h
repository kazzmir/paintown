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

#ifndef KEYINPUT_MANAGER_H
#define KEYINPUT_MANAGER_H

#include "keys.h"
//#include "keyinput.h"
#include "sigslot.h"
#include "timer.h"

class allegroKeyInput;

class keyInputManager
{
	public:
		//! Constructor
		keyInputManager();
		
		//! Destructor
		~keyInputManager();
		
		//! Evaluate keyboard and fire events
		void update();
		
		//! Key state (pass true to enable blocking checking)
		bool keyState(int unicode, bool blocking=false);
		
		
		//! Set delay in milleseconds (doesn't effect the keyStates)
		void setDelay(unsigned int msecs);
		
		//! \name Signals
		sigslot::signal1<const keys &> pressed;
		sigslot::signal1<const keys &> released;
		
	private:
		//! input pointer
		static allegroKeyInput input;
		
		//! Timer for delay
		guiTimer dTimer;
		
		//! Delay
		unsigned int delay;
		
		//! Key states
		bool keyHolder[keys::MAX];
		
		//! Blocker state
		bool keyBlocker[keys::MAX];
};

#endif /* KEYINPUT_MANAGER_H */
