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

#include "al_keyinput.h"
#include "keyinput_manager.h"
#include <iostream>
#include "globals.h"
#include "util/debug.h"

static std::ostream & debug( int level ){
	Global::debug( level ) << "[key input manager] ";
	return Global::debug( level );
}
	
#ifdef USE_ALLEGRO
allegroKeyInput keyInputManager::input;
#endif

guiTimer keyInputManager::dTimer[keys::MAX];
unsigned int keyInputManager::delay[keys::MAX];
bool keyInputManager::keyHolder[keys::MAX];
bool keyInputManager::keyBlocker[keys::MAX];
sigslot::signal1<const keys &> keyInputManager::pressed;
sigslot::signal1<const keys &> keyInputManager::released;

// Mouse Manager Constructor
keyInputManager::keyInputManager(){
    //dTimer.reset();
    //delay = 0;
    keyHolder[keys::SPACE]=false;
    keyHolder[keys::TAB]=false;
    keyHolder[keys::ENTER]=false;
    keyBlocker[keys::SPACE]=false;
    keyBlocker[keys::TAB]=false;
    keyBlocker[keys::ENTER]=false;
    for(int i=0;i<keys::MAX;++i)
    {
        dTimer[i].reset();
        delay[i]=0;
        keyHolder[i]=false;
        keyBlocker[i]=false;
    }
}

// Mouse Manager Destructor
keyInputManager::~keyInputManager(){
        // nothing
}
        
void keyInputManager::clear(){
#ifdef USE_ALLEGRO
    input.clear();
#endif
}

// Check mouse for changes and fire events
void keyInputManager::update(){
    //if(delay<=dTimer.msecs())dTimer.reset();

#ifdef USE_ALLEGRO
    // update the keyboard
    input.update();
    //if(dTimer.msecs()<=delay)
    //{
    // Do pressed queue
    while(!input.pressedEmpty()){
        keys k = input.checkNextPressed();
        if(delay[k.getValue()]<=dTimer[k.getValue()].msecs())dTimer[k.getValue()].reset();
        if(dTimer[k.getValue()].msecs()<=delay[k.getValue()])
        {
            input.dequeuePressed();
            keyHolder[k.getValue()] = true;
            debug( 5 ) << "Pressed key " << k.getValue() << std::endl;
            pressed.emit(k);
        }
    }

    // Do released queue
    while(!input.releasedEmpty())
    {
        keys k = input.dequeueReleased();
        keyHolder[k.getValue()] = false;
        released.emit(k);
    }
    //}
#endif
}

// Key state
bool keyInputManager::keyState(int unicode, bool blocking){
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
void keyInputManager::setDelay(unsigned int msecs, const keys::keyTypes key){
    delay[key] = msecs;
}
