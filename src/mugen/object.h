#ifndef _paintown_mugen_object_h
#define _paintown_mugen_object_h

// Implement object_attack
#include "paintown-engine/object/object_attack.h"

namespace Mugen{

class Stage;
class HitDefinition;

/* maybe put the physics stuff in a different file */
namespace Physics{

enum Type{
    None, /* N */
    Air, /* A */
    Stand, /* S */
    Crouch, /* C */
};

}

class Object: public Paintown::ObjectAttack {
public:
    Object(int alliance);
    Object(const int x, const int y, int alliance);
    Object(const Object & copy);

    virtual ~Object();
        
    /* paused from an attack */
    virtual bool isPaused() = 0;
    virtual Physics::Type getCurrentPhysics() const = 0;
    virtual double getGravity() const = 0;
    virtual void setYVelocity(double y) = 0;
    virtual double getYVelocity() const = 0;
    virtual void changeState(Mugen::Stage & stage, int state, const std::vector<std::string> & inputs) = 0;
    virtual double getXVelocity() const = 0;
    virtual void setXVelocity(double x) = 0;
    virtual bool canTurn() const = 0;
    virtual void doTurn(Mugen::Stage & stage) = 0;
    virtual double getStandingFriction() const = 0;
    virtual const std::string & getMoveType() const = 0;
    virtual HitDefinition & getHit() = 0;
    virtual const HitDefinition & getHit() const = 0;
    virtual int getCurrentJuggle() const = 0;
    virtual const std::vector<MugenArea> getAttackBoxes() const = 0;
    virtual const std::vector<MugenArea> getDefenseBoxes() const = 0;
    virtual int getDefaultSpark() const = 0;
    virtual int getDefaultGuardSpark() const = 0;
    virtual int getAttackDistance() const = 0;
    virtual void guarded(Object * enemy, const HitDefinition & hit) = 0;
    virtual void addPower(double d) = 0;
    virtual void wasHit(Mugen::Stage & stage, Object * enemy, const HitDefinition & hit) = 0;
    virtual void didHit(Object * enemy, Mugen::Stage & stage) = 0;
};

}

#endif
