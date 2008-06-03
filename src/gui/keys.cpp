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

#include "keys.h"

    // Constructor
    keys::keys()
    {
        value = 0;
	scancode = 0;
    }
    keys::keys(int v)
    {
        value = v;
    }
    keys::keys(int v, int s)
    {
	    value = v;
	    scancode = s;
    }
    
    // Destructor
    keys::~keys()
    {
        // Nothing to do
    }
    // Is the value a character
    bool keys::isCharacter() const
    {
	    return (getValue() >= 32 && getValue() <= 126);
    }
    // Is the value a number
    bool keys::isNumber() const
    {
	    return (getValue() >= 48 && getValue() <= 57);
    }
    // Is the value a letter
    bool keys::isLetter() const
    {
	    return (getValue() >= 65 && getValue() <= 90) || (getValue() >= 97 && getValue() <= 122);
    }
    // Get the Raw Value
    int keys::getValue() const
    {
        return value;
    }

    // Get scancode Value
    int keys::getScancodeValue() const
    {
	    return scancode;
    }
    
    // Set the value
    void keys::setValue(int v)
    {
        value = v;
    }
    
    void keys::setScancodeValue(int s)
    {
	    scancode = s;
    }
