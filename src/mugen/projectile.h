#ifndef _paintown_mugen_projectile_h
#define _paintown_mugen_projectile_h

#include "util/pointer.h"

namespace Graphics{
class Bitmap;
}

class MugenAnimation;
namespace Mugen{

class Character;
class Projectile{
public:
    Projectile(double x, double y, int id, const Character & owner, int animation, int hitAnimation, int dieAnimation,
               int cancelAnimation, double scaleX, double scaleY, bool autoRemove, int removeTime, 
               double velocityX, double velocityY, double removeVelocityX, double removeVelocityY,
               double accelerateX, double accelerateY, double velocityXMultipler, 
               double velocityYMultipler, int hits, int missTime, int priority, int spritePriority,
               int edge, int stageDistance, int lowBound, int highBound, int shadowRed,
               int shadowGreen, int shadowBlue, int superMoveTime, int pauseMoveTime,
               int afterImageTime, int afterImageLength);

    virtual ~Projectile();

    virtual int getSpritePriority() const;
    virtual void draw(const Graphics::Bitmap & work, double x, double y);
    virtual void logic();

    virtual bool isDead() const;

protected:
    const Character & owner;
    int spritePriority;
    ::Util::ReferenceCount<MugenAnimation> animation;
    double x, y;
    double velocityX, velocityY;
    double accelerateX, accelerateY;
    double velocityXMultipler, velocityYMultipler;
    double removeVelocityX, removeVelocityY;
    int removeTime;
    int removeAnimation;
    bool shouldRemove;
};

}

#endif
