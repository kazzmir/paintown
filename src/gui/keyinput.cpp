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
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OFkeyinput.cpp
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "keyinput.h"
#include <iostream>
#include "globals.h"

/*
static std::ostream & debug( int level ){
	Global::debug( level ) << "[key input] ";
	return Global::debug( level );
}
*/

 // Constructor
    keyInput::keyInput()
    {
        //Nothing to do
    }
    // Destructor
    keyInput::~keyInput()
    {
        // Nothing to do
    }
    // Is queue empty
    bool keyInput::pressedEmpty()
    {
        return pressedQueue.empty();
    }
    bool keyInput::releasedEmpty()
    {
        return releasedQueue.empty();
    }
    // Get the next key on the stack
    keys keyInput::dequeuePressed()
    {
        keys k = pressedQueue.front();
        pressedQueue.pop();
        return k;
    }
    keys keyInput::dequeueReleased()
    {
        keys k = releasedQueue.front();
        releasedQueue.pop();
        return k;
    }
    
    // Check key without popping
    keys keyInput::checkNextPressed()
    {
	    return pressedQueue.front();
    }
		
    // Check key without popping
    keys keyInput::checkNextReleased()
    {
	    return releasedQueue.front();
    }
    
    // Push a key
    void keyInput::queuePressed(keys k){
        pressedQueue.push(k);
	std::map<int, keys>::iterator p;
	p = pressedKeys.find(k.getValue());
	if(p == pressedKeys.end())pressedKeys.insert(std::make_pair(k.getValue(),k));

    }
		
	void keyInput::clear(){
		int pressedElements = pressedQueue.size();
		for ( int i = 0; i < pressedElements; i++ ){
			pressedQueue.pop();
		}
		int releasedElements = releasedQueue.size();
		for ( int i = 0; i < releasedElements; i++ ){
			releasedQueue.pop();
		}
		pressedKeys.clear();
	}

    // Push a key
    void keyInput::queueReleased(keys k)
    {
        releasedQueue.push(k);
	std::map<int, keys>::iterator p;
	p = pressedKeys.find(k.getValue());

	if(p != pressedKeys.end())deleteList.push_back(p);
    }
