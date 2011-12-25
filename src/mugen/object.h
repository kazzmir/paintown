#ifndef _paintown_mugen_object_h
#define _paintown_mugen_object_h

namespace Mugen{

class Stage;
struct HitDefinition;

/* maybe put the physics stuff in a different file */
namespace Physics{

enum Type{
    None, /* N */
    Air, /* A */
    Stand, /* S */
    Crouch, /* C */
};

}

enum Facing{
    FacingLeft, FacingRight
};

class Object{
public:
    Object(int alliance);
    Object(const int x, const int y, int alliance);
    Object(const Object & copy);

    virtual ~Object();

    inline double getY() const {
        return virtualy;
    }

    inline double getX() const {
        return virtualx;
    }

    inline double getZ() const {
        return virtualz;
    }

    virtual void setZ(double what);

    /* get the actual y coordinate on screen */
    virtual double getRY() const;

    virtual void setX(double what);
    virtual void setY(double what);
    virtual void moveX(double x, bool force = false);
    virtual void moveY(double y, bool force = false);
    virtual void moveLeft(double x);
    virtual void moveRight(double y);
    virtual void moveLeftForce(double x);
    virtual void moveRightForce(double y);

    virtual void unbind(Object * who) = 0;
    
    virtual double getHealth() const = 0;
        
    virtual void drawReflection(Graphics::Bitmap * work, int rel_x, int rel_y, int intensity) = 0;
    virtual void drawMugenShade(Graphics::Bitmap * work, int rel_x, int intensity, Graphics::Color color, double scale, int fademid, int fadehigh) = 0;
    virtual void draw(Graphics::Bitmap * work, int rel_x, int rel_y) = 0;
    
    virtual void act(std::vector<Mugen::Object*>*, Stage*, std::vector<Mugen::Object*>*) = 0;
    
    virtual int getAlliance() const;
    virtual void setAlliance(int alliance);
    virtual Facing getFacing() const;
    virtual void setFacing(Facing what);
    virtual Facing getOppositeFacing() const;
    virtual int getHeight() const = 0;
	
    virtual int getObjectId() const;

    virtual unsigned int getTicket() const;
    virtual void nextTicket();
    
    virtual void doMovement(const std::vector<Object*> & objects, Stage & stage);

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
    virtual double getGroundFriction() const = 0;
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
    virtual void didHitGuarded(Object * enemy, Mugen::Stage & stage) = 0;
protected:
    double virtualx;
    double virtualy;
    double virtualz;
    unsigned int attack_ticket;
    int alliance;
    Facing facing;
    int objectId;
    unsigned int ticket;
};

}

#endif
