#include "effect.h"
#include "animation.h"
#include "stage.h"
#include "character.h"
#include "util/graphics/bitmap.h"

namespace Mugen{

Effect::Effect(const Character * owner, PaintownUtil::ReferenceCount<Animation> animation, int id, int x, int y, double scaleX, double scaleY, int spritePriority):
owner(owner),
/* Copy the animation here so that it can start from frame 0 and not accidentally
 * be shared with another Effect
 */
animation(PaintownUtil::ReferenceCount<Animation>(new Animation(*animation))),
id(id),
x(x),
y(y),
scaleX(scaleX),
scaleY(scaleY),
spritePriority(spritePriority){
}
    
void Effect::draw(const Graphics::Bitmap & work, int cameraX, int cameraY){
    animation->render((int)(x - cameraX), (int)(y - cameraY), work, scaleX, scaleY);
}

void Effect::logic(){
    animation->logic();
}

void Effect::superPauseStart(){
}

void Effect::superPauseEnd(){
}
    
int Effect::getSpritePriority() const {
    return spritePriority;
}

bool Effect::isDead(){
    return animation->hasLooped();
}

Effect::~Effect(){
}

/* FIXME: can sparks be scaled? */
Spark::Spark(int x, int y, int spritePriority, PaintownUtil::ReferenceCount<Animation> animation):
Effect(NULL, animation, -1, x, y, 1, 1, spritePriority){
}

Spark::~Spark(){
}

/* FIXME: this came from state-controller.cpp. maybe it can be shared */
static void computePosition(double posX, double posY, const Character * owner, const Stage & stage, PositionType positionType, bool horizontalFlip, double & x, double & y){
    posX *= horizontalFlip ? -1 : 1;
    switch (positionType){
        case PositionPlayer1: {
            x = posX + owner->getX();
            y = posY + owner->getRY();
            break;
        }
        case PositionPlayer2: {
            Character * enemy = stage.getEnemy(owner);
            if (enemy != NULL){
                x = posX + enemy->getX();
                y = posX + enemy->getRY();
            }
            break;
        }
        case PositionFront: {
            if (owner->getFacing() == FacingRight){
                x = stage.maximumRight(owner) + posX;
            } else {
                x = stage.maximumLeft(owner) + posX;
            }
            y = stage.maximumUp() + posY;
            break;
        }
        case PositionBack: {
            if (owner->getFacing() == FacingLeft){
                x = stage.maximumRight(owner) + posX;
            } else {
                x = stage.maximumLeft(owner) + posX;
            }
            y = stage.maximumUp() + posY;
            break;
        }
        case PositionLeft: {
            x = stage.maximumLeft(owner) + posX;
            y = stage.maximumUp() + posY;
            break;
        }
        case PositionRight: {
            x = stage.maximumRight(owner) + posX;
            y = stage.maximumUp() + posY;
            break;
        }
        default: {
            x = posX;
            y = posY;
            break;
        }
    }
}

ExplodeEffect::ExplodeEffect(const Character * owner, const Mugen::Stage & stage, ::Util::ReferenceCount<Animation> animation, int id, int x, int y, double velocityX, double velocityY, double accelerationX, double accelerationY, int removeTime, int bindTime, PositionType positionType, int posX, int posY, double scaleX, double scaleY, int spritePriority, bool superMove, int superMoveTime, bool horizontalFlip, bool verticalFlip, bool ownPalette, bool removeOnHit):
Effect(owner, animation, id, x, y, scaleX, scaleY, spritePriority),
stage(stage),
velocityX(velocityX),
velocityY(velocityY),
accelerationX(accelerationX),
accelerationY(accelerationY),
removeTime(removeTime),
bindTime(bindTime),
positionType(positionType),
posX(posX),
posY(posY),
frozen(false),
superMovePersist(superMove),
superMoveTime(superMoveTime),
horizontalFlip(horizontalFlip),
verticalFlip(verticalFlip),
ownPalette(ownPalette),
removeOnHit(removeOnHit),
hitCount(owner->getWasHitCount()),
shouldRemove(false){
}



void ExplodeEffect::logic(){
    if (removeOnHit){
        shouldRemove = owner->getWasHitCount() > hitCount;
    }

    if (!frozen){
        Effect::logic();

        /* either stopped or not set at all.
         * FIXME: sort of a hack.. find a more elegant solution
         */
        if (bindTime == 0 || bindTime == -2){
            int reverse = horizontalFlip ? -1 : 1;
            x += velocityX * reverse;
            y += velocityY;
            velocityX += accelerationX * reverse;
            velocityY += accelerationY;
        } else {
        }

        if (removeTime > 0){
            removeTime -= 1;
        }
    } else {
        if (superMoveTime > 0){
            superMoveTime -= 1;
            /* If we run out of super move time then we should freeze */
            if (superMoveTime == 0){
                frozen = true;
            }
        }
    }

    /* FIXME: should we do the bind even if we are frozen? */
    if (bindTime != 0){
        /* bindTime could be negative in which case its active forever */
        if (bindTime > 0){
            bindTime -= 1;
        }

        /* -2 is the default bindtime that means it wasn't set in the explod.
         * if the bindtime is -1 then it should act indefinetely.
         */
        if (bindTime != -2){
            /* FIXME: should we always use the same horizontalFlip here? */
            computePosition(posX, posY, owner, stage, positionType, horizontalFlip, x, y);
        }
    }
}

void ExplodeEffect::draw(const Graphics::Bitmap & work, int cameraX, int cameraY){
    animation->render(horizontalFlip, verticalFlip, (int)(getX() - cameraX), (int)(getY() - cameraY), work, scaleX, scaleY, NULL);
}

bool ExplodeEffect::isDead(){
    if (shouldRemove){
        return true;
    }

    switch (removeTime){
        case -2: return Effect::isDead();
        case -1: return false;
        default : return removeTime == 0;
    }

    return true;
}

void ExplodeEffect::superPauseStart(){
    if (!superMovePersist && superMoveTime == 0){
        frozen = true;
    }
}

void ExplodeEffect::superPauseEnd(){
    /* Unfreeze no matter what */
    frozen = false;
    superMoveTime = 0;
}

}
