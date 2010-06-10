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

#include "timer.h"
#include "util/funcs.h"

	guiTimer::guiTimer()
	{
		// Setup initial
		reset();
	}
	
	guiTimer::~guiTimer()
	{
		// nothing to be done
	}

	unsigned int guiTimer::usecs()
	{
#if defined (_WIN32)
    		static bool onetime = 0;
    		static unsigned long long freq = 0;
    
    		if(!onetime) {
        		onetime = true;
	    		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    		}
    
    		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
    
    		currentTime *= 1000000;
    		currentTime /= freq;
    
    		return currentTime - startTime * 1000;
#else
    		gettimeofday(&currentTime, 0);
    
    		return (unsigned long)((currentTime.tv_sec - startTime.tv_sec) / 1000)
		    + (unsigned long)((currentTime.tv_usec - startTime.tv_usec));
#endif
	}

	unsigned int guiTimer::msecs()
	{
#if defined (_WIN32)
    		currentTime = GetTickCount();
    
    		return currentTime - startTime;
#else
    		gettimeofday(&currentTime, 0);
    
    		return (unsigned long)((currentTime.tv_sec - startTime.tv_sec) * 1000)
		    + (unsigned long)((currentTime.tv_usec - startTime.tv_usec) / 1000);
#endif
	}
	
	unsigned int guiTimer::secs()
	{
#if defined (_WIN32)
		currentTime = GetTickCount();
		
		return (currentTime - startTime) / 1000;
#else
    		gettimeofday(&currentTime, 0);
    
    		return (unsigned long)(currentTime.tv_sec - startTime.tv_sec);
#endif
	}
	
	// Reset timer
	void guiTimer::reset()
	{
#if defined (_WIN32)
    		startTime = GetTickCount();
#else
    		gettimeofday(&startTime, 0);
#endif
	}
	
	// Provides a method to sleep
	void guiTimer::sleep(int msecs)
	{
            Util::rest(msecs);
            /*
#if defined (_WIN32)
		Sleep(msecs);
#else
		struct timespec timeOut;
		timeOut.tv_sec = 0;
		timeOut.tv_nsec = (msecs * 1000000);
		nanosleep(&timeOut, NULL);
#endif
*/

	}

