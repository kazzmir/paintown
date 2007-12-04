#include "character.h"
#include "util/bitmap.h"
#include "util/load_exception.h"
#include "nameplacer.h"
#include "animation.h"
#include "util/font.h"
#include "util/funcs.h"
#include "buddy_player.h"
#include "factory/font_render.h"

static const char * PLAYER_FONT = "/fonts/arial.ttf";

BuddyPlayer::BuddyPlayer( const Character & chr ) throw( LoadException ):
Character( chr ){
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
}
	
void BuddyPlayer::takeDamage( World * world, ObjectAttack * obj, int x ){
	Character::takeDamage( world, obj, x );
}
	
void BuddyPlayer::hurt( int x ){
	if ( ! isInvincible() ){
		Character::hurt( x );
	}
}
