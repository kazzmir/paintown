#include "util/graphics/bitmap.h"
#include "network_character.h"
#include "object.h"
#include "object_messages.h"
#include "animation.h"
#include "util/font.h"
#include "util/funcs.h"
#include "factory/font_render.h"
#include "../game/nameplacer.h"
#include "util/file-system.h"
#include "globals.h"
#include "util/debug.h"
#include "../game/world.h"
#include <vector>
#include <math.h>

using namespace std;

namespace Paintown{

NetworkCharacter::NetworkCharacter( int alliance ):
Character(alliance),
name_id(-1),
show_name_time(0),
requestedName(false){
}

NetworkCharacter::NetworkCharacter( const char * filename, int alliance ):
Character( filename, alliance ),
name_id(-1),
show_name_time(0),
requestedName(false){
}

NetworkCharacter::NetworkCharacter( const Filesystem::AbsolutePath & filename, int alliance ):
Character( filename, alliance ),
name_id(-1),
show_name_time(0),
requestedName(false){
}

NetworkCharacter::NetworkCharacter( const Character & chr ):
Character(chr),
name_id(-1),
show_name_time(0),
requestedName(false){
}
	
NetworkCharacter::~NetworkCharacter(){
}
	
Object * NetworkCharacter::copy(){
    return new NetworkCharacter( *this );
}

/* player will send a grab message, network character should send a bogus message */
Network::Message NetworkCharacter::grabMessage( unsigned int from, unsigned int who ){
    Network::Message message;
    message.id = 0;
    message << World::IGNORE_MESSAGE;
    return message;
}

void NetworkCharacter::interpretMessage(World * world, Network::Message & message ){
    int type;
    message >> type;
    switch (type){
        case CharacterMessages::ShowName: {
            int amount;
            message >> amount;
            setNameTime(amount);
            break;
        }
        default : {
            message.reset();
            Character::interpretMessage(world, message);
            break;
        }
    }
}
	
void NetworkCharacter::setNameTime( int d ){
    show_name_time = d;
}
	
void NetworkCharacter::alwaysShowName(){
    setNameTime( -1 );
    Global::debug( 1 ) << getId() << " name time is " << show_name_time << endl;
}

void NetworkCharacter::drawFront(Graphics::Bitmap * work, int rel_x){
    if ( show_name_time > 0 || show_name_time == -1 ){
        int x1, y1;
        NamePlacer::getPlacement( x1, y1, name_id );

        if ( icon )
            icon->draw( x1, y1, *work );

        int hasIcon = icon ? icon->getWidth() : 0;

        const Font & player_font = Font::getDefaultFont(20, 20 );
        const string & name = getName();
        int nameHeight = player_font.getHeight( name ) / 2;
        nameHeight = 20 / 2;
        FontRender * render = FontRender::getInstance();
        render->addMessage( player_font, (hasIcon + x1) * 2, y1 * 2, Graphics::makeColor(255,255,255), Graphics::MaskColor(), name );
        drawLifeBar( hasIcon + x1, y1 + nameHeight, work );
        if ( show_name_time > 0 ){
            show_name_time -= 1;
        }
    } else {
        Global::debug( 2 ) << "Show name time for " << getId() << " is " << show_name_time << endl;
    }
}

/*
void NetworkCharacter::draw( Bitmap * work, int rel_x ){
	Character::draw( work, rel_x );
}
*/

Network::Message NetworkCharacter::requestNameMessage() const {
    Network::Message message;
    message.id = 0;
    message << World::RequestName;
    message << getId();
    return message;
}

/* just performs the current animation */
void NetworkCharacter::act( vector< Object * > * others, World * world, vector< Object * > * add ){
    /* by default the name will come from the definition file as opposed to from
     * the level itself. the name currently doesn't come in the create message
     * (although maybe it should), so instead for now we explicitly ask the server
     * to send us the name of this character.
     */
    if (!requestedName){
        world->addMessage(requestNameMessage());
        requestedName = true;
    }

    // Global::debug(3, __FILE__) << getId() << " status is " << getStatus() << endl;
    Character::act(others, world, add);
    if ((getStatus() == Status_Ground ||
         getStatus() == Status_Jumping) && animation_current->Act() ){
        // Global::debug( 0 ) << "Reset animation" << endl;
        if (animation_current->getName() != "idle" &&
            animation_current->getName() != "walk"){
            animation_current = getMovement("idle");
        }
        animation_current->reset();
    }
}
	
void NetworkCharacter::landed( World * world ){
    setThrown( false );
    switch (getStatus()){
        case Status_Falling : {
            if (landed_sound){
                landed_sound->play();
            }

            world->Quake((int)fabs(getYVelocity()));

            break;
        }
        case Status_Fell : {
            world->Quake((int)fabs(getYVelocity()));
            break;
        }
    }
}
	
void NetworkCharacter::deathReset(){
    setY(200);
    setMoving(true);
    setStatus(Status_Falling);
    setHealth(getMaxHealth());
    setInvincibility(400);
    setDeath(0);
    animation_current = getMovement("idle");
}

}
