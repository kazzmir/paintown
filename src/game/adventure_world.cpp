#include "util/funcs.h"
#include "util/bitmap.h"
#include "script/script.h"
#include "object/object.h"
#include "object/object_attack.h"
#include "object/character.h"
#include "util/load_exception.h"
#include "util/font.h"
#include "util/file-system.h"
#include "util/gradient.h"
#include "factory/font_render.h"
#include "globals.h"
#include "object/effect.h"
#include "object/enemy.h"
#include "level/scene.h"
#include "world.h"
#include "adventure_world.h"
#include "network/network.h"
#include "level/cacher.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

AdventureWorld::AdventureWorld():
World(),
draw_minimaps( true ),
mini_map( NULL ),
takeAScreenshot(false),
is_paused(false),
slowmotion(0),
descriptionTime(0),
descriptionGradient(0){
	scene = NULL;
	bang = NULL;
}

static const int DESCRIPTION_TIME = 700;

AdventureWorld::AdventureWorld( const vector< Object * > & players, const string & path, Level::Cacher * cacher, int _screen_size ) throw( LoadException ):
World(),
path( path ),
draw_minimaps( true ),
mini_map( NULL ),
takeAScreenshot(false),
is_paused(false),
slowmotion(0),
cacher(cacher),
descriptionTime(DESCRIPTION_TIME),
descriptionGradient(new Effects::Gradient(100, Bitmap::makeColor(255, 255, 255), Bitmap::makeColor(128, 128, 128))){
	scene = NULL;
	bang = NULL;
	screen_size = _screen_size;

	for ( vector<Object *>::const_iterator it = players.begin(); it != players.end(); it++ ){
		PlayerTracker t;
		t.min_x = 0;
		t.player = *it;
		// this->players.push_back( (PlayerTracker) { .min_x = 0, .player = *it } );
		this->players.push_back( t );
	}

	loadLevel(path);

        /* 1.3333 is the aspect ratio of screen_width/screen_height when the res is any standard of
         * 640,480 800,600, 1024,768
         * but it should use the actual values instead of guessing since the screen size
         * could theoretically change
         */
	mini_map = new Bitmap(screen_size, (int)((double) screen_size / 1.3333));

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

	if ( bang ){
		delete bang;
	}

	if ( scene ){
		delete scene;
	}
	
	if ( mini_map ){
		delete mini_map;
	}

        delete cacher;

        delete descriptionGradient;

	deleteObjects( &objects );
            
        for (deque<Bitmap*>::iterator it = screenshots.begin(); it != screenshots.end(); it++){
            delete *it;
        }
	
        /* this prevents the object from being destroyed in the Object destructor */
        for ( vector<PlayerTracker>::iterator it = players.begin(); it != players.end(); it++ ){
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

void AdventureWorld::reloadLevel() throw( LoadException ){
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		it->min_x = 0;
	}
	loadLevel( path );
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
	
void AdventureWorld::loadLevel( const string & path ) throw (LoadException){
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

	Scene * s = new Scene(path.c_str(), *cacher);
	if ( scene != NULL ){
		delete scene;
	}
	scene = s;
	
	string bang_path(Filesystem::find("/misc/flash/flash.txt"));
	Object * effect = new Effect( bang_path.c_str() );
	if ( bang != NULL ){
		delete bang;
	}
	bang = effect;
		
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		Object * const player = it->player;	
		player->setX( 100 + Util::rnd( 75 ) );
		player->setZ( getMinimumZ() + Util::rnd( (getMaximumZ() - getMinimumZ()) / 2 ) );
		player->setY( 0 );
	}

	deleteObjects( &objects );
	
	objects.clear();
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		objects.push_back( it->player );
	}

}

void AdventureWorld::deleteObjects( vector< Object * > * objects ){
    for ( vector< Object * >::iterator it = objects->begin(); it != objects->end(); it++ ){
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
            Object * const player = it->player;
            if ( f != -1 && player->getX() >= f ){
                return player->getX() >= f;
            }
        }
        return false;
    } else {
        return true;
    }
}
	
bool AdventureWorld::isPlayer( Object * o ) const {
    for ( vector< PlayerTracker >::const_iterator it = players.begin(); it != players.end(); it++ ){
        if ( it->player == o ){
            return true;
        }
    }
    return false;
}

void AdventureWorld::addMessage( Network::Message m, Network::Socket from, Network::Socket to){
	/* nothing */
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
        
const Block * AdventureWorld::currentBlock() const {
    if (scene == NULL){
        Global::debug(-1) << "*BUG* Scene is null" << endl;
        exit(1);
    }
    return scene->currentBlock();
}
        
int AdventureWorld::levelLength() const {
    if (scene == NULL){
        Global::debug(-1) << "*BUG* Scene is null" << endl;
        exit(1);
    }
    return scene->totalLength();
}
        
void AdventureWorld::addEnemy(Enemy * obj){
    if (scene){
        scene->addEnemy(obj);
    } else {
        /* scene should exist.. but just be safe */
        delete obj;
    }
}
        
void AdventureWorld::enterSlowMotion(const int amount){
    slowmotion = amount;
}

Object * AdventureWorld::findObject(int id){
    for (vector<Object*>::iterator it = objects.begin(); it != objects.end(); it++){
        Object * object = *it;
        if (object->getObjectId() == id){
            return object;
        }
    }
    return NULL;
}
        
void AdventureWorld::dyingObject(const Player & obj){
    /* 60 comes from character.cpp:act(), but I should probably use a function
     * that returns the number instead
     */
    /* don't do this for now */
    // enterSlowMotion(60);
}

void AdventureWorld::doLogic(){

    if (slowmotion > 0){
        slowmotion -= 1;
    }

    vector< Object * > added_effects;
    for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
        Object * good = *it;
        good->act( &objects, this, &added_effects );

        if ( good->getZ() < getMinimumZ() ){
            good->setZ( getMinimumZ() );
        }
        if ( good->getZ() > getMaximumZ() ){
            good->setZ( getMaximumZ() );
        }
        /* Check for collisions */
        if ( good->isAttacking() ){
            // ObjectAttack * o_good = dynamic_cast<ObjectAttack*>( good );
            ObjectAttack * o_good = (ObjectAttack *)good;
            for ( vector<Object*>::iterator fight = objects.begin(); fight != objects.end(); fight++){
                if ( fight != it && (*fight)->isCollidable( good ) && good->isCollidable( *fight ) ){
                    // cout << "Zdistance: " << good->ZDistance( *fight ) << " = " << (good->ZDistance( *fight ) < o_good->minZDistance()) << endl;
                    // cout << "Collision: " << (*fight)->collision( o_good ) << endl;
                    if ( good->ZDistance( *fight ) <= o_good->minZDistance() && (*fight)->collision( o_good ) ){
                        Global::debug(2) << o_good << " is attacking " << *fight << " with " << o_good->getAttackName() << endl;

                        // cout << "There was a collision" << endl;
                        // cout<<"Attacked " << *fight << " with animation "<< good->getAttackName() << " ticket " << o_good->getTicket() << endl;

                        // if ( good->isAttacking() ){
                        double x = 0, y = 0;
                        // o_good->getAttackCoords(x,y);
                        x = (*fight)->getX();
                        y = (*fight)->getRY() - (*fight)->getHeight() + (*fight)->getHeight() / 3;

                        if ( bang != NULL ){
                            Object * addx = bang->copy();
                            addx->setX( x );
                            addx->setY( 0 );
                            addx->setZ( y+addx->getHeight()/2 );
                            addx->setHealth( 1 );
                            added_effects.push_back( addx );
                            addMessage( createBangMessage( (int) x, 0, (int) y + addx->getHeight() / 2 ) );
                        }

                        o_good->attacked(this, *fight, added_effects );
                        (*fight)->collided(this, o_good, added_effects );
                        /* move this to the object now */
                        addMessage( (*fight)->collidedMessage() );
                        (*fight)->takeDamage(*this, o_good, o_good->getDamage() );


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
    }

    for ( vector<Object *>::iterator it = objects.begin(); it != objects.end(); ){
        if ( (*it)->getHealth() <= 0 ){
            (*it)->died( added_effects );
            if ( ! isPlayer( *it ) ){
                delete *it;
            }
            it = objects.erase(it);
        } else {
            ++it;
        }
    }

    /* special case for getting items */
    for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
        Character * const cplayer = (Character *) it->player; 
        if ( cplayer->getStatus() == Status_Get ){
            for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
                Object * const o = *it;
                if ( o->isGettable() && o->ZDistance( cplayer ) < 10 && o->collision( cplayer ) ){
                    o->touch( cplayer );
                    addMessage( deleteMessage( o->getId() ) );
                    /* hack */
                    addMessage( cplayer->healthMessage() );
                    delete o;
                    it = objects.erase( it );
                } else ++it;
            }
        }
    }

    objects.insert( objects.end(), added_effects.begin(), added_effects.end() );

    /* script engine tick. Is this the right place for it? */
    getEngine()->tick();
}

void AdventureWorld::killAllHumans( Object * player ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		if ( o != player ){
			// o->takeDamage( o->getMaxHealth() * 2 );
			o->takeDamage(*this, NULL, 999999 );
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

    if (!is_paused){
		
	doLogic();
	
	double lowest = 9999999;
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		Object * player = it->player;
		if ( player->getHealth() > 0 ){
			double mx = player->getX() - screen_size / 2;
			if ( it->min_x < mx ){
				it->min_x++;
			}

			if ( it->min_x > mx ){
				it->min_x = mx;
			}

			if ( it->min_x < 0 ){
				it->min_x = 0;
			}
		
			if ( it->min_x + screen_size >= scene->getLimit() ){
				it->min_x = scene->getLimit() - screen_size;
			}

			if ( it->min_x < lowest ){
				lowest = it->min_x;
			}
			
			if ( player->getX() < 0 ){
				player->setX( 0 );
			}
			/*
			if ( player->getX() < it->min_x ){
				player->setX( it->min_x );
			}
			*/

			if ( player->getX() > scene->getLimit() ){
				player->setX( scene->getLimit() );
			}
			if ( player->getZ() < getMinimumZ() ){
				player->setZ( getMinimumZ() );
			}
			if ( player->getZ() > getMaximumZ() ){
				player->setZ( getMaximumZ() );
			}
		}
	}

	doScene( (int) lowest, (int)(lowest + screen_size) );
	// scene->act( min_x, min_x + screen_size, &objects );
    }
}
	
void AdventureWorld::doScene( int min_x, int max_x ){
	scene->act( min_x, max_x, &objects );
}

void AdventureWorld::addObject( Object * o ){
	objects.push_back( o );
}

void AdventureWorld::drawWorld( const PlayerTracker & tracker, Bitmap * where, const map< int, vector< Object * > > & object_z ){
	int min_x = 0;

	min_x = (int) tracker.min_x;

	int max_x = (int)(tracker.player->getX() + screen_size / 2 > scene->getLimit() ? scene->getLimit() : tracker.player->getX() + screen_size / 2);
	min_x = (int)(max_x - screen_size);
	if ( min_x < 0 ){
		min_x = 0;
	}

	if ( min_x > tracker.min_x ){
		min_x = (int) tracker.min_x;
	}

	scene->drawBack( min_x, where );

	for ( map<int,vector<Object *> >::const_iterator it = object_z.begin(); it != object_z.end(); it++ ){
		const vector<Object *> & xx = (*it).second;
		for ( vector<Object *>::const_iterator mm = xx.begin(); mm != xx.end(); mm++ ){

			(*mm)->draw( where, min_x, 0 );
		}
	}

	scene->drawFront( min_x, where );

        /* need a special case to draw object stuff in front.
         * this is things like icon/name/health, not objects that are part of
         * the scene, and therefore the atmosphere doesn't apply to them.
         */
        for ( map<int,vector<Object *> >::const_iterator it = object_z.begin(); it != object_z.end(); it++ ){
		const vector<Object *> & xx = (*it).second;
		for ( vector<Object *>::const_iterator mm = xx.begin(); mm != xx.end(); mm++ ){
			(*mm)->drawFront( where, min_x );
		}
	}
}

void AdventureWorld::drawMiniMaps( bool b ){
	draw_minimaps = b;
}

bool AdventureWorld::shouldDrawMiniMaps(){
	return draw_minimaps;
}
        
const deque<Bitmap*> & AdventureWorld::getScreenshots(){
    return screenshots;
}

void AdventureWorld::doTakeScreenshot(Bitmap * work){
    takeAScreenshot = false;
    Global::debug(2) << "Take a screenshot" << endl;
    screenshots.push_back(new Bitmap(*work, true));

    /* don't store more than 4 screenshots, so if we go above this number
     * start throwing random ones out
     */
    const int change = 1000;
    int position = Util::rnd(change);
    while (screenshots.size() > 4){
        int index = position % screenshots.size();
        position += change + Util::rnd(change);

        deque<Bitmap*>::iterator kill;
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

void AdventureWorld::draw( Bitmap * work ){

    map< int, vector<Object*> > object_z;

    for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
        Object * n = *it;
        object_z[ n->getRZ() ].push_back( n );
    }

    Global::debug( 4 ) << "World draw" << endl;

    if (descriptionTime > 0 && scene->getDescription() != ""){
        const Font & font = Font::getFont(Filesystem::find(Global::DEFAULT_FONT), 30, 30);
        FontRender * render = FontRender::getInstance();
        string description = scene->getDescription();
        int trans = (DESCRIPTION_TIME - descriptionTime) / 2;
        if (trans >= 255){
            render->addMessage(font, work->getWidth() - font.textLength(description.c_str()) / 2, work->getHeight() / 2, descriptionGradient->current(), -1, description);
        } else {
            render->addMessage(font, work->getWidth() - font.textLength(description.c_str()) / 2, work->getHeight() / 2, descriptionGradient->current(), -1, trans, description);
        }
    }

    // min_x = (int)min_x_virtual;
    Bitmap mini( screen_size / 5, (int)( screen_size / 5.0 / ((double)work->getWidth() / (double) work->getHeight()) ) );
    int mini_position_x = work->getWidth() - mini.getWidth() - 1;
    int mini_position_y = work->getHeight() - mini.getHeight() - 1;
    for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
        Bitmap * on = mini_map;
        if ( it == players.begin() ){
            on = work;
        }

        drawWorld( *it, on, object_z );
        if ( on != work ){
            on->Stretch( mini );
            Bitmap::transBlender( 0, 0, 0, 128 );
            mini.border( 0, 1, Bitmap::makeColor( 255, 255, 255 ) );
            mini.drawTrans( mini_position_x, mini_position_y, *work );
            mini_position_x -= mini.getWidth() - 2;
            if ( mini_position_x <= 0 ){
                mini_position_y -= mini.getHeight() - 2;
                mini_position_x = work->getWidth() - mini.getWidth() - 1;
            }
        } else if ( ! shouldDrawMiniMaps() ){
            break;
        }
    }

    if (shouldTakeScreenshot() && (screenshots.empty() || Util::rnd(15) == 0)){
        doTakeScreenshot(work);
    } else {
        takeAScreenshot = false;
    }

    if (is_paused){
        const Font & font = Font::getFont(Filesystem::find(Global::DEFAULT_FONT), 15, 15);
        work->transBlender( 0, 0, 0, 128 );
        work->drawingMode( Bitmap::MODE_TRANS );
        work->rectangleFill( 0, 0, work->getWidth(), work->getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
        work->drawingMode( Bitmap::MODE_SOLID );
        font.printf( work->getWidth() / 2 - font.textLength("Paused") / 2, work->getHeight() / 2, Bitmap::makeColor( 255, 255, 255 ), *work, "Paused", 0 );
    }

    /*
       int min_x = 0;
       if ( players.size() > 0 ){
       min_x = (int) players[ 0 ].min_x;

       int max_x = (int)(players[ 0 ].player->getX() + screen_size / 2 > scene->getLimit() ? scene->getLimit() : players[ 0 ].player->getX() + screen_size / 2);
       min_x = (int)(max_x - screen_size);
       if ( min_x < 0 ){
       min_x = 0;
       }

       if ( min_x > players[ 0 ].min_x ){
       min_x = (int) players[ 0 ].min_x;
       }
       }

       scene->drawBack( min_x, work );
       for ( map<int,vector<Object *> >::iterator it = object_z.begin(); it != object_z.end(); it++ ){
       vector<Object *> & xx = (*it).second;
       for ( vector<Object *>::iterator mm = xx.begin(); mm != xx.end(); mm++ ){

       (*mm)->draw( work, min_x );
       }
       }
       scene->drawFront( min_x, work );
       */
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
		return Util::rnd( 9 ) - 4;
	return 0;
}

int AdventureWorld::getY(){
	if (getQuake() > 0)
		return Util::rnd( 9 ) - 4;
	return 0;
}
