#include "projectile.h"
#include "util/bitmap.h"
#include "character.h"
#include "animation.h"
#include "stage.h"

using std::vector;

namespace PaintownUtil = ::Util;

namespace Mugen{

Projectile::Projectile(double x, double y, int id, Character * owner, int animation, int hitAnimation, int dieAnimation,
               int cancelAnimation, double scaleX, double scaleY, bool autoRemove, int removeTime, 
               double velocityX, double velocityY, double removeVelocityX, double removeVelocityY,
               double accelerateX, double accelerateY, double velocityXMultipler, 
               double velocityYMultipler, int hits, int missTime, int priority, int spritePriority,
               int edge, int stageDistance, int lowBound, int highBound, int shadowRed,
               int shadowGreen, int shadowBlue, int superMoveTime, int pauseMoveTime,
               int afterImageTime, int afterImageLength, Facing facing, const HitDefinition & hit):
owner(owner),
spritePriority(spritePriority),
x(x),
y(y),
velocityX(velocityX),
velocityY(velocityY),
accelerateX(accelerateX),
accelerateY(accelerateY),
velocityXMultipler(velocityXMultipler),
velocityYMultipler(velocityYMultipler),
removeVelocityX(removeVelocityX),
removeVelocityY(removeVelocityY),
removeTime(removeTime),
removeAnimation(dieAnimation),
shouldRemove(false),
scaleX(scaleX),
scaleY(scaleY),
facing(facing),
hits(hits),
missTime(missTime),
activeMissTime(0),
hitAnimation(hitAnimation),
id(id),
hit(hit),
priority(priority),
cancelAnimation(cancelAnimation),
lastHitTicks(0),
lastCanceled(0){
    PaintownUtil::ReferenceCount<MugenAnimation> his = owner->getAnimation(animation);
    if (his != NULL){
        this->animation = PaintownUtil::ReferenceCount<MugenAnimation>(his->copy());
    }
}

Projectile::~Projectile(){
}

Character * Projectile::getOwner() const {
    return owner;
}

int Projectile::getLastCancelTicks() const {
    return lastCanceled;
}
    
const int Projectile::getId() const {
    return id;
}

double Projectile::getX() const {
    return x;
}

double Projectile::getY() const {
    return y;
}
    
void Projectile::draw(const Graphics::Bitmap & work, double x, double y){
    if (animation != NULL){
        /* FIXME: multiply by proj.doscale */
	animation->render(facing == FacingLeft, false, (int)(this->x - x), (int)(this->y -  y), work, scaleX, scaleY);
    }
}
    
const std::vector<MugenArea> Projectile::getAttackBoxes() const {
    if (!shouldRemove && animation != NULL){
        return animation->getAttackBoxes(facing == FacingLeft);
    }
    return vector<MugenArea>();
}
    
const std::vector<MugenArea> Projectile::getDefenseBoxes() const {
    if (!shouldRemove && animation != NULL){
        return animation->getDefenseBoxes(facing == FacingLeft);
    }
    return vector<MugenArea>();
}
    
void Projectile::doCollision(Object * mugen, const Stage & stage){
    hits -= 1;
    if (hits <= 0){
        PaintownUtil::ReferenceCount<MugenAnimation> his = owner->getAnimation(hitAnimation);
        if (his != NULL){
            this->animation = PaintownUtil::ReferenceCount<MugenAnimation>(his->copy());
        }
        velocityX = removeVelocityX;
        velocityY = removeVelocityY;
        shouldRemove = true;
    }
    activeMissTime = missTime;
    lastHitTicks = stage.getTicks();
}
    
unsigned long int Projectile::getLastHitTicks() const {
    return lastHitTicks;
}

bool Projectile::canCollide() const {
    return !shouldRemove && activeMissTime == 0 && hits > 0;
}
    
int Projectile::getPriority() const {
    return priority;
}
    
void Projectile::canceled(const Stage & stage, Projectile * higher){
    shouldRemove = true;
    velocityX = removeVelocityX;
    velocityY = removeVelocityY;
    lastCanceled = stage.getTicks();
    PaintownUtil::ReferenceCount<MugenAnimation> his = owner->getAnimation(cancelAnimation);
    if (his != NULL){
        this->animation = PaintownUtil::ReferenceCount<MugenAnimation>(his->copy());
    }
}
    
void Projectile::logic(){
    if (animation != NULL){
        animation->logic();
    }

    x += velocityX * (facing == FacingLeft ? -1 : 1);
    y += velocityY;
    /* FIXME: not sure if this calculation is right */
    velocityX = (velocityX + accelerateX) * velocityXMultipler;
    velocityY = (velocityY + accelerateY) * velocityYMultipler;

    if (removeTime > -1){
        removeTime -= 1;

        if (removeTime == 0){
            PaintownUtil::ReferenceCount<MugenAnimation> his = owner->getAnimation(removeAnimation);
            if (his != NULL){
                this->animation = PaintownUtil::ReferenceCount<MugenAnimation>(his->copy());
            }
            velocityX = removeVelocityX;
            velocityY = removeVelocityY;
            shouldRemove = true;
        }
    }
}

bool Projectile::isDead() const {
    return shouldRemove && (animation == NULL || animation->isDone() || animation->hasLooped());
}
    
int Projectile::getSpritePriority() const {
    return spritePriority;
}

}
