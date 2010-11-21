#include "character.h"

namespace Mugen{
/* copy all data from the parent somehow, maybe lazily. to speed things up */
class Helper: public Object {
public:
    Helper(Character & parent);
    virtual ~Helper();

    /* Pure methods from Mugen::Object */

    /* paused from an attack */
    virtual bool isPaused();
    virtual Physics::Type getCurrentPhysics() const;
    virtual double getGravity() const;
    virtual void setYVelocity(double y);
    virtual double getYVelocity() const;
    virtual void changeState(MugenStage & stage, int state, const std::vector<std::string> & inputs);
    virtual double getXVelocity() const;
    virtual void setXVelocity(double x);
    virtual bool canTurn() const;
    virtual void doTurn(MugenStage & stage);
    virtual double getStandingFriction() const;
    virtual const std::string & getMoveType() const;
    virtual HitDefinition & getHit();
    virtual const HitDefinition & getHit() const;
    virtual int getCurrentJuggle() const;
    virtual const std::vector<MugenArea> getAttackBoxes() const;
    virtual const std::vector<MugenArea> getDefenseBoxes() const;
    virtual int getDefaultSpark() const;
    virtual int getDefaultGuardSpark() const;
    virtual int getAttackDistance() const;
    virtual void guarded(Object * enemy, const HitDefinition & hit);
    virtual void addPower(double d);
    virtual void wasHit(MugenStage & stage, Object * enemy, const HitDefinition & hit);
    virtual void didHit(Object * enemy, MugenStage & stage);
	
    /* Pure methods from Paintown::Object */
    virtual void act( std::vector< Paintown::Object * > * others, World * world, std::vector< Paintown::Object * > * add );
    virtual void draw(Bitmap * work, int rel_x, int rel_y);
    /* weve been grabbed */
    virtual void grabbed(Paintown::Object * obj);
    /* stop grabbing */
    virtual void unGrab();
    virtual bool isGrabbed();
    virtual Paintown::Object * copy();
    virtual const std::string & getAttackName();
    virtual bool collision( ObjectAttack * obj );
    virtual int getDamage() const;
    virtual bool isCollidable(Paintown::Object * obj );
    virtual bool isGettable();
    virtual bool isGrabbable(Paintown::Object * obj);
    virtual bool isAttacking();
    virtual int getWidth() const;
    virtual int getHeight() const;
    virtual Network::Message getCreateMessage();

    /* Pure methods from Paintown::ObjectAttack */

    virtual void getAttackCoords( int & x, int & y);
    virtual double minZDistance() const;
    virtual void attacked( World * world, Paintown::Object * something, std::vector< Paintown::Object * > & objects );

protected:
    Character & owner;
    HitDefinition hit;

};

}
