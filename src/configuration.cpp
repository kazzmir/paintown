#include "configuration.h"
#include "util/keyboard.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "globals.h"
#include "object/animation.h"
#include "object/object.h"
#include <sstream>

#include <map>

using namespace std;

Configuration Configuration::defaultPlayer1Keys(){
	Configuration config;
	config.setRight( Keyboard::Key_RIGHT );
	config.setLeft( Keyboard::Key_LEFT );
	config.setUp( Keyboard::Key_UP );
	config.setDown( Keyboard::Key_DOWN );
	config.setAttack1( Keyboard::Key_A );
	config.setAttack2( Keyboard::Key_S );
	config.setAttack3( Keyboard::Key_D );
	config.setJump( Keyboard::Key_SPACE );
	return config;
}

Configuration Configuration::defaultPlayer2Keys(){
	Configuration config;
	config.setRight( Keyboard::Key_L );
	config.setLeft( Keyboard::Key_J );
	config.setUp( Keyboard::Key_I );
	config.setDown( Keyboard::Key_COMMA );
	config.setAttack1( Keyboard::Key_R );
	config.setAttack2( Keyboard::Key_T );
	config.setAttack3( Keyboard::Key_Y );
	config.setJump( Keyboard::Key_B );
	return config;
}

static map< int, Configuration * > configs;

Configuration & Configuration::config( int set ){
	if ( configs[ set ] == NULL ){
		configs[ set ] = new Configuration();
		switch( set ){
			case 0 : {
				*configs[ set ] = defaultPlayer1Keys();
				break;
			}
			case 1 : {
				*configs[ set ] = defaultPlayer2Keys();
				break;
			}
		}
	}
	return *configs[ set ];
}

Configuration::Configuration():
right( 0 ),
left( 0 ),
up( 0 ),
down( 0 ),
attack1( 0 ),
attack2( 0 ),
attack3( 0 ),
jump( 0 ){
}

Configuration::Configuration( const Configuration & config ):
right( config.getRight() ),
left( config.getLeft() ),
up( config.getUp() ),
down( config.getDown() ),
attack1( config.getAttack1() ),
attack2( config.getAttack2() ),
attack3( config.getAttack3() ),
jump( config.getJump() ){
}
	
Configuration & Configuration::operator=( const Configuration & config ){
	setRight( config.getRight() );
	setLeft( config.getLeft() );
	setUp( config.getUp() );
	setDown( config.getDown() );
	setAttack1( config.getAttack1() );
	setAttack2( config.getAttack2() );
	setAttack3( config.getAttack3() );
	setJump( config.getJump() );
	return *this;
}
	
Configuration::~Configuration(){
}

int Configuration::getKey( int which, int facing ){
	switch( which ){
		case PAIN_KEY_FORWARD : {
			if ( facing == Object::FACING_LEFT ) 
				return left;
			else 	return right;
		}
		case PAIN_KEY_BACK : {
			if ( facing == Object::FACING_LEFT )
				return right;
			else	return left;
		}
		case PAIN_KEY_UP : return up;
		case PAIN_KEY_DOWN : return down;
		case PAIN_KEY_ATTACK1 : return attack1;
		case PAIN_KEY_ATTACK2 : return attack2;
		case PAIN_KEY_ATTACK3 : return attack3;
		case PAIN_KEY_JUMP : return jump;
		default : return -1;
	}
}

void Configuration::setRight( int i ){
	right = i;
}

void Configuration::setLeft( int i ){
	left = i;
}

void Configuration::setUp( int i ){
	up = i;
}

void Configuration::setDown( int i ){
	down = i;	
}

void Configuration::setAttack1( int i ){
	attack1 = i;
}

void Configuration::setAttack2( int i ){
	attack2 = i;
}

void Configuration::setAttack3( int i ){
	attack3 = i;
}

void Configuration::setJump( int i ){
	jump = i;
}

int Configuration::getRight() const {
	return right;
}

int Configuration::getLeft() const {
	return left;
}

int Configuration::getUp() const {
	return up;
}

int Configuration::getDown() const {
	return down;
}

int Configuration::getAttack1() const {
	return attack1;
}

int Configuration::getAttack2() const {
	return attack2;
}

int Configuration::getAttack3() const {
	return attack3;
}

int Configuration::getJump() const {
	return jump;
}

#ifdef WINDOWS
#else
static string configFile(){
	ostringstream str;
	str << getenv( "HOME" ) << "/.paintownrc";
	return str.str();
}
#endif

void Configuration::loadConfigurations(){
	try{
		string file = configFile();
		TokenReader tr( file );
		Token * head = tr.readToken();
		if ( *head != "configuration" ){
			Global::debug( 0 ) << "Config file " << configFile() << " does not use the configuration format" << endl;
		}
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load configuration file " << configFile() << ": " << le.getReason() << endl;
	} catch ( const TokenException & t ){
		Global::debug( 0 ) << "Error loading configuration file '" << configFile() << "': " << t.getReason() << endl;
	}
}

void Configuration::saveConfiguration(){
	Token head( "configuration", false );
	head.toString( Global::debug( 0 ), string("") );
}
