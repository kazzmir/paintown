#include "animation.h"
#include "animation_event.h"

AnimationEvent::AnimationEvent( ){
}

void AnimationEvent::Interact( Animation * parent ){

}

AnimationEvent::~AnimationEvent(){
}

AnimationEventAttack::AnimationEventAttack(const Attack & a){
    attack = a;
}

void AnimationEventAttack::Interact( Animation * animation ){
    animation->setAttack(attack);
}
/*
void AnimationEventAttack::Interact( Animation * animation ){
	animation->setAttack( x1, y1, x2, y2 );
	animation->setDamage( damage );
}
*/
