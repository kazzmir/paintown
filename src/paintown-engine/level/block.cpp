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
                setLength(l);
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
                    Util::ReferenceCount<BlockObject> object(new BlockObject(current));

                    /* cache the object in the factory */
                    // Object * tmp = ObjectFactory::createObject(so);
                    Paintown::Object * tmp = cacher.cache(object);
                    if (tmp == NULL){
                        current->print(" ");
                        throw LoadException(__FILE__, __LINE__, "Could not cache object" );
                    } else {
                        /* clean up! */

                        delete tmp;
                        objects.push_back(object);
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
            Global::debug(0) << "Ignoring error: "<< le.getTrace() <<endl;
            // throw le;
        }
    }

}
    
Block::Block():
id(-1),
finished(-1),
continuous(false){
}
    
void Block::addBlockObject(const Util::ReferenceCount<BlockObject> & object){
    objects.push_back(object);
}
	
bool Block::empty(){
    return objects.empty();
}

Block::~Block(){
}
