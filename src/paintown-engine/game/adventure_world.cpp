#include "r-tech1/funcs.h"
#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/font.h"
#include "r-tech1/file-system.h"
#include "r-tech1/graphics/gradient.h"
#include "r-tech1/message-queue.h"
#include "r-tech1/network/network.h"
#include "r-tech1/thread.h"
#include "r-tech1/events.h"
#include "../script/script.h"
#include "../object/object.h"
#include "../object/object_attack.h"
#include "../object/character.h"
#include "../object/player-common.h"
#include "factory/font_render.h"
#include "../object/effect.h"
#include "../object/enemy.h"
#include "../object/player.h"
#include "../level/scene.h"
#include "../level/random-scene.h"
#include "world.h"
#include "adventure_world.h"
#include "../level/cacher.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

Camera::Camera(double x, double y):
x(x),
y(y),
moveX(x),
moveY(y),
speed(0),
ticksLeft(0){
}

Camera::~Camera(){
}

void Camera::moveTo(double x, double y, int ticks){
    moveX = x;
    moveY = y;
    ticksLeft = ticks;
    speed = ticks;
}

void Camera::move(){
    if (ticksLeft > 0 && speed > 0){
        x += (moveX - x) / speed;
        y += (moveY - y) / speed;
        ticksLeft -= 1;
    }
}

double Camera::getX() const {
    return x;
}

double Camera::getY() const {
    return y;
}

AdventureWorld::AdventureWorld():
World(),
draw_minimaps( true ),
takeAScreenshot(false),
is_paused(false),
slowmotion(0),
descriptionTime(0),
descriptionGradient(0),
gameTicks(0),
replayEnabled(false),
camera(0, 0){
	scene = NULL;
	bang = NULL;
}

static const int DESCRIPTION_TIME = 700;

AdventureWorld::AdventureWorld( const vector< Paintown::Object * > & players, const Filesystem::AbsolutePath & path, Level::Cacher * cacher, int _screen_size ):
World(),
path( path ),
draw_minimaps( true ),
takeAScreenshot(false),
is_paused(false),
slowmotion(0),
cacher(cacher),
descriptionTime(DESCRIPTION_TIME),
descriptionGradient(new Effects::Gradient(100, Graphics::makeColor(255, 255, 255), Graphics::makeColor(128, 128, 128))),
gameTicks(0),
replayEnabled(false),
camera(_screen_size / 2, 0){
	scene = NULL;
	bang = NULL;
	screen_size = _screen_size;

    /* we add players to the player tracker here because the world wants to set
     * some attributes (x/y coordinates).
     * then we load the world and afterwards set up the script object
     */
	for ( vector<Paintown::Object *>::const_iterator it = players.begin(); it != players.end(); it++ ){
		PlayerTracker t;
		t.min_x = 0;
		t.player = *it;
		// this->players.push_back( (PlayerTracker) { .min_x = 0, .player = *it } );
		this->players.push_back(t);
	}

	threadedLoadLevel(path);

    for ( vector<PlayerTracker>::iterator it = this->players.begin(); it != this->players.end(); it++ ){
        PlayerTracker & tracker = *it;
        void * handle = Script::Engine::getEngine()->createPlayer(tracker.player);
        tracker.player->setScriptObject(handle);
        tracker.script = handle;
    }

	/*
	if ( player != NULL ){
		objects.push_back( player );
	}
	*/
}

AdventureWorld::~AdventureWorld(){
    if (bang){
        delete bang;
    }

    delete cacher;

    delete descriptionGradient;

    deleteObjects(&objects);

    for (deque<Graphics::Bitmap*>::iterator it = screenshots.begin(); it != screenshots.end(); it++){
        delete *it;
    }

    /* this prevents the object from being destroyed in the Object destructor */
    for (vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++){
        PlayerTracker & tracker = *it;
        void * handle = tracker.script;
        if (handle != NULL){
            Script::Engine::getEngine()->destroyObject(handle);
        }
        tracker.player->setScriptObject(NULL);
    }
}
        
Script::Engine * AdventureWorld::getEngine() const {
    return Script::getEngine();
    /*
    if (scene){
        return scene->getScript();
    }
    Global::debug(-1) << "Script is null somehow" << endl;
    exit(1);
    */
}

void AdventureWorld::begin(){
    scene->playIntro();
    scene->startMusic();
}

void AdventureWorld::end(){
    scene->playEnding();
}
    
void AdventureWorld::trackPlayer(Paintown::Player * player){
    PlayerTracker tracker;
    tracker.min_x = 0;
    tracker.player = player;
    this->players.push_back(tracker);
}

void AdventureWorld::reloadLevel(){
    for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
        it->min_x = 0;
    }
    loadLevel(path);
}
        
void AdventureWorld::changePause(){
    is_paused = ! is_paused;
}
        
bool AdventureWorld::isPaused(){
    return is_paused;
}

void AdventureWorld::pause(){
    is_paused = true;
}

void AdventureWorld::unpause(){
    is_paused = false;
}

struct LoadLevelInfo{
    LoadLevelInfo(AdventureWorld * who, const Filesystem::AbsolutePath & path):
        who(who), path(path){}

    AdventureWorld * who;
    const Filesystem::AbsolutePath & path;
};

void * AdventureWorld::do_load_level(void * arg){
    LoadLevelInfo * info = (LoadLevelInfo *) arg;
    AdventureWorld * world = info->who;
    world->loadLevel(info->path);
    delete info;
    return NULL;
}

void AdventureWorld::threadedLoadLevel(const Filesystem::AbsolutePath & path){
    /* this looks dangerous, passing a temporary to a thread but in theory
     * the temporary should live past the end of the thread life
     * so the memory will stick around.
     */
    /*
    LoadLevelInfo * info = new LoadLevelInfo(this, path);
    Util::WaitThread thread((Util::Thread::ThreadFunction) do_load_level, info);
    Util::EventManager manager;
    manager.waitForThread(thread);
    */

    loadLevel(path);
}

bool AdventureWorld::randomLevel() const {
    /* FIXME: let the user decide to make this true or not */
    return false;
}

static Network::Message removeMessage(int id){
    Network::Message message;

    message.id = 0;
    message << World::REMOVE;
    message << id;

    return message;
}

bool AdventureWorld::respawnPlayers(const std::vector<Paintown::Object*> & players){
    bool alive = false;
    for (vector<Paintown::Object *>::const_iterator it = players.begin(); it != players.end(); it++){
        Paintown::Character * player = (Paintown::Character *) *it;
        if (player->getHealth() <= 0){
            if (player->spawnTime() == 0){

                /* if the player has at least 1 life left then make them lose a life
                 * and respawn them. if they have infinite lives enabled then they
                 * might have 1 life but deathReset() won't actually decrement the
                 * number of lives.
                 * if the player is reduced to 0 lives then remove them from the
                 * world entirely.
                 */
                if (player->getLives() >= 1){
                    player->deathReset();
                    if (player->getLives() > 0){
                        if (player->isPlayer()){
                            alive = true;
                        }
                        addMessage(removeMessage(player->getId()));
                        addObject(player);
                        addMessage(player->getCreateMessage());
                        addMessage(player->movedMessage());
                        addMessage(player->animationMessage());
                    } else {
                        addMessage(removeMessage(player->getId()));
                    }
                }
            }
        } else {
            alive = true;
        }
    }
    return alive;
}

void AdventureWorld::loadLevel( const Filesystem::AbsolutePath & path ){
	/*
	if ( scene ){
		delete scene;
		scene = NULL;
	}
	if ( bang ){
		delete bang;
		bang = NULL;
	}
	*/

    MessageQueue::info("Loading scenery");
    Scene * s = NULL;
    if (randomLevel()){
        s = new RandomScene(path, *cacher);
    } else {
        s = new Scene(path, *cacher);
    }
    scene = Util::ReferenceCount<Scene>(s);

    Filesystem::AbsolutePath bang_path(Storage::instance().find(Filesystem::RelativePath("misc/flash/flash.txt")));
    Paintown::Object * effect = new Paintown::Effect(bang_path.path().c_str());
	if ( bang != NULL ){
		delete bang;
	}
	bang = effect;
		
	for ( vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++ ){
        Paintown::Object * const player = it->player;
        player->setX(100 + Util::rnd( 75 ));
        player->setZ(getMinimumZ() + Util::rnd( (getMaximumZ() - getMinimumZ()) / 2 ) );
        player->setY(0);
	}

	deleteObjects( &objects );
	
	objects.clear();
	for (vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++){
            objects.push_back( it->player );
	}

}

void AdventureWorld::deleteObjects( vector< Paintown::Object * > * objects ){
    for ( vector< Paintown::Object * >::iterator it = objects->begin(); it != objects->end(); it++ ){
        if ( ! isPlayer( *it ) ){
            delete *it;
        }
    }
}

/* true if the player has crossed the finish line */
bool AdventureWorld::finished() const {
    int f = scene->getFinished();
    if ( players.size() > 0 ){
        for ( vector< PlayerTracker >::const_iterator it = players.begin(); it != players.end(); it++ ){
            Paintown::Object * const player = it->player;
            if ( f != -1 && player->getX() >= f ){
                return player->getX() >= f;
            }
        }
        return false;
    } else {
        return true;
    }
}
	
bool AdventureWorld::isPlayer( Paintown::Object * o ) const {
    for (vector< PlayerTracker >::const_iterator it = players.begin(); it != players.end(); it++){
        if (it->player == o){
            return true;
        }
    }
    return false;
}

void AdventureWorld::addMessage(Network::Message message, Network::Socket from, Network::Socket to){
    if (replayEnabled){
        replay.push_back(ReplayEvent(gameTicks, message));
    }
}
	
Network::Message AdventureWorld::createBangMessage( int x, int y, int z ){
	Network::Message message;

	message.id = 0;
	message << CREATE_BANG;
	message << x << y << z;

	return message;
}
	
Network::Message AdventureWorld::deleteMessage( unsigned int id ){
	Network::Message message;
	message.id = 0;
	message << DELETE_OBJ;
	message << id;

	return message;
}
        
const Util::ReferenceCount<Block> AdventureWorld::currentBlock() const {
    if (scene == NULL){
        Global::debug(-1) << "*BUG* Scene is null" << endl;
        throw std::exception();
    }
    return scene->currentBlock();
}
        
int AdventureWorld::levelLength() const {
    if (scene == NULL){
        Global::debug(-1) << "*BUG* Scene is null" << endl;
        throw std::exception();
    }
    return scene->totalLength();
}
        
/* called from the scripting engine */
void AdventureWorld::addEnemy(Paintown::Enemy * obj){
    if (scene != NULL){
        obj->created(*scene);
        scene->addObject(obj);
        // scene->addEnemy(obj);
    } else {
        /* scene should exist.. but just be safe */
        delete obj;
    }
}
        
void AdventureWorld::enterSlowMotion(const int amount){
    slowmotion = amount;
}

Paintown::Object * AdventureWorld::findObject(int id){
    for (vector<Paintown::Object*>::iterator it = objects.begin(); it != objects.end(); it++){
        Paintown::Object * object = *it;
        if (object->getObjectId() == id){
            return object;
        }
    }
    return NULL;
}
        
void AdventureWorld::dyingObject(const Paintown::Player & obj){
    /* 60 comes from character.cpp:act(), but I should probably use a function
     * that returns the number instead
     */
    /* don't do this for now */
    // enterSlowMotion(60);
}

void AdventureWorld::handleCollisions(Paintown::ObjectAttack * o_good, vector<Paintown::Object*> & added_effects){
    for (vector<Paintown::Object*>::iterator fight = objects.begin(); fight != objects.end(); fight++){
        if (*fight != o_good && (*fight)->isCollidable(o_good) && o_good->isCollidable(*fight)){
            // cout << "Zdistance: " << good->ZDistance( *fight ) << " = " << (good->ZDistance( *fight ) < o_good->minZDistance()) << endl;
            // cout << "Collision: " << (*fight)->collision( o_good ) << endl;
            if ( o_good->ZDistance(*fight) <= o_good->minZDistance() && (*fight)->collision(o_good)){
                Global::debug(2) << o_good << " is attacking " << *fight << " with " << o_good->getAttackName() << endl;

                // cout << "There was a collision" << endl;
                // cout<<"Attacked " << *fight << " with animation "<< good->getAttackName() << " ticket " << o_good->getTicket() << endl;

                // if ( good->isAttacking() ){
                double x = 0, y = 0;
                // o_good->getAttackCoords(x,y);
                x = (*fight)->getX();
                y = (*fight)->getRY() - (*fight)->getHeight() + (*fight)->getHeight() / 3;

                if ( bang != NULL ){
                    Paintown::Object * addx = bang->copy();
                    addx->setX( x );
                    addx->setY( 0 );
                    addx->setZ( y+addx->getHeight()/2 );
                    addx->setHealth( 1 );
                    added_effects.push_back( addx );
                    addMessage(createBangMessage((int) x, 0, (int) y + addx->getHeight() / 2));
                }

                o_good->attacked(this, *fight, added_effects);
                (*fight)->collided(this, o_good, added_effects);
                /* move this to the object now */
                addMessage((*fight)->collidedMessage());
                (*fight)->takeDamage(*this, o_good, o_good->getDamage(), o_good->getForceX(), o_good->getForceY());


                /* TODO: enter slow motion for bosses
                   if ((*fight)->getHealth() <= 0){
                   enterSlowMotion(100);
                   }
                   */

                takeScreenshot();
                // }
            }
        }
    }
}

void AdventureWorld::updateObject(Paintown::Object * good, vector<Paintown::Object*> & added_effects){
    good->act(&objects, this, &added_effects);

    if (good->getZ() < getMinimumZ()){
        good->setZ(getMinimumZ());
    }

    if (good->getZ() > getMaximumZ()){
        good->setZ(getMaximumZ());
    }

    /* Check for collisions */
    if (good->isAttacking()){
        // ObjectAttack * o_good = dynamic_cast<ObjectAttack*>( good );
        Paintown::ObjectAttack * o_good = (Paintown::ObjectAttack *)good;
        handleCollisions(o_good, added_effects);
    }
}

void AdventureWorld::doLogic(){
    if (slowmotion > 0){
        slowmotion -= 1;
    }

    /* increase game time */
    gameTicks += 1;

    vector<Paintown::Object *> added_effects;
    for (vector<Paintown::Object *>::iterator it = objects.begin(); it != objects.end(); it++){
        Paintown::Object * good = *it;
        updateObject(good, added_effects);
    }

    eraseDeadObjects(added_effects);
    getItems();

    objects.insert( objects.end(), added_effects.begin(), added_effects.end() );

    /* script engine tick. Is this the right place for it? */
    getEngine()->tick();
}

void AdventureWorld::eraseDeadObjects(vector<Paintown::Object*> & added_effects){
    for ( vector<Paintown::Object *>::iterator it = objects.begin(); it != objects.end(); ){
        if ( (*it)->getHealth() <= 0 ){
            (*it)->died(scene, added_effects);
            if (! isPlayer(*it)){
                delete *it;
            }
            it = objects.erase(it);
        } else {
            ++it;
        }
    }
}

void AdventureWorld::getItems(){
    /* special case for getting items */
    for (vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++){
        Paintown::Character * const cplayer = (Paintown::Character *) it->player; 
        if (cplayer->getStatus() == Paintown::Status_Get){
            for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); ){
                Paintown::Object * const o = *it;
                if (o->isGettable() && o->ZDistance( cplayer ) < 10 && o->collision(cplayer)){
                    o->touch( cplayer );
                    addMessage( deleteMessage( o->getId() ) );
                    /* hack */
                    addMessage( cplayer->healthMessage() );
                    delete o;
                    it = objects.erase(it);
                } else ++it;
            }
        }
    }
}

void AdventureWorld::killAllHumans( Paintown::Object * player ){
    for ( vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
        Paintown::Object * o = *it;
        if ( o != player ){
            // o->takeDamage( o->getMaxHealth() * 2 );
            o->takeDamage(*this, NULL, 999999, 0, 0);
        }
    }
}

int AdventureWorld::getMinimumZ(){
    return scene->getMinimumZ();
}

int AdventureWorld::getMaximumZ(){
    return scene->getMaximumZ();
}

void AdventureWorld::act(){
    if (quake_time > 0){
        quake_time--;
    }

    if (descriptionTime > 0){
        descriptionTime -= 1;
        descriptionGradient->update();
    }

    camera.move();

    if (!is_paused){
        /* keep track of the positions that players can have if they are bound together */
        for (vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++){
            PlayerTracker & tracker = *it;
            tracker.minimumX = 0;
            tracker.maximumX = scene->getLimit();
            Paintown::PlayerCommon * maybe = (Paintown::PlayerCommon*) tracker.player;
            if (maybe->getHealth() > 0 && maybe->isPlayer()){
                Paintown::Player * player = (Paintown::Player*) maybe;
                vector<Paintown::Player*> binds = player->getBinds();

                double distance = player->getX();
                for (vector<Paintown::Player*>::iterator bind = binds.begin(); bind != binds.end(); bind++){
                    Paintown::Player* bound = *bind;
                    distance += bound->getX();
                }
                double average = (double) distance / (binds.size() + 1);
                tracker.minimumX = average - screen_size / 2;
                if (tracker.minimumX < 0){
                    tracker.minimumX = 0;
                }
                tracker.maximumX = average + screen_size / 2;
                if (tracker.maximumX >= scene->getLimit()){
                    tracker.maximumX = scene->getLimit();
                }

                double cameraLimit = tracker.maximumX - screen_size;
                if (cameraLimit < 0){
                    cameraLimit = 0;
                }
                camera.moveTo(cameraLimit, 0, 15);
            }
        }
		
	doLogic();
	
	double lowest = 9999999;
        for (vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++){
            PlayerTracker & tracker = *it;
            Paintown::Object * player = it->player;
            if (player->getHealth() > 0){
                double mx = player->getX() - screen_size / 2;
                if (it->min_x < mx){
                    it->min_x += 1;
                }

                if (it->min_x > mx){
                    // it->min_x = mx;
                    it->min_x -= 1;
                }

                if (it->min_x < tracker.minimumX){
                    it->min_x = tracker.minimumX;
                }

                /*
                if (it->min_x + screen_size >= tracker.maximumX){
                    it->min_x = tracker.maximumX - screen_size;
                }
                */
                if (it->min_x + screen_size >= scene->getLimit()){
                    it->min_x = scene->getLimit() - screen_size;
                }

                if (it->min_x < lowest){
                    lowest = it->min_x;
                }

                if (player->getX() < tracker.minimumX){
                    player->setX(tracker.minimumX);
                }
                /*
                   if ( player->getX() < it->min_x ){
                   player->setX( it->min_x );
                   }
                   */

                if (player->getX() > tracker.maximumX){
                    player->setX(tracker.maximumX);
                }
                if (player->getZ() < getMinimumZ()){
                    player->setZ(getMinimumZ());
                }
                if (player->getZ() > getMaximumZ()){
                    player->setZ(getMaximumZ());
                }
            }
        }

	doScene((int) lowest, (int)(lowest + screen_size));
    }
}
	
void AdventureWorld::doScene( int min_x, int max_x ){
    scene->act(min_x, max_x, &objects);
}

void AdventureWorld::addObject( Paintown::Object * o ){
    objects.push_back(o);
}

void AdventureWorld::drawWorld(const PlayerTracker & tracker, const Graphics::Bitmap & where, const map< int, vector< Paintown::Object * > > & object_z, double cameraX){

    scene->drawBack((int) cameraX, where);

    for (map<int, vector<Paintown::Object *> >::const_iterator it = object_z.begin(); it != object_z.end(); it++ ){
        const vector<Paintown::Object *> & xx = (*it).second;
        for ( vector<Paintown::Object *>::const_iterator mm = xx.begin(); mm != xx.end(); mm++ ){

            (*mm)->draw(where, (int) cameraX, 0);
        }
    }

    scene->drawFront((int) cameraX, where);

    /* need a special case to draw object stuff in front.
     * this is things like icon/name/health, not objects that are part of
     * the scene, and therefore the atmosphere doesn't apply to them.
     */
    for (map<int,vector<Paintown::Object *> >::const_iterator it = object_z.begin(); it != object_z.end(); it++){
        const vector<Paintown::Object *> & xx = (*it).second;
        for ( vector<Paintown::Object *>::const_iterator mm = xx.begin(); mm != xx.end(); mm++ ){
            (*mm)->drawFront(where, cameraX);
        }
    }
}

void AdventureWorld::drawMiniMaps( bool b ){
    draw_minimaps = b;
}

bool AdventureWorld::shouldDrawMiniMaps(){
	return draw_minimaps;
}
        
const deque<Graphics::Bitmap*> & AdventureWorld::getScreenshots(){
    return screenshots;
}

void AdventureWorld::doTakeScreenshot(const Graphics::Bitmap & work){
    takeAScreenshot = false;
    Global::debug(2) << "Take a screenshot" << endl;
    screenshots.push_back(new Graphics::Bitmap(work, true));

    /* don't store more than 4 screenshots, so if we go above this number
     * start throwing random ones out
     */
    const int change = 1000;
    int position = Util::rnd(change);
    while (screenshots.size() > 4){
        int index = position % screenshots.size();
        position += change + Util::rnd(change);

        deque<Graphics::Bitmap*>::iterator kill;
        int i;
        for (i = 0, kill = screenshots.begin(); i < index && kill != screenshots.end(); kill++, i++){
            // nothing
        }
        delete *kill;
        screenshots.erase(kill);
    }

    /*
    while (screenshots.size() > 4){
        Bitmap * front = screenshots.front();
        delete front;
        screenshots.pop_front();
    }
    */
}

void AdventureWorld::drawMiniMap(const Graphics::Bitmap & work, const PlayerTracker & player, const map<int, vector<Paintown::Object*> > & object_z, int x, int y, int width, int height){
    if (mini_map == NULL){
        /* 1.3333 is the aspect ratio of screen_width/screen_height when the res is any standard of
         * 640,480 800,600, 1024,768
         * but it should use the actual values instead of guessing since the screen size
         * could theoretically change
         */
        mini_map = new Graphics::Bitmap(screen_size, (int)((double) screen_size / 1.3333));
    }

    drawWorld(player, *mini_map, object_z, player.min_x);
    Graphics::Bitmap mini(width, height);
    mini_map->Stretch(mini);
    Graphics::Bitmap::transBlender(0, 0, 0, 160);
    mini.border(0, 1, Graphics::makeColor(255, 255, 255));
    mini.translucent().draw(x, y, work);
}

void AdventureWorld::showDescription(const Graphics::Bitmap & work, int time, const string & description){
    const Font & font = Font::getDefaultFont(30, 30);
    FontRender * render = FontRender::getInstance();
    int trans = (DESCRIPTION_TIME - time) / 2;
    if (trans >= 255){
        render->addMessage(font, work.getWidth() - font.textLength(description.c_str()) / 2, work.getHeight() / 2, descriptionGradient->current(), Graphics::MaskColor(), description);
    } else {
        render->addMessage(font, work.getWidth() - font.textLength(description.c_str()) / 2, work.getHeight() / 2, descriptionGradient->current(), Graphics::MaskColor(), trans, description);
    }
}

void AdventureWorld::draw(const Graphics::Bitmap & work){

    map<int, vector<Paintown::Object*> > object_z;

    for (vector< Paintown::Object * >::iterator it = objects.begin(); it != objects.end(); it++){
        Paintown::Object * n = *it;
        object_z[n->getRZ()].push_back(n);
    }

    if (descriptionTime > 0 && scene->getDescription() != ""){
        showDescription(work, descriptionTime, scene->getDescription());
    }

    // min_x = (int)min_x_virtual;
    int mini_width = screen_size / 5;
    int mini_height = (int)(screen_size / 5.0 / ((double)work.getWidth() / (double) work.getHeight()));

    int mini_position_x = work.getWidth() - mini_width - 1;
    int mini_position_y = work.getHeight() - mini_height - 1;

    for (vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++ ){
        /* this logic is a bit whacky. we assume the first element in the player tracker
         * list is a real player so we draw the world on the real buffer (on = work).
         * after that, the rest of the players might be AI controlled so they can
         * appear off-screen and should show up on the mini map. If mini-maps are 
         * turned off then there is no reason to iterate through the rest of the elements.
         * Some objects might be other players though, and in that case we don't need
         * to do anything because they don't want to redraw the world nor do they need
         * to draw a minimap.
         */
        if (it == players.begin()){
            drawWorld(*it, work, object_z, camera.getX());
            /* Don't need a minimap for the main player */
            continue;
        } else if (!shouldDrawMiniMaps()){
            break;
        } else if (((Paintown::Character*) it->player)->isPlayer()){
            continue;
        }

        /* draw the minimaps where the camera is always centered on the guy */
        // drawMiniMap(work, *it, object_z, mini_position_x, mini_position_y, mini_width, mini_height);

        mini_position_x -= mini_width - 2;
        if (mini_position_x <= 0){
            mini_position_y -= mini_width - 2;
            mini_position_x = work.getWidth() - mini_width - 1;
        }
    }

    if (shouldTakeScreenshot() && (screenshots.empty() || Util::rnd(15) == 0)){
        doTakeScreenshot(work);
    } else {
        takeAScreenshot = false;
    }

    if (is_paused){
        work.transBlender( 0, 0, 0, 128 );
        work.translucent().fill(Graphics::makeColor(0, 0, 0));
        const Font & font = Font::getDefaultFont(15, 15);
        font.printf(work.getWidth() / 2 - font.textLength("Paused") / 2, work.getHeight() / 2, Graphics::makeColor( 255, 255, 255 ), work, "Paused", 0 );
    }
}
        
double AdventureWorld::ticks(const double in) const{
    if (slowmotion > 0){
        return in / 2.0;
    } else {
        return in;
    }
}

int AdventureWorld::getX(){
    if (getQuake() > 0)
        return Util::rnd(-7, 7);
    return 0;
}

int AdventureWorld::getY(){
    if (getQuake() > 0)
        return Util::rnd(-7, 7);
    return 0;
}
