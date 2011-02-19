#include "util/bitmap.h"
#include "item.h"
#include "object_attack.h"
#include "stimulation.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/debug.h"
#include "../game/world.h"
#include "util/ebox.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include <iostream>
#include <string>

using namespace std;

namespace Paintown{

Item::Item( const Filesystem::AbsolutePath & filename, Stimulation * const stimulation ) throw( LoadException ):
ObjectNonAttack( 0, 0 ),
collide( 0 ),
stimulation( stimulation ){
	TokenReader tr(filename.path());

	setMaxHealth( 1 );
	setHealth( 1 );

        try{
            Token * head;
            head = tr.readToken();

            if ( *head != "item" ){
                throw LoadException(__FILE__, __LINE__, "Item does not begin with 'item'" );
            }

            TokenView view = head->view();
            while (view.hasMore()){
                const Token * next = NULL;
                view >> next;
                if (*next == "frame"){
                    string file;
                    next->view() >> file;
                    picture.load(Filesystem::find(Filesystem::RelativePath(file)).path());
                } else if (*next == "sound"){
                    string path;
                    next->view() >> path;
                    sound = Sound(Filesystem::find(Filesystem::RelativePath(path)).path());
                }
            }
        } catch (const TokenException & ex){
            // Global::debug(0) << "Could not read "<<filename.path()<< ": "<<ex.getReason()<<endl;
            throw LoadException(__FILE__, __LINE__, ex, "Could not open item file");
	}

	collide = new ECollide( picture );
	path = filename;
}

Item::Item( const Item & item ):
ObjectNonAttack( item ),
collide( 0 ),
stimulation( item.copyStimulation() ){
	this->picture = item.picture;
	collide = new ECollide( this->picture );
	setHealth( item.getHealth() );
	sound = item.sound;
	path = item.getPath();
	setX( item.getX() );
	setY( item.getY() );
	setZ( item.getZ() );
}
	
Stimulation * Item::copyStimulation() const {
	return getStimulation()->copy();
}
	
Object * Item::copy(){
	return new Item( *this );
}
	
bool Item::isGettable(){
	return true;
}
	
void Item::touch( Object * obj ){
	obj->stimulate( *getStimulation() );
	sound.play();
}

ECollide * Item::getCollide() const {
	return collide;
}
	
bool Item::collision( ObjectAttack * obj ){
	if ( getAlliance() == obj->getAlliance() ){
		return false;
	}

	if ( this->getCollide() != 0 && obj->getCollide() != 0 ){
		bool my_xflip = false;
		bool his_xflip = false;
		if ( getFacing() == FACING_LEFT )
			my_xflip = true;
		if ( obj->getFacing() == FACING_LEFT )
			his_xflip = true;

		int mx, my;
		int ax, ay;

		ECollide * me = getCollide();
		ECollide * him = obj->getCollide();

		mx = this->getRX() - getWidth() / 2;
		my = this->getRY() - getHeight();
		ax = obj->getRX() - obj->getWidth() / 2;
		ay = obj->getRY() - obj->getHeight();

		return ( me->Collision( him, mx, my, ax, ay, my_xflip, false, his_xflip, false ) );

	}
	return false;
}

void Item::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	// cout << "tough actin tinactin: " << this << endl;
}

void Item::draw( Graphics::Bitmap * work, int rel_x, int rel_y ){
	// cout << "draw item at " << getRX() - rel_x << " " << getRY() << endl;
	picture.draw( getRX() - rel_x - picture.getWidth() / 2, getRY() - picture.getHeight(), *work );

	if ( Global::getDebug() > 5 ){
		work->circleFill( getRX() - rel_x, (int) getZ(), 5, Graphics::Bitmap::makeColor( 255, 255, 255 ) );
	}
}
	
Network::Message Item::getCreateMessage(){
	Network::Message message;

	message.id = 0;
	message << World::CREATE_ITEM;
	message << getId();
	message << (int) getX();
	message << (int) getZ();
	this->stimulation->createMessage( message );

        Filesystem::RelativePath mypath = Filesystem::cleanse(path);
        Global::debug(2) << "Create item id " << getId() << " path " << mypath.path() << endl;
        message << mypath.path();
        /*
	mypath.erase( 0, Util::getDataPath().length() );
	message << mypath;
        */

	return message;
}

bool Item::isCollidable( Object * obj ){
	return false;
}

int Item::getWidth() const {
	return picture.getWidth();
}

int Item::getHeight() const {
	return picture.getHeight();
}
	
Item::~Item(){
	delete collide;
	delete stimulation;
}

}
