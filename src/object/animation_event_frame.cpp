#include "animation_event_frame.h"
#include "animation_event.h"
#include "animation.h"
#include <string>

using namespace std;

AnimationEventFrame::AnimationEventFrame( const string & _path ){
	path = _path;
}

void AnimationEventFrame::Interact( Animation * animation ){

	animation->setFrame( path );
	animation->Delay();

}

AnimationEventFrame::~AnimationEventFrame(){
}
