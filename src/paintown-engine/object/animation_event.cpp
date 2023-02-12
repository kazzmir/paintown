#include "animation.h"
#include "animation_event.h"
#include "projectile.h"
#include "r-tech1/token.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/tokenreader.h"
#include "r-tech1/funcs.h"
#include "r-tech1/file-system.h"
#include <string>

using namespace std;

namespace Paintown{

AnimationEvent::AnimationEvent( ){
}

void AnimationEvent::Interact( Animation * parent ){

}

AnimationEvent::~AnimationEvent(){
}

AnimationEventAttack::AnimationEventAttack(const vector<Attack> & a){
    attacks = a;
}
    
AnimationEvent::Type AnimationEventAttack::getType(){
    return AttackEvent;
}

void AnimationEventAttack::Interact(Animation * animation){
    animation->setAttacks(attacks);
}

/*
void AnimationEventAttack::Interact( Animation * animation ){
	animation->setAttack( x1, y1, x2, y2 );
	animation->setDamage( damage );
}
*/

AnimationEventBBox::AnimationEventBBox( int _x1, int _y1, int _x2, int _y2 ):
x1(_x1),
y1(_y1),
x2(_x2),
y2(_y2){
}


void AnimationEventBBox::Interact( Animation * animation ){
	animation->setBBox( x1, y1, x2, y2 );
}
    
AnimationEvent::Type AnimationEventBBox::getType(){
    return BBox;
}

AnimationEventCoords::AnimationEventCoords( int _x, int _y, int _z ):
AnimationEvent(),
x( _x ),
y( _y ),
z( _z ){
}
    
AnimationEvent::Type AnimationEventCoords::getType(){
    return Coords;
}

void AnimationEventCoords::Interact( Animation * animation ){
	if ( x != -1 )
		animation->setX( x );
	if ( y != -1 )
		animation->setY( y );
	if ( z != -1 )
		animation->setZ( z );
}

AnimationEventDelay::AnimationEventDelay( double _delay ):
AnimationEvent(),
delay(_delay){
}

AnimationEvent::Type AnimationEventDelay::getType(){
    return Delay;
}

void AnimationEventDelay::Interact( Animation * animation ){
    animation->setDelay(delay);
}

AnimationEventFace::AnimationEventFace( int direction ):
direction(direction){
}
    
AnimationEvent::Type AnimationEventFace::getType(){
    return Facing;
}
    
void AnimationEventFace::Interact( Animation * animation ){
	animation->setFacing( direction );
}

AnimationEventFrame::AnimationEventFrame( const string & _path ){
    path = _path;
}

AnimationEvent::Type AnimationEventFrame::getType(){
    return Frame;
}

void AnimationEventFrame::Interact( Animation * animation ){
    animation->setFrame( path );
    animation->Delay();
}

AnimationEventFrame::~AnimationEventFrame(){
}

AnimationEventJump::AnimationEventJump( double vx, double vy, double vz ):
vx( vx ),
vy( vy ),
vz( vz ){
}
    
AnimationEvent::Type AnimationEventJump::getType(){
    return Jump;
}

void AnimationEventJump::Interact( Animation * animation ){
    animation->jump(vx, vy, vz);
}

AnimationEventMove::AnimationEventMove(int _x, int _y, int _z):
x(_x),
y(_y),
z(_z){
}

void AnimationEventMove::Interact(Animation * animation){
    animation->moveX(x * animation->getScale());
    animation->moveY(y * animation->getScale());
    animation->moveZ(z * animation->getScale());
}
    
AnimationEvent::Type AnimationEventMove::getType(){
    return Move;
}

AnimationEventNOP::AnimationEventNOP():
AnimationEvent(){
}

void AnimationEventNOP::Interact( Animation * animation ){
    animation->Delay();
}
    
AnimationEvent::Type AnimationEventNOP::getType(){
    return Nop;
}

AnimationEventOffset::AnimationEventOffset( int _x, int _y ):
AnimationEvent(),
x(_x),
y(_y){
}
    
AnimationEvent::Type AnimationEventOffset::getType(){
    return Offset;
}

void AnimationEventOffset::Interact(Animation * animation){
    animation->setOffsetX(x * animation->getScale());
    animation->setOffsetY(y * animation->getScale());
}

AnimationEventRelativeOffset::AnimationEventRelativeOffset(int x, int y):
x(x), y(y){
}

void AnimationEventRelativeOffset::Interact(Animation * animation){
    animation->setOffsetX(animation->getOffsetX() + x * animation->getScale());
    animation->setOffsetY(animation->getOffsetY() + y * animation->getScale());
}

AnimationEvent::Type AnimationEventRelativeOffset::getType(){
    return RelativeOffset;
}

AnimationEventProjectile::AnimationEventProjectile(const Token * token ):
projectile( NULL ),
x( 0 ),
y( 0 ),
dx( 0 ),
dy( 0 ),
life( 0 ){
    if ( *token != "projectile" ){
        throw LoadException(__FILE__, __LINE__, "Token starts with " + token->getName() + " instead of 'projectile'." );
    }

    TokenView view = token->view();
    while (view.hasMore()){
        const Token * current;
        view >> current;
        if ( *current == "at" ){
            int x, y;
            current->view() >> x >> y;
            setX( x );
            setY( y );
        } else if ( *current == "path" ){
            string path;
            current->view() >> path;
            Filesystem::AbsolutePath full = Storage::instance().find(Filesystem::RelativePath(path));
            TokenReader reader;
            try{
                /* Its important that the result from readTokenFromFile is put into
                 * a temporary variable to ensure that the Storage::File reference
                 * is destroyed before we create the Projectile class.
                 */
                Token * data = reader.readTokenFromFile(*Storage::instance().open(full));
                projectile = new Projectile(data);
            } catch (const TokenException & ex){
                // cerr<< "Could not read " << full.path() <<" : " << ex.getReason() << endl;
                // delete head;
                throw LoadException(__FILE__, __LINE__, ex, "Could not open projectile file: " + full.path());
            }
        } else if ( *current == "life" ){
            int life;
            current->view() >> life;
            setLife( life );
        } else if ( *current == "speed" ){
            double dx, dy;
            current->view() >> dx >> dy;
            setDX( dx );
            setDY( dy );
        }
    }

    if ( projectile == NULL ){
        throw LoadException(__FILE__, __LINE__, "No 'path' token given for projectile" );	
    }

    if ( life == 0 ){
        throw LoadException(__FILE__, __LINE__, "No 'life' token given for projectile" );
    }
}

void AnimationEventProjectile::Interact(Animation * animation){
    Projectile * copy = (Projectile *) projectile->copy();
    copy->setDX(getDX());
    copy->setDY(getDY());
    copy->setLife(getLife());
    animation->createProjectile(getX() * animation->getScale(), -getY() * animation->getScale(), copy);
}
	
AnimationEventProjectile::~AnimationEventProjectile(){
    if (projectile != NULL){
        delete projectile;
    }
}
    
AnimationEvent::Type AnimationEventProjectile::getType(){
    return ProjectileEvent;
}

AnimationEventShadow::AnimationEventShadow( int x, int y ):
AnimationEvent(),
x( x ),
y( y ){
}
    
AnimationEvent::Type AnimationEventShadow::getType(){
    return Shadow;
}

void AnimationEventShadow::Interact( Animation * animation ){
	animation->setShadowX( x );
	animation->setShadowY( y );
}

AnimationEventSound::AnimationEventSound( const string & _path ){
	path = _path;
}

AnimationEvent::Type AnimationEventSound::getType(){
    return Sound;
}

void AnimationEventSound::Interact( Animation * animation ){
	animation->playSound( path );
}
	
AnimationEventSound::~AnimationEventSound(){
}

AnimationEventStatus::AnimationEventStatus( int _stat ):
AnimationEvent(),
status( _stat ){
}

void AnimationEventStatus::Interact( Animation * animation ){
	animation->setStatus( status );
}
    
AnimationEvent::Type AnimationEventStatus::getType(){
    return Status;
}

AnimationEventTicket::AnimationEventTicket():
AnimationEvent(){
}

void AnimationEventTicket::Interact( Animation * animation ){
	animation->nextTicket();
}
    
AnimationEvent::Type AnimationEventTicket::getType(){
    return Ticket;
}

AnimationEventTrail::AnimationEventTrail(const int produce, const int life):
produce(produce),
life(life){
}

void AnimationEventTrail::Interact(Animation * animation){
    animation->setTrails(produce, life);
}
    
AnimationEvent::Type AnimationEventTrail::getType(){
    return Trail;
}

AnimationEventZDistance::AnimationEventZDistance( const double d ):
d( d ){
}

void AnimationEventZDistance::Interact( Animation * animation ){
	animation->setMinZDistance( d );
}
    
AnimationEvent::Type AnimationEventZDistance::getType(){
    return ZDistance;
}

AnimationEventCanBeHit::AnimationEventCanBeHit(bool hit):
hit(hit){
}

void AnimationEventCanBeHit::Interact(Animation * animation){
    animation->setHittable(hit);
}
    
AnimationEvent::Type AnimationEventCanBeHit::getType(){
    return CanBeHit;
}

AnimationPerpetual::AnimationPerpetual(bool enabled):
enabled(enabled){
}

void AnimationPerpetual::Interact(Animation * animation){
    animation->setPerpetual(enabled);
}

AnimationEvent::Type AnimationPerpetual::getType(){
    return Perpetual;
}

}
