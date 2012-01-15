#include "projectile.h"
#include "util/bitmap.h"

namespace Mugen{

Projectile::Projectile(double x, double y, int id, int animation, int hitAnimation, int dieAnimation,
               int cancelAnimation, double scaleX, double scaleY, bool autoRemove, int removeTime, 
               double velocityX, double velocityY, double removeVelocityX, double removeVelocityY,
               double accelerateX, double accelerateY, double velocityXMultipler, 
               double velocityYMultipler, int hits, int missTime, int priority, int spritePriority,
               int edge, int stageDistance, int lowBound, int highBound, int shadowRed,
               int shadowGreen, int shadowBlue, int superMoveTime, int pauseMoveTime,
               int afterImageTime, int afterImageLength):
spritePriority(spritePriority){
}

Projectile::~Projectile(){
}
    
void Projectile::draw(const Graphics::Bitmap & work, double x, double y){
}
    
void Projectile::logic(){
}
    
int Projectile::getSpritePriority() const {
    return spritePriority;
}

}
