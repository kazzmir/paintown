#include "projectile.h"
#include "util/bitmap.h"
#include "character.h"
#include "animation.h"

namespace PaintownUtil = ::Util;

namespace Mugen{

Projectile::Projectile(double x, double y, int id, const Character & owner, int animation, int hitAnimation, int dieAnimation,
               int cancelAnimation, double scaleX, double scaleY, bool autoRemove, int removeTime, 
               double velocityX, double velocityY, double removeVelocityX, double removeVelocityY,
               double accelerateX, double accelerateY, double velocityXMultipler, 
               double velocityYMultipler, int hits, int missTime, int priority, int spritePriority,
               int edge, int stageDistance, int lowBound, int highBound, int shadowRed,
               int shadowGreen, int shadowBlue, int superMoveTime, int pauseMoveTime,
               int afterImageTime, int afterImageLength):
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
shouldRemove(false){
    PaintownUtil::ReferenceCount<MugenAnimation> his = owner.getAnimation(animation);
    if (his != NULL){
        this->animation = PaintownUtil::ReferenceCount<MugenAnimation>(his->copy());
    }
}

Projectile::~Projectile(){
}
    
void Projectile::draw(const Graphics::Bitmap & work, double x, double y){
    if (animation != NULL){
        animation->render((int)(this->x - x), (int)(this->y -  y), work);
    }
}
    
void Projectile::logic(){
    if (animation != NULL){
        animation->logic();
    }

    x += velocityX;
    y += velocityY;
    /* FIXME: not sure if this calculation is right */
    velocityX = (velocityX + accelerateX) * velocityXMultipler;
    velocityY = (velocityY + accelerateY) * velocityYMultipler;

    if (removeTime > -1){
        removeTime -= 1;

        if (removeTime == 0){
            PaintownUtil::ReferenceCount<MugenAnimation> his = owner.getAnimation(removeAnimation);
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
