#ifndef _paintown_mugen_projectile_h
#define _paintown_mugen_projectile_h

#include "util/pointer.h"
#include "object.h"
#include "animation.h"
#include "common.h"
#include <vector>

namespace Graphics{
class Bitmap;
}

class MugenAnimation;
namespace Mugen{

class Object;
class Character;
class Projectile{
public:
    Projectile(double x, double y, int id, Character * owner, int animation, int hitAnimation, int dieAnimation,
               int cancelAnimation, double scaleX, double scaleY, bool autoRemove, int removeTime, 
               double velocityX, double velocityY, double removeVelocityX, double removeVelocityY,
               double accelerateX, double accelerateY, double velocityXMultipler, 
               double velocityYMultipler, int hits, int missTime, int priority, int spritePriority,
               int edge, int stageDistance, int lowBound, int highBound, int shadowRed,
               int shadowGreen, int shadowBlue, int superMoveTime, int pauseMoveTime,
               int afterImageTime, int afterImageLength, Facing facing, const HitDefinition & hit);

    virtual ~Projectile();

    virtual int getSpritePriority() const;
    virtual void draw(const Graphics::Bitmap & work, double x, double y);
    virtual void logic();

    /* Projectile priority. If this projectile has higher priority than another projectile
     * then if the projectiles collide, the other projectile will cancel.
     */
    virtual int getPriority() const;

    /* This projectile is being canceled by the higher priority projectile */
    virtual void canceled(const Stage & stage, Projectile * higher);

    virtual double getX() const;
    virtual double getY() const;

    virtual const int getId() const;

    /* Ticks since this projectile hit something */
    virtual unsigned long int getLastHitTicks() const;

    /* Ticks since this projectile was canceled */
    virtual int getLastCancelTicks() const;

    /* False if the miss time is still active */
    bool canCollide() const;

    virtual Character * getOwner() const;
        
    const std::vector<MugenArea> getAttackBoxes() const;
    const std::vector<MugenArea> getDefenseBoxes() const;

    void doCollision(Object * mugen, const Stage & stage);

    virtual bool isDead() const;

    virtual const HitDefinition & getHitDefinition() const {
        return hit;
    }

protected:
    Character * owner;
    int spritePriority;
    ::Util::ReferenceCount<MugenAnimation> animation;
    double x, y;
    double velocityX, velocityY;
    double accelerateX, accelerateY;
    double velocityXMultipler, velocityYMultipler;
    double removeVelocityX, removeVelocityY;
    int removeTime;
    const int removeAnimation;
    bool shouldRemove;
    double scaleX, scaleY;
    Facing facing;
    int hits;
    const int missTime;
    int activeMissTime;
    const int hitAnimation;
    const int id;
    HitDefinition hit;
    int priority;
    int cancelAnimation;
    unsigned long int lastHitTicks;
    int lastCanceled;
};

}

#endif
