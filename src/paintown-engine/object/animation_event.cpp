#include "animation.h"
#include "animation_event.h"
#include "projectile.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "util/funcs.h"
#include "util/file-system.h"
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

AnimationEventCoords::AnimationEventCoords( int _x, int _y, int _z ):
AnimationEvent(),
x( _x ),
y( _y ),
z( _z ){
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

void AnimationEventDelay::Interact( Animation * animation ){
    animation->setDelay(delay);
}

AnimationEventFace::AnimationEventFace( int direction ):
direction(direction){
}

void AnimationEventFace::Interact( Animation * animation ){
	animation->setFacing( direction );
}

AnimationEventFrame::AnimationEventFrame( const string & _path ){
    path = _path;
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

void AnimationEventJump::Interact( Animation * animation ){
	animation->jump( vx, vy, vz );
}

AnimationEventMove::AnimationEventMove( int _x, int _y, int _z ):
x( _x ),
y( _y ),
z( _z ){
}

void AnimationEventMove::Interact( Animation * animation ){
	animation->moveX( x );
	animation->moveY( y );
	animation->moveZ( z );
}

AnimationEventNOP::AnimationEventNOP():
AnimationEvent(){
}

void AnimationEventNOP::Interact( Animation * animation ){
	animation->Delay();
}

AnimationEventOffset::AnimationEventOffset( int _x, int _y ):
AnimationEvent(),
x( _x ),
y( _y ){
}

void AnimationEventOffset::Interact( Animation * animation ){
	animation->setOffsetX( x );
	animation->setOffsetY( y );
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
            TokenReader reader(full.path());
            try{
                projectile = new Projectile(reader.readToken());
            } catch ( const TokenException & ex ){
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

void AnimationEventProjectile::Interact( Animation * animation ){
	Projectile * copy = (Projectile *) projectile->copy();
	copy->setDX( getDX() );
	copy->setDY( getDY() );
	copy->setLife( getLife() );
	animation->createProjectile( getX(), -getY(), copy );
}
	
AnimationEventProjectile::~AnimationEventProjectile(){
	if ( projectile != NULL ){
		delete projectile;
	}
}

AnimationEventShadow::AnimationEventShadow( int x, int y ):
AnimationEvent(),
x( x ),
y( y ){
}

void AnimationEventShadow::Interact( Animation * animation ){
	animation->setShadowX( x );
	animation->setShadowY( y );
}

AnimationEventSound::AnimationEventSound( const string & _path ){
	path = _path;
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

AnimationEventTicket::AnimationEventTicket():
AnimationEvent(){
}

void AnimationEventTicket::Interact( Animation * animation ){
	animation->nextTicket();
}

AnimationEventTrail::AnimationEventTrail(const int produce, const int life):
produce(produce),
life(life){
}

void AnimationEventTrail::Interact(Animation * animation){
    animation->setTrails(produce, life);
}

AnimationEventZDistance::AnimationEventZDistance( const double d ):
d( d ){
}

void AnimationEventZDistance::Interact( Animation * animation ){
	animation->setMinZDistance( d );
}

}
