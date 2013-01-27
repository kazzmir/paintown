#ifndef _paintown_mugen_effect_h
#define _paintown_mugen_effect_h

#include "util/pointer.h"
#include "common.h"

namespace Graphics{
    class Bitmap;
}

namespace Mugen{

class Animation;
class Character;

class Effect{
public:
    Effect(const Character * owner, ::Util::ReferenceCount<Animation> animation, int id, int x, int y, double scaleX, double scaleY, int spritePriority);
    
    virtual void draw(const Graphics::Bitmap & work, int cameraX, int cameraY);
    virtual void logic();
    virtual bool isDead();
    virtual int getSpritePriority() const;

    virtual inline const Character * getOwner() const {
        return owner;
    }

    virtual void superPauseStart();
    virtual void superPauseEnd();

    virtual inline double getX() const {
        return x;
    }

    virtual inline double getY() const {
        return y;
    }

    virtual void setX(double x){
        this->x = x;
    }

    virtual void setY(double y){
        this->y = y;
    }

    virtual inline int getId() const {
        return id;
    }

    virtual ~Effect();
protected:
    const Character * owner;
    ::Util::ReferenceCount<Animation> animation;
    int id;
    double x;
    double y;
    double scaleX;
    double scaleY;
    int spritePriority;
};

class Spark: public Effect {
public:
    Spark(int x, int y, int spritePriority, ::Util::ReferenceCount<Animation> animation);
    virtual ~Spark();
};

class ExplodeEffect: public Effect {
    public:
        ExplodeEffect(const Character * owner, const Mugen::Stage & stage, ::Util::ReferenceCount<Animation> animation, int id, int x, int y, double velocityX, double velocityY, double accelerationX, double accelerationY, int removeTime, int bindTime, PositionType positionType, int posX, int posY, double scaleX, double scaleY, int spritePriority, bool superMove, int superMoveTime, bool horizontalFlip, bool verticalFlip, bool ownPalette, bool removeOnHit);

        void setVelocityX(double x){
            velocityX = x;
        }

        double getVelocityX() const {
            return velocityX;
        }

        void setVelocityY(double y){
            velocityY = y;
        }
        
        double getVelocityY() const {
            return velocityY;
        }

        void setAccelerationX(double x){
            accelerationX = x;
        }

        void setBindTime(int time){
            bindTime = time;
        }

        double getAccelerationX() const {
            return accelerationX;
        }

        void setAccelerationY(double y){
            accelerationY = y;
        }

        double getAccelerationY() const {
            return accelerationY;
        }

        void setRemoveTime(int time){
            removeTime = time;
        }

        int getRemoveTime() const {
            return removeTime;
        }
    
        virtual void superPauseStart();
        virtual void superPauseEnd();
        virtual void logic();
        virtual void draw(const Graphics::Bitmap & work, int cameraX, int cameraY);
        virtual bool isDead();

        const Mugen::Stage & stage;
        double velocityX;
        double velocityY;
        double accelerationX;
        double accelerationY;
        int removeTime;
        int bindTime;
        PositionType positionType;
        const int posX;
        const int posY;
        bool frozen;
        const bool superMovePersist;
        int superMoveTime;
        const bool horizontalFlip;
        const bool verticalFlip;
        const bool ownPalette;
        const bool removeOnHit;
        const unsigned int hitCount;
        bool shouldRemove;

};

}

#endif
