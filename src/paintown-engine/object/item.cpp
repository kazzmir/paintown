#include "util/graphics/bitmap.h"
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
#include <math.h>

using namespace std;

namespace Paintown{

Item::Item(const Filesystem::AbsolutePath & filename, const Util::ReferenceCount<Stimulation> & stimulation):
ObjectNonAttack(0, 0),
collide(0),
stimulation(stimulation){
    TokenReader tr;

    setMaxHealth(1);
    setHealth(1);

    try{
        Token * head;
        head = tr.readTokenFromFile(filename.path());

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
                picture.load(Storage::instance().find(Filesystem::RelativePath(file)).path());
            } else if (*next == "sound"){
                string path;
                next->view() >> path;
                sound = Sound(Storage::instance().find(Filesystem::RelativePath(path)).path());
            } else if (*next == "float"){
                floating.enabled = true;
                double value = 0;
                if (next->match("_/height", value)){
                    floating.height = value;
                }
                if (next->match("_/amplitude", value)){
                    floating.amplitude = value;
                }
                if (next->match("_/period", value)){
                    if (value < 1){
                        value = 1;
                    }
                    floating.period = value;
                }
            }
        }
    } catch (const TokenException & ex){
        // Global::debug(0) << "Could not read "<<filename.path()<< ": "<<ex.getReason()<<endl;
        throw LoadException(__FILE__, __LINE__, ex, "Could not open item file");
    }

    collide = new ECollide( picture );
    path = filename;
}

Item::Item(const Item & item):
ObjectNonAttack(item),
collide(0),
stimulation(item.stimulation),
floating(item.floating){
    this->picture = item.picture;
    collide = new ECollide(this->picture);
    setHealth(item.getHealth());
    sound = item.sound;
    path = item.getPath();
    setX(item.getX());
    setY(item.getY());
    setZ(item.getZ());
}
	
/*
Stimulation * Item::copyStimulation() const {
    return getStimulation()->copy();
}
*/
	
Object * Item::copy(){
    return new Item(*this);
}
	
bool Item::isGettable(){
    return true;
}
	
void Item::touch(Object * obj){
    if (getStimulation() != NULL){
        obj->stimulate(*getStimulation());
    }
    sound.play();
}
    
void Item::setStimulation(const Util::ReferenceCount<Stimulation> & stimulation){
    this->stimulation = stimulation;
}

vector<ECollide*> Item::getCollide() const {
    vector<ECollide*> out;
    out.push_back(collide);
    return out;
}
	
bool Item::collision(ObjectAttack * obj){
    if (getAlliance() == obj->getAlliance()){
        return false;
    }

    vector<ECollide*> myCollides = getCollide();
    vector<ECollide*> himCollides = obj->getCollide();
    for (vector<ECollide*>::iterator mine = myCollides.begin(); mine != myCollides.end(); mine++){
        for (vector<ECollide*>::iterator his = himCollides.begin(); his != himCollides.end(); his++){
            bool my_xflip = false;
            bool his_xflip = false;
            if (getFacing() == FACING_LEFT)
                my_xflip = true;
            if (obj->getFacing() == FACING_LEFT)
                his_xflip = true;

            int mx, my;
            int ax, ay;

            ECollide * me = *mine;
            ECollide * him = *his;

            mx = this->getRX() - getWidth() / 2;
            my = this->getRY() - getHeight();
            ax = obj->getRX() - obj->getWidth() / 2;
            ay = obj->getRY() - obj->getHeight();

            if (me->Collision( him, mx, my, ax, ay, my_xflip, false, his_xflip, false )){
                return true;
            }
        }
    }

    return false;
}

void Item::act(vector< Object * > * others, World * world, vector< Object * > * add){
    if (floating.enabled){
        floating.time += 1;
    }
}

void Item::drawShadow(Graphics::Bitmap * work, int cameraX, int cameraY, double scale){
    int x = (int)(getRX() - cameraX - picture.getWidth()/2);
    int y = (int)(getRZ() + getY() * scale);

    picture.drawShadow(*work, x, y, 128, Graphics::makeColor(0, 0, 0), scale, true);
}

void Item::draw(Graphics::Bitmap * work, int rel_x, int rel_y){
    drawShadow(work, rel_x, rel_y, -0.5);

    int finalX = getRX() - rel_x - picture.getWidth() / 2;
    int finalY = getRY() - picture.getHeight();

    if (floating.enabled){
        double angle = Util::radians(360.0 * (double) (floating.time % floating.period) / (double) floating.period);
        finalY -= floating.height + floating.amplitude * sin(angle);
    }

    picture.draw(finalX, finalY, *work);

    if (Global::getDebug() > 5){
        work->circleFill(getRX() - rel_x, (int) getZ(), 5, Graphics::makeColor( 255, 255, 255 ));
    }
}
	
Network::Message Item::getCreateMessage(){
    Network::Message message;

    message.id = 0;
    message << World::CREATE_ITEM;
    message << getId();
    message << (int) getX();
    message << (int) getZ();
    if (this->stimulation != NULL){
        this->stimulation->createMessage(message);
    }

    Filesystem::RelativePath mypath = Storage::instance().cleanse(path);
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
}

}
