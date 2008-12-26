#include <iostream>
#include <string>
#include <vector>

#include "util/bitmap.h"
#include "block.h"
#include "object/heart.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "object/object.h"
#include "scene.h"
#include "globals.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "environment/atmosphere.h"
#include "../script.h"

using namespace std;

static const string dataPath( const string & str ){
	return Util::getDataPath() + str;
}

Panel::Panel( Bitmap * _pic, Bitmap * _neon, Bitmap * _my_screen ){
	pic = _pic;
	neon = _neon;
	screen_overlay = _my_screen;
}

Panel::~Panel(){
	if ( pic ) delete pic;
	if ( neon ) delete neon;
	if ( screen_overlay ) delete screen_overlay;
}

Scene::Scene( const char * filename ) throw( LoadException ):
background( NULL ),
block_length( 0 ),
minimum_z( 0 ),
maximum_z( 0 ),
current_block( NULL ),
blockNumber( 1 ),
backgroundParallax( 5 ),
foregroundParallax( 1.2 ),
atmosphere( NULL ),
script(NULL){

    script = new Script::NoEngine();

	TokenReader tr( filename );

	// Token * current = tr.readToken();
	Token * current;

	/* the first panel index */
	// char panel_num = 'a';

	try {
		current = tr.readToken();

		if ( *current != "level" )
			throw LoadException("Not a level");

		while ( current->hasTokens() ){

			Token * tok;
			*current >> tok;
			if ( *tok == "music" ){
				*tok >> music;
			} else if ( *tok == "background" ){
				string n;
				*tok >> n;
				background = new Bitmap( dataPath( n ) );
			} else if ( *tok == "background-parallax" ){
				double d;
				*tok >> d;
				setBackgroundParallax( d );
			} else if ( *tok == "foreground-parallax" ){
				double d;
				*tok >> d;
				setForegroundParallax( d );
                        } else if ( *tok == "script" ){
                            string kind;
                            string scriptPath;
                            *tok >> kind >> scriptPath;
                            delete getScript();
                            setScript(Script::getEngine(kind, scriptPath));
			} else if ( *tok == "atmosphere" ){
				string s;
				*tok >> s;
				atmosphere = Atmosphere::createAtmosphere( s );
			} else if ( *tok == "z" ){
				while ( tok->hasTokens() ){
					Token * next;
					*tok >> next;
					if ( *next == "minimum" ){
						int m;
						*next >> m;
						setMinimumZ( m );
					} else if ( *next == "maximum" ){
						int m;
						*next >> m;
						setMaximumZ( m );
					}
				}
			} else if ( *tok == "panel" ){
				int num;
				string normal, neon, s_screen;
				*tok >> num >> normal >> neon >> s_screen;

				Bitmap * x_normal = NULL;
				Bitmap * x_neon = NULL;
				Bitmap * x_screen = NULL;
				if ( normal != "none" ){
					x_normal = new Bitmap( dataPath( normal ) );
				}
				x_neon = new Bitmap();
				x_screen = new Bitmap();
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
			} else if ( *tok == "block" ){
				Block * b = new Block( tok );
				level_blocks.push_back( b );
			} else if ( *tok == "frontpanel" ){
				string file;
				*tok >> file;
				Bitmap * front = new Bitmap( dataPath( file ) );
				front_panels.push_back( front );
			} else if ( *tok == "order" ){
				// *tok >> order;
				while ( tok->hasTokens() ){
					int x;
					*tok >> x;
					order.push_back( x );
				}
			} else {
				Global::debug( 0 ) <<"Unhandled scene attribute: "<<endl;
				tok->print(" ");
			}
		}

	} catch ( const TokenException & ex ){
		// delete current;
		string m( "Level parse error: " );
		m += ex.getReason();
		throw LoadException( m );
	} catch ( const LoadException & ex ){
		// delete current;
		throw ex;
	}

	if ( level_blocks.empty() )
		throw LoadException("No level blocks defined");

	current_block = level_blocks.front();
	level_blocks.pop_front();

	arrow = new Bitmap( Util::getDataPath() + "/sprites/arrow.png" );
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
        
const int Scene::totalLength() const {
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
		Bitmap * normal = cur->pic;
		// normal->draw( fx-x, 0, *work );
		scene_length += normal->getWidth();
	}
}

/* returns the farthest point the player can go */
int Scene::getLimit(){
	return block_length + current_block->getLength();
}

const int Scene::getFinished() const {
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
	return (current_block->isContinuous() && x >= getLimit() - 320) || (hearts.empty() && current_block->empty() && x >= getLimit() - 320);
}


void Scene::act( int min_x, int max_x, vector< Object * > * objects ){
	clearHearts();

	if ( canContinue( min_x ) ){
		advanceBlocks( blockNumber + 1 );
		Global::debug( 3 ) << "[Scene] Current block is " << blockNumber << ". Length is " << current_block->getLength() << " Minimum x is " << min_x << endl;	
	}

	vector< Heart * > new_hearts = current_block->createObjects( block_length, min_x, max_x, getMinimumZ(), getMaximumZ(), objects );
	hearts.insert( hearts.end(), new_hearts.begin(), new_hearts.end() );

	if ( atmosphere ){
		atmosphere->act();
	}
}

/* draw the background */
void Scene::drawBack( int x, Bitmap * work ){
	if ( background ){
		int y = 0;
		background->Blit( (int)(x/getBackgroundParallax()) % background->getWidth() - background->getWidth(), 0, 0, y, *work );
		background->Blit( (int)(x/getBackgroundParallax()) % background->getWidth(), 0, 0, y, *work );
	}
		
	int fx = 0;
	for ( unsigned int q = 0; q < order.size(); q++ ){
		Panel *& cur = panels[ order[q] ];
		if ( cur == NULL ){
			continue;
		}
		Bitmap * normal = cur->pic;
		normal->draw( fx-x, 0, *work );
		
		/* This stuff is from beats of rage, but I probably
		 * wont use it anymore
		 */
		/*
		Bitmap * xscreen = cur->screen_overlay;
		if ( xscreen )
			xscreen->draw( fx-x, 0, *work );

		Bitmap * neon = cur->neon;
		if ( neon )
			neon->draw( fx-x, 0, *work );
		*/

		fx += normal->getWidth();
	}
}

/* draw the foreground */
void Scene::drawFront( int x, Bitmap * work ){
	double fx = 0;
	if ( front_panels.size() > 0 ){
		while ( fx < scene_length * getForegroundParallax() ){
			for ( vector< Bitmap * >::iterator it = front_panels.begin(); it != front_panels.end(); it++ ){
				Bitmap * b = *it;
				b->draw( (int)(fx - x * getForegroundParallax()), 0, *work );
				fx += b->getWidth();
			}
		}
	}

	if ( hearts.empty() && current_block->empty() && x < getLimit() - 320 ){
		if ( arrow_blink++ > 5 ){
			arrow->draw( work->getWidth() - ( arrow->getWidth() + 10 ), 50, *work );
		}
		if ( arrow_blink > 10 )
			arrow_blink = 0;
	}

	if ( atmosphere ){
		atmosphere->draw( work );
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

Scene::~Scene(){
	delete current_block;
	for ( deque< Block * >::iterator it = level_blocks.begin(); it != level_blocks.end(); it++ ){
		delete *it;
	}
        for (vector<Block*>::iterator it = old_level_blocks.begin(); it != old_level_blocks.end(); it++){
            delete *it;
        }

	if ( background )
		delete background;

	if ( arrow )
		delete arrow;

	for ( vector< Bitmap * >::iterator it = front_panels.begin(); it != front_panels.end(); it++ ){
		delete *it;
	}
	for ( map< int, Panel * >::iterator it = panels.begin(); it != panels.end(); it++ ){
		delete (*it).second;
	}
	if ( atmosphere ){
		delete atmosphere;
	}

        if (script){
            delete script;
        }
	
	/*
	for ( vector< Heart * >::iterator it = hearts.begin(); it != hearts.end(); it++ ){
		delete *it;
	}
	*/

}
