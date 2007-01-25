#include "animation.h"
#include "animation_event.h"
#include "animation_event_coords.h"

AnimationEventCoords::AnimationEventCoords( int _x, int _y, int _z ):
AnimationEvent(),
x( _x ),
y( _y ),
z( _z ){
}

void AnimationEventCoords::Interact( Animation * animation ){
	if ( x != -1 )
		animation->setX( x );
	if ( y != -1 )
		animation->setY( y );
	if ( z != -1 )
		animation->setZ( z );
}
