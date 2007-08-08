#include "world.h"
#include "versus_world.h"
#include "object/character.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "object/effect.h"

VersusWorld::VersusWorld( Character * player1, Character * player2 ):
World(),
player1( player1 ),
player2( player2 ){
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
	return 150;
}

int VersusWorld::getMinimumZ(){
	return getMaximumZ();
}
	
void VersusWorld::act(){
	if ( quake_time > 0 ){
		quake_time--;
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

						o_good->attacked( *fight, added_effects );
						(*fight)->collided( o_good, added_effects );
						(*fight)->takeDamage( o_good, o_good->getDamage() );
						// }
					}
				}
			}
		}

		it++;
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
	
	// min_x = (int)min_x_virtual;
	
	for ( map<int,vector<Object *> >::iterator it = object_z.begin(); it != object_z.end(); it++ ){
		vector<Object *> & xx = (*it).second;
		for ( vector<Object *>::iterator mm = xx.begin(); mm != xx.end(); mm++ ){

			(*mm)->draw( work, min_x );
		}
	}
}

VersusWorld::~VersusWorld(){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
		if ( *it == player1 || *it == player2 ){
			it = objects.erase( it );
		} else {
			it++;
		}
	}
}
