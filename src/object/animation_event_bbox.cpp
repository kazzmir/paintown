#include "animation.h"
#include "animation_event.h"
#include "animation_event_bbox.h"


AnimationEventBBox::AnimationEventBBox( int _x1, int _y1, int _x2, int _y2 ):
x1( _x1 ),
y1( _y1 ),
x2( _x2 ),
y2( _y2 ){
}


void AnimationEventBBox::Interact( Animation * animation ){
	animation->setBBox( x1, y1, x2, y2 );
}
