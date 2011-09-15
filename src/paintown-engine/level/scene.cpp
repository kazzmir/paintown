#include <iostream>
#include <string>
#include <vector>

#include "util/bitmap.h"
#include "block.h"
#include "../object/heart.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "../object/object.h"
#include "scene.h"
#include "../object/enemy.h"
#include "globals.h"
#include "util/debug.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/file-system.h"
#include "util/music.h"
#include "../environment/atmosphere.h"
#include "../script/script.h"
#include "../trigger/trigger.h"
#include "cacher.h"
#include "exceptions/shutdown_exception.h"

using namespace std;

Panel::Panel( Graphics::Bitmap * _pic, Graphics::Bitmap * _neon, Graphics::Bitmap * _my_screen ){
    pic = _pic;
    neon = _neon;
    screen_overlay = _my_screen;
}

Panel::~Panel(){
    if ( pic ) delete pic;
    if ( neon ) delete neon;
    if ( screen_overlay ) delete screen_overlay;
}

Scene::Scene(const char * filename, const Level::Cacher & cacher):
background(NULL),
block_length(0),
minimum_z(0),
maximum_z(0),
current_block(NULL),
blockNumber(1),
backgroundParallax(5),
foregroundParallax(1.2),
frontBuffer(NULL),
hasMusic(false){

    TokenReader tr(filename);

    // Token * current = tr.readToken();
    Token * current;

    Script::newEngine("none", Filesystem::RelativePath("none"));

    /* the first panel index */
    // char panel_num = 'a';

    try {
        current = tr.readToken();

        if ( *current != "level" )
            throw LoadException(__FILE__, __LINE__, "Not a level");

        TokenView view = current->view();
        while (view.hasMore()){
            /* stop loading if a shutdown occurs */
            if (Global::shutdown()){
                throw ShutdownException();
            }

            const Token * tok;
            view >> tok;
            if (*tok == "music"){
                hasMusic = true;
                try{
                    TokenView view = tok->view();
                    while (true){
                        string name;
                        view >> name;
                        music.push_back(name);
                    }
                } catch (const TokenException & fail){
                }
            } else if ( *tok == "background" ){
                string n;
                tok->view() >> n;
                background = new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath(n)).path());
            } else if ( *tok == "background-parallax" ){
                double d;
                tok->view() >> d;
                setBackgroundParallax( d );
            } else if ( *tok == "foreground-parallax" ){
                double d;
                tok->view() >> d;
                setForegroundParallax( d );
            } else if (*tok == "description"){
                tok->view() >> this->description;
            } else if ( *tok == "script" ){
                string kind;
                string scriptPath;
                tok->view() >> kind >> scriptPath;
                Script::newEngine(kind, Filesystem::RelativePath(scriptPath));
            } else if ( *tok == "atmosphere" ){
                TokenView atmosphereView = tok->view();
                while (atmosphereView.hasMore()){
                    string s;
                    atmosphereView >> s;
                    Atmosphere * atmosphere = Atmosphere::createAtmosphere(s);
                    if (atmosphere != NULL){
                        atmospheres.push_back(atmosphere);
                    }
                }
            } else if ( *tok == "z" ){
                TokenView zToken = tok->view();
                while (zToken.hasMore()){
                    const Token * next;
                    zToken >> next;
                    if ( *next == "minimum" ){
                        int m;
                        next->view() >> m;
                        setMinimumZ( m );
                    } else if ( *next == "maximum" ){
                        int m;
                        next->view() >> m;
                        setMaximumZ( m );
                    }
                }
            } else if ( *tok == "panel" ){
                int num;
                string normal, neon, s_screen;
                tok->view() >> num >> normal >> neon >> s_screen;

                Graphics::Bitmap * x_normal = NULL;
                Graphics::Bitmap * x_neon = NULL;
                Graphics::Bitmap * x_screen = NULL;
                if ( normal != "none" ){
                    x_normal = new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath(normal)).path());
                }
                x_neon = new Graphics::Bitmap();
                x_screen = new Graphics::Bitmap();
                /*
                   if ( neon != "none" ){
                   x_neon = new Bitmap( neon );
                   }
                   if ( s_screen != "none" ){
                   x_screen = new Bitmap( s_screen );
                   }
                   */
                Panel * p = new Panel( x_normal, x_neon, x_screen );
                panels[ num ] = p;
                // panel_num++;
            } else if (*tok == "trigger"){
                Trigger * trigger = Trigger::parse(tok);
                triggers.push_back(trigger);
            } else if ( *tok == "block" ){
                Block * b = new Block(tok, cacher);
                level_blocks.push_back(b);
            } else if ( *tok == "frontpanel" ){
                string file;
                tok->view() >> file;
                Graphics::Bitmap * front = new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath(file)).path());
                front_panels.push_back( front );
            } else if ( *tok == "order" ){
                // *tok >> order;
                TokenView orderView = tok->view();
                while (orderView.hasMore()){
                    int x;
                    orderView >> x;
                    order.push_back( x );
                }
            } else {
                Global::debug( 0 ) <<"Unhandled scene attribute: "<<endl;
                tok->print(" ");
            }
        }

    } catch ( const TokenException & ex ){
        // delete current;
        throw LoadException(__FILE__, __LINE__, ex, "Level parse error");
    } catch ( const LoadException & ex ){
        // delete current;
        throw ex;
    }

    if ( level_blocks.empty() )
        throw LoadException(__FILE__, __LINE__, "No level blocks defined");

    current_block = level_blocks.front();
    level_blocks.pop_front();

    arrow = new Graphics::Bitmap(Storage::instance().find(Filesystem::RelativePath("sprites/arrow.png")).path());
    arrow_blink = 0;

    // delete current;

    Global::debug( 1 ) <<"Loaded level "<< filename << endl;

    calculateLength();

    int blength = 0;
    for ( deque< Block * >::iterator it = level_blocks.begin(); it != level_blocks.end(); it++ ){
        blength += (*it)->getLength();
    }
    Global::debug( 1 ) <<"Scene length = "<<scene_length<<". Length used = "<<blength<<endl;
}
        
int Scene::totalLength() const {
    int blength = 0;
    for ( deque< Block * >::const_iterator it = level_blocks.begin(); it != level_blocks.end(); it++ ){
        blength += (*it)->getLength();
    }
    return blength;
}

void Scene::calculateLength(){
    scene_length = 0;
    for ( unsigned int q = 0; q < order.size(); q++ ){
        Panel *& cur = panels[ order[q] ];
        if ( cur == NULL ){
            continue;
        }
        Graphics::Bitmap * normal = cur->pic;
        // normal->draw( fx-x, 0, *work );
        scene_length += normal->getWidth();
    }
}

/* returns the farthest point the player can go */
int Scene::getLimit(){
    return block_length + current_block->getLength();
}

int Scene::getFinished() const {
	int finished = current_block->getFinished();
	if ( finished == -1 ){
		return -1;
	}
	return block_length + current_block->getFinished();
}

void Scene::clearHearts(){
	for ( vector< Heart * >::iterator it = hearts.begin(); it != hearts.end(); ){
		Heart * h = *it;
		if ( !h->getAlive() ){
			// delete h;
			it = hearts.erase( it );
		} else
			it++;
	}
}

void Scene::advanceBlocks( int n ){
    while ( blockNumber < n ){
        if ( level_blocks.empty() ){
            break;
        }

        block_length += current_block->getLength();
        // delete current_block;
        /* store blocks so that they are deleted in the destructor.
         * this way the scripting engine can do stuff with it
         */
        old_level_blocks.push_back(current_block);
        current_block = level_blocks.front();
        level_blocks.pop_front();
        blockNumber += 1;
    }
}

bool Scene::canContinue( int x ){
	return (current_block->isContinuous() && x >= getLimit() - 320) ||
               (hearts.empty() && current_block->empty() && x >= getLimit() - 320);
}

/* put the enemy into a vector so that it can be added into the game objects
 * later on. This is because modifying the world object list during
 * the logic() method will corrupt the iterators.
 */
void Scene::addEnemy(Paintown::Enemy * const obj){
    hearts.push_back(obj->getHeart());
    added_objects.push_back(obj);
}

void Scene::doTriggers(){
    for (vector<Trigger*>::iterator it = triggers.begin(); it != triggers.end(); /**/){
        Trigger * trigger = *it;
        if (trigger->shouldExecute()){
            trigger->execute(this);
            delete trigger;
            it = triggers.erase(it);
        } else {
            it++;
        }
    }
}

void Scene::act(int min_x, int max_x, vector<Paintown::Object *> * objects){
    clearHearts();

    if (canContinue(min_x)){
        advanceBlocks(blockNumber + 1);
        Global::debug(3) << "[Scene] Current block is " << blockNumber << ". Length is " << current_block->getLength() << " Minimum x is " << min_x << endl;	
    }

    doTriggers();

    if (objects != 0){
        // Global::debug(0) << "Creating new objects" << endl;
        vector<Heart *> new_hearts = current_block->createObjects(block_length, min_x, max_x, getMinimumZ(), getMaximumZ(), objects);
        hearts.insert(hearts.end(), new_hearts.begin(), new_hearts.end());
        objects->insert(objects->end(), added_objects.begin(), added_objects.end());
        added_objects.clear();
    }

    for (vector<Atmosphere*>::iterator it = atmospheres.begin(); it != atmospheres.end(); it++){
        Atmosphere * atmosphere = *it;
        atmosphere->act(*this, objects);
    }
}

/* draw the background */
void Scene::drawBack( int x, Graphics::Bitmap * work ){
    if (background){
        int y = 0;
        background->Blit( (int)(x/getBackgroundParallax()) % background->getWidth() - background->getWidth(), 0, 0, y, *work );
        background->Blit( (int)(x/getBackgroundParallax()) % background->getWidth(), 0, 0, y, *work );
    }

    int fx = 0;
    for ( unsigned int q = 0; q < order.size(); q++ ){
        Panel *& cur = panels[ order[q] ];
        if (cur == NULL){
            continue;
        }
        Graphics::Bitmap * normal = cur->pic;
        normal->draw( fx-x, 0, *work );
        fx += normal->getWidth();
    }

    for (vector<Atmosphere*>::iterator it = atmospheres.begin(); it != atmospheres.end(); it++){
        Atmosphere * atmosphere = *it;
        atmosphere->drawBackground(work, x);
    }

    arrow_blink = (arrow_blink + 1) % 10;
}

/* draw the foreground */
void Scene::drawFront( int x, Graphics::Bitmap * work ){

    for (vector<Atmosphere*>::iterator it = atmospheres.begin(); it != atmospheres.end(); it++){
        Atmosphere * atmosphere = *it;
        atmosphere->drawForeground(work, (int)(x * getForegroundParallax()));
    }

    /* lazily initialize to ensure the buffer is the same size as the bitmap
     * to be drawn on.
     */
    if (frontBuffer == NULL){
        frontBuffer = new Graphics::Bitmap(work->getWidth(), work->getHeight());
    }

    frontBuffer->clearToMask();

    double fx = 0;
    if ( front_panels.size() > 0 ){
        while ( fx < scene_length * getForegroundParallax() ){
            for ( vector< Graphics::Bitmap * >::iterator it = front_panels.begin(); it != front_panels.end(); it++ ){
                Graphics::Bitmap * b = *it;
                b->draw( (int)(fx - x * getForegroundParallax()), 0, *frontBuffer);
                fx += b->getWidth();
            }
        }
    }

    /* just draw on the foreground */
    for (vector<Atmosphere*>::iterator it = atmospheres.begin(); it != atmospheres.end(); it++){
        Atmosphere * atmosphere = *it;
        atmosphere->drawFront(frontBuffer, x);
    }

    frontBuffer->draw(0, 0, *work);
    
    /* draw anything on the entire screen */
    for (vector<Atmosphere*>::iterator it = atmospheres.begin(); it != atmospheres.end(); it++){
        Atmosphere * atmosphere = *it;
        atmosphere->drawScreen(work, x);
    }

    if ( hearts.empty() && current_block->empty() && x < getLimit() - 320 ){
        if (arrow_blink > 5){
            arrow->draw( work->getWidth() - ( arrow->getWidth() + 10 ), 50, *work);
        }
    }

    /*
       for ( vector< Bitmap * >::iterator it = front_panels.begin(); it != front_panels.end(); it++ ){
       Bitmap * b = *it;
       b->draw( fx - x, 0, *work );
       fx += b->getWidth();
       }
       */
    /*
       for ( unsigned int q = 0; q < order.size(); q++ ){
       Panel *& cur = panels[ order[q] ];
       Bitmap * normal = cur->pic;
       normal->draw( fx-x, 100, *work );
       fx += normal->getWidth();
       }
       */
}

/*
void Scene::Draw( int x, Bitmap * work ){
	if ( background )
		background->Blit( x, 0, 0, 20, *work );

	int fx = 0;
	for ( unsigned int q = 0; q < order.size(); q++ ){
		Panel *& cur = panels[ order[q] ];
		Bitmap * normal = cur->pic;
		normal->draw( fx-x, 100, *work );
		fx += normal->getWidth();
	}
}
*/
    
void Scene::startMusic(){
    if (hasMusic){
        Music::pause();
        vector<Filesystem::AbsolutePath> songs;

        /* this lets you give music paths like foo*.mp3 or use a sub directory */
        try{
            Filesystem::AbsolutePath root = Storage::instance().find(Filesystem::RelativePath("music"));
            for (vector<string>::iterator it = music.begin(); it != music.end(); it++){
                vector<Filesystem::AbsolutePath> more = Storage::instance().getFiles(root, Filesystem::RelativePath(*it), false);
                songs.insert(songs.end(), more.begin(), more.end());
            }
        } catch (const Filesystem::NotFound & fail){
        }

        /*
        for (vector<Filesystem::AbsolutePath>::iterator it = songs.begin(); it != songs.end(); it++){
            Global::debug(0) << "Found " << it->path() << std::endl;
        }
        */

        Music::loadSong(songs);
    } else {
        /* choose a song randomly */
        Music::changeSong();
    }
}

Scene::~Scene(){
    for ( deque< Block * >::iterator it = level_blocks.begin(); it != level_blocks.end(); it++ ){
        delete *it;
    }

    if (frontBuffer){
        delete frontBuffer;
    }

    if ( background )
        delete background;

    if ( arrow )
        delete arrow;

    for ( vector< Graphics::Bitmap * >::iterator it = front_panels.begin(); it != front_panels.end(); it++ ){
        delete *it;
    }
    for ( map< int, Panel * >::iterator it = panels.begin(); it != panels.end(); it++ ){
        delete (*it).second;
    }

    for (vector<Atmosphere*>::iterator it = atmospheres.begin(); it != atmospheres.end(); it++){
        Atmosphere * atmosphere = *it;
        delete atmosphere;
    }

    for (vector<Trigger*>::iterator it = triggers.begin(); it != triggers.end(); it++){
        Trigger * trigger = *it;
        delete trigger;
    }

    /*
       for ( vector< Heart * >::iterator it = hearts.begin(); it != hearts.end(); it++ ){
       delete *it;
       }
       */
    Script::newEngine("none", Filesystem::RelativePath("none"));
}
