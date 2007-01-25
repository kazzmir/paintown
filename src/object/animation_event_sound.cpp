#include "animation.h"
#include "animation_event.h"
#include "animation_event_sound.h"
#include <string>

using namespace std;

AnimationEventSound::AnimationEventSound( const string & _path ){
	path = _path;
}

void AnimationEventSound::Interact( Animation * animation ){
	animation->playSound( path );
}
	
AnimationEventSound::~AnimationEventSound(){
}
