#include "animation.h"
#include "animation_event.h"
#include "animation_event_projectile.h"
#include "projectile.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include <iostream>

using namespace std;

AnimationEventProjectile::AnimationEventProjectile( Token * token ) throw( LoadException ):
projectile( NULL ),
x( 0 ),
y( 0 ),
dx( 0 ),
dy( 0 ),
life( 0 ){
	if ( *token != "projectile" ){
		throw LoadException(__FILE__, __LINE__, "Token starts with " + token->getName() + " instead of 'projectile'." );
	}

	while ( token->hasTokens() ){
		Token * current;
		*token >> current;
		if ( *current == "at" ){
			int x, y;
			*current >> x >> y;
			setX( x );
			setY( y );
		} else if ( *current == "path" ){
			string path;
			*current >> path;
                        Filesystem::AbsolutePath full = Filesystem::find(Filesystem::RelativePath(path));
			TokenReader reader(full.path());
			try{
				projectile = new Projectile(reader.readToken());
			} catch ( const TokenException & ex ){
				cerr<< "Could not read " << full.path() <<" : " << ex.getReason() << endl;
				// delete head;
				throw LoadException(__FILE__, __LINE__, "Could not open projectile file: " + full.path());
			}
		} else if ( *current == "life" ){
			int life;
			*current >> life;
			setLife( life );
		} else if ( *current == "speed" ){
			double dx, dy;
			*current >> dx >> dy;
			setDX( dx );
			setDY( dy );
		}
	}

	if ( projectile == NULL ){
		throw LoadException(__FILE__, __LINE__, "No 'path' token given for projectile" );	
	}

	if ( life == 0 ){
		throw LoadException(__FILE__, __LINE__, "No 'life' token given for projectile" );
	}
}

void AnimationEventProjectile::Interact( Animation * animation ){
	Projectile * copy = (Projectile *) projectile->copy();
	copy->setDX( getDX() );
	copy->setDY( getDY() );
	copy->setLife( getLife() );
	animation->createProjectile( getX(), -getY(), copy );
}
	
AnimationEventProjectile::~AnimationEventProjectile(){
	if ( projectile != NULL ){
		delete projectile;
	}
}
