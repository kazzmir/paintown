#ifndef _animation_event_sound_h
#define _animation_event_sound_h

#include "animation_event.h"
#include <string>

class Animation;

class AnimationEventSound: public AnimationEvent {
public:
	AnimationEventSound( const std::string & path );

	virtual void Interact( Animation * animation ); 

	virtual ~AnimationEventSound();

protected:

        std::string path;

};

#endif
