#include <iostream>

#include "animation.h"
#include "util/graphics/bitmap.h"
#include "effect.h"
#include "globals.h"
#include "util/load_exception.h"
#include "object.h"
#include "object_nonattack.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "../game/world.h"

using namespace std;

namespace Paintown{

Effect::Effect( int alliance ):
ObjectNonAttack( 0, 0, alliance ),
image( NULL ){
	image = NULL;
}

Effect::Effect( const Effect & ef ):
ObjectNonAttack( ef ){
	image = new Animation( *(ef.image), NULL );
}

Effect::Effect( const char * _filename, int alliance ):
ObjectNonAttack( 0, 0, alliance ),
image(NULL){

    TokenReader tr;

    setMaxHealth( 100 );

    Token * head;
    try{ 
        head = tr.readTokenFromFile(_filename);
    } catch ( const TokenException * te ){
        throw LoadException(__FILE__, __LINE__, "Could not load effect");
    }

    if ( *head != "effect" ){
        cout<<_filename<< " is not an effect"<<endl;
        // delete head;
        throw LoadException(__FILE__, __LINE__, "Not an effect");
    }

    TokenView view = head->view();
    while (view.hasMore()){
        const Token * n = NULL;
        try{
            view >> n;

            // for ( int q = 0; q < head->numTokens(); q++ ){
            // n = head->getToken( q );

            if ( *n == "anim" ){
                if ( image ){
                    // cout<<"Mulitple animations specified"<<endl;
                    throw LoadException(__FILE__, __LINE__, "Multiple animations specified");
                }

                image = new Animation(n, NULL);
            } else {
                cout<<"Unhandled effect attribute: "<<endl;
                n->print(" ");
            }

            // }

            /* catch exception?? */
        } catch( const exception & ex ){
            // delete head;
            if (n != NULL){
                n->print(" ");
            }
            /*
               if ( n ){
               cout<<"Error with: "<<n<<endl;
               } else 
               cout<<"Something bad happened in character"<<endl;
               throw ex;
               */
            throw LoadException(__FILE__, __LINE__, "Effect parse error");
        }

    }

    // delete head;

}

Object * Effect::copy(){
	return new Effect( *this );
}
	
bool Effect::isGettable(){
	return false;
}
	
Network::Message Effect::getCreateMessage(){
    Network::Message m;
    return m;
}
	
int Effect::getWidth() const{
    if ( image )
        return image->getWidth();
    return 0;
}
	
bool Effect::isCollidable( Object * obj ){
    return false;
}

int Effect::getHeight() const{
    if ( image )
        return image->getHeight();
    return 0;
}

void Effect::act( vector< Object * > * others, World * world, vector< Object * > * add ){
    if ( image ){
        if ( image->Act() ){
            setHealth( -1 );
            // image->reset();
        }
    }
}
	
void Effect::draw( Graphics::Bitmap * work, int rel_x, int rel_y ){
    if (image){
        image->Draw( getRX() - rel_x, getRY(), NULL, work);
    }
}

Effect::~Effect(){
    delete image;
}

}
