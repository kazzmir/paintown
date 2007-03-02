#include "music.h"
#include <string>
#include <aldumb.h>
#include <iostream>
// #include "defs.h"

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <pthread.h>
#include "util/funcs.h"

using namespace std;

static Music * instance = NULL;

static double volume = 1.0;
// static bool muted = false;
static pthread_t musicThread;
static pthread_mutex_t musicMutex;
static bool alive = true;

static void * playMusic( void * );

#define synchronized for( int __l( ! pthread_mutex_lock( &musicMutex ) ); __l; __l = 0, pthread_mutex_unlock( &musicMutex ) )

#define LOCK pthread_mutex_lock( &musicMutex );
#define UNLOCK pthread_mutex_unlock( &musicMutex );

/*
#undef LOCK
#undef UNLOCK
#define LOCK
#define UNLOCK
*/

Music::Music():
playing( false ),
fading( 0 ),
player( NULL ),
music_file( NULL ){

	if ( instance != NULL ){
		cerr << "Trying to instantiate music object twice!" << endl;
		return;
	}

	instance = this;

	pthread_mutex_init( &musicMutex, NULL );
	pthread_create( &musicThread, NULL, playMusic, (void *)instance );
}

/*
static bool isAlive(){
	bool f = false;
	synchronized{
		f = alive;
	}
	return f;
}
*/

static void * playMusic( void * _music ){
	Music * music = (Music *) _music;

	cout << "Playing music" << endl;

	/*
	unsigned int tick = 0;
	unsigned int counter;
	*/

	bool playing = true;
	while ( playing ){

		LOCK;{
			playing = alive;
			music->doPlay();
		}
		UNLOCK;
		rest( 10 );

		// Util::YIELD();
		// pthread_yield();
	}

	// cout << "Done with music thread" << endl;

	return NULL;
}

double Music::getVolume(){
	LOCK;{
		return volume;
	}
	UNLOCK;
}

void Music::doPlay(){
	if ( this->playing ){
		double f = fading / 500.0;
		switch ( fading ){
			case -1 : {
				if ( volume + f < 0 ){
					fading = 0;
					volume = 0;
				} else {
					volume += f;
					this->_setVolume( volume );
				}
				break;
			}
			case 1 : {
				if ( volume + f > 1.0 ){
					fading = 0;
					volume = 1.0;
				} else {
					volume += f;
					this->_setVolume( volume );
				}
				break;
			}
		}
		if ( al_poll_duh( this->player ) != 0 ){
		}
	}
}

/*
Music::Music( const char * song ):
volume( 1.0 ),
muted( false ),
player( NULL ),
music_file( NULL ){

	loadSong( song );

}

Music::Music( const string & song ):
volume( 1.0 ),
muted( false ),
player( NULL ),
music_file( NULL ){
	loadSong( song );
}
*/

void Music::fadeIn(){
	LOCK;{
		instance->_fadeIn();
	}
	UNLOCK;
}

void Music::fadeOut(){
	LOCK;{
		instance->_fadeOut();
	}
	UNLOCK;
}

void Music::_fadeIn(){
	fading = 1;
}

void Music::_fadeOut(){
	fading = -1;
}

bool Music::loadSong( const char * song ){
	bool loaded = false;
	LOCK;{
		loaded = instance->internal_loadSong( song );
	}
	UNLOCK;
	return loaded;
	// muted = false;
}

/* remove an element from a vector at index 'pos' and return it */
template< class Tx_ >
static Tx_ removeVectorElement( vector< Tx_ > & toRemove, int pos ){
	int count = 0;
	typename vector< Tx_ >::iterator it;
	for ( it = toRemove.begin(); it != toRemove.end() && count < pos; count++, it++ );

	if ( it == toRemove.end() ){
		/* this isnt right, but whatever */
		return toRemove.front();
	}

	const Tx_ & removed = toRemove[ pos ];
	toRemove.erase( it );
	return removed;
	
}
	
void Music::loadSong( const vector< string > & Songs ){

	/*
	cout << "Songs = " << &Songs << endl;
	if ( ! loadSong( "music/song5.xm" ) ){
		cerr << "Could not load music/song5.xm" << endl;
	}
	return;
	*/

	vector< string > _songs = Songs;
	vector< string > songs;
	while ( ! _songs.empty() ){
		int i = Util::rnd( _songs.size() );
		songs.push_back( removeVectorElement< string >( _songs, i ) );
	}

	/*
	songs.clear();
	songs.push_back( "music/song3.xm" );
	*/

	for ( vector< string >::iterator it = songs.begin(); it != songs.end(); it++ ){
		cout << "Trying to load song " << *it << endl;
		if ( loadSong( *it ) ){
			break;
		}
	}
}

bool Music::loadSong( const string & song ){
	return loadSong( song.c_str() );
}

void Music::_play(){
	if ( playing == false && this->player != NULL ){
		al_resume_duh( this->player );
	}
	playing = true;
}

void Music::play(){
	LOCK;{
		instance->_play();
	}
	UNLOCK;
}

void Music::_pause(){
	playing = false;
	if ( this->player != NULL ){
		al_pause_duh( this->player );
	}
}

void Music::pause(){
	LOCK;{
		instance->_pause();
	}
	UNLOCK;
}

void Music::soften(){
	LOCK;{
		instance->_soften();
	}
	UNLOCK;
}

void Music::_soften(){
	if ( volume > 0.1 ){
		volume -= 0.1;
	} else {
		volume = 0.0;
	}
	
	_setVolume( volume );
}

void Music::louden(){
	LOCK;{
		instance->_louden();
	}
	UNLOCK;
}

void Music::_louden(){
	if ( volume < 0.9 ){
		volume += 0.1;
	} else {
		volume = 1.0;
	}
	
	_setVolume( volume );
}

void Music::mute(){
	setVolume( 0 );
}

void Music::setVolume( double vol ){

	LOCK;{
		volume = vol;
		if ( volume > 1.0 ){
			volume = 1.0;
		}
		if ( volume < 0 ){
			volume = 0;
		}
		instance->_setVolume( volume );
	}
	UNLOCK;
}

void Music::_setVolume( double vol ){

	if ( player ){
		al_duh_set_volume( player, vol );
	}

}
	
Music::~Music(){

	LOCK;{
		if ( player ){
			al_stop_duh( player );
			unload_duh( music_file );
		}

		alive = false;
	}
	UNLOCK;

	cout << "Waiting for music thread to die" << endl;
	pthread_join( musicThread, NULL );

}

/*
void Music::pause(){
	al_pause_duh( player );
}
*/

/*
void Music::resume(){
	al_resume_duh( player );
}
*/

bool Music::internal_loadSong( const char * path ){

	// cout << "Trying to load '" << path << "'" << endl;

	if ( player != NULL ){
		al_stop_duh( player );
		unload_duh( music_file );
		player = NULL;
		music_file = NULL;
	}

	// music_file = dumb_load_mod( path );
	/*
	music_file = dumb_load_mod( path );
	if ( !music_file ){
		music_file = dumb_load_xm( path );
	}
	if ( !music_file ){
		music_file = dumb_load_s3m( path );
	}
	if ( !music_file ){
		music_file = dumb_load_it( path );
	}
	*/

	for ( int i = 0; i < 4; i++ ){
		switch ( i ){
			case 0 : {
				music_file = dumb_load_xm( path );
				break;
			}
			case 1 : {
				music_file = dumb_load_s3m( path );
				break;
			}
			case 2 : {
				music_file = dumb_load_it( path );
				break;
			}
			case 3 : {
				music_file = dumb_load_mod( path );
				break;
			}
		}
		if ( music_file != NULL ){
			cout << "Loaded " << path << " type " << i << endl;
			break;
		}
	}

	if ( music_file ){
		int buf = 1 << 11;
		player = al_start_duh( music_file, 2, 0, volume, buf, 22050 );
		// cout << "Loaded music player " << player << endl;

		/*
		while ( 1 ){
			al_poll_duh( player );
			rest( 1 );
		}
		*/

		playing = true;
	} else {
		cout<<"Could not load "<<path<<endl;
		return false;
	}
	return true;

}

#undef synchronized
#undef LOCK
#undef UNLOCK
