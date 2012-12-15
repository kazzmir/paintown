#include <iostream>

#include "block.h"
#include "blockobject.h"
#include "../object/enemy.h"
#include "globals.h"
#include "util/exceptions/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "util/token_exception.h"
#include "../factory/object_factory.h"
#include "cacher.h"

using namespace std;

/*
static const string dataPath( const string & str ){
	return Util::getDataPath() + str;
}
*/
	
Block::Block(const Token * tok, const Level::Cacher & cacher):
id(-1),
finished( -1 ),
continuous( false ){

    if ( *tok != "block" ){
        throw LoadException(__FILE__, __LINE__, "Not a scene block");
    }

    TokenView view = tok->view();
    while (view.hasMore()){
        try{
            const Token * current;
            view >> current;
            if (*current == "length"){
                int l;
                current->view() >> l;
                setLength( l );
            } else if ( *current == "wait" ){
                current->view() >> wait;
            } else if ( *current == "id" ){
                int id;
                current->view() >> id;
                setId(id);
            } else if ( *current == "continuous" ){
                setContinuous( true );
            } else if ( *current == "finish" ){
                int f;
                current->view() >> f;
                setFinished( f );
            } else if ( *current == "object" ){
                try{ 
                    BlockObject * so = new BlockObject(current);

                    /* cache the object in the factory */
                    // Object * tmp = ObjectFactory::createObject(so);
                    Paintown::Object * tmp = cacher.cache(*so);
                    if (tmp == NULL){
                        current->print(" ");
                        delete so;
                        throw LoadException(__FILE__, __LINE__, "Could not cache object" );
                    } else {
                        /* clean up! */

                        delete tmp;
                        objects.push_back( so );
                    }
                } catch (const LoadException & le){
                    throw le;
                } catch (const Filesystem::NotFound & fail){
                    throw LoadException(__FILE__, __LINE__, fail, "Could not load object");
                }
            }
        } catch( const TokenException & te ){
            throw LoadException(__FILE__, __LINE__, te, "Block parse exception");
        } catch( const LoadException & le ){
            cout<<"Ignoring error: "<< le.getTrace() <<endl;
            // throw le;
        }
    }

}
    
Block::Block():
id(-1),
finished(-1),
continuous(false){
}
    
void Block::addBlockObject(BlockObject * object){
    objects.push_back(object);
}
	
bool Block::empty(){
	return objects.empty();
}

vector< Heart * > Block::createObjects( int total_length, int min_x, int max_x, int min_z, int max_z, vector< Paintown::Object * > * list ){
    vector< Heart * > hearts;

    /* to silence compiler warnings */
    /*
    min_x = min_x;
    max_x = max_x;
    */

    // cout<<"Total length = "<<total_length<<" Min_x = "<<min_x<<" Max_x = " << max_x << endl;
    // cout<<"Current block has "<<objects.size()<<" total = "<<total_length<<" min = "<<min_x<<" max_x = "<<max_x<<endl;
    for ( vector< BlockObject * >::iterator it = objects.begin(); it != objects.end(); ){
        BlockObject * obj = *it;

        int x, z;
        obj->getCoords( x, z );
        x += total_length;
        /* dont care about distance.. */
        if ( true ){

            Paintown::Object * newobj = ObjectFactory::createObject( obj );
            if ( newobj == NULL ){
                delete obj;
                it = objects.erase( it );
                continue;
            }

            /* does this violate some OOP principle? oh wel */
            if ( obj->getType() == ObjectFactory::EnemyType ){
                Heart * h = ((Paintown::Enemy *)newobj)->getHeart();
                hearts.push_back( h );
            }
            // newobj->moveX( total_length );
            newobj->moveRight( total_length );
            newobj->moveZ( min_z );
            if ( newobj->getZ() < min_z ){
                newobj->setZ( min_z );
            }
            if ( newobj->getZ() > max_z ){
                newobj->setZ( max_z );
            }
            list->push_back( newobj );
            // cout<<"Created new object at "<<newobj->getX()<<" " <<newobj->getZ()<<". Min_x = "<<min_x<<". block = "<<total_length<<endl;

            delete obj;
            it = objects.erase( it );
        } else {
            // cout<<"Coords not in range "<<x<<endl;
            it++;
        }
    }

    return hearts;
}
	
Block::~Block(){
	for ( vector<BlockObject *>::iterator it = objects.begin(); it != objects.end(); it++ ){
		delete *it;
	}
}
