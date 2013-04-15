#ifndef _paintown_animation_event_h
#define _paintown_animation_event_h

#include <vector>

#include "attack.h"
#include "util/exceptions/load_exception.h"

class Token;

namespace Paintown{

class Projectile;
class Animation;

class AnimationEvent{
public:
    /* Event is suffixed when the name conflicts with some other type */
    enum Type{
        /* Defines an attack box and associated parameters (damage, force) */
        AttackEvent,

        /* Moves the objects in x/y/z planes */
        Move,

        /* Animation trails */
        Trail,

        /* Forces the object to increase its attack ticket */
        Ticket,

        /* Change status - ground, jumping, etc */
        Status,

        /* Play a sound */
        Sound,

        /* ?? */
        Shadow,

        /* Absolute animation offset */
        Offset,

        /* Relative animation offset */
        RelativeOffset,

        /* Nothing */
        Nop,
        Coords,
        Jump,
        ZDistance,
        Frame,
        Delay,
        Facing,
        ProjectileEvent,
        BBox,
        CanBeHit,
        Perpetual
    };

    AnimationEvent();
    virtual void Interact(Animation * animation);
    virtual Type getType() = 0;
    virtual ~AnimationEvent();
};

class Animation;

class AnimationEventAttack: public AnimationEvent {
public:
    AnimationEventAttack(const std::vector<Attack> & a);
    virtual void Interact(Animation * animation); 
    virtual Type getType();

protected:
    std::vector<Attack> attacks;
};

class AnimationEventBBox: public AnimationEvent {
public:
    AnimationEventBBox( int _x1, int _y1, int _x2, int _y2 );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    int x1, y1, x2, y2;
};

class AnimationEventCoords: public AnimationEvent{
public:
    AnimationEventCoords( int _x, int _y, int _z );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    int x, y, z;
};

class AnimationEventDelay: public AnimationEvent{
public:
    AnimationEventDelay( double _delay );

    virtual void Interact( Animation * animation );
    virtual Type getType();

protected:
    double delay;
};

class AnimationEventFace: public AnimationEvent{
public:
    AnimationEventFace( int direction );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    int direction;
};

class AnimationEventFrame: public AnimationEvent{
public:
    AnimationEventFrame( const std::string & _path );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

    virtual ~AnimationEventFrame();

protected:
    std::string path;
};

class AnimationEventJump: public AnimationEvent{
public:
    AnimationEventJump( double vx, double vy, double vz );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    double vx, vy, vz;
};

class AnimationEventMove: public AnimationEvent{
public:
    AnimationEventMove( int _x, int _y, int _z );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    int x, y, z;
};

class AnimationEventNOP: public AnimationEvent{
public:
    AnimationEventNOP();

    virtual void Interact( Animation * animation ); 
    virtual Type getType();
};

/* sets the current offsets of the animation */
class AnimationEventOffset: public AnimationEvent {
public:
    AnimationEventOffset( int _x, int _y );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    int x, y;
};

/* adds to the current offsets of the animation */
class AnimationEventRelativeOffset: public AnimationEvent {
public:
    AnimationEventRelativeOffset(int x, int y);

    virtual void Interact(Animation * animation); 
    virtual Type getType();

protected:
    int x, y;
};

class AnimationEventProjectile: public AnimationEvent {
public:
    AnimationEventProjectile(const Token * token );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

    virtual ~AnimationEventProjectile();

protected:
    inline void setX( int x ){
        this->x = x;
    }

    inline void setY( int y ){
        this->y = y;
    }

    inline int getX() const {
        return x;
    }

    inline int getY() const {
        return y;
    }

    inline void setDX( double dx ){
        this->dx = dx;
    }

    inline void setDY( double dy ){
        this->dy = dy;
    }

    inline double getDX() const {
        return this->dx;
    }

    inline double getDY() const {
        return this->dy;
    }

    inline void setLife( int life ){
        this->life = life;
    }

    inline int getLife() const {
        return life;
    }

    Projectile * projectile;
    int x, y;
    double dx, dy;
    int life;
};

class AnimationEventShadow: public AnimationEvent {
public:
    AnimationEventShadow( int x, int y );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:

    int x, y;
};

class AnimationEventSound: public AnimationEvent {
public:
    AnimationEventSound( const std::string & path );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

    virtual ~AnimationEventSound();

protected:

    std::string path;
};

class AnimationEventStatus: public AnimationEvent{
public:
    AnimationEventStatus( int _stat );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    int status;
};

class AnimationEventTicket: public AnimationEvent{
public:
    AnimationEventTicket();

    virtual void Interact( Animation * animation ); 
    virtual Type getType();
};

class AnimationEventTrail: public AnimationEvent {
public:
    AnimationEventTrail(const int produce, const int life);
    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    const int produce;
    const int life;
};

class AnimationEventZDistance: public AnimationEvent {
public:
    AnimationEventZDistance( const double d );

    virtual void Interact( Animation * animation ); 
    virtual Type getType();

protected:
    double d;
};

class AnimationEventCanBeHit: public AnimationEvent {
public:
    AnimationEventCanBeHit(bool hit);

    virtual void Interact(Animation * animation);
    virtual Type getType();

protected:
    bool hit;
};

class AnimationPerpetual: public AnimationEvent {
public:
    AnimationPerpetual(bool enabled);
    virtual void Interact(Animation * animation);
    virtual Type getType();

protected:
    bool enabled;
};

}

#endif
