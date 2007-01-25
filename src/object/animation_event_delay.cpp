#include "animation_event_delay.h"
#include "animation_event.h"
#include "animation.h"
	
AnimationEventDelay::AnimationEventDelay( int _delay ):
AnimationEvent(),
delay( _delay ){
}

void AnimationEventDelay::Interact( Animation * animation ){

	animation->setDelay( delay );

}

