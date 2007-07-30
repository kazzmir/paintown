#include "animation.h"
#include "animation_event.h"
#include "animation_event_projectile.h"
#include "projectile.h"
#include <iostream>

AnimationEventProjectile::AnimationEventProjectile( Projectile * projectile ):
projectile( projectile ){
}

void AnimationEventProjectile::Interact( Animation * animation ){
	std::cout << "Create projectile" << std::endl;
	animation->createProjectile( (Projectile *) projectile->copy() );
}
	
AnimationEventProjectile::~AnimationEventProjectile(){
	delete projectile;
}
