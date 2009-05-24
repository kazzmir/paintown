#include "animation.h"
#include "animation_event_trails.h"

AnimationEventTrail::AnimationEventTrail(const int produce, const int life):
produce(produce),
life(life){
}

void AnimationEventTrail::Interact(Animation * animation){
    animation->setTrails(produce, life);
}
