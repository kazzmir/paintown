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
#include "stimulation.h"
#include "gib.h"

#include "factory/shadow.h"
#include "util/funcs.h"
#include "util/bitmap.h"
#include "util/lit_bitmap.h"
#include "util/ebox.h"
#include "util/load_exception.h"
#include "util/sound.h"
#include "util/timedifference.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "world.h"


using namespace std;

static const string dataPath( const string & str ){
	return Util::getDataPath() + str;
}

Character::Character( int alliance ):
ObjectAttack( alliance ),
shadow( 0 ),
icon( NULL ),
own_stuff( false ),
x_velocity( 0 ),
y_velocity( 0 ),
z_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
speed( 1 ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
invincibility( 0 ),
toughness( 10 ),
explode( false ),
lives( 0 ){
}

Character::Character( const string & filename, int alliance ) throw( LoadException ):
ObjectAttack( 0, 0, alliance ),
type( 0 ),
shadow( 0 ),
icon( NULL ),
own_stuff( false ),
x_velocity( 0 ),
y_velocity( 0 ),
z_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
speed( 1 ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
invincibility( 0 ),
toughness( 10 ),
explode( false ),
lives( 0 ){
	name = "";

	loadSelf( filename.c_str() );
}

Character::Character( const char * filename, int alliance ) throw( LoadException ):
ObjectAttack( 0, 0, alliance ),
type( 0 ),
shadow( 0 ),
icon( NULL ),
own_stuff( false ),
x_velocity( 0 ),
y_velocity( 0 ),
z_velocity( 0 ),
status( Status_Ground ),
death( 0 ),
thrown_status( false ),
speed( 1 ),
linked( NULL ),
moving( 0 ),
current_map( 0 ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
invincibility( 0 ),
toughness( 10 ),
explode( false ),
lives( 0 ){
	name = "";

	loadSelf( filename );
}

Character::Character( const Character & chr ) throw( LoadException ):
ObjectAttack(chr),
own_stuff( false ),
death( 0 ),
thrown_status( false ),
moving( 0 ),
current_map( chr.current_map ),
die_sound( NULL ),
landed_sound( NULL ),
squish_sound( NULL ),
explode( false ){

	/* these are set in object.cpp */
	// setHealth( chr.getHealth() );
	// setMaxHealth( chr.getMaxHealth() );
	// setFacing( chr.getFacing() );
	// setX( chr.getX() );
	// setY( chr.getY() );
	// setZ( chr.getZ() );

	name = chr.getName();
	lives = chr.lives;
	type = chr.type;
	shadow = chr.shadow;
	icon = chr.icon;
	setSpeed( chr.getSpeed() );

	path = chr.getPath();

	linked = NULL;
	setYVelocity( 0 );
	setXVelocity( 0 );
	setZVelocity( 0 );

	body_parts = chr.body_parts;

	setJumpingYVelocity( chr.getMaxJumpingVelocity() );
	setShadow( chr.getShadow() );
	status = chr.getStatus();
	invincibility = chr.invincibility;
	toughness = chr.getToughness();

	if ( chr.die_sound != NULL ) 
		die_sound = new Sound( *(chr.die_sound) );

	if ( chr.landed_sound != NULL ){
		landed_sound = new Sound( *(chr.landed_sound) );
	}

	if ( chr.squish_sound != NULL ){
		squish_sound = new Sound( *(chr.squish_sound) );
	}

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
	
	setMap( chr.getCurrentMap() );
}
	
Network::Message Character::getCreateMessage(){
	Network::Message message;

	message.id = 0;
	message << World::CREATE_CHARACTER;
	message << getAlliance();
	message << getId();
	message << getCurrentMap();
	Global::debug( 1 ) << "Character create id " << getId() << endl;
	string path = getPath();
	Global::debug( 1 ) << "Character create path: '" << path << "'" << endl;
	path.erase( 0, Util::getDataPath().length() );
	Global::debug( 1 ) << "Character create sending: '" << path << "'" << endl;
	message << path;

	return message;
}

void Character::loadSelf( const char * filename ) throw ( LoadException ){

	// setInvincibility( 1000 );
	TokenReader tr( filename );

	Token * head = NULL;
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
				if ( name.length() > 0 && (name[0] >= 'a' && name[0] <= 'z') ){
					name[0] = name[0] - 'a' + 'A';
				}
					
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
				setMaxHealth( h );
				setHealth( h );
			} else if ( *n == "jump-velocity" ){
				double h;
				*n >> h;
				setJumpingYVelocity( h );
			} else if ( *n == "hit-sound" ){
				string _snd;
				*n >> _snd;
				setHit( Sound( dataPath( _snd ) ) );
			} else if ( *n == "die-sound" ){
				string _snd;
				*n >> _snd;
				die_sound = new Sound( dataPath( _snd ) );
			} else if ( *n == "landed" ){
				string st;
				*n >> st;
				landed_sound = new Sound( dataPath( st ) );
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
				own_stuff = true;
				// cout<<"Loading icon "<<icon_path<<endl;
				icon = new Bitmap( dataPath( icon_path ) );
			} else if ( *n == "remap" ){
				string first;
				string second;
				*n >> first >> second;
				remaps[ dataPath( second ) ] = dataPath( first );
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

	squish_sound = new Sound( dataPath( "sounds/squish.wav" ) );

	for ( map<string,string>::iterator it = remaps.begin(); it != remaps.end(); it++ ){
		const string & x1 = (*it).first;
		const string & alter = (*it).second;
		reMap( alter, x1, getMapper().size() );
	}

	if ( getMovement( "idle" ) == NULL ){
		throw LoadException("No 'idle' movement");
	}

	if ( getMovement( "pain" ) == NULL ){
		throw LoadException("No 'pain' movement");
	}
	
	if ( getMovement( "rise" ) == NULL ){
		throw LoadException("No 'rise' movement");
	}
	/*
	if ( movements[ "grab" ] == NULL ){
		throw LoadException("No 'grab' movement");
	}
	*/

	if ( getMovement( "fall" ) == NULL ){
		throw LoadException("No 'fall' movement");
	}

	// animation_current = movements[ "idle" ];
	animation_current = getMovement( "idle" );

	body_parts = getBodyParts( getMovement( "idle" ) );

	path = filename;
}

static int nonMaskingPixels( Bitmap * bitmap ){
	int total = 0;
	for ( int x = 0; x < bitmap->getWidth(); x++ ){
		for ( int y = 0; y < bitmap->getHeight(); y++ ){
			if ( bitmap->getPixel( x, y ) != Bitmap::MaskColor ){
				total += 1;
			}
		}
	}
	return total;
}

static void replacePart( vector< BodyPart > & parts, Bitmap * bitmap ){
	int i = Util::rnd( parts.size() );
	delete parts[ i ].image;
	parts[ i ].image = bitmap;
}
	
vector< BodyPart > Character::getBodyParts( Animation * animation ){
	vector< BodyPart > parts;

	Bitmap * bitmap = animation->getFrame( 0 );
	if ( bitmap == NULL ){
		return parts;	
	}

	const int gib_size = 12;
	for ( int x = 0; x < bitmap->getWidth(); x += gib_size ){
		for ( int y = 0; y < bitmap->getHeight(); y += gib_size ){
			Bitmap * sub = new Bitmap( *bitmap, x, y, gib_size, gib_size );
			if ( 100.0 * (double) nonMaskingPixels( sub ) / (double) (sub->getWidth() * sub->getHeight()) < 10.0 ){
				delete sub;
			} else {
				parts.push_back( BodyPart( x - getWidth() / 2, getHeight() - y, sub ) );
			}
		}
	}

	const char * more[] = { "/misc/body/arm.png",
				"/misc/body/bone.png",
				"/misc/body/hand.png",
				"/misc/body/intestines.png",
				"/misc/body/leg.png",
				"/misc/body/ribcage.png",
				"/misc/body/skull.png",
				"/misc/body/spine.png",
				"/misc/body/torso.png" };

	for ( unsigned int i = 0; i < sizeof(more) / sizeof(char*); i++ ){
		replacePart( parts, new Bitmap( dataPath( more[ i ] ) ) );	
	}

	return parts;
}

void Character::nextMap(){
	nextMap( getCurrentMap() );
}

void Character::nextMap( int x ){
	unsigned int next = x + 1;
	if ( next >= mapper.size() ){
		next = 0;
	}
	setMap( next );
}

const int Character::getNextMap() const {
	return getNextMap( current_map );
}

const int Character::getNextMap( unsigned int x ) const {
	if ( x + 1 >= mapper.size() ){
		return 0;
	}
	return x + 1;
}
	
void Character::setMap( const unsigned int x ){
	current_map = x;
	if ( current_map >= mapper.size() ){
		current_map = mapper.size() - 1;
	}
	if ( current_map < 0 ){
		current_map = 0;
	}
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
			if ( to_col != -1 && from_col != to_col ){
				remap_colors[ from_col ] = to_col;
			}
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
	if ( getStatus() == Status_Fell || getStatus() == Status_Rise ){
		return false;
	}

	return obj->getAlliance() != getAlliance();
	// return true;
}
	
bool Character::isGettable(){
	return false;
}
	
Animation * Character::getCurrentMovement(){
	return this->animation_current;
}
	
void Character::setMovement( Animation * animation, const string & name ){
	map< string, Animation * > & xmap = mapper[ current_map ];
	xmap[ name ] = animation;
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
	World w;
	vector< Object * > others;
	vector< Object * > more;
	act( &others, &w, &more );
	return animation_current->Act();
}

void Character::testReset(){
	animation_current->reset();
}

void Character::testAnimation( string name ){
	animation_current = getMovement( name );
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

void Character::doJump( double vx, double vz ){
	setStatus( Status_Jumping );
	setXVelocity( vx );
	setZVelocity( vz );
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
	
void Character::takeDamage( World * world, ObjectAttack * obj, int damage ){
	/* I think if death >= 1 here I should just return */
	Object::takeDamage( world, obj, damage );
	
	Global::debug( 2 ) << getName() << " has " << currentDamage() << " damage" << endl;
	if ( (currentDamage() > getToughness() || getHealth() <= 0 || getStatus() == Status_Jumping) && getStatus() != Status_Fell ){

		if ( currentDamage() > getToughness() && getHealth() <= 0 ){
			setExplode( true );
			world->addMessage( explodeMessage() );
		}

		reduceDamage( currentDamage() );
		/*
		animation_current = movements[ "fall" ];
		setStatus( Status_Fell );
		if ( getLink() != NULL ){
			getLink()->unGrab();
		}
		*/

		fall( -1.7, 4.4 );
		world->addMessage( fallMessage( -1.7, 4.4 ) );
	} else	{
		animation_current = getMovement( "pain" );
		if ( getStatus() != Status_Grabbed ){
			setStatus( Status_Hurt );
		}
	}
	animation_current->reset();

	Global::debug( 1 ) << this << " explode " << getExplode() << " health " << getHealth() << " death " << death << endl;
	if ( ! getExplode() && getHealth() <= 0 && death == 0 ){
		death = 1;
		setHealth( 1 );
		Global::debug( 1 ) << this << " set death to 1. Health " << getHealth() << endl;

		if ( die_sound ){
			die_sound->play();
		}
	}

	world->addMessage( animationMessage() );
	world->addMessage( healthMessage() );
}
	
void Character::died( vector< Object * > & objects ){
	if ( getExplode() ){
		for ( vector< BodyPart >::iterator it = body_parts.begin(); it != body_parts.end(); it++ ){
			const BodyPart & part = *it;
			
			int x = (int) getX() + part.x;
			int y = (int) getY() + part.y;
			double dx = (Util::rnd( 11 ) - 5) / 4.2;
			double dy = (Util::rnd( 10 ) + 4) / 3.5;
			objects.push_back( new Gib( x, y, (int) getZ(), dx, dy, part.image ) );
		}
		if ( squish_sound != NULL ){
			squish_sound->play();
		}
		/*
		for ( int i = 0; i < 20 + Util::rnd( 10 ); i++ ){
		// for ( int i = 0; i < 1; i++ ){
			int x = (int) getX();
			int y = (int) (getY() + Util::rnd( getHeight() ));
			// int y = (int) (getY() - 50);

			double dx = (Util::rnd( 11 ) - 5) / 3.5;
			double dy = (Util::rnd( 10 ) + 4) / 3.0;
			objects.push_back( new Gib( x, y, (int) getZ(), dx, dy ) );
		}
		*/
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
	
void Character::stimulate( const Stimulation & stim ){
	stim.stimulate( *(Character *) this );
}
	
void Character::decreaseXVelocity(){
	x_velocity -= 0.6;
}

void Character::decreaseZVelocity(){
	z_velocity -= 0.6;
}

void Character::landed( World * world ){
	if ( death >= 2 ) return;

	setThrown( false );

	switch( getStatus() ){
		case Status_Falling : {
			if ( landed_sound ){
				landed_sound->play();
			}

			world->Quake( (int)fabs(getYVelocity()) );

			setStatus( Status_Ground );
			animation_current = getMovement( "idle" );
			animation_current->reset();
			world->addMessage( movedMessage() );
			world->addMessage( animationMessage() );
			break;
		}
		case Status_Fell : {
	
			if ( landed_sound ){
				landed_sound->play();
			}
		
			double cur = fabs( getYVelocity() ) + fabs( getXVelocity() );
			// cout<<getName()<<" taking "<<cur<<" from falling. Health = "<<getHealth()<<endl;
			Object::takeDamage( world, NULL, (int)cur );

			world->Quake( (int)fabs(getYVelocity()) );

			// setStatus( Status_Hurt );
			
			if ( (getHealth() <= 0 && death == 0) || death == 1 ){
				Global::debug( 2 ) << this << " died from fall" << endl;
				death = 2;
				setHealth( 1 );
			} else {
				// setStatus( Status_Hurt );
				setStatus( Status_Rise );
				// animation_current = movements[ "rise" ];
				animation_current = getMovement( "rise" );
				animation_current->reset();
			}

			world->addMessage( healthMessage() );
			world->addMessage( movedMessage() );
			world->addMessage( animationMessage() );

			// cout<<getName()<<" landed: death = "<<death<<endl;

			break;
		}
		case Status_Jumping : {
		
			setStatus( Status_Ground );
			// animation_current = movements["idle"];
			animation_current = getMovement( "idle" );
			animation_current->reset();
			
			world->addMessage( movedMessage() );
			world->addMessage( animationMessage() );

			break;
		}
	}
}

void Character::createProjectile( Projectile * projectile ){
	projectiles.push_back( (Object *) projectile );
}
	
Network::Message Character::grabMessage( unsigned int from, unsigned int who ){
	Network::Message message;
	message.id = 0;
	message << World::GRAB;
	message << from;
	message << who;

	return message;
}

void Character::act( vector< Object * > * others, World * world, vector< Object * > * add ){

	if ( invincibility > 0 ){
		invincibility--;
	}

	for ( vector< Object * >::iterator it = projectiles.begin(); it != projectiles.end(); it++ ){
		Object * obj = *it;
		obj->setAlliance( getAlliance() );
		obj->setFacing( getFacing() );
	}
	add->insert( add->end(), projectiles.begin(), projectiles.end() );
	projectiles.clear();

	reduceDamage();
	// cout << getName() << " now has " << currentDamage() << endl;

	/* when the character moves not because of a move or walking */
	if ( isMoving() ){
		// cout << this << " is moving with velocities " << getXVelocity() << " " << getYVelocity() << " " << getZVelocity() << endl;
			
		/* force of gravity, subtract acceleration for
		 * downward movement
		 */
		decreaseYVelocity();
		moveX( getXVelocity() );
		moveY( getYVelocity() );
		moveZ( getZVelocity() );
	
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
			Global::debug( 1 ) << this << " dying" << endl;
			setHealth( 0 );
			world->addMessage( healthMessage() );
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
			if ( getLink() == NULL ){
				setStatus( Status_Ground );	
			} else {
				setStatus( Status_Grabbed );
				// animation_current = movements["pain"];
				animation_current = getMovement( "pain" );
			}
		}
	} else if ( getStatus() == Status_Rise || getStatus() == Status_Get ){
		if ( animation_current->Act() ){
			animation_current = getMovement( "idle" );
			setStatus( Status_Ground );
		}
	} else if ( getStatus() == Status_Grabbed ){
		if ( ++grab_time > 120 ){
			setStatus( Status_Ground );
			if ( getLink() ){
				getLink()->unGrab();
				setLink( NULL );
			}
			world->addMessage( ungrabMessage() );
		} else {
			if ( getLink() ){
				world->addMessage( grabMessage( getLink()->getId(), getId() ) );
			}
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
	setZ( obj->getZ() + 1 );
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
	
void Character::deathReset(){
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
	
/* used to find the x/y where the attack takes place for effects */
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
	
void Character::attacked( Object * something, vector< Object * > & objects ){
	animation_current->contacted();
}

void Character::collided( ObjectAttack * obj, vector< Object * > & objects ){
	/*
	last_obj = obj;
	last_collide = obj->getTicket();
	*/

	// cout << this << " collided with " << obj << endl;
	Object::collided( obj, objects );

	if ( obj != NULL ){
		collision_objects[ obj ] = obj->getTicket();
		setFacing( obj->getOppositeFacing() );
	}

	if ( getStatus() != Status_Grabbed ){
		moveX( -5 );
	}
}

const int Character::getRX() const {
	if ( animation_current ){
		if ( getFacing() == FACING_LEFT ){
			return Object::getRX() - animation_current->getOffsetX();
		} else {
			return Object::getRX() + animation_current->getOffsetX();
		}
	}
	return Object::getRX();
}

const int Character::getRZ() const {
	/*
	if ( animation_current ){
		return Object::getZ() + animation_current->getOffsetY();
	}
	*/
	return Object::getRZ();
}

const int Character::getRY() const {
	if ( animation_current ){
		return Object::getRY() + animation_current->getOffsetY();
	}
	return Object::getRY();
}

bool Character::realCollision( ObjectAttack * obj ){

	ECollide * myCollide = this->getNormalCollide();
	ECollide * hisCollide = obj->getCollide();
	// cout << "Obj attacking with " << obj->getAttackName() << " my collide = " << myCollide << " his collide = " << hisCollide << endl;
	if ( myCollide != 0 && hisCollide ){
		bool my_xflip = false;
		bool his_xflip = false;
		if ( getFacing() == FACING_LEFT )
			my_xflip = true;
		if ( obj->getFacing() == FACING_LEFT )
			his_xflip = true;

		/*
		my_xflip = false;
		his_xflip = false;
		*/

		int mx, my;
		int ax, ay;

		// ECollide * me = getCollide();
		// ECollide * him = obj->getCollide();
		/*
		if ( !me ){
			// cout<<"No collide"<<endl;
			return false;
		}
		*/
		
		mx = this->getRX() - getWidth() / 2;
		my = this->getRY() - getHeight();
		ax = obj->getRX() - obj->getWidth() / 2;
		ay = obj->getRY() - obj->getHeight();

		/*
		mx = getRX();
		my = getRY();
		ax = obj->getRX();
		ay = obj->getRY();
		*/

		// Bitmap Screen( screen );
		/*
		myCollide->draw( *Bitmap::Screen, mx, my, Bitmap::makeColor( 255, 0, 128 ), my_xflip );
		hisCollide->draw( *Bitmap::Screen, ax, ay, Bitmap::makeColor( 128, 255, 0 ), his_xflip );
		*/

		// cout<<"Mx: "<<mx<< " My: "<<my<<" Width: "<<myCollide->getWidth()<<" Height: "<<myCollide->getHeight()<<endl;
		// cout<<"Ax: "<<ax<< " Ay: "<<ay<<" Width: "<<hisCollide->getWidth()<<" Height: "<<hisCollide->getHeight()<<endl;

		bool b = myCollide->Collision( hisCollide, mx, my, ax, ay, my_xflip, false, his_xflip, false );
		if ( b && false ){
			myCollide->draw( *Bitmap::Screen, 50, 50, my_xflip );
			hisCollide->draw( *Bitmap::Screen, 50 + (ax - mx), 50 + (ay - my), his_xflip );
			Util::rest( 500 );
		}
		return b;
	}
	return false;

}
	
const double Character::minZDistance() const {
	if ( animation_current ){
		return animation_current->getMinZDistance();
	}
	return 10;
}

bool Character::collision( ObjectAttack * obj ){

	if ( getInvincibility() > 0 ){
		return false;
	}

	if ( getThrown() ){
		return false;
	}

	if ( collision_objects[ obj ] == obj->getTicket() ){
		// if ( last_obj == obj && last_collide == obj->getTicket() )
		Global::debug( 2 ) << this << " already collided with " << obj << endl;
		return false;
	}

	return realCollision( obj );
}
	
void Character::drawLifeBar( int x, int y, Bitmap * work ){
	drawLifeBar( x, y, getHealth(), work );
}
	
/* draw a nifty translucent life bar */
void Character::drawLifeBar( int x, int y, int health, Bitmap * work ){
	Bitmap::drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	const int health_height = 7;
	int max = getMaxHealth() < 100 ? getMaxHealth() : 100;
	work->rectangleFill( x, y, x + max, y + health_height, Bitmap::makeColor( 192, 32, 32 ) );
	Bitmap::transBlender( 0, 0, 0, 64 );

	int colors[ 5 ] = { Bitmap::makeColor( 16, 162, 246 ),
			     Bitmap::makeColor( 214, 184, 48 ),
			     Bitmap::makeColor( 244, 16, 12 ),
			     Bitmap::makeColor( 237, 173, 71 ),
			     Bitmap::makeColor( 183, 217, 180 ) };

	int color = 0;
	for ( int s = 0; s < health; s += 100 ){
		int e = s + 100 < health ? s + 100 : health;
		for ( int y1 = y; y1 <= y+health_height; y1++ ){
			work->rectangleFill( x, y, x + e - s, y1, colors[ color ] );
		}
		color += 1;
		if ( color > 4 ){
			color = 4;
		}
	}

	Bitmap::drawingMode( Bitmap::MODE_SOLID );
}

const int Character::getShadowX(){
	if ( animation_current ){
		if ( getFacing() == FACING_LEFT ){
			return -animation_current->getShadowX();
		} else {
			return animation_current->getShadowX();
		}
	}
	return 0;
}

const int Character::getShadowY(){
	if ( animation_current ){
		return animation_current->getShadowY();
	}
	return 0;
}
	
Network::Message Character::movedMessage(){
	Network::Message message = Object::movedMessage();
	message << getStatus();
	message << isMoving();
	return message;
}
	
Network::Message Character::explodeMessage(){
	Network::Message message;

	message.id = getId();
	message << CHARACTER_EXPLODE;

	return message;
}
	
Network::Message Character::fallMessage( double x, double y ){
	Network::Message message;

	message.id = getId();
	message << CHARACTER_FALL;
	message << (int)(x * 100);
	message << (int)(y * 100);

	return message;
}
	
Network::Message Character::ungrabMessage(){
	Network::Message message;

	message.id = getId();
	message << CHARACTER_UNGRAB;

	return message;
}
	
Network::Message Character::showNameMessage( int amount ){
	Network::Message message;
	message.id = getId();
	message << CHARACTER_SHOW_NAME;
	message << amount;

	return message;
}
	
Network::Message Character::healthMessage(){
	Network::Message message;

	message.id = getId();
	message << CHARACTER_HEALTH;
	message << getHealth();

	return message;
}

Network::Message Character::jumpMessage( double x, double z ){
	Network::Message message;
	message.id = getId();

	message << CHARACTER_JUMP;
	message << (int)(x * 100);
	message << (int)(z * 100);

	return message;
}
	
void Character::interpretMessage( Network::Message & message ){
	int type;
	message >> type;
	message.reset();
	Object::interpretMessage( message );
	switch ( type ){
		case OBJECT_MOVED : {
			int status;
			int moving;
			message >> status;
			message >> moving;
			setStatus( status );
			setMoving( moving );
			break;
		}
		case CHARACTER_JUMP : {
			int x, z;
			message >> x >> z;
			doJump( x / 100.0, z / 100.0 );
			animation_current = getMovement( "jump" );
			break;
		}
		case CHARACTER_HEALTH : {
			int health;
			message >> health;
			setHealth( health );
			Global::debug( 1 ) << "Health for " << getId() << " is " << getHealth() << endl;
			break;
		}
		case CHARACTER_EXPLODE : {
			setExplode( true );
			break;
		}
		case CHARACTER_FALL : {
			int x, y;
			message >> x >> y;
			fall( x / 100.0, y / 100.0 );
			break;
		}
		case CHARACTER_UNGRAB : {
			setStatus( Status_Ground );
			if ( getLink() ){
				getLink()->unGrab();
				setLink( NULL );
			}
			break;
		}
		case CHARACTER_ANIMATION : {
			int map;
			message >> map;
			setMap( map );
			animation_current = getMovement( message.path );
			if ( animation_current == NULL ){
				Global::debug( 1 ) << "Could not find animation for '" << message.path << "'" << endl;
				animation_current = getMovement( "idle" );
			}
			if ( message.path != "walk" && message.path != "idle" ){
				animation_current->reset();
				nextTicket();
			}
			break;
		}
	}
}
	
Network::Message Character::animationMessage(){
	Network::Message m;

	m.id = getId();
	m << CHARACTER_ANIMATION;
	m << getCurrentMap();
	m.path = getCurrentMovement()->getName();

	return m;
}

void Character::draw( Bitmap * work, int rel_x ){	

	if ( death >= 15 ){
		if ( (death/8) % 2 == 0 ){
			return;
		}
	}

	if ( animation_current ){
		Bitmap const * shadow = Shadow::getShadow( getShadow() );
		// set_multiply_blender( 0, 0, 0, 164 );
		Bitmap::multiplyBlender( 0, 0, 0, 164 );
		shadow->drawTrans( getRX() - shadow->getWidth() / 2 - rel_x + getShadowX(), (int) Object::getZ() - shadow->getHeight() / 2 + getShadowY(), *work );


		// printf( "invincibility = %d\n", invincibility );
		if ( invincibility > 0 ){
			// Bitmap::drawingMode( Bitmap::MODE_LIT );
			// Bitmap::transBlender( 0, 0, (int)(sin( getInvincibility() ) * 200 + 10), 0 );
			double f = sin( 3.14159 / 180.0 * getInvincibility() * 6 );
			int max_white = 80;
			int base_blue = 120;
			int c = (int)(f * max_white);
			if ( c < 0 ) c = 0;
			Bitmap::transBlender( c, c, (int)(f * (255 - (max_white + base_blue)) + base_blue + c), 50 );
			if ( getFacing() == Object::FACING_RIGHT ){
				animation_current->DrawLit( getRX() - rel_x, getRY(), work );
			} else {
				animation_current->DrawLitFlipped( getRX() - rel_x, getRY(), work ); 
			}
		} else {

			if ( getFacing() == Object::FACING_RIGHT ){
				animation_current->Draw( getRX() - rel_x, getRY(), work );
			} else {
				animation_current->DrawFlipped( getRX() - rel_x, getRY(), work ); 
			}
		}


		if ( Global::getDebug() > 5 ){
			int x = (int)(getX() - rel_x);
			int y = (int) getRY();
			int x2 = x + animation_current->getRange();
			if ( getFacing() == Object::FACING_LEFT ){
				x2 = x - animation_current->getRange();
			}
			work->rectangle( x, y, x2, y + 1, Bitmap::makeColor(255,255,255) );
		}

		/*
		if ( invincibility > 0 ){
			Bitmap::drawingMode( Bitmap::MODE_SOLID );
		}
		*/

		/*
		if ( global_debug ){
			work->circleFill( getRX() - rel_x, (int) getZ(), 3, Bitmap::makeColor(255,255,0) );
		}
		*/
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

ECollide * Character::getCollide() const {
	if ( animation_current ){
		return animation_current->getCollide( getFacing() );
	}
	cout<<"No animation collide"<<endl;
	return NULL;
}
	
ECollide * Character::getNormalCollide() const {
	if ( animation_current ){
		return animation_current->getNormalCollide();
	}
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

		for ( vector< BodyPart >::iterator it = body_parts.begin(); it != body_parts.end(); it++ ){
			BodyPart & b = *it;
			delete b.image;
		}
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

	delete die_sound;
	delete landed_sound;
	delete squish_sound;

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

