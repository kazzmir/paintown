#ifndef _paintown_mugen_projectile_h
#define _paintown_mugen_projectile_h

namespace Mugen{

class Projectile{
public:
    Projectile(double x, double y, int id, int animation, int hitAnimation, int dieAnimation,
               int cancelAnimation, double scaleX, double scaleY, bool autoRemove, int removeTime, 
               double velocityX, double velocityY, double removeVelocityX, double removeVelocityY,
               double accelerateX, double accelerateY, double velocityXMultipler, 
               double velocityYMultipler, int hits, int missTime, int priority, int spritePriority,
               int edge, int stageDistance, int lowBound, int highBound, int shadowRed,
               int shadowGreen, int shadowBlue, int superMoveTime, int pauseMoveTime,
               int afterImageTime, int afterImageLength);

    virtual ~Projectile();
};

}

#endif
