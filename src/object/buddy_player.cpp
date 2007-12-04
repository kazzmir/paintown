#include "character.h"
#include "util/bitmap.h"
#include "util/load_exception.h"
#include "nameplacer.h"
#include "animation.h"
#include "util/font.h"
#include "world.h"
#include "globals.h"
#include "util/funcs.h"
#include "buddy_player.h"
#include "factory/font_render.h"

static const char * PLAYER_FONT = "/fonts/arial.ttf";

BuddyPlayer::BuddyPlayer( const Character & chr ) throw( LoadException ):
Character( chr ),
want_x( -1 ),
want_z( -1 ){
	show_life = getHealth();
	int x, y;
	NamePlacer::getPlacement( x, y, name_id );
}
	
void BuddyPlayer::draw( Bitmap * work, int rel_x ){
	Character::draw( work, rel_x );

	int x1, y1;
	NamePlacer::getPlacement( x1, y1, name_id );
	
	if ( icon )
		icon->draw( x1, y1, *work );

	int hasIcon = icon ? icon->getWidth() : 0;
	

	if ( show_life < 0 ){
		show_life = 0;
	}

	const Font & player_font = Font::getFont( Util::getDataPath() + PLAYER_FONT, 20, 20 );
	const string & name = getName();
	int nameHeight = player_font.getHeight( name ) / 2;
	nameHeight = 20 / 2;
	FontRender * render = FontRender::getInstance();
	render->addMessage( player_font, (hasIcon + x1) * 2, y1 * 2, Bitmap::makeColor(255,255,255), -1, name );
	drawLifeBar( hasIcon + x1, y1 + nameHeight, work );
	int max = getMaxHealth() < 100 ? getMaxHealth() : 100;
	render->addMessage( player_font, (x1 + hasIcon + max + 5) * 2, y1 + nameHeight, Bitmap::makeColor(255,255,255), -1, "x %d", getLives() );

}
	
void BuddyPlayer::drawLifeBar( int x, int y, Bitmap * work ){
	drawLifeBar( x, y, show_life, work );
}
	
Object * BuddyPlayer::copy(){
	return new BuddyPlayer( *this );
}

static int furthestFriend( vector< Object * > * others, int alliance ){
	double x = -1;
	for ( vector< Object * >::iterator it = others->begin(); it != others->end(); it++ ){
		Object * o = *it;
		if ( o->getAlliance() == alliance && o->getX() > x ){
			x = o->getX();
		}
	}

	return (int) x;
}

const Object * BuddyPlayer::findClosest( const vector< Object * > & enemies ){
	Object * e = NULL;
	double max = 0;
	for ( vector< Object * >::const_iterator it = enemies.begin(); it != enemies.end(); it++ ){
		Object * current = *it;
		/* should probably see if current is a character.. */
		double distance = fabs( current->getX() - getX() );
		if ( e == NULL || distance < max ){
			e = current;
			max = distance;
		}
	}

	return e;
}

void BuddyPlayer::act( vector< Object * > * others, World * world, vector< Object * > * add ){
	Character::act( others, world, add );

	vector< Object * > enemies;
	
	if ( getStatus() != Status_Ground && getStatus() != Status_Jumping )
		return;

	filterEnemies( enemies, others );
		
	if ( animation_current->Act() ){
		animation_current->reset();
		// nextTicket();
		// animation_current = movements[ "idle" ];
		animation_current = getMovement( "idle" );
		animation_current->reset();
	}

	if ( enemies.empty() && want_x == -1 && want_z == -1 && Util::rnd( 10 ) == 0 ){
		want_x = Util::rnd( 80 ) - 40 + furthestFriend( others, getAlliance() );
		want_z = Util::rnd( world->getMinimumZ(), world->getMaximumZ() );
	} else if ( ! enemies.empty() ){
		const Object * enemy = findClosest( enemies );
		want_x = (int)(Util::rnd( 30 ) - 15 + enemy->getX());
		want_z = (int)(Util::rnd( 3 ) - 1 + enemy->getZ());
	}

	if ( want_x != -1 && want_z != -1 ){
		bool walk = false;
		if ( getX() - want_x < -2 ){
			moveX( getSpeed() );
			setFacing( FACING_RIGHT );
			walk = true;
		} else if ( getX() - want_x > 2 ){
			setFacing( FACING_LEFT );
			moveX( getSpeed() );
			walk = true;
		}
		
		if ( getZ() < want_z ){
			moveZ( getSpeed() );
			walk = true;
		} else if ( getZ() > want_z ){
			moveZ( -getSpeed() );
			walk = true;
		}
			
		if ( walk ){
			animation_current = getMovement( "walk" );
		}

		if ( fabs(getX() - want_x) <= 2 &&
		     fabs(getZ() - want_z) <= 2 ){
			want_x = -1;
			want_z = -1;
			animation_current = getMovement( "idle" );
		}
	}
}
	
void BuddyPlayer::takeDamage( World * world, ObjectAttack * obj, int x ){
	Character::takeDamage( world, obj, x );
}
	
void BuddyPlayer::hurt( int x ){
	if ( ! isInvincible() ){
		Character::hurt( x );
	}
}
