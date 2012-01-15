#ifndef _paintown_mugen_projectile_h
#define _paintown_mugen_projectile_h

#include "util/pointer.h"
#include "object.h"
#include "animation.h"
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
    Projectile(double x, double y, int id, const Character * owner, int animation, int hitAnimation, int dieAnimation,
               int cancelAnimation, double scaleX, double scaleY, bool autoRemove, int removeTime, 
               double velocityX, double velocityY, double removeVelocityX, double removeVelocityY,
               double accelerateX, double accelerateY, double velocityXMultipler, 
               double velocityYMultipler, int hits, int missTime, int priority, int spritePriority,
               int edge, int stageDistance, int lowBound, int highBound, int shadowRed,
               int shadowGreen, int shadowBlue, int superMoveTime, int pauseMoveTime,
               int afterImageTime, int afterImageLength, Facing facing);

    virtual ~Projectile();

    virtual int getSpritePriority() const;
    virtual void draw(const Graphics::Bitmap & work, double x, double y);
    virtual void logic();

    virtual double getX() const;
    virtual double getY() const;

    virtual const int getId() const;

    /* False if the miss time is still active */
    bool canCollide() const;

    virtual const Character * getOwner() const;
        
    const std::vector<MugenArea> getAttackBoxes() const;

    void doCollision(Object * mugen);

    virtual bool isDead() const;

protected:
    const Character * owner;
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
};

}

#endif
