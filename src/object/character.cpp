#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <math.h>

#include "animation.h"
#include "character.h"
#include "globals.h"
#include "object.h"
#include "object_attack.h"

#include "factory/shadow.h"
#include "util/bitmap.h"
#include "util/ebox.h"
#include "util/load_exception.h"
#include "util/sound.h"
#include "util/timedifference.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "world.h"

using namespace std;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#endif

/* probably should never be run...
 */
Character::Character( int alliance ):
ObjectAttack( alliance ),
x_velocity( 0 ),
y_velocity( 0 ),
death( 0 ),
thrown_status( false ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ){
}

Character::Character( const string & filename, int alliance ) throw( LoadException ):
ObjectAttack( 0, 0, alliance ),
speed( 0 ),
type( 0 ),
shadow( 0 ),
icon( NULL ),
x_velocity( 0 ),
y_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ){
	name = "";

	loadSelf( filename.c_str() );
}

Character::Character( const char * filename, int alliance ) throw( LoadException ):
ObjectAttack( 0, 0, alliance ),
speed( 0 ),
type( 0 ),
shadow( 0 ),
icon( NULL ),
x_velocity( 0 ),
y_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ){
	name = "";

	loadSelf( filename );

	
}

Character::Character( const Character & chr ) throw( LoadException ):
ObjectAttack(chr),
death( 0 ),
thrown_status( false ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ){
	own_stuff = false;

	/* these are set in object.cpp */
	// setHealth( chr.getHealth() );
	// setMaxHealth( chr.getMaxHealth() );
	// setFacing( chr.getFacing() );
	// setX( chr.getX() );
	// setY( chr.getY() );
	// setZ( chr.getZ() );

	name = chr.getName();
	speed = chr.speed;
	type = chr.type;
	shadow = chr.shadow;
	icon = chr.icon;

	linked = NULL;
	setYVelocity( 0 );
	setXVelocity( 0 );

	current_map = 0;

	setJumpingYVelocity( chr.getMaxJumpingVelocity() );
	setShadow( chr.getShadow() );
	status = chr.getStatus();

	if ( chr.die_sound != NULL ) 
		die_sound = new Sound( *(chr.die_sound) );

	if ( chr.landed_sound != NULL )
		landed_sound = new Sound( *(chr.landed_sound) );

	/*
	for ( map<string,Animation*>::const_iterator it = chr.movements.begin(); it != chr.movements.end(); it++ ){
		const Animation * const & ani_copy = (*it).second;
		movements[ ani_copy->getName() ] = new Animation( *ani_copy, this );
	}

	animation_current = movements[ "idle" ];
	*/

	/* Copy the movements */
	const map<int, map<string,Animation * > > & his_mapper = chr.getMapper();
	for ( map<int,map<string,Animation*> >::const_iterator it = his_mapper.begin(); it != his_mapper.end(); it++ ){
		int id = (*it).first;
		const map<string,Animation*> & his = (*it).second;
		map< string, Animation * > & mine = mapper[ id ];
	
		for ( map<string,Animation*>::const_iterator m_it = his.begin(); m_it != his.end(); m_it++ ){
			if ( (*m_it).second != NULL ){
				const Animation * const & ani_copy = (*m_it).second;
				// Animation * ani_copy = (*m_it).second;

				mine[ ani_copy->getName() ] = new Animation( *ani_copy, this );
			}
		}

	}

	animation_current = getMovement( "idle" );

}

void Character::loadSelf( const char * filename ) throw ( LoadException ){

	TokenReader tr( filename );

	Token * head;
	try{
		head = tr.readToken();
	} catch( const TokenException & ex ){
		cerr<< "Could not read "<<filename<<" : "<<ex.getReason()<<endl;
		// delete head;
		throw LoadException("Could not open character file");
	}
	string xls = "Load time for ";
	xls += filename;

	if ( *head != "character" ){
		cout<<filename<< " is not a character"<<endl;
		// delete head;
		throw LoadException("File is not a character");
	}

	current_map = 0;

	map< string, string > remaps;

	Token * n = NULL;
	try{

		while ( head->hasTokens() ){
			*head >> n;

			if ( *n == "name" ){
				*n >> name;
				if ( name.length() > 0 && (name[0] >= 'a' && name[0] <= 'z') )
					name[0] = name[0] - 'a' + 'A';
					
			} else if ( *n == "anim" ){

				Animation * ani = NULL;

				try{
					ani = new Animation( n, this );
				} catch( const LoadException & le ){
					// delete ani;
					throw le;
				}

				/*
				if ( movements[ ani->getName() ] != 0 ){
					delete movements[ ani->getName() ];
				}
				movements[ ani->getName() ] = ani;
				*/

				map< string, Animation * > & cur = mapper[ current_map ];
				if ( cur[ ani->getName() ] != 0 ){
					delete cur[ ani->getName() ];
				}
				cur[ ani->getName() ] = ani;

			} else if ( *n == "health" ){
				int h;
				*n >> h;
				setHealth( h );
				setMaxHealth( h );
			} else if ( *n == "jump-velocity" ){
				double h;
				*n >> h;
				setJumpingYVelocity( h );
			} else if ( *n == "diesound" ){

				string _snd;
				*n >> _snd;

				die_sound = new Sound( _snd );

				/* TODO: implement a sound class
				die_sound = load_sample( _snd.c_str() );
				if ( !die_sound ){
					cout<<"*WARNING*: Could not load sound "<<_snd<<endl;
					// throw exception();
				}
				*/
			} else if ( *n == "landed" ){
				string st;
				*n >> st;
				landed_sound = new Sound( st );
			} else if ( *n == "speed" ){
				*n >> speed;
			} else if ( *n == "type" ){
				*n >> type;
			} else if ( *n == "shadow" ){
				int x;
				*n >> x;
				setShadow( x );
			} else if ( *n == "icon" ){
				string icon_path;
				*n >> icon_path;
				cout<<"Loading icon "<<icon_path<<endl;
				icon = new Bitmap( icon_path );
			} else if ( *n == "remap" ){
				string first;
				string second;
				*n >> first >> second;
				remaps[ second ] = first;
			} else {
				cout<<"Unhandled character attribute: "<<endl;
				n->print(" ");
			}

		}

	} catch( const TokenException & tex ){
		cout<< "TokenException: " << tex.getReason() << endl;
		n->print(" ");
		cout<<"* Dumping character"<<endl;
		head->print("*");
		// delete head;

		throw LoadException("Error parsing character");
	} catch( const LoadException & lex ){
		throw lex;
	}

	// delete head;

	/*
	map< string, Animation * > & movements = mapper[ current_map ];
	movements = mapper[ current_map ];
	*/

	for ( map<string,string>::iterator it = remaps.begin(); it != remaps.end(); it++ ){
		const string & x1 = (*it).first;
		const string & alter = (*it).second;
		reMap( alter, x1, getMapper().size() );
	}

	// if ( movements[ "idle" ] == NULL ){
	if ( getMovement( "idle" ) == NULL ){
		throw LoadException("No 'idle' movement");
	}

	// if ( movements[ "pain" ] == NULL ){
	if ( getMovement( "pain" ) == NULL ){
		throw LoadException("No 'pain' movement");
	}
	/*
	if ( movements[ "grab" ] == NULL ){
		throw LoadException("No 'grab' movement");
	}
	*/

	// if ( movements[ "fall" ] == NULL ){
	if ( getMovement( "fall" ) == NULL ){
		throw LoadException("No 'fall' movement");
	}

	// animation_current = movements[ "idle" ];
	animation_current = getMovement( "idle" );

}
	
void Character::setMap( const unsigned int x ){
	current_map = x;
	if ( current_map >= mapper.size() )
		current_map = mapper.size() - 1;
	animation_current = getMovement( animation_current->getName() );
}

/* swap some colors around */
void Character::reMap( const string & from, const string & to, int id ){
	
	if ( mapper.find( id ) != mapper.end() )
		return;
	
	Bitmap b_from( from );
	Bitmap b_to( to );

	map< int, int > remap_colors;

	for ( int x1 = 0; x1 < b_from.getWidth(); x1++ ){
		for ( int y1 = 0; y1 < b_from.getHeight(); y1++ ){
			int from_col = b_from.getPixel( x1, y1 );
			int to_col = b_to.getPixel( x1, y1 );
			if ( to_col != -1 && from_col != to_col )
				remap_colors[ from_col ] = to_col;
		}
	}

	map< string, Animation * > & new_map = mapper[ id ];
	const map< string, Animation * > & old_map = getMovements();
	for ( map<string,Animation*>::const_iterator it = old_map.begin(); it != old_map.end(); it++ ){
		const string & name = (*it).first;
		const Animation * old_ani = (*it).second;

		Animation * new_ani = new Animation( *old_ani, this );
		new_ani->reMap( remap_colors );
		// cout<<"Remap "<<new_ani->getName()<<" attack = "<<new_ani->isAttack()<<endl;

		new_map[ name ] = new_ani;
	}

}
	
bool Character::isCollidable( Object * obj ){
	if ( death >= 1 )
		return false;
	if ( obj == this )
		return false;

	if ( getThrown() ){
		// cout<<this<<": Thrown obj is "<<thrown_from<<". Obj = "<<obj<<endl;
		return thrown_from != obj;
	}

	// if ( getStatus() == Status_Fell || getStatus() == Status_Hurt )
	if ( getStatus() == Status_Fell || getStatus() == Status_Rise )
		return false;
	return obj->getAlliance() != getAlliance();
	// return true;
}
	
Animation * Character::getCurrentMovement(){
	return this->animation_current;
}
	
Animation * Character::getMovement( const string & str ){
	// cout<<getName()<<" map = "<<current_map<<endl;
	map< string, Animation * > & xmap = mapper[ current_map ];
	// map< string, Animation * > & xmap = getMovements();
	return xmap[ str ];
}

Animation * Character::getMovement( const unsigned int x ){
	// cout<<"Current map = "<<current_map<<endl;
	map< string, Animation * > & xmap = mapper[ current_map ];
	// cout<<"Have "<<xmap.size()<<" animations"<<endl;

	Animation * ret = NULL;
	if ( x < xmap.size() ){
		unsigned int q = 0;
		map<string,Animation*>::iterator it;
		for ( it = xmap.begin(); q < x; q++, it++ );
		ret = (*it).second;
		// animation_current = (*it).second;
		// animation_current->reset();
	} else {
		// cout<<"Could not find animation for "<<x<<endl;
		ret = animation_current;
	}

	return ret;
}

bool Character::testAnimation(){
	return animation_current->Act();
}

void Character::testReset(){
	animation_current->reset();
}

void Character::testAnimation( unsigned int x ){
	animation_current = getMovement( x );
	animation_current->reset();
	/*
	if ( x < movements.size() ){
		int q = 0;
		map<string,Animation*>::iterator it;
		for ( it = movements.begin(); q < x; q++, it++ );
		animation_current = (*it).second;
		animation_current->reset();
	}
	*/
}
	
const int Character::getAlliance() const{
	/*
	if ( getStatus() == Status_Fell && isMoving() )
		return ALLIANCE_NONE;
	*/
	if ( getThrown() )
		return ALLIANCE_NONE;

	return Object::getAlliance();
}

Object * Character::copy(){
	return new Character( *this );
}
	
int Character::getDamage() const{
	if ( animation_current != NULL ){
		return animation_current->getDamage();
	}
	return 0;
}
	
void Character::setName( const string & str ){
	name = str;
}

void Character::doJump( double x_ ){
	setStatus( Status_Jumping );
	setXVelocity( x_ );
	initJumpingYVelocity();
	// cout<<"Jumping velocity: "<<getMaxJumpingVelocity()<<endl;
	// cout<<"Do jump: "<<getJumpingYVelocity()<<endl;
}

/** TODO **/
void Character::jump(){
}

/** TODO **/
bool Character::isJumping(){
	return getStatus() == Status_Jumping;
}

const string Character::getName() const{
	return name;
}

void Character::fall( double x_vel, double y_vel ){
	animation_current = getMovement( "fall" );
	// animation_current = movements[ "fall" ];
	animation_current->reset();
	setStatus( Status_Fell );
	if ( getLink() != NULL ){
		getLink()->unGrab();
		setLink( NULL );
	}

	/* fall backwards */
	setXVelocity( x_vel );
	setYVelocity( y_vel );

}
	
void Character::takeDamage( ObjectAttack * obj, int x ){
	// if ( getStatus() == Status_Dead ) return;
	// cout<<this<<" "<<getName()<<" taking "<<x<<" damage"<<endl;
	Object::takeDamage( obj, x );
	
	if ( (x > 6 || getHealth() <= 0 || getStatus() == Status_Jumping) && getStatus() != Status_Fell ){
		/*
		animation_current = movements[ "fall" ];
		setStatus( Status_Fell );
		if ( getLink() != NULL ){
			getLink()->unGrab();
		}

		/ * fall backwards * /
		setXVelocity( -1.7 );
		setYVelocity( 4.9 );
		*/
		fall( -1.7, 4.4 );
		// cout<<getName()<<" falling. death = "<<death<<" health = "<<getHealth()<<" status = "<<getStatus()<<endl;
	} else	{
		animation_current = getMovement( "pain" );
		// animation_current = movements[ "pain" ];
		setStatus( Status_Hurt );
	}
	// if ( animation_current->empty() )
		animation_current->reset();

	if ( getHealth() <= 0 && death == 0 ){
		death = 1;
		setHealth( 1 );

		if ( die_sound ){
			die_sound->play();
		}
		// cout<<getName()<<" died from regular damage"<<endl;
		// setStatus( Status_Dead );
	}
}

void Character::upperCase( string & who ){
	for ( string::size_type q = 0; q < who.length(); q++ )
		if ( who[q] >= 'a' && who[q] <= 'z' )
			who[q] = who[q] - 'a' + 'A';
}
	
const string & Character::getAttackName(){
	return animation_current->getName(); 
}

void Character::decreaseYVelocity(){
	y_velocity -= 0.18;
}
	
void Character::decreaseXVelocity(){
	x_velocity -= 0.6;
}

void Character::landed( World * world ){
	if ( death >= 2 ) return;

	setThrown( false );

	switch( getStatus() ){
		case Status_Fell : {
	
			if ( landed_sound )
				landed_sound->play();
		
			double cur = fabs( getYVelocity() ) + fabs( getXVelocity() );
			// cout<<getName()<<" taking "<<cur<<" from falling. Health = "<<getHealth()<<endl;
			Object::takeDamage( NULL, (int)cur );

			world->Quake( (int)fabs(getYVelocity()) );

			// setStatus( Status_Hurt );
			
			if ( (getHealth() <= 0 && death == 0) || death == 1 ){
				death = 2;
				setHealth( 1 );
			} else {
				// setStatus( Status_Hurt );
				setStatus( Status_Rise );
				// animation_current = movements[ "rise" ];
				animation_current = getMovement( "rise" );
				animation_current->reset();
			}

			// cout<<getName()<<" landed: death = "<<death<<endl;

			break;
		}
		case Status_Jumping : {
		
			setStatus( Status_Ground );
			// animation_current = movements["idle"];
			animation_current = getMovement( "idle" );
			animation_current->reset();

			break;
		}
	}
}

void Character::act( vector< Object * > * others, World * world ){
		
	if ( isMoving() ){
			
		decreaseYVelocity();
		moveX( getXVelocity() );
		moveY( getYVelocity() );
	
		if ( getY() == 0 ){
			landed( world );
			setMoving( false );
		}
	}

	/*
	if ( animation_current ){
		if ( animation_current->Act() ){
			// animation_current->reset();
			nextTicket();
			if ( getStatus() == Status_Fell ){
				animation_current = movements[ "rise" ];
				setStatus( Status_Ground );
			} else	animation_current = movements[ "idle" ];

			animation_current->reset();
		}
	}
	*/

	// cout<<getName()<<" death is "<<death<<endl;
	if ( death >= 2 ){
		// cout<<getName()<<" Death is "<<death<<endl;
		if ( ++death > 60 ){
			setHealth( 0 );
		}
	}

	if ( getStatus() == Status_Fell ){
		// setLink( NULL );
		
		/*
		if ( getY() == 0 ){

			double cur = fabs( getYVelocity() ) + fabs( getXVelocity() );
			cout<<getName()<<" taking "<<cur<<" from falling"<<endl;
			// Object::takeDamage( NULL, (int)cur );

			// setStatus( Status_Hurt );
			
			/ *
			if ( getHealth() <= 0 && death == 0 ){
				death = 1;
				setHealth( 1 );
				// setStatus( Status_Dead );
			}
			* /

			setXVelocity( 0 );
			setYVelocity( 0 );
		} else 
			decreaseYVelocity();

		if ( animation_current->Act() && death < 2 ){
			if ( death == 0 ){
				if ( getY() == 0 ){
					setStatus( Status_Hurt );
					animation_current = movements[ "rise" ];
					animation_current->reset();
				}
			} else {
				death = 2;
			}
		}
		*/

		animation_current->Act();

	} else if ( getStatus() == Status_Hurt ){
		if ( animation_current->Act() ){
			if ( getLink() == NULL )
				setStatus( Status_Ground );	
			else	{
				setStatus( Status_Grabbed );
				// animation_current = movements["pain"];
				animation_current = getMovement( "pain" );
			}
		}
	} else if ( getStatus() == Status_Rise ){
		if ( animation_current->Act() ){
			animation_current = getMovement( "idle" );
			setStatus( Status_Ground );
		}
	} else if ( getStatus() == Status_Grabbed ){
		if ( ++grab_time > 120 ){
			setStatus( Status_Ground );
			if ( getLink() )
				getLink()->unGrab();
		}
	}

	/*
	if ( isJumping() ){
		// cout<<"Jumping: "<<getJumpingYVelocity()<<endl;
		// moveX( getXVelocity() );
		// moveY( getYVelocity() );
		decreaseYVelocity();
		if ( getY() == 0 ){
			// setY( 0 );
			setStatus( Status_Ground );
			animation_current = movements["idle"];
			animation_current->reset();

			setXVelocity( 0 );
			setYVelocity( 0 );
		}
		// cout<<"Y: "<<getY() <<endl;
	}
	*/
}
	
void Character::unGrab(){
	setStatus( Status_Ground );
	setLink( NULL );
}
	
void Character::grabbed( Object * obj ){
	setStatus( Status_Grabbed );

	/* make sure grabber is in front */
	// setZ( obj->getZ() - 1 );
	setX( obj->getX() );
	// moveX( obj->getFacing(), obj->getWidth() / 4 );
	moveX( obj->getFacing(), 36 );
	setFacing( obj->getOppositeFacing() );
	setLink( obj );
	grab_time = 0;
	setY( 0 );
	// animation_current = movements[ "pain" ];
	animation_current = getMovement( "pain" );
	animation_current->reset();
}

void Character::filterEnemies( vector< Object * > & mine, vector< Object * > * all ){
	for ( vector< Object * >::iterator it = all->begin(); it != all->end(); it++ ){
		Object * n = *it;
		int q = n->getAlliance();
		if ( q != ALLIANCE_NONE && q != getAlliance() )
			mine.push_back( n );

			/*
		if ( n->getAlliance() == ALLIANCE_PLAYER ){
			mine->push_back( n );
		}
		*/
	}
}
	
bool Character::isAttacking(){	
	/*
	if ( getStatus() == Status_Fell && isMoving() )
		return true;
	*/
	if ( getThrown() )
		return true;

	if ( animation_current )
		return animation_current->isAttack();
	return false;
}
	
void Character::thrown(){
	setThrown( true );
	thrown_from = getLink();
}
	
void Character::getAttackCoords( int & x, int & y){
	if ( animation_current ){
		animation_current->getAttackCoords( x, y );
		if ( getFacing() == FACING_LEFT ){
			x = getRX() - x + getWidth() / 2;
		} else {
			x += getRX() - getWidth()/2;
		}
		y += getRY() - getHeight();
	}
}
	
const map<string,Animation*> & Character::getMovements() {
	return mapper[ current_map ];
}
	
void Character::attacked( Object * something ){
	animation_current->contacted();
}
	
void Character::collided( ObjectAttack * obj ){
	/*
	last_obj = obj;
	last_collide = obj->getTicket();
	*/

	collision_objects[ obj ] = obj->getTicket();

	setFacing( obj->getOppositeFacing() );
}

bool Character::realCollision( Object * obj ){

	if ( this->getCollide() != 0 && obj->getCollide() != 0 ){
		bool my_xflip = false;
		bool his_xflip = false;
		if ( getFacing() == FACING_LEFT )
			my_xflip = true;
		if ( obj->getFacing() == FACING_LEFT )
			his_xflip = true;

		int mx, my;
		int ax, ay;

		ECollide * me = getCollide();
		ECollide * him = obj->getCollide();
		if ( !me ){
			// cout<<"No collide"<<endl;
			return false;
		}
		
		mx = this->getRX() - getWidth() / 2;
		my = this->getRY() - getHeight();
		ax = obj->getRX() - obj->getWidth() / 2;
		ay = obj->getRY() - obj->getHeight();

		/*
		Bitmap Screen( screen );
		me->draw( &Screen, mx, my, my_xflip );
		him->draw( &Screen, ax, ay, his_xflip );
		*/
		

		// cout<<"Mx: "<<mx<< " My: "<<my<<" Width: "<<me->getWidth()<<" Height: "<<me->getHeight()<<endl;
		// cout<<"Ax: "<<ax<< " Ay: "<<ay<<" Width: "<<him->getWidth()<<" Height: "<<him->getHeight()<<endl;

		return ( me->Collision( him, mx, my, ax, ay, my_xflip, false, his_xflip, false ) );

	}
	return false;

}

bool Character::collision( ObjectAttack * obj ){

	if ( collision_objects[ obj ] == obj->getTicket() )
	// if ( last_obj == obj && last_collide == obj->getTicket() )
		return false;

	return realCollision( obj );
}

/* draw a nifty translucent life bar */
void Character::drawLifeBar( int x, int y, int he, Bitmap * work ){
	// set_trans_blender( 0, 0, 0, 128 );
	Bitmap::drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	// drawing_mode( DRAW_MODE_TRANS, NULL, 0, 0 );
	const int health_height = 7;
	work->rectangleFill( x, y, x + getMaxHealth(), y + health_height, Bitmap::makeColor( 192, 32, 32 ) );
	// set_trans_blender( 0, 0, 0, 64 );
	Bitmap::transBlender( 0, 0, 0, 64 );
	for ( int y1 = y; y1 <= y+health_height; y1++ ){
		work->rectangleFill( x, y, x + he, y1, Bitmap::makeColor( 16, 162, 246 ) );
	}
	// drawing_mode( DRAW_MODE_SOLID, NULL, 0, 0 );
	Bitmap::drawingMode( Bitmap::MODE_SOLID );

	// work->rectangleFill( x, y, x + getHealth(), y + 10, Bitmap::makeColor( 255, 255, 255 ) );
}

void Character::draw( Bitmap * work, int rel_x ){	

	if ( death >= 15 ){
		if ( (death/8) % 2 == 0 ){
			return;
		}
	}

	Bitmap const * shadow = Shadow::getShadow( getShadow() );
	// set_multiply_blender( 0, 0, 0, 164 );
	Bitmap::multiplyBlender( 0, 0, 0, 164 );
	shadow->drawTrans( getRX() - shadow->getWidth() / 2 - rel_x, getZ() - shadow->getHeight() / 2, *work );

	if ( animation_current ){
		if ( getFacing() == Object::FACING_RIGHT ){
			animation_current->Draw( getRX() - rel_x, getRY(), work );
		} else {
			animation_current->DrawFlipped( getRX() - rel_x, getRY(), work ); 
		}
	}

	// work->circleFill( getRX(), getRY(), 3, Bitmap::makeColor(255,255,0) );

	/*
	if ( icon )
		icon->draw( 1, 1, *work );
	*/
	
}

/*
bool Character::collision( Object * obj ){
	cout<<"X: "<<getRX()<< " Y: "<<getRY()<<endl;
	Object::collision( obj );
	return false;
}
*/
	
const int Character::getWidth() const{
	if ( animation_current ){
		return animation_current->getWidth();
	}
	return 0;
}

const int Character::getHeight() const{
	if ( animation_current ){
		return animation_current->getHeight();
	}
	return 0;
}

ECollide * Character::getCollide(){
	if ( animation_current ){
		return animation_current->getCollide( getFacing() );
	}
	cout<<"No animation collide"<<endl;
	return NULL;
}

void Character::print() const{
	cout<<"Name: "<<name<<endl;
	cout<<"Health: "<<getHealth()<<endl;
	cout<<"Speed: "<<speed<<endl;
	cout<<"Type: "<<type<<endl;
	cout<<"Shadow: "<<shadow<<endl;
	if ( icon )
		cout<<"Icon: "<<icon->getPath()<<endl;
}

Character::~Character(){
	if ( own_stuff ){
		if ( icon ) delete icon;
		/* TODO: use sound class here */
		// if ( die_sound ) destroy_sample( die_sound );
	}

	for ( map< int, map<string,Animation*> >::iterator it = mapper.begin(); it != mapper.end(); it++ ){
		map< string, Animation * > & use = (*it).second;
		for ( map<string,Animation*>::iterator m_it = use.begin(); m_it != use.end(); m_it++ ){
			Animation* & aa = (*m_it).second;
			if ( (*m_it).second != NULL ){
				delete aa;
			}
		}
	}

	if ( die_sound ){
		delete die_sound;
	}
	if ( landed_sound ){
		delete landed_sound;
	}

	/*
	for ( map<string,Animation*>::iterator it = movements.begin(); it != movements.end(); it++ ){
		Animation *& aa = (*it).second;
		delete aa;
	}
	*/

	/*
	for ( vector< Animation * >::iterator it = animation_attacks.begin(); it != animation_attacks.end(); it++ )
		delete *it;
	*/
}

