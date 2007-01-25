#ifndef _animation_event_sound_h
#define _animation_event_sound_h

#include "animation_event.h"

#include <string>

using namespace std;

class Animation;

class AnimationEventSound: public AnimationEvent{
public:
	AnimationEventSound( const string & path );

	virtual void Interact( Animation * animation ); 

	virtual ~AnimationEventSound();

protected:

	string path;

};

#endif
