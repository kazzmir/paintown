#include "game/adventure_world.h"
#include "versus_world.h"
#include "object/character.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "object/effect.h"

VersusWorld::VersusWorld( int z, Character * player1, Character * player2 ):
AdventureWorld(),
player1( player1 ),
player2( player2 ),
z( z ){
	this->addObject( player1 );
	this->addObject( player2 );

	string bang_path( Util::getDataPath() + "/misc/flash/flash.txt" );
	Object * effect = new Effect( bang_path.c_str() );
	if ( bang != NULL ){
		delete bang;
	}
	bang = effect;
}
	
int VersusWorld::getMaximumZ(){
	return z;
}

int VersusWorld::getMinimumZ(){
	return getMaximumZ();
}
	
void VersusWorld::act(){
	if ( quake_time > 0 ){
		quake_time--;
	}
		
	if ( player1->getX() < 50 ){
		player1->setX( 50 );
	}
	if ( player1->getX() > 600 ){
		player1->setX( 600 );
	}
	
	if ( player2->getX() < 50 ){
		player2->setX( 50 );
	}
	if ( player2->getX() > 600 ){
		player2->setX( 600 );
	}

	player1->setZ( getMinimumZ() );
	player2->setZ( getMinimumZ() );

	if ( player1->getX() < player2->getX() ){
		player1->setFacing( Object::FACING_RIGHT );
		player2->setFacing( Object::FACING_LEFT );
	} else {
		player1->setFacing( Object::FACING_LEFT );
		player2->setFacing( Object::FACING_RIGHT );
	}

	vector< Object * > added_effects;
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
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
					// cout << o_good << " is attacking " << *fight << " with " << o_good->getAttackName() << endl;

					// cout << "Zdistance: " << good->ZDistance( *fight ) << " = " << (good->ZDistance( *fight ) < o_good->minZDistance()) << endl;
					// cout << "Collision: " << (*fight)->collision( o_good ) << endl;
					if ( good->ZDistance( *fight ) <= o_good->minZDistance() && (*fight)->collision( o_good ) ){ 

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
						}

						o_good->attacked(this, *fight, added_effects );
						(*fight)->collided( o_good, added_effects );
						(*fight)->takeDamage( this, o_good, o_good->getDamage() );
						// }
					}
				}
			}
		}

		if ( *it != player1 && *it != player2 && (*it)->getHealth() <= 0 ){
			(*it)->died( added_effects );
			delete *it;
			it = objects.erase( it );
		} else ++it;
	}

	objects.insert( objects.end(), added_effects.begin(), added_effects.end() );
}
	
const bool VersusWorld::finished() const {
	return player1->getHealth() <= 0 || player2->getHealth() <= 0;
}

void VersusWorld::draw( Bitmap * work ){

	map< int, vector<Object*> > object_z;

	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * n = *it;
		object_z[ n->getRZ() ].push_back( n );
	}

	for ( map<int,vector<Object *> >::iterator it = object_z.begin(); it != object_z.end(); it++ ){
		vector<Object *> & xx = (*it).second;
		for ( vector<Object *>::iterator mm = xx.begin(); mm != xx.end(); mm++ ){

			(*mm)->draw( work, 0 );
		}
	}
}

VersusWorld::~VersusWorld(){
	/* everything in 'objects' gets delete'd in the world destructor, so
	 * remove the two objects that were created elsewhere
	 */
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
		if ( *it == player1 || *it == player2 ){
			it = objects.erase( it );
		} else {
			it++;
		}
	}
}
