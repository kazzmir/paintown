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

#ifndef gui_TIMER_H
#define gui_TIMER_H

class guiTimer{
private:
    unsigned long startTime;
    /*
#if defined (_WIN32)
    //! Start time for timer
    unsigned long long startTime;

    //! Current time for comparison against startTime
    unsigned long long currentTime;
#else

    //! Start time for timer
    timeval startTime;

    //! Current time for comparison against startTime
    timeval currentTime;
#endif
*/
public:
    //! Constructor
    guiTimer();

    //! Destructor
    ~guiTimer();

    //! Provides the currentTime in microseconds
    // unsigned int usecs();

    //! Provides the currentTime in milliseconds
    unsigned int msecs();

    //! Provides the currentTime in seconds
    unsigned int secs();

    //! Reset timer
    void reset();

    //! Provides a method to sleep in milliseconds
    static void sleep(int msecs);
};


#endif
