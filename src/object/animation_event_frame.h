#ifndef _animation_event_frame_h
#define _animation_event_frame_h

#include "animation_event.h"
#include <string>

using namespace std;

class Animation;

class AnimationEventFrame: public AnimationEvent{
public:
	AnimationEventFrame( const string & _path );

	virtual void Interact( Animation * animation ); 

	virtual ~AnimationEventFrame();

protected:
	string path;

};

#endif
